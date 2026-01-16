--TEST--
\Solarwinds\Sampler\setting() Basic test
--EXTENSIONS--
apm_ext
--INI--
apm_ext.service_key=token:unknown
--FILE--
<?php
$ret = \Solarwinds\Sampler\setting();
var_dump(strlen($ret) == 0);

// Poll for the setting to become non-empty, up to ~1 second total.
$maxAttempts = 100;
$intervalMicros = 10000; // 10ms
for ($i = 0; $i < $maxAttempts; $i++) {
    $ret = \Solarwinds\Sampler\setting();
    if (strlen($ret) > 0) {
        break;
    }
    usleep($intervalMicros);
}
var_dump(strlen($ret) > 0);
?>
--EXPECT--
bool(true)
bool(true)