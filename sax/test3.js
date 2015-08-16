
var fs = require('fs');
var readStream = fs.createReadStream('test.xml');
readStream.pipe(process.stdout);
