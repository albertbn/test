
var fs, parser, readStream, xml;

fs = require('fs');
xml = require('./xml-object-stream');

// readStream = fs.createReadStream('cotd.xml');
// readStream = fs.createReadStream('cotd_big.xml');
readStream = fs.createReadStream('goo.xml');
// readStream = fs.createReadStream('goo2.xml');
// readStream = fs.createReadStream('goo3.xml');
// readStream = fs.createReadStream('err_xos.log');

var parser_options = { "bytes_cleanup":true };
parser = xml.parse ( readStream, parser_options );

var count = 0;
// parser.each('row', function(row) {
parser.each('entry', function(row) {

  if (row && row['custom_label_1'] && row['custom_label_1']['$text'] && row['custom_label_1']['$text']=='Computers' &&  ++count){

    row['$children'] && (delete row['$children']);
    console.log (JSON.stringify(row));
  }
});

parser.on ('end', function(){

  console.log ('thats it, count is:',count);
});

parser.on('error', function(err){

  console.log(err);
});
