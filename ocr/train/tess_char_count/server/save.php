<?php

//this folk is used for ajax stuff from html (pages having html body) callers
header ( 'Content-Type: application/json; charset=utf-8' );

// The message
$proj = $_REQUEST['proj']; !strlen($proj) && ($proj='jimarata');
$file = "../proj/$proj.txt";
$text= $_REQUEST['text']; !strlen($text) && ($text="test");

// echo $text;
// $text = json_decode($text);

$fileHandle = fopen($file, "w") or die("Unable to open file!");
fwrite($fileHandle, $text);
fclose($fileHandle);

//and return the js/json answer to caller
echo ( "{ \"status\": \"OK\"  }" );

?>
