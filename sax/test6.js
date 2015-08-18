
var file = './dump_obj.txt';

var fs = require('fs')
, util = require('util')
, stream = require('stream')
, es = require("event-stream");

var lineNr = 0;

var _fd = fs.openSync(file,'r');

//console.log(_fd);

//fs.closeSync(_fd);

var s;
function read(from_where){

  s = fs.createReadStream( null, {fd:_fd, start:from_where} )
        .pipe(es.split())
        .pipe(es.mapSync(function(line){

          // pause the readstream
          s.pause();

          debugger;
          
          ++lineNr;

          (function(){

            // process line here and call s.resume() when rdy
            console.log( line );
            // resume the readstream
            s.resume();
            //s.end();

          })();
        })
              .on('error', function(){
                console.log('Error while reading file.');
                //fs.closeSync(_fd);
              })
              .on('end', function(){
                console.log('Read entirefile.');
                //fs.closeSync(_fd);
                //read(from_where+20);
                //read(100);
              })
             );

}

read(0);

setTimeout( function(){
  debugger;
  s.destroy();
  s = null;
  read(100);
}, 10000  );




