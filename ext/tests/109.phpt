--TEST--
\Solarwinds\Cache pid max length test
--EXTENSIONS--
apm_ext
--FILE--
<?php

$vpid = '';
for ($i = 0; $i < 8; $i++) {
    $vpid .= 'a';
}

var_dump(\Solarwinds\Cache\putBucketState($vpid, "1"));
var_dump(\Solarwinds\Cache\getBucketState($vpid));

$ivpid = '';
for ($i = 0; $i < 9; $i++) {
    $ivpid .= 'a';
}

var_dump(\Solarwinds\Cache\putBucketState($ivpid, "1"));
var_dump(\Solarwinds\Cache\getBucketState($ivpid));

?>
--EXPECT--
bool(true)
string(1) "1"
apm_ext: pid length 9 exceeds max length 8
bool(false)
bool(false)

