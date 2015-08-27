
var fs = require('fs');
var path_dump_obj = './dump.log';
var count = 0;
//========
var bytes_read = 0;
//========

var readBuffer = new Buffer( Math.pow(2,16) ),
    bufferOffset = 0,
    bufferLength = readBuffer.length,
    filePosition = null
;

var fd = fs.openSync( './cotd.xml', 'r');

fs.existsSync( path_dump_obj ) && fs.unlinkSync(path_dump_obj);

while((bytes_read = fs.readSync(
  fd, readBuffer,
  bufferOffset,
  bufferLength,
  filePosition
  ))
)
{
  //console.log(++count, bytes_read);
  fs.appendFileSync ( path_dump_obj, readBuffer.slice(0, bytes_read) );
  //console.log ( readBuffer.slice(0, bytes_read).toString() );
}

fs.closeSync(fd);

//console.log('fd closed...',fd);

// var stdin = process.openStdin();

// stdin.on ( 'data', function(d){
//   console.log ( 'you typed: ', d );
// });
