
//focking amateurs writing async insert to db - fuck u

//strict
var strict = true
,BULK_SIZE = 1000
var counter = 0 /*counter for the bulk inset*/
;

//requires
var sax = require("sax")
//the writable sax memory stream
,printer = sax.createStream(strict, {lowercasetags:true, trim:true })
, fs = require("fs")
,xml2js = require("xml2js")
,async = require("async")
,mongoose = require("mongoose")
,bulk = null /*used for bulk inserts...*/
;

//var xmlfile = './xml0.xml',
var xmlfile = './cotd.xml',
    path_dump_obj = 'dump_obj.txt',
    Schema = mongoose.Schema,
    //HERE
    ROOT = 'rows',
    ARR = 'row'
    //TO HERE
;

//set streaming stuff
//var entrySchema = new Schema({ id:Number, name:String, music:String },{ strict: false });
var entrySchema = new Schema({ _id:{ type:Schema.Types.ObjectId, unique:true } },{ strict: false });
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
  bulk_add_to_mongoose(elem);
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
      bulk_add_to_mongoose(elem);
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

function bulk_add_to_mongoose ( row ) {

  async.series (
    [
      function ( callback ) {

        bulk.insert( row );  // Bulk is okay if you don't need schema
        //row = null;

        ++counter;

        if ( !( counter % BULK_SIZE) ) {

          fstr.pause(); //lets stop reading from file until we finish writing

          console.log('writing %d records to mongooooose...', counter);

          bulk.execute(function(err,result) {

            console.log ( 'result from bulk.exec is: %l', result  );

            if (err) throw err;   // or do something
            // possibly do something with result
            bulk = Entry.collection.initializeOrderedBulkOp();
            fstr.resume();
            callback();
          });
        } else {
          //if(counter<1000)fstr.resume();
          fstr.resume();
          callback();
        }
      }
      //log also to file if needed to test
      // ,function ( callback ) {

      //   //console.log('async first shet... write to file');
      //   fs.appendFile( path_dump_obj, JSON.stringify(row) + '\n', function (err) {
      //     row = null;
      //     if (err) console.error(err);
      //   });
      //   callback();
      // }
    ],
    function (err) {
      // each iteration is done
      //console.log('results are: ', results);
    }
  );
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

fstr.on("end",function() {

  if ( counter % BULK_SIZE )
  {
    // bulk.execute(function(err,result) {
    //   if (err) throw err;   // or something
    //   // maybe look at result
    //   do_final();
    // });
    do_final();
  }
  else{
    do_final();
  }
});

function do_final ( ) {

  // db.close( function(err){
  //   console.log ( 'written %d lines to db...', counter );
  //   process.exit(0);
  // });
}
