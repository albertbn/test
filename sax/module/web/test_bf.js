
var big_feed = require('./big_feed.js');

var options = {

  "xmlfile":"./goo.xml",
  "entry" :"entry",
  "filter" : " row && row['custom_label_1'] && row['custom_label_1']['$text'] && row['custom_label_1']['$text']=='Computers' ",
  "mutate" : "row && row['description'] && (delete row['description'])",
  "bytes_cleanup": true
};

var bf = new big_feed( options);

bf.pipe(process.stdout);
