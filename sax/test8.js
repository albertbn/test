
//focking amateurs writing async insert to db - fuck u

//strict
var strict = true
,BULK_SIZE = 1000
,counter = 0 /*counter for the bulk inset*/
;

//requires
var sax = require("sax");
//the writable sax memory stream
var printer = sax.createStream(strict, {lowercasetags:true, trim:true });
var fs = require("fs");
var xml2js = require("xml2js");
//var async = require("async");
//var mongoose = require("mongoose");
var bulk = null /*used for bulk inserts...*/;

//var xmlfile = './xml0.xml',
var xmlfile = './cotd.xml',
    path_dump_obj = 'dump_obj.txt',
    //HERE
    ROOT = 'rows',
    ARR = 'row'
    //TO HERE
;
//var Schema = mongoose.Schema;

//set streaming stuff
//var entrySchema = new Schema({ id:Number, name:String, music:String },{ strict: false });
// var entrySchema = new Schema({ },{ strict: false });
// var Entry = mongoose.model( "row", entrySchema );

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

printer.on ( "opentag", opentag);
function opentag (tag) {

  var i;
  //open/create first element
  if ( tag.name===ARR ){

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
  bulk_write(elem);
}

printer.on ( "closetag", closetag);
function closetag ( tag ) {

  this.level --;

  if ( tag===ARR ) {

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
      bulk_write(elem);
    }

    //TODO - here write with decent stream to mongo via mongoose
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

function bulk_write ( row ) {

  //console.info(row);
  // fstr.pause();
  // fs.appendFile( path_dump_obj, JSON.stringify(row) + '\n', function (err) {

  //   if (err) console.error(err);
  //   fstr.resume();
  // });
  console.log("count is: %d", counter++);
  row = null;

  fstr.resume();
}

//the readable filestream
var fstr = fs.createReadStream( xmlfile, { encoding: "utf8", highWaterMark:65535 } );

debugger;

fstr.once('error', function(err){
  console.log ( 'error in read str: ', err );
});

fstr.once('open', function(fd){

  console.log('started reading big xml... with fd: ',fd);

  fs.existsSync( path_dump_obj ) && fs.unlinkSync(path_dump_obj);
  //fstr.pipe(printer);  // yay! starts the piping/streaming..
  //fstr.pipe(process.stdout);  // yay! starts the piping/streaming..
});


fstr.on('data',ondata);

function ondata(data){

  //cleanup();

  fstr.pause();

  //console.log('on data of fstr');

  setTimeout( function() {

    //fstr.once('data',ondata);
    bulk_write(data);
    data = null;

    // if ( !process.stdout.write(data) ) {
    // }

    // if ( !printer.write(data) ) {
    // }
  },
  100  );

  // fstr.once('data',ondata);
  // bulk_write(data);
  // data = null;

};

function cleanup() {
  // remove all event listeners created in this promise
  fstr.removeListener('data', ondata)
}

// process.stdout.on("drain", function () {
//   fstr.resume();
// });

fstr.once("end",function() {

  console.log('end of fstr read...');
  debugger;
  //do_final();
});

function do_final ( ) {

  process.exit(0);
}
