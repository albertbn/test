
var collection = [1, 2, 3, 4, 1, 1,3,3,3,3,3,3,3,3,2,2,2];

function iterator(sum, elem) {
  return sum + elem;
}

var sum = collection.reduce(iterator, 0);
console.log( 'sum is %d', sum );

function it ( countt, elem ){

  countt[elem] && (++countt[elem]) || (countt[elem]=1);
  return countt;
}

var res = collection.reduce(it,{});

console.log('res is: %j', res);

//=============

function isEven(value) {
  return !(value & 1); /*focking amateurs... Stalone*/
}
var evenElements = collection.filter(isEven);
console.log('even elements of %j are: %j', collection, evenElements);
