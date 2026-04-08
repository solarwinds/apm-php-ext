--TEST--
\Solarwinds\Cache update bucket state test
--EXTENSIONS--
apm_ext
--FILE--
<?php

// Insert new values
for ($i = 0; $i < 2; $i++) {
  var_dump(\Solarwinds\Cache\putBucketState("p".$i, $i));
}

// Retrieve existing values
for ($i = 0; $i < 2; $i++) {
  var_dump(\Solarwinds\Cache\getBucketState("p".$i));
}

// Update values
for ($i = 0; $i < 2; $i++) {
  var_dump(\Solarwinds\Cache\putBucketState("p".$i, "new".$i));
}

// Retrieve updated values
for ($i = 0; $i < 2; $i++) {
  var_dump(\Solarwinds\Cache\getBucketState("p".$i));
}

?>
--EXPECT--
bool(true)
bool(true)
string(1) "0"
string(1) "1"
bool(true)
bool(true)
string(4) "new0"
string(4) "new1"
