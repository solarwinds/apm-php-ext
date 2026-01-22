--TEST--
Check for apm_ext-test presence
--SKIPIF--
<?php if (!extension_loaded("apm_ext_test")) print "skip - apm_ext_test extension not available"; ?>
--FILE--
<?php
echo "apm_ext extension is available";
?>
--EXPECT--
apm_ext extension is available