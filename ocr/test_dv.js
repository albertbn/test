
var dv = require('dv');
var fs = require('fs');
// var image = new dv.Image('jpg', fs.readFileSync('./trader-joe.jpg'));
var image = new dv.Image('jpg', fs.readFileSync('./trader-joe-orig.jpg'));
var tesseract = new dv.Tesseract('eng', image);

console.log(tesseract.findText('plain'));



















