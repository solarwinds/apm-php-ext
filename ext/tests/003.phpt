--TEST--
\Solarwinds\Sampler\put test
--EXTENSIONS--
apm_ext
--FILE--
<?php

// Retrieve non-existing value
for ($i = 0; $i < 2; $i++) {
    for ($j = 0; $j < 2; $j++) {
        for ($k = 0; $k < 2; $k++) {
            // Retrieve non-existing value
            var_dump(\Solarwinds\Sampler\get("c".$i, "t".$j, "n".$k));
        }
    }
}

// Insert new values
for ($i = 0; $i < 2; $i++) {
    for ($j = 0; $j < 2; $j++) {
        for ($k = 0; $k < 2; $k++) {
            // Retrieve non-existing value
            var_dump(\Solarwinds\Sampler\put("c".$i, "t".$j, "n".$k, $i.$j.$k));
        }
    }
}

// Retrieve existing values
for ($i = 0; $i < 2; $i++) {
    for ($j = 0; $j < 2; $j++) {
        for ($k = 0; $k < 2; $k++) {
            // Retrieve non-existing value
            var_dump(\Solarwinds\Sampler\get("c".$i, "t".$j, "n".$k));
        }
    }
}

?>
--EXPECT--
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
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
