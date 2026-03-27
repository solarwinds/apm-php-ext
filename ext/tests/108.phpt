--TEST--
\Solarwinds\Cache bucket state LRU test
--EXTENSIONS--
apm_ext
--INI--
apm_ext.bucket_state_cache_max_entries=3
--FILE--
<?php

var_dump(\Solarwinds\Cache\putBucketState("p1", "1"));
var_dump(\Solarwinds\Cache\putBucketState("p2", "12"));
var_dump(\Solarwinds\Cache\putBucketState("p3", "123"));
// cache moved
var_dump(\Solarwinds\Cache\getBucketState("p1"));
// p2 is out
var_dump(\Solarwinds\Cache\putBucketState("p4", "1234"));
// p2 not found
var_dump(\Solarwinds\Cache\getBucketState("p2"));
// p3,p4,p1 are present
var_dump(\Solarwinds\Cache\getBucketState("p3"));
var_dump(\Solarwinds\Cache\getBucketState("p4"));
var_dump(\Solarwinds\Cache\getBucketState("p1"));

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

