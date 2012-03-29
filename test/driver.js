/*
 copyright 2012 sam l'ecuyer
*/

var cons = require('console');
var test = require('test');
var assert = require('assert');

assert.ok(cons.log, 'log exists');
assert.ok(test.run, 'run exists');

require('testBytes').runTests();
require('testTest').runTests();
require('testFs').runTests();