
var strict = false;

var fs = require("fs");
var xml2js = require("xml2js");

// var xml = "<root>";
//    xml += "<entry><id>111</id><name>xxx</name></entry>";
//    xml += "<entry><id>222</id><name>yyy & b</name></ent>";
// xml += "</root>";

var xml = fs.readFileSync('./cotd.xml');

var fn_attrNameProcessor = function(name){
  return name.toLowerCase();
};

//this is used to parse each row/array element... into JSON
var xml2js_parser = new xml2js.Parser({
  strict:strict, normalizeTags:true, explicitArray:false, mergeAttrs:true,
  attrNameProcessors:[fn_attrNameProcessor]
});

for(var i=0; i< 12; ++i ){

  //parse the elem string gathered...
  xml2js_parser.parseString ( xml, xml2js_parserString_cb );

  //xml2js callback - !resumes readstream at the end
  function xml2js_parserString_cb ( err, res ) {

    // console.log( JSON.stringify(res) );
    console.log( res['rows']['row'].length  );
  }
}
