--TEST--
\Solarwinds\Cache update test
--EXTENSIONS--
apm_ext
--FILE--
<?php

// Insert new values
for ($i = 0; $i < 2; $i++) {
    for ($j = 0; $j < 2; $j++) {
        for ($k = 0; $k < 2; $k++) {
            // Retrieve non-existing value
            var_dump(\Solarwinds\Cache\put("c".$i, "t".$j, "n".$k, $i.$j.$k));
        }
    }
}

// Retrieve existing values
for ($i = 0; $i < 2; $i++) {
    for ($j = 0; $j < 2; $j++) {
        for ($k = 0; $k < 2; $k++) {
            // Retrieve non-existing value
            var_dump(\Solarwinds\Cache\get("c".$i, "t".$j, "n".$k));
        }
    }
}

// Update new values
for ($i = 0; $i < 2; $i++) {
    for ($j = 0; $j < 2; $j++) {
        for ($k = 0; $k < 2; $k++) {
            // Retrieve non-existing value
            var_dump(\Solarwinds\Cache\put("c".$i, "t".$j, "n".$k, $j.$k.$i));
        }
    }
}

// Retrieve existing values
for ($i = 0; $i < 2; $i++) {
    for ($j = 0; $j < 2; $j++) {
        for ($k = 0; $k < 2; $k++) {
            // Retrieve non-existing value
            var_dump(\Solarwinds\Cache\get("c".$i, "t".$j, "n".$k));
        }
    }
}

?>
--EXPECT--
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(3) "000"
string(3) "001"
string(3) "010"
string(3) "011"
string(3) "100"
string(3) "101"
string(3) "110"
string(3) "111"
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
bool(true)
string(3) "000"
string(3) "010"
string(3) "100"
string(3) "110"
string(3) "001"
string(3) "011"
string(3) "101"
string(3) "111"
