
var fs = require('fs');
var url = require('url');
var big_feed = require('./big_feed.js');

require('http').createServer(function(req, res) {

  // credits: http://stackoverflow.com/questions/8590042/parsing-query-string-in-node-js
  var qry = url.parse(req.url, true).query;

  if( !qry || !qry['entry'] ) return;

  console.log(qry);

  var options = {

    "xmlfile":"./goo.xml",
    "entry" : qry['entry'],
    "filter" : qry['filter'],
    "mutate" : qry['mutate'],
    "bytes_cleanup": JSON.parse(qry['bytes_cleanup'])/*TEMP*/
  };

  var bf = new big_feed( options);

  bf.pipe(res);

}).listen(8090);
