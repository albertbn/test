
var fs = require('fs');
var expat = require('node-expat');
var parser = new expat.Parser('UTF-8');

var count = 0;
parser.on('startElement', function (name, attrs) {
  // console.log(name, attrs);
});

parser.on('endElement', function (name) {

  // if(name=='row') console.log(name);
  if(name=='row') ++count;
  // if(name=='entry') ++count;
});

parser.on('text', function (text) {
  // console.log(text);
});

parser.on('error', function (error) {
  console.error(error);
  parser.resume();
});

// var fstr = fs.createReadStream( './cotd.xml', { encoding: "utf8" } );
var fstr = fs.createReadStream( './cotd_big.xml', { encoding: "utf8" } );
// var fstr = fs.createReadStream( './goo.xml', { encoding: "utf8" } );

// parser.write(fstr);
fstr.pipe(parser);

fstr.once('end', function(){
  console.log('thats it, count is', count);
});


// parser.write('<html><head><title>Hello World</title></head><body><p>Foobar</p></body></html>');
