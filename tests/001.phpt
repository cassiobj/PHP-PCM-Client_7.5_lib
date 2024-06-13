--TEST--
Check if pcm_client is loaded
--EXTENSIONS--
pcm_client
--FILE--
<?php
echo 'The extension "pcm_client" is available';
?>
--EXPECT--
The extension "pcm_client" is available
