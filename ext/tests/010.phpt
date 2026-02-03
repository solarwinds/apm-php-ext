--TEST--
\Solarwinds\Cache token max length test
--EXTENSIONS--
apm_ext
--FILE--
<?php

$vt = '';
for ($i = 0; $i < 128; $i++) {
    $vt .= 'a';
}

var_dump(\Solarwinds\Cache\put("c", $vt, "n", "1"));
var_dump(\Solarwinds\Cache\get("c", $vt, "n"));

$ivt = '';
for ($i = 0; $i < 129; $i++) {
    $ivt .= 'a';
}
var_dump(\Solarwinds\Cache\put("c", $ivt, "n", "1"));
var_dump(\Solarwinds\Cache\get("c", $ivt, "n"));


?>
--EXPECT--
bool(true)
string(1) "1"
apm_ext: token length 129 exceeds max length 128
bool(false)
bool(false)
