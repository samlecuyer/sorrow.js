/*
 copyright 2012 sam l'ecuyer
*/

var ok = require('assert').ok;
var test = require('test');

exports.runTests =  function() {
test.run({
    // verify that the basic test works
    testOk: function() {
        ok(true, 'should have been true');
    },
    notTest: function() {
        ok(false, 'should not have run');
    },
    TestCaps: function() {
        ok(true, 'should not have failed');
    }
});
};