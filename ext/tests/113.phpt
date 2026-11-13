--TEST--
\Solarwinds\Cache bucket state LRU eviction with many entries
--EXTENSIONS--
apm_ext
--INI--
apm_ext.bucket_state_cache_max_entries=100
--FILE--
<?php

for ($i = 0; $i < 1000; $i++) {
    \Solarwinds\Cache\putBucketState((string)$i, "v$i");
    // keep "0" hot so it is never the LRU entry
    \Solarwinds\Cache\getBucketState("0");
}

$present = 0;
$missing = 0;
for ($i = 1; $i < 1000; $i++) {
    $v = \Solarwinds\Cache\getBucketState((string)$i);
    if ($v === false) {
        $missing++;
    } elseif ($v === "v$i" && $i >= 901) {
        $present++;
    }
}
var_dump(\Solarwinds\Cache\getBucketState("0"));
var_dump($present);
var_dump($missing);

// update keeps size bounded and replaces value
var_dump(\Solarwinds\Cache\putBucketState("950", "updated"));
var_dump(\Solarwinds\Cache\getBucketState("950"));

?>
--EXPECT--
string(2) "v0"
int(99)
int(900)
bool(true)
string(7) "updated"
