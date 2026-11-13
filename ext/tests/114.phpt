--TEST--
\Solarwinds\Cache stays usable after memory_limit bailout during get
--EXTENSIONS--
apm_ext
--SKIPIF--
<?php
if (getenv('USE_ZEND_ALLOC') === '0') die('skip memory_limit is not enforced without Zend MM');
// Some PHP builds (e.g. Ubuntu/ondrej PHP 8.1.34) exit immediately on memory
// exhaustion without running shutdown functions, which this test relies on.
$php = getenv('TEST_PHP_EXECUTABLE') ?: PHP_BINARY;
// No double quotes: escapeshellarg() strips them on Windows.
$probe = "register_shutdown_function(function () { echo 'shutdown ran'; });"
    . "\$a = str_repeat('y', 6 << 20); \$b = str_repeat('x', 6 << 20);";
$out = shell_exec(escapeshellarg($php) . ' -n -d memory_limit=12M -d display_errors=0 -r '
    . escapeshellarg($probe) . ' 2>&1');
if (strpos((string)$out, 'shutdown ran') === false) die('skip shutdown functions do not run after memory exhaustion on this PHP build');
?>
--INI--
memory_limit=12M
apm_ext.bucket_state_max_length=8388608
--FILE--
<?php

$big = str_repeat('x', 6 * 1024 * 1024);
var_dump(\Solarwinds\Cache\putBucketState("big", $big));
var_dump(\Solarwinds\Cache\putBucketState("small", "ok"));
unset($big);

register_shutdown_function(function () {
    // Must not deadlock: the cache lock must have been released by the
    // failed get below.
    var_dump(\Solarwinds\Cache\getBucketState("small"));
    var_dump(\Solarwinds\Cache\putBucketState("small", "still ok"));
    var_dump(\Solarwinds\Cache\getBucketState("small"));
});

$filler = str_repeat('y', 6 * 1024 * 1024);
// Copying the 6MB value into a PHP string exceeds memory_limit.
\Solarwinds\Cache\getBucketState("big");
echo "unreachable\n";
?>
--EXPECTF--
bool(true)
bool(true)

Fatal error: Allowed memory size of %d bytes exhausted%s
string(2) "ok"
bool(true)
string(8) "still ok"
