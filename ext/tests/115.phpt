--TEST--
\Solarwinds\Cache unbounded bucket state cache grows without evicting
--EXTENSIONS--
apm_ext
--INI--
apm_ext.bucket_state_cache_max_entries=0
--FILE--
<?php

// max_entries=0 means unbounded. The hash table starts with 16 buckets and
// doubles when full, so 2000 distinct keys force several rehashes
// (16 -> 32 -> ... -> 2048), migrating every bucket chain each time.
$n = 2000;

$putOk = 0;
for ($i = 0; $i < $n; $i++) {
    $putOk += (int)\Solarwinds\Cache\putBucketState("k$i", "v$i");
}
var_dump($putOk);

// Nothing was evicted and every value survived the rehashes.
$found = 0;
for ($i = 0; $i < $n; $i++) {
    $found += (int)(\Solarwinds\Cache\getBucketState("k$i") === "v$i");
}
var_dump($found);

// Updates after growth replace values in place.
$updOk = 0;
for ($i = 0; $i < $n; $i += 3) {
    $updOk += (int)\Solarwinds\Cache\putBucketState("k$i", "u$i");
}
var_dump($updOk);

$updated = 0;
$unchanged = 0;
for ($i = 0; $i < $n; $i++) {
    $v = \Solarwinds\Cache\getBucketState("k$i");
    if ($i % 3 === 0) {
        $updated += (int)($v === "u$i");
    } else {
        $unchanged += (int)($v === "v$i");
    }
}
var_dump($updated);
var_dump($unchanged);

// Unknown keys still miss.
var_dump(\Solarwinds\Cache\getBucketState("k$n"));

?>
--EXPECT--
int(2000)
int(2000)
int(667)
int(667)
int(1333)
bool(false)
