--TEST--
\Solarwinds\Cache put edge test
--EXTENSIONS--
apm_ext
--FILE--
<?php

var_dump(\Solarwinds\Cache\put("c", "t", "n", "s"));
var_dump(\Solarwinds\Cache\put(null, "t", "n", "s"));
var_dump(\Solarwinds\Cache\put("c", null, "n", "s"));
var_dump(\Solarwinds\Cache\put("c", "t", null, "s"));
var_dump(\Solarwinds\Cache\put("c", "t", "n", null));

var_dump(\Solarwinds\Cache\get("c", "t", "n"));
var_dump(\Solarwinds\Cache\get(null, "t", "n"));
var_dump(\Solarwinds\Cache\get("c", null, "n"));
var_dump(\Solarwinds\Cache\get("c", "t", null));

?>
--EXPECTF--
bool(true)

Deprecated: Solarwinds\Cache\put(): Passing null to parameter #1 ($collector) of type string is deprecated in %s on line %d
bool(false)

Deprecated: Solarwinds\Cache\put(): Passing null to parameter #2 ($token) of type string is deprecated in %s on line %d
bool(false)

Deprecated: Solarwinds\Cache\put(): Passing null to parameter #3 ($serviceName) of type string is deprecated in %s on line %d
bool(false)

Deprecated: Solarwinds\Cache\put(): Passing null to parameter #4 ($settings) of type string is deprecated in %s on line %d
bool(false)
string(1) "s"

Deprecated: Solarwinds\Cache\get(): Passing null to parameter #1 ($collector) of type string is deprecated in %s on line %d
bool(false)

Deprecated: Solarwinds\Cache\get(): Passing null to parameter #2 ($token) of type string is deprecated in %s on line %d
bool(false)

Deprecated: Solarwinds\Cache\get(): Passing null to parameter #3 ($serviceName) of type string is deprecated in %s on line %d
bool(false)
