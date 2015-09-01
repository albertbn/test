
//focking amateurs writing async insert to db - fuck u

//strict
var strict = true
,BULK_SIZE = 1000
,counter = 0 /*counter for the bulk inset*/
;

var fs = require("fs");

//========
var bytes_read = 0;
//========
var readBuffer = new Buffer( Math.pow(2,16) ),
    bufferOffset = 0
;

var r_clean = /<\/?row[^>]*>\n?/gi;

var fd = fs.openSync( './xml0.xml', 'r');

//requires
var sax = require("sax");
//the writable sax memory stream
var printer = sax.createStream(strict,
                               {
                                 lowercasetags:true,
                                 trim:true,
                                 position:true
                               }
                              );
var xml2js = require("xml2js");
//var async = require("async");
var mongoose = require("mongoose");
var bulk = null /*used for bulk inserts...*/;

var xmlfile = './xml0.xml',
    path_dump_obj = 'dump_obj.txt',
    //HERE
    ROOT = 'rows',
    ARR = 'row'
    //TO HERE
;
var Schema = mongoose.Schema;

//set streaming stuff
//var entrySchema = new Schema({ id:Number, name:String, music:String },{ strict: false });
var entrySchema = new Schema({ },{ strict: false });
var Entry = mongoose.model( "row", entrySchema );

//xml2ja
var fn_attrNameProcessor = function(name){
  return name.toLowerCase();
};

