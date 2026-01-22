--TEST--
Check for apm_ext-test presence
--FILE--
<?php
    if (!extension_loaded("apm_ext_test")) {
        echo "apm_ext extension is not available";
    } else {
        echo "apm_ext extension is available";
    }
?>
--EXPECT--
apm_ext extension is available