
module.exports = inspect;

// =========

// @xmlfile - string path to file
// @callback(err,obj)
//   example obj is { root:"rows", arr:"row", level:2 }
// @options - additional options
function inspect ( xmlfile, callback, options ) {

  // strict
  var strict = false;

  // requires
  var fs = require("fs"),
      sax = require("sax");

  // the writable sax memory stream
  var printer = sax.createStream(strict,
                                 {
                                   trim:true,
                                   position:false /*no need to record pos for stats */
                                 }
                                );
  // USED - this cold naybe used further..., we'll see, yep! use now
  printer.level = 0;

  //===========
  // start events...
  //===========
  //actual structure is pasted at the end of file...
  var stats = { root:"" };
  var elem_open = []; /*see if that will be used*/
  printer.on ( "opentag", opentag);
  function opentag (tag) {

    this.level ++;

    //count
    if(this.level===1){
      stats['root'] = tag.name;
    }
    //inner elements - level2,3, etc...
    else if( this.level > 1 ){

      //count - also use the return value of increment - levelX further...
      var levelX = increment( 'count', tag.name );

      //attr - gather attributes across rows - some rows might have Mir than other...
      for ( var i in tag.attributes ) {
        !levelX[tag.name]['attr'] && (levelX[tag.name]['attr']=[]);
        levelX[tag.name]['attr'].indexOf(i)<0 && levelX[tag.name]['attr'].push(i);
      }

      //update the elem_open arr
      levelX[tag.name]['parent']=elem_open[elem_open.length-1];
      //end else if
    }

    (elem_open.indexOf(tag.name)<0) && elem_open.push(tag.name);
  }

  // @key could be [count,text, cdata, etc...]
  //if not present, creates @key, then increments by 1
  //in the stats['level'+this.level] obj
  //@tag is optional - if not supplied - get it from the elem_open arr...
  //@@returns levelX - levelX is also init-ed if needed
  function increment ( key, tag ) {

    var levelX = stats['level'+printer.level];
    !levelX && (levelX = stats['level'+printer.level] = {} );

    !tag && (tag = elem_open[elem_open.length-1]);

    !levelX[tag] && (levelX[tag]={});
    !levelX[tag][key] && (levelX[tag][key]=0); /*init @key*/
    ++levelX[tag][key]; /*then increment it...*/

    return levelX;
  }

  printer.on ( "text", ontext );
  //printer.on("doctype", ontext);
  function ontext (text) {
    //print(text);
    //console.log(text);
    increment('text');
  }

  printer.on("cdata", function (data) {

    //console.log("<![CDATA["+data+"]]>");
    increment('cdata');
  });

  printer.on ( "closetag", closetag);
  function closetag ( tag ) {

    //console.log('closing tag ',tag);
    this.level --;
    elem_open.pop();
  }

  printer.on("error", function (error) {

    console.error(error);
    //throw error;
  });

  var fstr;
  var do_init = function(){

    stats = { root:"" };
    elem_open = [];

    //the readable filestream
    fstr = fs.createReadStream( xmlfile, { encoding: "utf8" } );

    fstr.pipe(printer);  // yay! starts the piping/streaming..

    fstr.once('end', do_final );
  };

  function do_final ( ) {

    console.log ( "OK, let see what we've got... %j", stats );
    // console.log ( "OK, let see what we've got... %s", JSON.stringify(stats,null,2) );
    console.log ( "and root_n_arr is: %j", guess_root_n_array() );
    fstr.unpipe(printer);
    fstr.destroy();
    fstr = null;
  }

  // currently the first algo is - root==root
  // arr - the lowest possible level after root having the greatest count
  //also for now level2 and level3
  function guess_root_n_array ( ) {

    var ret = { root:null, arr:null };

    stats && ( ret['root']=stats['root'] );

    // returns true in case the level was found and level check found a count > 1
    // even if level check  is 1, the ret[''arr] will still be set
    var check_level = function(level){

      var count, arr_count = 0;
      if ( stats[level] ) {

        for ( var key in stats[level] ) {

          ((count=stats[level][key]['count']) > 0) && (arr_count<count) && (arr_count=count) && (ret['arr']=key);
        }
      }

      return arr_count;
    };

    // MAYBE there could be duplicate names across levels - that's why maybe use level, which will help when parsing the actual file
    var l = 2, c=0;
    while ( (c=check_level('level'+ l++))>0 && c<2  ) ;
    ret['level'] = l-1;

    return ret;
  }

  do_init();
}
