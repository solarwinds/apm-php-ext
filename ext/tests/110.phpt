--TEST--
\Solarwinds\Cache bucket state length boundary test
--EXTENSIONS--
apm_ext
--INI--
apm_ext.bucket_state_max_length=128
--FILE--
<?php

$vstate = '';
for ($i = 0; $i < 128; $i++) {
    $vstate .= 'a';
}

var_dump(\Solarwinds\Cache\putBucketState("p1", $vstate));
var_dump(\Solarwinds\Cache\getBucketState("p1"));

$ivstate = '';
for ($i = 0; $i < 129; $i++) {
    $ivstate .= 'a';
}
var_dump(\Solarwinds\Cache\putBucketState("p2", $ivstate));
var_dump(\Solarwinds\Cache\getBucketState("p2"));

?>
--EXPECT--
bool(true)
string(128) "aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa"
apm_ext: bucket state length 129 exceeds max length 128
bool(false)
bool(false)

