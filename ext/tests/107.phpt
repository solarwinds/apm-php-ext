--TEST--
\Solarwinds\Cache bucket state max entries test
--EXTENSIONS--
apm_ext
--INI--
apm_ext.bucket_state_cache_max_entries=3
--FILE--
<?php

var_dump(\Solarwinds\Cache\putBucketState("p1", "1"));
var_dump(\Solarwinds\Cache\putBucketState("p2", "12"));
var_dump(\Solarwinds\Cache\putBucketState("p3", "123"));

var_dump(\Solarwinds\Cache\getBucketState("p1"));
var_dump(\Solarwinds\Cache\getBucketState("p2"));
var_dump(\Solarwinds\Cache\getBucketState("p3"));

var_dump(\Solarwinds\Cache\putBucketState("p4", "1234"));

var_dump(\Solarwinds\Cache\getBucketState("p1"));
var_dump(\Solarwinds\Cache\getBucketState("p2"));
var_dump(\Solarwinds\Cache\getBucketState("p3"));
var_dump(\Solarwinds\Cache\getBucketState("p4"));

var_dump(\Solarwinds\Cache\putBucketState("p5", "12345"));

var_dump(\Solarwinds\Cache\getBucketState("p1"));
var_dump(\Solarwinds\Cache\getBucketState("p2"));
var_dump(\Solarwinds\Cache\getBucketState("p3"));
var_dump(\Solarwinds\Cache\getBucketState("p4"));
var_dump(\Solarwinds\Cache\getBucketState("p5"));

var_dump(\Solarwinds\Cache\putBucketState("p6", "123456"));

var_dump(\Solarwinds\Cache\getBucketState("p1"));
var_dump(\Solarwinds\Cache\getBucketState("p2"));
var_dump(\Solarwinds\Cache\getBucketState("p3"));
var_dump(\Solarwinds\Cache\getBucketState("p4"));
var_dump(\Solarwinds\Cache\getBucketState("p5"));
var_dump(\Solarwinds\Cache\getBucketState("p6"));

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
string(1) "1"
string(2) "12"
string(3) "123"
bool(true)
bool(false)
string(2) "12"
string(3) "123"
string(4) "1234"
bool(true)
bool(false)
bool(false)
string(3) "123"
string(4) "1234"
string(5) "12345"
bool(true)
bool(false)
bool(false)
bool(false)
string(4) "1234"
string(5) "12345"
string(6) "123456"

