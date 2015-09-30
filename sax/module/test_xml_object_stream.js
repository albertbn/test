var fs, parser, readStream, xml;

xml = require('./xml-object-stream');

fs = require('fs');

// readStream = fs.createReadStream('cotd.xml');
// readStream = fs.createReadStream('cotd_big.xml');
readStream = fs.createReadStream('goo.xml');
// readStream = fs.createReadStream('goo2.xml');
// readStream = fs.createReadStream('goo3.xml');

parser = xml.parse(readStream);

var count = 0;
// parser.each('row', function(row) {
parser.each('entry', function(row) {
  ++count;
});

parser.on('end', function(){

  console.log('thats it, count is:',count);
});

parser.on('error', function(err){

  console.log(err);
});
