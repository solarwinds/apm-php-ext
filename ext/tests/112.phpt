--TEST--
\Solarwinds\Cache bucket state forking test
--EXTENSIONS--
apm_ext
pcntl
--FILE--
<?php

var_dump(\Solarwinds\Cache\putBucketState("p1", "1"));
var_dump(\Solarwinds\Cache\getBucketState("p1"));

$pid = pcntl_fork();

if ($pid == -1) {
  die('could not fork');
}
else if ($pid) {
    // parent process
    pcntl_wait($status); // Protect against Zombie children
    var_dump("parent");
    var_dump(\Solarwinds\Cache\getBucketState("p1"));
    var_dump(\Solarwinds\Cache\putBucketState("p2", "2"));
    var_dump(\Solarwinds\Cache\getBucketState("p2"));

} else {
    // child process
    var_dump("child");
    var_dump(\Solarwinds\Cache\getBucketState("p1"));
    var_dump(\Solarwinds\Cache\putBucketState("p3", "3"));
    var_dump(\Solarwinds\Cache\getBucketState("p3"));
}

?>
--EXPECT--
bool(true)
string(1) "1"
string(5) "child"
bool(false)
bool(true)
string(1) "3"
string(6) "parent"
bool(false)
bool(true)
string(1) "2"

