--TEST--
\Solarwinds\Cache\putBucketState test
--EXTENSIONS--
apm_ext
--FILE--
<?php

// Retrieve non-existing values
for ($i = 0; $i < 2; $i++) {
  var_dump(\Solarwinds\Cache\getBucketState("p".$i));
}

// Insert new values
for ($i = 0; $i < 2; $i++) {
  var_dump(\Solarwinds\Cache\putBucketState("p".$i, $i));
}

// Retrieve existing values
for ($i = 0; $i < 2; $i++) {
  var_dump(\Solarwinds\Cache\getBucketState("p".$i));
}

?>
--EXPECT--
bool(false)
bool(false)
bool(true)
bool(true)
string(1) "0"
string(1) "1"

