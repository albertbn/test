
var file = './dump_obj.txt';

var fs = require('fs-ext')
, util = require('util')
, stream = require('stream')
, es = require("event-stream");

var lineNr = 0, tests=0;

var _fd = fs.openSync(file,'r');

//fs.seekSync(_fd, 100, 0);

console.log(_fd);

//fs.closeSync(_fd);

var s;
function read(from_where){

  s = fs.createReadStream( null, {fd:_fd, start:from_where,  autoClose:false } )
        .pipe(es.split())
        .pipe(es.mapSync(function(line){

          s.autoDestroy = false;
          // pause the readstream
          s.pause();

          //debugger;

          ++lineNr;

          (function(){

            // process line here and call s.resume() when rdy
            console.log( line );
            // resume the readstream

            if(lineNr<4)
              s.resume();
            else if(tests++<10){
              lineNr=0;
              s.end();
              s.destroy();
              read(0);
            }
            else{
              s.resume();
            }

          })();
        })
              .on('error', function(){
                console.log('Error while reading file.');
                //fs.closeSync(_fd);
              })
              .on('end', function(){
                console.log('Read entirefile. Closing fd...');
                console.log(_fd);
                fs.closeSync(_fd);
                //read(from_where+20);
                //read(100);
              })
             );

}

read(0);

// setTimeout( function(){
//   debugger;
//   s.destroy();
//   s = null;

//   fs.seekSync(_fd, 0, 0);

//   read(100);
// }, 2000  );




