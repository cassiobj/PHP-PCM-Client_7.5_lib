--TEST--
test1() Basic test
--EXTENSIONS--
pcm_client
--FILE--
<?php
$ret = test1();

var_dump($ret);
?>
--EXPECT--
The extension pcm_client is loaded and working!
NULL
