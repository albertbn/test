
var xmlfile = "1.xml";

var util = require('util');
var fs = require('fs');
var Readable = require('stream').Readable;

var MyStream = function(options) {

  var tthis = this;

  Readable.call(this, options); // pass through the options to the Readable constructor

  // this.read_arr=[];
  // this.has_reader_ended = false;

  this.in_reader = fs.createReadStream(xmlfile, { encoding: "utf8" });

  this.in_reader.on('data', function(data) {
    tthis.push(data);
    // tthis.read_arr.push(data);
  });

  this.in_reader.on('end', function(data) {
    // tthis.has_reader_ended = true;
    tthis.push(null);
  });
};

util.inherits(MyStream, Readable); // inherit the prototype methods

// Object.defineProperty ( MyStream.prototype, "in_reader", {
//   get: function() {
//     if(this._in_reader==null){
//       this._in_reader =  fs.createReadStream(xmlfile, { encoding: "utf8" });
//     }
//     return this._in_reader;
//   }
// });

MyStream.prototype._read = function() {

  // console.log('read_arr...', this.read_arr);

  // (!this.read_arr || !this.read_arr.length) && this.push('');

   /*extract and push the first element in reader array*/
  // this.read_arr && this.read_arr.length && this.push( this.read_arr.shift() );

  // stop the stream when done
  // this.has_reader_ended && (!this.read_arr || !this.read_arr.length) && this.push(null);
};

var mystream = new MyStream();
mystream.pipe(process.stdout);
