
var big_feed = require('./big_feed.js');

var bf = new big_feed( {"xmlfile":"./1.xml", "filter": "row && row['g:custom_label_1'] && row['g:custom_label_1']=='Computers'" }  );
bf.pipe(process.stdout);
