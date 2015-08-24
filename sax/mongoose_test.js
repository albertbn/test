
var LineInputStream = require("line-input-stream"),
    fs = require("fs"),
    async = require("async"),
    mongoose = require("mongoose"),
    Schema = mongoose.Schema;

mongoose.connect('mongodb://adcore:adcore@ds057862.mongolab.com:57862/adcoretest');

//set streaming stuff
var entrySchema = new Schema({ id:Number, name:String, music:String },{ strict: false })
var Entry = mongoose.model( "row", entrySchema );

var db = mongoose.connection;

db.on('error', console.error.bind(console, 'connection error:'));
db.on('open',function(err,conn) {

  //debugger;

  var stream = LineInputStream( fs.createReadStream("./dump_obj.txt",{ flags: "r" } ) );
  stream.setDelimiter( '\n' );

  // lower level method, needs connection
  var bulk = Entry.collection.initializeOrderedBulkOp();
  var counter = 0;

  stream.on("error",function(err) {
    console.log(err); // or otherwise deal with it
  });

  stream.on("line",function(line) {

    //debugger;
    console.log('line is: ', line);

    async.series(
      [
        function(callback) {

          //debugger;
          //var row = line.split(",");     // split the lines on delimiter
          //var obj = new Entry( JSON.parse(line) );
          // other manipulation

          bulk.insert( JSON.parse(line) );  // Bulk is okay if you don't need schema
          // defaults. Or can just set them.

          ++counter;

          if ( counter % 1000 == 0 ) {

            stream.pause(); //lets stop reading from file until we finish writing

            bulk.execute(function(err,result) {

              if (err) throw err;   // or do something
              // possibly do something with result
              bulk = Entry.collection.initializeOrderedBulkOp();
              stream.resume();
              callback();
            });
          } else {
            callback();
          }
        }
      ],
      function (err) {
        // each iteration is done
      }
    );

  });

  stream.on("end",function() {

    if ( counter % 1000 != 0 )
    {
      bulk.execute(function(err,result) {
        if (err) throw err;   // or something
        // maybe look at result
        do_final();
      });
    }
    else{
      do_final();
    }
  });

  var do_final = function(){

    db.close( function(err){
      process.exit(0);
    });
  }

});
