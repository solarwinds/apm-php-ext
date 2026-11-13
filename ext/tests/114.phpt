--TEST--
\Solarwinds\Cache stays usable after memory_limit bailout during get
--EXTENSIONS--
apm_ext
--SKIPIF--
<?php
if (getenv('USE_ZEND_ALLOC') === '0') die('skip memory_limit is not enforced without Zend MM');
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
