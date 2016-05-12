<?php

//this folk is used for ajax stuff from html (pages having html body) callers
header ( 'Content-Type: application/json; charset=utf-8' );

// The message
$proj = $_REQUEST['proj']; !strlen($proj) && ($proj='jimarata');
$file = "../proj/$proj.txt";
$file_back = "../proj/$proj".rand(1,10).".txt";
$text= $_REQUEST['text']; !strlen($text) && ($text="test");

// echo $text;
$p = '/\\\\(["\'\\\\])/';
$r = '$1';
$text = preg_replace ( $p, $r, $text );

if ( file_exists($file) ){
    copy ( $file, $file_back ) || die ( "Unable to copy backup!" );
}

$fileHandle = fopen($file, "w") or die("Unable to open file!");
fwrite($fileHandle, $text);
fclose($fileHandle);

//and return the js/json answer to caller
echo ( "{ \"status\": \"OK\"  }" );

?>
