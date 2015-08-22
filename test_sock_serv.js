
require('net').createServer(function(socket) {

  // socket.setTimeout(20000, function() {
  //   socket.end('idle timeout, disconnecting, bye!');
  // });

  socket.on('data', function(data){
    //console.log(data); /*this prints the buff bytes*/

    if ( /^(quit|bye|close|end)$/i.test(data.toString().trim()) ) {

      socket.write('bye bye! and go have sex...\n');
      socket.end();
    }
    else
    {
      console.log( data.toString().toUpperCase() );
      try{
        console.log( eval(data.toString()) );
        socket.write( (eval(data.toString())).toString() + '\n' );
      }
      catch( e ) {
        //safe swallow
        socket.write( data.toString().toUpperCase() );
      }
    }
  });

  var rs = require('fs').createReadStream('dump.txt');
  //rs.pipe(socket);
  rs.pipe(socket, { end : false });

}).listen(4001);
