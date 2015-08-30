
//30 Aug 2015
//INSPECT, STATS
//this folk should inspect an unknown file...
//it should return a struct object, based on stats...
//showing root, level2, count of repeating elements, etc...
//it should get input params, as how many 'elements' to process... 100, 1000, all
//so that the stats could be useful
//example structure of the return struct obj:
//{ root:'rows', level2:[row:{count:100,attr:[id,name,music],cdata:100 /*max_len*/ }, row2:{count:50,attr:[id,name2,music]}], level3:[row_text:100,row_id2:200]  }

//strict
var strict = false;

//requires
var fs = require("fs"),
    sax = require("sax");

//the writable sax memory stream
var printer = sax.createStream(strict,
                               {
                                 lowercasetags:true,
                                 trim:true,
                                 position:false /*no need to record pos for stats */
                               }
                              );
var xmlfile = './xml0.xml',
    path_dump_obj = 'dump_obj.txt';

//this cold naybe used further..., we'll see, yep! use now
printer.level = 0;

//===========
// start events...
//===========
//{ root:'rows', level2:{row:{count:100,attr:[id,name,music],cdata:100 /*max_len*/ },
//row2:{count:50,attr:[id,name2,music]} },
//level3:{row_text:100,row_id2:200}  }
var stats = { root:"" };
var elem = null; /*see if that will be used*/
printer.on ( "opentag", opentag);
function opentag (tag) {

  this.level ++;

  //count
  if(this.level===1){
    stats['root'] = tag.name;
  }
  //inner elements - level2,3, etc...
  else if( this.level>1 ){

    var levelX = stats['level'+this.level];
    !levelX && (levelX = stats['level'+this.level] = {} );
    //count
    !levelX[tag.name] && (levelX[tag.name]={count:0}); /*init count*/
    ++levelX[tag.name]['count']; /*increment it...*/

    //attr - gather attributes across rows - some rows might have Mir than other...
    for( var i in tag.attributes ) {
      !levelX[tag.name]['attr'] && (levelX[tag.name]['attr']=[]);
      levelX[tag.name]['attr'].indexOf(i)<0 && levelX[tag.name]['attr'].push(i);
    }

    //end else if
  }
}

printer.on("text", ontext);
//printer.on("doctype", ontext);
function ontext (text) {
  //print(text);
  //console.log(text);
}

printer.on ( "closetag", closetag);
function closetag ( tag ) {

  //console.log('closing tag ',tag);
  this.level --;
}

printer.on("cdata", function (data) {

  //console.log("<![CDATA["+data+"]]>");
});

printer.on("error", function (error) {

  console.error(error);
  //throw error;
  this._parser.error = null;
  this._parser.resume();
});


//the readable filestream
var fstr = fs.createReadStream( xmlfile, { encoding: "utf8" } );

var do_init = function(){

  fs.existsSync( path_dump_obj ) && fs.unlinkSync(path_dump_obj);
  fstr.pipe(printer);  // yay! starts the piping/streaming..

  fstr.once('end', fstr_end);
};

function fstr_end() {

  do_final();
};

function do_final ( ) {

  console.log("OK, let see what we've got... %s", JSON.stringify(stats,null,2) );
  fstr.unpipe(printer);
  fstr.destroy();
  fstr = null;

  global && global.gc &&  global.gc();
}

//this final shit here is as readline... not to end the process, as in console debug
var stdin = process.openStdin();
stdin.on ( 'data', function(d){

  console.log ( 'you typed: %s, now doing all again... ', d );
  do_init();
});

do_init();

