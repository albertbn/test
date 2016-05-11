<?php

//this folk is used for ajax stuff from html (pages having html body) callers
header ( 'Content-Type: application/json; charset=utf-8' );

// The message
$proj = $_REQUEST['proj']; !strlen($proj) && ($proj='jimarata');
$file = "../proj/$proj.txt";
$text= '';

if ( file_exists($file) ){
    $text = file_get_contents($file);
}

$text = json_encode($text);

//and return the js/json answer to caller
echo ( "{ \"text\": $text  }" );

?>
