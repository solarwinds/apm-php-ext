--TEST--
\Solarwinds\Cache settings max length test
--EXTENSIONS--
apm_ext
--INI--
apm_ext.settings_max_length=5
--FILE--
<?php

var_dump(\Solarwinds\Cache\put("c", "t", "n1", "1"));
var_dump(\Solarwinds\Cache\put("c", "t", "n2", "12"));
var_dump(\Solarwinds\Cache\put("c", "t", "n3", "123"));
var_dump(\Solarwinds\Cache\put("c", "t", "n4", "1234"));
var_dump(\Solarwinds\Cache\put("c", "t", "n5", "12345"));
var_dump(\Solarwinds\Cache\put("c", "t", "n6", "123456"));

var_dump(\Solarwinds\Cache\get("c", "t", "n1"));
var_dump(\Solarwinds\Cache\get("c", "t", "n2"));
var_dump(\Solarwinds\Cache\get("c", "t", "n3"));
var_dump(\Solarwinds\Cache\get("c", "t", "n4"));
var_dump(\Solarwinds\Cache\get("c", "t", "n5"));
var_dump(\Solarwinds\Cache\get("c", "t", "n6"));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
apm_ext: settings length 6 exceeds max length 5
bool(false)
string(1) "1"
string(2) "12"
string(3) "123"
string(4) "1234"
string(5) "12345"
bool(false)
