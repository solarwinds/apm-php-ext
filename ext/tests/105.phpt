--TEST--
\Solarwinds\Cache put/getBucketState edge test
--EXTENSIONS--
apm_ext
--FILE--
<?php

var_dump(\Solarwinds\Cache\putBucketState("p", "s"));
var_dump(\Solarwinds\Cache\putBucketState(null, "s"));
var_dump(\Solarwinds\Cache\putBucketState("p", null));

var_dump(\Solarwinds\Cache\getBucketState("p"));
var_dump(\Solarwinds\Cache\getBucketState(null));

?>
--EXPECTF--
bool(true)

Deprecated: Solarwinds\Cache\putBucketState(): Passing null to parameter #1 ($pid) of type string is deprecated in %s on line %d
bool(false)

Deprecated: Solarwinds\Cache\putBucketState(): Passing null to parameter #2 ($bucketState) of type string is deprecated in %s on line %d
bool(false)
string(1) "s"

Deprecated: Solarwinds\Cache\getBucketState(): Passing null to parameter #1 ($pid) of type string is deprecated in %s on line %d
bool(false)

