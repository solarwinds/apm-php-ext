--TEST--
\Solarwinds\Cache collector max length test
--EXTENSIONS--
apm_ext
--FILE--
<?php

$vc = '';
for ($i = 0; $i < 100; $i++) {
    $vc .= 'a';
}

var_dump(\Solarwinds\Cache\put($vc, "t", "n", "1"));
var_dump(\Solarwinds\Cache\get($vc, "t", "n"));

$ivc = '';
for ($i = 0; $i < 101; $i++) {
    $ivc .= 'a';
}

var_dump(\Solarwinds\Cache\put($ivc, "t", "n", "1"));
var_dump(\Solarwinds\Cache\get($ivc, "t", "n"));

?>
--EXPECT--
bool(true)
string(1) "1"
apm_ext: collector length 101 exceeds max length 100
bool(false)
bool(false)
