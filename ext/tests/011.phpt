--TEST--
\Solarwinds\Cache service name max length test
--EXTENSIONS--
apm_ext
--FILE--
<?php

$vn = '';
for ($i = 0; $i < 128; $i++) {
    $vn .= 'a';
}

var_dump(\Solarwinds\Cache\put("c", "t", $vn, "1"));
var_dump(\Solarwinds\Cache\get("c", "t", $vn));

$ivn = '';
for ($i = 0; $i < 129; $i++) {
    $ivn .= 'a';
}
var_dump(\Solarwinds\Cache\put("c", "t", $ivn, "1"));
var_dump(\Solarwinds\Cache\get("c", "t", $ivn));


?>
--EXPECT--
bool(true)
string(1) "1"
apm_ext: service name length 129 exceeds max length 128
bool(false)
bool(false)