//this is used to parse each row/array element... into JSON
var xml2js_parser = new xml2js.Parser({
  strict:false, normalizeTags:true, explicitArray:false, mergeAttrs:true,
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
function opentag (tag) {

  var i;
  //open/create first element
  if ( tag.name===ARR ){

    start_tag_pos = this._parser.startTagPosition-1;
    console.log('open tag at startTag: %s, pos: %s, bufferCheckPosition: %s', start_tag_pos, this._parser.position, this._parser.bufferCheckPosition);

    elem = {};

    for ( i in tag.attributes ) {
      elem[i] = tag.attributes[i];
    }

    elem['content'] = [''];
  }
  else if(tag.name!==ROOT){
    elem['content'].push('');
  };

  this.level ++;
  print( "<"+tag.name,  tag.name===ARR );
  for ( i in tag.attributes ) {
    print(" "+i+"=\""+entity(tag.attributes[i])+"\"",  tag.name===ARR );
  }
  print(">",  tag.name===ARR );
}

printer.on("text", ontext);
//printer.on("doctype", ontext);
function ontext (text) {
  print(text);
}

//returns {is_text:1/0} and modifies the elem['text']
function check_text_from_content ( ) {

  var ret = {is_text:false}, txt;

  if( elem['content'].length>0 && elem['content'][0].length ) {

    //just a heuristics for checking if the stuff inside is text or inner elements...
    for ( var i=0; i<elem['content'].length; ++i ) {

      if( !elem['content'][i].length ) continue;

      txt =  elem['content'][i].trim();
      //TODO - change here the whole text/hmtl logic
      //that is. for html tags within text -
      //get the original text using new readstream from the specific possition...
      if ( txt[0]!=='<' || txt[ txt.length-1  ]!=='>'  ){
        ret.is_text = true;
        break;
      }
    }
  }
  elem['text'] = elem['content'].join(' ');
  //just a temp heuristics for cleaning html
  elem['text'] = elem['text'].replace(/<br><\/br>/gi,'<br/>');
  elem['text'] = elem['text'].replace(/<\/br>/gi,'');

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
  bulk_add_to_mongoose(elem);
}

printer.on ( "closetag", closetag);
function closetag ( tag ) {

  this.level --;

  if ( tag===ARR ) {

    console.log('~~~close tag (startTag: %s), pos: %s', start_tag_pos, this._parser.position);

    bytes_read = fs.readSync(
      fd, readBuffer,
      bufferOffset,
      this._parser.position - start_tag_pos,
      start_tag_pos
    );

    console.log(
      readBuffer.slice(0,bytes_read).toString().replace(r_clean,'')
    );

    //debugger;
    fstr.pause();

    var is_text = check_text_from_content().is_text;

    if( !is_text ){

      //wrap this chap in some root stuff..., so that xml2js can handle it swell
      elem['text'] = '<'+ARR+'>' + elem['text'] +  '</'+ARR+'>';

      //parse the elem string gathered...
      xml2js_parser.parseString ( elem['text'], xml2js_parserString_cb );
    }
    else{
      bulk_add_to_mongoose(elem);
    }

    //DONE - here write with decent stream to mongo via mongoose
    elem = null;
  }

  print("</"+tag+">");
}

printer.on("cdata", function (data) {

  print("<![CDATA["+data+"]]>");
});

// printer.on("comment", function (comment) {

//   // this.indent();
//   // print("<!--"+comment+"-->");

// });

printer.on("error", function (error) {

  console.error(error);
  //throw error;
});


function print ( c, is_ignore_in_output ) {

  var len;
  if ( !is_ignore_in_output && elem!=null && elem['content'] && (len = elem['content'].length) ){

    elem['content'][len-1]+=c;
  }

  // if ( !process.stdout.write(c) ) {
  //   fstr.pause();
  // }
}

// process.stdout.on("drain", function () {
//   fstr.resume();
// });

var is_bulk_writing = false;
var drain_jar = [];
function bulk_add_to_mongoose ( row ) {

  if ( !is_bulk_writing ) {

    //empty drain jar
    drain_jar.forEach(function(r){
      bulk.insert(r);
    });
    drain_jar = [];

    bulk.insert( row );  // Bulk is okay if you don't need schema
    row = null;
  }
  else {
    drain_jar.push(row);
    row = null;
  }
  ++counter;

  if ( !( counter % BULK_SIZE) ) {

    //debugger;
    fstr.pause(); //lets stop reading from file until we finish writing
    is_bulk_writing = true;
    console.log('writing %d records to mongooooose...', counter);

    bulk.execute ( function(err,result) {

      if (err)
        throw err;   // or do something
      else
        console.log('ok, inserted bulk to mong... sucker');
      // possibly do something with result
      bulk = null;
      bulk = Entry.collection.initializeOrderedBulkOp();
      is_bulk_writing = false;
      fstr.resume();

    });
  } else {
    if( !is_bulk_writing )
      fstr.resume();
  }
}

//the readable filestream
var fstr = fs.createReadStream( xmlfile, { encoding: "utf8" } );
mongoose.connect('mongodb://adcore:adcore@ds057862.mongolab.com:57862/adcoretest');

var db = mongoose.connection;
db.on ('error', console.error.bind(console, 'connection error:') );
db.once ( 'open', function (callback) {

  // lower level method, needs connection!!! - use after connection created
  bulk = Entry.collection.initializeOrderedBulkOp();

  fs.existsSync( path_dump_obj ) && fs.unlinkSync(path_dump_obj);
  fstr.pipe(printer);  // yay! starts the piping/streaming...
});

fstr.once('end', fstr_end);

function fstr_end() {

  if ( counter % BULK_SIZE )
  {
    bulk.execute(function(err,result) {

      if (err) throw err;   // or something
      // maybe look at result
      bulk = null;
      do_final();
    });
  }
  else{
    do_final();
  }
};

function do_final ( ) {

  console.log ( 'written %d lines to db...', counter );
  // db.close( function(err){
  //   console.log ( 'written %d lines to db...', counter );
  //   process.exit(0);
  // });
  fstr.unpipe(printer);
  fstr.destroy();
  fstr = null;
  bulk = null;
  counter = 0;

  global && global.gc &&  global.gc();
}

//this final shit here is as readline... not to end the process, as in console debug
var stdin = process.openStdin();
stdin.on ( 'data', function(d){

  console.log ( 'you typed: %s, now doing all again... ', d );

  // lower level method, needs connection!!! - use after connection created
  bulk = Entry.collection.initializeOrderedBulkOp();

  fs.existsSync( path_dump_obj ) && fs.unlinkSync(path_dump_obj);

  fstr = fs.createReadStream( xmlfile, { encoding: "utf8" } );
  fstr.once('end', fstr_end);

  fstr.pipe(printer);  // yay! starts the piping/streaming...
});

