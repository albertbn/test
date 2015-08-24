
var a = 0;
function init() {
  a = 1;
}
function incr() {
  ++a; /*yep! node inspec + code editing! - in .node-inspectorrc put { "save-live-edit": true } */
}

init();
console.log('a before: %d', a);

incr();
console.log('a after: %d', a);
