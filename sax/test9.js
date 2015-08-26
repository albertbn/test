
var fs = require('fs');

fs.open('./cotd.xml', 'r', function opened(err, fd) {

  if (err) { throw err; }

  var readBuffer = new Buffer( Math.pow(2,16) ),
      bufferOffset = 0,
      bufferLength = readBuffer.length, filePosition = null
      ;

  var count = 0;
  var fn_read = function()
  {
    fs.read( fd, readBuffer,
             bufferOffset,
             bufferLength,
             filePosition,

             function read( err, readBytes ) {

               if ( err ) { throw err; }

               //console.log('just read ' + readBytes + ' bytes');

               if (readBytes > 0) {

                 //console.log(readBuffer.slice(0, readBytes));
                 var is_drained = false;
                 //process.stdout.write(++count + '\t' + readBytes.toString() + '\n');
                 //console.log(readBuffer.toString());
                 process.stdout.on ( 'drain', function(){
                   is_drained = true;

                   fs.read( fd, readBuffer,
                            bufferOffset,
                            bufferLength,
                            filePosition,
                            function(){}
                          );
                 } );

                 process.stdout.write(readBuffer);

                 //while(!is_drained){} /*wait for drain to finish*/
                 //console.log('ok, going on with fn-read...');
                 is_drained &&
                   
                   fs.read( fd, readBuffer,
                            bufferOffset,
                            bufferLength,
                            filePosition,
                            function(){}
                          );
               }
               else{
                 // //process.stdout.write(readBuffer);
                 // process.stdout.once ( 'drain', function ( ) {
                 //   //console.log('closing file after last drain');
                 //   fs.closeSync(fd);
                 // });
                 process.stdout.write('\nclosing fd ...no more data\n');
               }
             }
           );
  };

  fn_read();

});












