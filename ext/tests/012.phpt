--TEST--
\Solarwinds\Cache forking test
--EXTENSIONS--
apm_ext
pcntl
--FILE--
<?php

var_dump(\Solarwinds\Cache\put("c", "t", "n1", "1"));
var_dump(\Solarwinds\Cache\get("c", "t", "n1"));

$pid = pcntl_fork();

if ($pid == -1) {
  die('could not fork');
}
else if ($pid) {
    // parent process
    pcntl_wait($status); //Protect against Zombie children
    var_dump("parent");
    var_dump(\Solarwinds\Cache\get("c", "t", "n1"));
    var_dump(\Solarwinds\Cache\put("c", "t", "n2", "2"));
    var_dump(\Solarwinds\Cache\get("c", "t", "n2"));

} else {
    // child process
    var_dump("child");
    var_dump(\Solarwinds\Cache\get("c", "t", "n1"));
    var_dump(\Solarwinds\Cache\put("c", "t", "n3", "3"));
    var_dump(\Solarwinds\Cache\get("c", "t", "n3"));
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
