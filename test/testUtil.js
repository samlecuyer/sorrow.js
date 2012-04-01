/*
 copyright 2012 sam l'ecuyer
*/

var ok = require('assert').ok;
var equal = require('assert').equal;

var test = require('test');
var util = require('util');


function assertingStream(stringToMatch) {
    return {
        writeLine: function(arg) {
            equal(stringToMatch, arg);
        }
    }
}

exports.runTests =  function() {
test.run({
    
    testSprintf_oneString: function() {
        var format = 'hello there';
        var stream = assertingStream(format);
        util.sprintf(stream, format);
    },
    
    testSprintf_formatStringNoArgs: function() {
        var format = 'hello there, %s';
        var stream = assertingStream(format);
        util.sprintf(stream, format);
    },
    
    testSprintf_formatStringRightNumArgs: function() {
        var format = 'hello there, %s';
        var stream = assertingStream('hello there, world');
        util.sprintf(stream, format, 'world');
    },

    testSprintf_formatStringTooManyArgs: function() {
        var format = 'hello there, %s';
        var stream = assertingStream('hello there, world holla');
        util.sprintf(stream, format, 'world', 'holla');
    },

    testPrintf_shouldPrintToStdout: function() {
        util.sprintf = function() {
            equal(2, arguments.length);
            equal(require('system').stdout, arguments[0]);
        };
        util.printf('123');
    },
    
});
};
