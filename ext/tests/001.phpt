--TEST--
Check if apm_ext is loaded
--EXTENSIONS--
apm_ext-test
--FILE--
<?php
echo 'The extension "apm_ext-test" is available';
?>
--EXPECT--
The extension "apm_ext-test" is available
