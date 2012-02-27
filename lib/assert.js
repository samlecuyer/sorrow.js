/*
 copyright 2012 sam l'ecuyer
*/

function AssertionError(err) {
  this.message  = err.message;
  this.actual   = err.actual;
  this.expected = err.expected;
};

AssertionError.prototype = new Error();
AssertionError.__proto__ = Error.prototype;
AssertionError.prototype.constructor = AssertionError;
AssertionError.prototype.toString = function() {
    return  'AssertionError: \n'+
            '\texpected: \t'+this.expected+' \n'+
            '\tactual: \t'+this.actual+' \n'+
            '\tmessage: \t'+this.message+' \n';
};

exports.AssertionError = AssertionError;

exports.fail = function(message, expected, actual) {
    throw new AssertionError({ message: message, actual: actual, expected: expected });
}

exports.ok = function(guard, message) {
    if (!!guard) return;
    exports.fail(message, true, !!guard);
}

exports.equal = function(actual, expected, message) {
    if (actual == expected) return;
    exports.fail(message, expected, actual);
}

exports.notEqual = function(actual, expected, message) {
    if (actual != expected) return;
    exports.fail(message, expected, actual);
}

exports.strictEqual = function(actual, expected, message) {
    if (actual === expected) return;
    exports.fail(message, expected, actual);
}

exports.strictNotEqual = function(actual, expected, message) {
    if (actual !== expected) return;
    exports.fail(message, expected, actual);
}

exports.throws = function(block, message) {
    var actual;
    try {
        block();
    } catch(e) {
        return;
    }
    exports.fail('', 'should have thrown an exception', 'didnt');
}
