
var fs = require('fs');

fs.open('./fuck_u_ido.txt', 'r', function opened(err, fd) {

  if (err) { throw err; }

  var readBuffer = new Buffer(8),
      bufferOffset = 0,
      bufferLength = readBuffer.length, filePosition = null,
      write_buff = []
      ;

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

                 console.log(readBuffer.toString());
                 //write_buff = write_buff.concat(readBuffer);
                 fn_read();
               }
               else{
                 console.log( 'closing fd' );
                 fs.closeSync(fd);
               }
             }
           );
  };

  fn_read();

});












