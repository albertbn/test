
//var xmlfile = './test.xml';
var xmlfile = './xml0.xml';
var strict = true;
var path_dump_obj = 'dump_obj.txt';
var ROOT = 'rows';
var ARR = 'row';

var sax = require("sax")
, printer = sax.createStream(strict, {lowercasetags:true, trim:true })
, fs = require("fs")
,xml2js = require("xml2js")
;

var fn_attrNameProcessor = function(name){

  return name.toLowerCase();
};

//this is used to parse each row/array element... into JSON
var xml2js_parser = new xml2js.Parser( { strict:false, normalizeTags:true, explicitArray:false, mergeAttrs:true, attrNameProcessors:[fn_attrNameProcessor] } );

function entity (str) {
  return str.replace('"', '&quot;');
}

printer.tabstop = 2;
printer.level = 0;
printer.indent = function () {
  print("\n", 'is_indent' );
  for (var i = this.level; i > 0; i --) {
    for (var j = this.tabstop; j > 0; j --) {
      print(" ", 'is_indent' );
    }
  }
};

//===========
// start events...
//===========
var elem = null; /*elem.content = [] will hold all text + nodes*/
printer.on("opentag", function (tag) {

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

  this.indent();
  this.level ++;
  print("<"+tag.name,  tag.name===ARR );
  for ( i in tag.attributes ) {
    print(" "+i+"=\""+entity(tag.attributes[i])+"\"",  tag.name===ARR );
  }
  print(">",  tag.name===ARR );
});

printer.on("text", ontext);
//printer.on("doctype", ontext);
function ontext (text) {

  this.indent();
  print(text);
}

printer.on ( "closetag", function (tag) {

  this.level --;

  if( tag===ARR ){
    var is_text = false, txt;
    if( elem['content'].length>1 || elem['content'][0].length ){

      //debugger;

      for(var i=0; i<elem['content'].length; ++i){

        if(!elem['content'][i].length) continue;

        txt =  elem['content'][i].trim();
        if ( txt[0]!=='<' || txt[ txt.length-1  ]!=='>'  ){
          is_text = true;
          break;
        }
      }

      elem['text'] = elem['content'].join(' ');
      elem['text'] = elem['text'].replace(/<br><\/br>/gi,'<br/>');
      elem['text'] = elem['text'].replace(/<\/br>/gi,'');

      if( is_text ){
        //perl
      }
      else{

        //wrap this chap in some root stuff..., so that xml2js can handle it swell
        elem['text'] = '<'+ARR+'>' + elem['text'] +  '</'+ARR+'>';

        fstr.pause();
        //TODO - go on from here...
        //parse the elem string gathered...
        xml2js_parser.parseString(elem['text'], function ( err, res ) {

          if (err) console.error(err.message);
          // obj[ARR].push(res[ARR.toLowerCase()]);

          for(var key in res[ARR] ){
            elem[key] = res[ARR][key];
          }

          delete elem['text'];

          fstr.resume();

        });
      }
    }

    delete elem['content'];

    //fs.appendFile( path_dump_obj, JSON.stringify(elem, null, 2) + '\n', function (err) {
    fs.appendFile( path_dump_obj, JSON.stringify(elem) + '\n', function (err) {

      if (err) console.error(err);
    });

    elem = null;
  }

  this.indent();
  print("</"+tag+">");

});

printer.on("cdata", function (data) {
  this.indent();
  print("<![CDATA["+data+"]]>");
});

printer.on("comment", function (comment) {

  // this.indent();
  // print("<!--"+comment+"-->");

});

printer.on("error", function (error) {

  console.error(error);
  //throw error;
});

var fstr = fs.createReadStream(xmlfile, { encoding: "utf8" });

function print ( c, is_ignore_in_output ) {

  var len;
  if ( !is_ignore_in_output && elem!=null && elem['content'] && (len = elem['content'].length) ){

    elem['content'][len-1]+=c;
  }

  if ( !process.stdout.write(c) ) {
    fstr.pause();
  }
}

process.stdout.on("drain", function () {
  fstr.resume();
});

fs.existsSync( path_dump_obj ) && fs.unlinkSync(path_dump_obj);

fstr.pipe(printer);
