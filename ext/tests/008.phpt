--TEST--
\Solarwinds\Cache LRU test
--EXTENSIONS--
apm_ext
--INI--
apm_ext.cache_max_entries=3
--FILE--
<?php

var_dump(\Solarwinds\Cache\put("c", "t", "n1", "1"));
var_dump(\Solarwinds\Cache\put("c", "t", "n2", "12"));
var_dump(\Solarwinds\Cache\put("c", "t", "n3", "123"));
// cache moved
var_dump(\Solarwinds\Cache\get("c", "t", "n1"));
// n2 is out
var_dump(\Solarwinds\Cache\put("c", "t", "n4", "1234"));
// n2 not found
var_dump(\Solarwinds\Cache\get("c", "t", "n2"));
// n3,n4, n1 are present
var_dump(\Solarwinds\Cache\get("c", "t", "n3"));
var_dump(\Solarwinds\Cache\get("c", "t", "n4"));
var_dump(\Solarwinds\Cache\get("c", "t", "n1"));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(1) "1"
bool(true)
bool(false)
string(3) "123"
string(4) "1234"
string(1) "1"
