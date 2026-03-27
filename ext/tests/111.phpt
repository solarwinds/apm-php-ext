--TEST--
\Solarwinds\Cache bucket state empty input test
--EXTENSIONS--
apm_ext
--FILE--
<?php

var_dump(\Solarwinds\Cache\putBucketState("p", "s"));
var_dump(\Solarwinds\Cache\putBucketState("", "s"));
var_dump(\Solarwinds\Cache\putBucketState("p2", ""));

var_dump(\Solarwinds\Cache\getBucketState("p"));
var_dump(\Solarwinds\Cache\getBucketState(""));
var_dump(\Solarwinds\Cache\getBucketState("missing"));

?>
--EXPECT--
bool(true)
bool(false)
bool(false)
string(1) "s"
bool(false)
bool(false)

