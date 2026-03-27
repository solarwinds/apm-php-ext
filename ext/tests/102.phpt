--TEST--
\Solarwinds\Cache\getBucketState test
--EXTENSIONS--
apm_ext
--FILE--
<?php

// Retrieve non-existing values
for ($i = 0; $i < 2; $i++) {
  var_dump(\Solarwinds\Cache\getBucketState("p".$i));
}

?>
--EXPECT--
bool(false)
bool(false)
