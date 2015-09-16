
var sax = require("sax"),
  strict = false, // set to false for html-mode
    //parser = sax.parser(strict),
    fs = require('fs');

// stream usage
// takes the same options as the parser
var saxStream = require("sax").createStream(strict, {});

// saxStream.onerror = function (e) {
//   // an error happened.
//   console.log('error: %s',e);
// };

// saxStream.ontext = function (t) {
//   // got some text.  t is the string of text.
//   console.log(t);
// };

// saxStream.onopentag = function (node) {
//   // opened a tag.  node has "name" and "attributes"
//   console.log(node);
// };

saxStream.onattribute = function (attr) {
  // an attribute.  attr has "name" and "value"
  console.log( '\t attr::: \t%j', attr );
};

saxStream.onend = function () {
  // parser stream is done, and ready to have more stuff written to it.
  console.log('focking end...');
};

// saxStream.write('<xml>Hello, <who name="world">world &amp; me in it. ET go home</who>!</xml>').close();


saxStream.on ("error", function (e) {
  // unhandled errors will throw, since this is a proper node
  // event emitter.
  console.error("error!", e);
  // clear the error
  this._parser.error = null;
  this._parser.resume();
});

saxStream.on("opentag", function (node) {
  // same object as above
  console.log("<%s> %j",node.name, node);
});

saxStream.on("closetag", function (node) {
  // same object as above
  console.log( "</%s>", node );
});

// pipe is supported, and it's readable/writable
// same chunks coming in also go out.
var stream = fs.createReadStream("test.xml")
      .pipe(saxStream);
  //.pipe(fs.createWriteStream("test-copy.xml"));
