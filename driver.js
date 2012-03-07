/*
 copyright 2012 sam l'ecuyer
*/

var assert = require('assert');
var cons = require('console');
var test = require('test');
var b    = require('binary');

assert.ok(cons.log, 'log exists');
assert.ok(test.run, 'run exists');

var results = test.run({
    // verify that the basic test works
    testOk: function() {
        assert.ok(true, 'should have been true');
    },
    notTest: function() {
        assert.ok(false, 'should not have run');
    },
    TestDog: function() {
        assert.ok(true, 'should not have failed');
    },
    
    // test binary asssumptions
    testBinaryThrowsError: function() {
        assert.throws(b.Binary, 'Binary should throw an exception');
    },
    testByteArrayIsBinary: function() {
        var ba = new b.ByteArray();
        assert.ok(ba instanceof b.ByteArray, 'should be instanceof ByteArray');
        assert.ok(ba instanceof b.Binary, 'should be instanceof Binary');
        assert.ok(ba instanceof Object, 'should be instanceof Object');
    },
    testByteArrayConstructors_empty: function() {
        var a = new b.ByteArray();
        assert.equal(a.length, 0, 'empty constructor should be length 0');
    },
    testByteArrayConstructors_length: function() {
        var a = new b.ByteArray(10);
        assert.equal(a.length, 10, 'should be length 10');
        for (var i = 0; i < 10; i++) {
            assert.equal(a[i], 0, 'all values should be 0');
        }
    },
    testByteArrayConstructors_copy: function() {
        var a = new b.ByteArray([1,2,3,4,5]);
        var c = new b.ByteArray(a);
        assert.equal(a.length, 5, 'should be length 10');
        for (var i = 0; i < 5; i++) {
            assert.equal(a[i], c[i], 'all values should be the same');
        }
    },
    testByteArrayConstructors_stringCharset: function() {
        assert.ok(new b.ByteArray('Καλημέρα κόσμε', 'us-ascii'), 'should support ascii');
        assert.ok(new b.ByteArray('こんにちは 世界', 'utf-8'), 'should support utf-8');
        assert.throws(function() {
            new b.ByteArray('Hello, world', 'ebcdic');
        }, 'should not support ebcdic');
    }
    
});

assert.equal(results, 0);

cons.log('DONE', 'info');