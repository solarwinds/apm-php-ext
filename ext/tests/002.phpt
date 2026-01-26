--TEST--
\Solarwinds\Sampler\settings() Basic test
--EXTENSIONS--
apm_ext
--FILE--
<?php
$ret = \Solarwinds\Sampler\settings("apm.collector.na-01.cloud.solarwinds.com", "token:unknown");
var_dump(strlen($ret) == 0);

// Poll for the setting to become non-empty, up to ~1 second total.
$maxAttempts = 100;
$intervalMicros = 10000; // 10ms
for ($i = 0; $i < $maxAttempts; $i++) {
    $ret = \Solarwinds\Sampler\settings("apm.collector.na-01.cloud.solarwinds.com", "token:unknown");
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
