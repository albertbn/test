
var xmlfile = './test.xml';
var strict = true;
var path_dump_obj = 'dump_obj.txt';
var ROOT = 'RootElement';
var ARR = 'SecondElement';

var sax = require("sax")
, printer = sax.createStream(strict, {lowercasetags:true, trim:true})
, fs = require("fs");

function entity (str) {
  return str.replace('"', '&quot;');
}

printer.tabstop = 2;
printer.level = 0;
printer.indent = function () {
  print("\n");
  for (var i = this.level; i > 0; i --) {
    for (var j = this.tabstop; j > 0; j --) {
      print(" ");
    }
  }
}

//===========
// start events...
//===========
var obj = {};
obj[ARR] = [];
var elem = null;
printer.on("opentag", function (tag) {

  //open/create first element
  if ( tag.name===ARR ){
    elem = {};

    for (var i in tag.attributes) {
      elem[i] = tag.attributes[i];
    }
  }

  this.indent();
  this.level ++;
  print("<"+tag.name);
  for (var i in tag.attributes) {
    print(" "+i+"=\""+entity(tag.attributes[i])+"\"");
  }
  print(">");
})

printer.on("text", ontext);
//printer.on("doctype", ontext);
function ontext (text) {
  this.indent();
  print(text);

  if(!elem) return;
  if(!elem['text']) elem['text'] = '';
  elem['text']+=text;
}

printer.on("closetag", function (tag) {

  this.level --;
  this.indent();
  print("</"+tag+">");

  if(tag===ARR){
    obj[ARR].push(elem);
    elem = null;
  }
  else if( tag===ROOT ){

    fs.appendFile( path_dump_obj, JSON.stringify(obj,null,2), function (err) {

      if (err) console.error(err);
    });
  }
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

function print (c) {

  if (!process.stdout.write(c)) {
    fstr.pause();
  }
}

process.stdout.on("drain", function () {
  fstr.resume();
});

fs.existsSync( path_dump_obj ) && fs.unlinkSync(path_dump_obj);

fstr.pipe(printer);
