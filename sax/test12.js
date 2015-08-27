
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

function doit (){

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
    for(var i = Math.pow(2,20); i>=0; --i){ /*some focking CPU mania...*/  }
  }

  fs.closeSync(fd)
  console.log('fd closed...',fd);
};

console.log ( 'doing it...' );
doit();

var stdin = process.openStdin();

stdin.on ( 'data', function(d){

  console.log ( 'you typed: %s, now doing all again... ', d );
  doit();

});
