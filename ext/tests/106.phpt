--TEST--
\Solarwinds\Cache bucket state max length test
--EXTENSIONS--
apm_ext
--INI--
apm_ext.bucket_state_max_length=5
--FILE--
<?php

var_dump(\Solarwinds\Cache\putBucketState("p1", "1"));
var_dump(\Solarwinds\Cache\putBucketState("p2", "12"));
var_dump(\Solarwinds\Cache\putBucketState("p3", "123"));
var_dump(\Solarwinds\Cache\putBucketState("p4", "1234"));
var_dump(\Solarwinds\Cache\putBucketState("p5", "12345"));
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
bool(true)
bool(true)
apm_ext: bucket state length 6 exceeds max length 5
bool(false)
string(1) "1"
string(2) "12"
string(3) "123"
string(4) "1234"
string(5) "12345"
bool(false)

