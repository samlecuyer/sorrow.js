/*
 copyright 2012 sam l'ecuyer
*/

var io = require('io');
var b  = require('binary');
var fs = internals.fs;



function open(path, mode) {
    var stream = new io.Stream(path, mode);
    if (mode.match(/b/)) return stream;
    return new io.TextStream(stream, {});
}

function workingDirectory() {
    return fs.cwd;
};

function changeWorkingDirectory(path) {
    fs.cwd = path;
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

/* A/0 methods */
exports.cwd = fs.cwd;
exports.move = fs.move;
exports.remove = fs.remove;
exports.touch = fs.touch;
exports.makeDirectory = fs.mkdir;
exports.removeDirectory = fs.rmdir;
exports.canonical = fs.canonical;
exports.owner = fs.owner;
exports.changeOwner = fs.chown;
exports.group = fs.group;
exports.changeGroup = fs.chgroup;
exports.permissions = fs.perm;
exports.changePermissions = fs.chperm;
exports.exists = fs.exists;
exports.isFile = fs.isfile;
exports.isDirectory = fs.isdir;
exports.isLink = fs.islink;
exports.isReadable = fs.isreadable;
exports.isWriteable = fs.iswriteable;
exports.same = fs.same;
exports.sameFilesystem = fs.samefs;
exports.size = fs.size;
exports.lastModified = fs.lastmod;
exports.list = fs.list;
exports.symbolicLink = fs.slink;
exports.hardLink = fs.hlink;
exports.readLink = fs.rlink;

exports.changeWorkingDirectory = changeWorkingDirectory;
exports.workingDirectory = workingDirectory;
exports.open = open;
exports.split = split;
exports.join  = join;
exports.absolute = absolute;
exports.directory = directory;
