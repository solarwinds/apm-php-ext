--TEST--
Check if apm_ext is loaded
--FILE--
<?php
    if (!extension_loaded("apm_ext")) {
        echo "apm_ext extension is not available";
    } else {
        echo "apm_ext extension is available";
    }
?>
--EXPECT--
apm_ext extension is available
