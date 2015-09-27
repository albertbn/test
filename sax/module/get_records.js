
var fs = require("fs");
var mongoose = require("mongoose");
// var xml2js = require('xml2js');

var js2xmlparser = require("js2xmlparser");

// var entry = JSON.parse(fs.readFileSync('./fuck.json'));
// var builder = new xml2js.Builder();
// var xml = builder.buildObject(entry);
// console.log(xml);

var options = {

  //declaration : {include:false}
  arrayMap: {
        "g:id": "id"
   }
};

mongoose.connect('mongodb://adcore:adcore@ds057862.mongolab.com:57862/adcoretest');

var db = mongoose.connection;
db.on ('error', console.error.bind(console, 'connection error:') );
db.once ( 'open', function (callback) {

  var collection = db.collection('gpf_takealot');

  collection.find( {"g:custom_label_1": "Computers"}  ).toArray(function(err, entries) {

    console.log(entries.length);

    // var builder = new xml2js.Builder({cdata:true, strict:false, rootName:'entry', xmldec:false});

    var xml, key1;
    for(var i=0; i<entries.length; ++i){

      entries[i] = JSON.parse(JSON.stringify(entries[i]));
      delete entries[i]['g:description'];
      // xml = builder.buildObject ( entries[i] );
      // fs.appendFileSync ( 'takealot_computers.xml', xml );
      for(var key in entries[i]){

        if ( !key || !entries[i][key] || !/^g:/i.test(key.toString()) ) continue;

        key1 = key.toString().replace(/^g:/i,'');
        entries[i][key1] = entries[i][key];

        delete  entries[i][key];
      }
    }

    // xml = builder.buildObject ( {"entry":entries} );
    xml = js2xmlparser ( "entries", { "entry":entries } );

    fs.writeFileSync ( 'takealot_computers.xml', xml );
    console.log('file saved.');
  });
});
