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

sleep(1);

$ret = \Solarwinds\Sampler\setting();
var_dump(strlen($ret) > 0);
?>
--EXPECT--
bool(true)
bool(true)