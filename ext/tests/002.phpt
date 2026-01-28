--TEST--
\Solarwinds\Cache\get test
--EXTENSIONS--
apm_ext
--FILE--
<?php

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
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
bool(false)
