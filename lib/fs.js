/*
 copyright 2012 sam l'ecuyer
*/

var io = require('io');
var b  = require('binary');

function open(path, mode) {
    var stream = new io.Stream(path, mode);
    if (mode.match(/b/)) return stream;
    return new io.TextStream(stream, {});
}

exports.open = open;
