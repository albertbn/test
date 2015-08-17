
var xmlfile = './test.xml';
var strict = true;
var path_dump_obj = 'dump_obj.txt';
var ROOT = 'RootElement';
var ARR = 'SecondElement';

var sax = require("sax")
, printer = sax.createStream(strict, { lowercasetags:true, trim:true  })
, fs = require("fs")
, xml2js = require("xml2js")
;

//this is used to parse each row/array element... into JSON
var xml2js_parser = new xml2js.Parser( { strict:false, normalizeTags: true } );

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
};

//===========
// start events...
//===========
var obj = {};
obj[ARR] = [];
var elem = null;

printer.on("opentag", function (tag) {

  //open/create first element
  if ( tag.name===ARR ){
    elem = '';
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
}

printer.on("closetag", function (tag) {

  this.level --;
  this.indent();
  print("</"+tag+">");

  if ( tag===ARR ) {

    console.error('about to parse %s', elem);

    //parse the elem string gathered...
    xml2js_parser.parseString(elem, function (err, res) {
      if (err) console.error(err.message);
      obj[ARR].push(res[ARR.toLowerCase()]);
    });

    //this here is critical, so that the concats gathers only the wanted element...
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

  //console.error('elem is: %s', elem);

  //concat elem as string so that it could be parsed by xml2js
  ( elem!=null )  && ( elem+= c );

  if (!process.stdout.write(c)) {
    fstr.pause();
  }
}

process.stdout.on("drain", function () {
  fstr.resume();
});

fs.existsSync( path_dump_obj ) && fs.unlinkSync(path_dump_obj);

fstr.pipe(printer);
