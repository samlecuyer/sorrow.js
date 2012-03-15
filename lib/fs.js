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

function workingDirectory() {
    return require('_impl').cwd;
};

function changeWorkingDirectory(path) {
    require('_impl').cwd = path;
};

function split(path) {
    var parts = path.split('/');
    return parts;
}

function join() {
     var args = Array.prototype.slice.call(arguments);
     return args.join('/');
}

function normal(path) {
    var parts = split(path);
    var newParts = parts.filter(function(e) {
        return e !== '.';
    });
    return join.apply(this,newParts);
}

function absolute(path) {
    return normal(join(workingDirectory(), path));
}

function directory(path) {
    var parts = split(path);
    return join.apply(this, parts.slice(0, parts.length-1));
}

exports.changeWorkingDirectory = changeWorkingDirectory;
exports.workingDirectory = workingDirectory;
exports.cwd = require('_impl').cwd;
exports.open = open;
exports.split = split;
exports.join  = join;
exports.absolute = absolute;
exports.directory = directory;
