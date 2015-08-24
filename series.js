
var async = require('async'),
    request = require('request');

function done(err, results) {
  if (err) {
    throw err;
  }
  console.log('results: %j', results);
}

async.series([

  function( next ) {
    request.post( {uri: 'http://localhost:4001', body: '4'},

                 function(err, res, body) {
                   next( err, body );
                 });
  },

  function( next ) {

    request.post({uri: 'http://localhost:4001', body: '5'},

                 function(err, res, body) {
                   next( err, body );
                 });
  }
], done);
