--TEST--
Check if apm_ext is loaded
--EXTENSIONS--
apm_ext
--FILE--
<?php
echo 'The extension "apm_ext" is available';
?>
--EXPECT--
The extension "apm_ext" is available
