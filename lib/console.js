/*
 copyright 2012 sam l'ecuyer
*/
var io = require('io');
exports.log = function() {
    io.stdout.writeLine(arguments[0]);
}
