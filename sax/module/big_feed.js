
// reads large xml file with streaming and outputs JSON {...}

// obj - will get an object used in constructor
module.exports = big_feed;

//------end node entry---------

// works with large xml files
var fs = require("fs"); /*stream reads the xml file*/

// implement readable interface so one could call this folk from wherever place
var util = require('util');
var Readable = require('stream').Readable;

var sax = require( "sax" ); /*parses the xml*/
var xml2js = require("xml2js"); /*converts xml 2 json - note that in naked text cases parsing, there is a slight change*/

var inspect = require('./inspect.js'); /*gets the xml structure by reading the begining of file*/

// TODO - deal with field names collision
// TODO - add cdata as well

//constructor
function big_feed ( obj ) {

  Readable.call(this, obj); // pass through the options to the Readable constructor
  this.xmlfile = obj['xmlfile'];
  this.filter = obj['filter'];
  this.get();
}

// implement readable :: inherit the prototype methods
util.inherits(big_feed, Readable);

// a must dummy for implementing readable
big_feed.prototype._read = function() {};

// calls inspect and then parse
big_feed.prototype.get = function () {

  var tthis = this;

  this.push(''); /*push an empty thing to pulse the consumer/client of this reader something is happening*/

  // call the inspect
  inspect( this.xmlfile, function(err, inspect_result){

    tthis.parse(inspect_result);
  });
};

big_feed.prototype.parse = function ( inspect_result ){

  tthis = this;

  //strict
  var strict = false
  ,counter = 0 /*counter for the bulk inset*/
  ;


  var ROOT = inspect_result['root'],
      ARR = inspect_result['arr'];

  //========
  var bytes_read = 0;
  //========
  var readBuffer = new Buffer( Math.pow(2,16) ), bufferOffset = 0;

  var r_clean = new RegExp('\\n?</?'+ARR+'[^>]*>\\n?','gi'); /*!*/

  var fd = fs.openSync( tthis.xmlfile, 'r' );

  //the writable sax memory stream
  var printer = sax.createStream(strict,
                                 {
                                   lowercasetags:true,
                                   trim:true,
                                   position:true
                                 }
                                );

  var fn_attrNameProcessor = function(name){
    return name.toLowerCase();
  };

  //this is used to parse each row/array element... into JSON
  var xml2js_parser = new xml2js.Parser({
    strict:strict, normalizeTags:true, explicitArray:false, mergeAttrs:true,
    attrNameProcessors:[fn_attrNameProcessor]
  });
  //end xml2ja

  function entity (str) {
    return str.replace('"', '&quot;');
  }

  //this cold naybe used further..., we'll see
  printer.level = 0;

  //===========
  // start events...
  //===========
  var elem = null; /*elem.content = [] will hold all text + nodes*/
  var start_tag_pos=0;
  printer.on ( "opentag", opentag);
  function opentag ( tag ) {

    var i;
    //open/create first element
    if ( tag.name===ARR ){

      start_tag_pos = this._parser.startTagPosition-1;

      elem = {};

      for ( i in tag.attributes ) {
        elem[i] = tag.attributes[i];
      }

      elem['content'] = [''];
    }
    else if(tag.name!==ROOT && elem && elem['content']){
      elem['content'].push('');
    };

    this.level ++;

    if ( !(elem && elem['content']) ) return;

    print( "<"+tag.name,  tag.name===ARR );
    for ( i in tag.attributes ) {
      print(" "+i+"=\""+entity(tag.attributes[i])+"\"",  tag.name===ARR );
    }
    print(">",  tag.name===ARR );
  }

  printer.on("text", ontext);
  function ontext (text) {
    print(text);
  }

  // TODO - add also CDATA to this game
  printer.on("cdata", function (data) {

    print("<![CDATA["+data+"]]>");
  });

  printer.on("error", function (error) {

    console.error(error);
    //throw error; - don't be a bitch
  });

  printer.on ( "closetag", closetag );
  function closetag ( tag ) {

    --this.level;

    if ( tag===ARR ) {

      //debugger;
      // fstr.pause();

      var is_text = check_text_from_content().is_text;
      // console.log ( 'is_text in closetag: ', is_text );

      if ( !is_text ){

        //wrap this chap in some root stuff..., so that xml2js can handle it swell
        elem['text'] = '<'+ARR+'>' + elem['text'] +  '</'+ARR+'>';

        //parse the elem string gathered...
        xml2js_parser.parseString ( elem['text'], xml2js_parserString_cb );
      }
      else {
        output(elem);
      }

      //DONE - here write with decent stream to mongo via mongoose
      elem = null;
    }

    print("</"+tag+">");
  }

  // returns {is_text:1/0} and modifies the elem['text']
  function check_text_from_content ( ) {

    // currently use the info from the inspect stuff
    var ret = {
      is_text: (inspect_result && inspect_result['has_naked_text'])
    };

    //for real text - read it originally from buffer
    if ( ret['is_text'] ) {

      bytes_read = fs.readSync(
        fd, readBuffer,
        bufferOffset,
        printer._parser.position - start_tag_pos,
        start_tag_pos
      );

      elem['text'] = readBuffer.slice(0,bytes_read).toString().replace(r_clean,'').trim();
      // !elem['text'].length && (delete elem['text']);
    }
    else {
      elem['text'] = elem['content'].join(' ');
    }

    delete elem['content'];

    return ret;
  }

  //xml2js callback - !resumes readstream at the end
  function xml2js_parserString_cb ( err, res ) {

    delete elem['text'];

    if (err) console.error(err.message);

    for ( var key in res[ARR] ) {
      elem[key] = res[ARR][key];
    }

    output(elem);
  }

  function print ( c, is_ignore_in_output ) {

    var len;
    if ( !is_ignore_in_output && elem!=null && elem['content'] && (len = elem['content'].length) ) {

      elem['content'][len-1]+=c;
    }
  }

  function output ( row ) {

    if(!counter++){

      tthis.push("{\"arr\":[\n")
    }

     if ( !eval(tthis.filter)  ){
      return;
    }
    else if(counter>1){
      tthis.push('\n,');
    }

    // tthis.push(JSON.stringify(JSON.parse(JSON.stringify(row))) ); /*push to reader...*/
    tthis.push( JSON.stringify(row) ); /*push to reader...*/

    // TEMP
    // if(counter>10) fstr_end();
  }

  function fstr_end () {

    tthis.push("\n]}");
    tthis.push(null); /*mark end of reader*/
    do_final();
  };

  function do_final ( ) {

    // console.log ( 'written %d records to output...', counter );

    fstr.unpipe(printer);
    fstr.destroy();
    fstr = null;
    counter = 0;

    // printer.destroy(); - no such animal
    printer = null;

    global && global.gc &&  global.gc();
  }

  //get read stream and call the thing...
  var fstr = fs.createReadStream( tthis.xmlfile, { encoding: "utf8" } );
  fstr.pipe(printer);  // yay! starts the piping/streaming...
  fstr.once('end', fstr_end);
}
