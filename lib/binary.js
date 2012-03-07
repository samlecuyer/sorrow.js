var Binary    = require('_impl').Binary;
var ByteArray = require('_impl').ByteArray;

ByteArray.prototype.toByteArray = function() {
    return new ByteArray(this);
};

ByteArray.prototype.get = function(i) {
    return this[i];
};

ByteArray.prototype.codeAt = function(i) {
    return this[i];
};

ByteArray.prototype.copy = function(target, start, stop, targetStart) {
    if(!start) start = 0;
    if(!stop)  stop = this.length;
    if(!targetStart) targetStart = 0;
    
    for (var i = start; i < stop; i++) {
        target[i+targetStart] = this[i];
    }
};

ByteArray.prototype.toString = function() {
    return '[ByteArray '+this.length+']';
};


exports.Binary    = Binary;
exports.ByteArray = ByteArray;
