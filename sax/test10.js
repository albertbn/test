
var fs = require('fs');
var fflushed = 0;
//========
var bytes_read;
//========

var readBuffer = new Buffer( Math.pow(2,16) ),
    bufferOffset = 0,
    bufferLength = readBuffer.length, filePosition = null
;

var fd = fs.openSync( './cotd.xml', 'r');

function drained(){
  fflushed = 1;
  console.log('drained...', readBuffer.length);

  if(1==1){
    console.log('reading more in drain...');
    bytes_read = fs.readSync(
      fd, readBuffer,
      bufferOffset,
      bufferLength,
      filePosition
    );

    console.log(bytes_read);

    //console.log('%d\t%d',++count,bytes_read);
    bytes_read && process.stdout.write( readBuffer );
  }
}

function flushed(init){
  fflushed = 1;
  console.log('flushed...');

  if(init || bytes_read){
    bytes_read = fs.readSync(
      fd, readBuffer,
      bufferOffset,
      bufferLength,
      filePosition
    );

    //console.log('%d\t%d',++count,bytes_read);
    fflushed = process.stdout.write( readBuffer, null, flushed );
  }
}
process.stdout.on('drain',drained);

flushed('start it');

//while( !fflushed ){}













