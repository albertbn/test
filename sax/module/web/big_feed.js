
/*

reads large xml file with streaming and outputs JSON {...}

for an example usage see ./test_bf.js

will get a reference to the big_feed constructor

caller should do:

var big_feed = require('./big_feed');
var bf = new bf({options...})

options are:

var options = {

  "xmlfile":"./goo.xml",
  "entry" :"entry",
  "filter" : " row && row['custom_label_1'] && row['custom_label_1']['$text'] && row['custom_label_1']['$text']=='Computers' ",
  "mutate" : "row && row['description'] && (delete row['description'])",
  "bytes_cleanup": true
};

*/

module.exports = big_feed;

//------end node entry---------

// works with large xml files
var fs = require("fs"); /*stream reads the xml file*/
var xml = require('./xml-object-stream');

// implement readable interface so one could call this folk from wherever place
var util = require('util');
var Readable = require('stream').Readable;

//constructor
function big_feed ( obj ) {

  Readable.call(this, obj); // pass through the options to the Readable constructor
  this.xmlfile = obj['xmlfile'];
  this.filter = obj['filter'];
  this.mutate = obj['mutate'];
  this.entry = obj['entry']; /*the record/entry name, ex.: "row"*/
  this.bytes_cleanup = obj['bytes_cleanup']; /*if to clean garbage bytes - !slower performance*/
  this.parse();
}

// implement readable :: inherit the prototype methods
util.inherits(big_feed, Readable);

// a must dummy for implementing readable
big_feed.prototype._read = function() {};

big_feed.prototype.parse = function () {

  var tthis = this;

  this.push(''); /*push an empty thing to pulse the consumer/client of this reader something is happening*/

  //get read stream and call the thing...
  var fstr = fs.createReadStream( tthis.xmlfile, { encoding: "utf8" } );
  var parser_options = { "bytes_cleanup": tthis.bytes_cleanup };
  var parser = xml.parse ( fstr, parser_options );

  var counter = 0;
  parser.each ( tthis.entry, function(row) {

    if(counter<1){

      tthis.push( "{\"arr\":[\n" ); /*push to reader... start of array*/
      ++counter;
    }

    if ( !eval(tthis.filter)  ){
      return;
    }
    else if(++counter>2){
      tthis.push('\n,'); /*push to reader...new line and comma after first record*/
    }
    // clear $children - not needed...
    row['$children'] && (delete row['$children']);

    tthis.mutate && (eval(tthis.mutate)); /*additional cleanup to make stuff lighter for output*/

    tthis.push( JSON.stringify(row) ); /*push to reader...*/
  });

  parser.on ('end', function(){

    tthis.push("\n]}");
    tthis.push(null); /*mark end of reader*/
  });

  // parser.on error is not actually implemented in ./xml-object-stream
};

