/*
 copyright 2012 sam l'ecuyer
*/

var ok = require('assert').ok;
var equal = require('assert').equal;
var notEqual = require('assert').notEqual;
var throws = require('assert').throws;

var test = require('test');
var b  = require('binary');

exports.runTests =  function() {
test.run({

    // test binary asssumptions
    testBinaryThrowsError: function() {
        throws(b.Binary, 'Binary should throw an exception');
    },
    
    testByteArrayIsBinary: function() {
        var ba = new b.ByteArray();
        ok(ba instanceof b.ByteArray, 'should be instanceof ByteArray');
        ok(ba instanceof b.Binary, 'should be instanceof Binary');
        ok(ba instanceof Object, 'should be instanceof Object');
    },
    testByteArrayConstructors_empty: function() {
        var a = new b.ByteArray();
        equal(a.length, 0, 'empty constructor should be length 0');
    },
    testByteArrayConstructors_length: function() {
        var a = new b.ByteArray(10);
        equal(a.length, 10, 'should be length 10');
        for (var i = 0; i < 10; i++) {
            equal(a[i], 0, 'all values should be 0');
        }
        
    },
    testByteArrayConstructors_copyFromArray: function() {
        var a = new b.ByteArray([1,2,3,4,5]);
        var c = new b.ByteArray(a);
        equal(a.length, 5, 'should be length 5');
        for (var i = 0; i < 5; i++) {
            equal(a[i], c[i], 'all values should be the same');
        }
    },
    testByteArrayConstructors_copyFromString: function() {
        var a = new b.ByteString([1,2,3,4,5]);
        var c = new b.ByteArray(a);
        equal(a.length, 5, 'should be length 5');
        for (var i = 0; i < 5; i++) {
            equal(a.codeAt(i), c[i], 'all values should be the same');
        }
    },
    testByteArrayConstructors_copyFromStringMustBeCopy: function() {
        var a = new b.ByteString([1,2,3,4,5]);
        var c = new b.ByteArray(a);
        equal(a.length, 5, 'should be length 5');
        //change all the calues of the array and make sure that didn't change the BS
        for (var i = 0; i < 5; i++) { c[i] = 17; }
        for (var i = 0; i < 5; i++) {
            notEqual(a.codeAt(i), c[i], 'all values should not be the same');
        }
    },
    testByteArrayConstructors_ArrayOfNonBytes: function() {
        throws(function() {
            var a = new b.ByteString([1,2,-3,4,270]); // -3, 270 is not a byte
        }, 'should throw an expection when it hits -3 or 270');
    },
    testByteArrayConstructors_stringCharset: function() {
        ok(new b.ByteArray('Hello, world', 'us-ascii'), 'should support ascii');
        ok(new b.ByteArray('こんにちは 世界', 'utf-8'), 'should support utf-8');
        throws(function() {
            new b.ByteArray('Καλημέρα κόσμε', 'EBCDIC'); // not supported by iconv
        }, 'EBCDIC is not a supported encoding and this should have failed');
    },
    testByteArray_LengthMustBeMutable: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        a.length = 10;
        equal(a.length, 10, 'length must be mutable');
        for (var i = 0; i < 5; i++) {
            equal(a[i], data[i], 'all values should still be data');
        }
        for (var i = 5; i < 10; i++) {
            equal(a[i], 0, 'all values should be 0');
        }
    },
    testByteArray_IndexMustBeMutable: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        equal(a[1], 2, 'uhhh....');
        a[1] = 12;
        equal(a[1], 12, 'uhhh....');
    },
    testByteArray_toArray: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        var t = a.toArray();
        equal(t.length, 5);
    },
    testByteArray_toString: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        equal(a.toString(), '[ByteArray 5]');
    },
    testByteArray_toSource: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        equal(a.toSource(), 'ByteArray([1,2,3,4,5])');
    },
    testByteArray_toByteArray: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        var c = a.toByteArray();
        ok(c instanceof b.ByteArray, 'must be ByteArray');
        equal(a.length, c.length, 'length of copy should be the same');
    },
    testByteArray_toByteArrayCharset: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        var c = a.toByteArray('ascii', 'utf-8');
        ok(c instanceof b.ByteArray, 'must be ByteArray');
        equal(a.length, c.length, 'length of copy should be the same');
    },
    testByteArray_toByteString: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        var c = a.toByteString();
        ok(c instanceof b.ByteString, 'must be ByteString');
        equal(a.length, c.length, 'length of copy should be the same');
    },
    testByteArray_byteAt: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        var c = a.byteAt(1);
        ok(c instanceof b.ByteString, 'must be ByteString')
        equal(1, c.length, 'length of copy should be the same');
    },
    testByteArray_valueAt: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        var d = a.byteAt(1);
        var e = a.valueAt(1);
        
        equal(d.length, e.length, 'both should return a single byte');
        equal(d.codeAt(0), e.codeAt(0), 'both should return a single byte');
    },
    
    testByteArray_getCodeAt: function() {
        var a = new b.ByteArray([1,2,3,4,5]);
        equal(a[3], a.get(3), 'accessors should behave the same');
        equal(a[3], a.codeAt(3), 'accessors should behave the same');
    },
    testByteArray_copy: function() {
        var c = new b.ByteArray([1,2,3,4,5]);
        var d = new b.ByteArray(10);
        
        c.copy(d, 0, 4, 3);
        
        equal(   0, d[0]);
        equal(   0, d[1]);
        equal(   0, d[2]);
        equal(c[0], d[3]);
        equal(c[1], d[4]);
        equal(c[2], d[5]);
        equal(c[3], d[6]);
        equal(   0, d[7]);
        equal(   0, d[8]);
        equal(   0, d[9]);
    },
    testByteArray_fillNumber: function() {
        var d = new b.ByteArray(10);
        d.fill(7);
        for (var i = 0; i < 10; i++) {
            equal(d[i], 7, 'all values should be the same');
        }
    },
    testByteArray_fillByteArray: function() {
        var a = new b.ByteArray(10);
        var d = new b.ByteArray([13]);
        a.fill(d);
        for (var i = 0; i < 10; i++) {
            equal(a[i], 13, 'all values should be the same');
        }
    },
    testByteArray_concatByteArray: function() {
        var a = new b.ByteArray(10);
        var c = new b.ByteArray([13]);
        var d = a.concat(c);
        ok(d instanceof b.ByteArray, 'must be ByteArray');
        equal(d.length, 11);
        equal(d[10], 13, 'must have put 13 on the end');
    },
    testByteArray_concatArray: function() {
        var a = new b.ByteArray(10);
        var d = a.concat([13]);
        ok(d instanceof b.ByteArray, 'must be ByteArray');
        equal(d.length, 11);
        equal(d[10], 13, 'must have put 13 on the end');
    },
    
    testByteStringIsBinary: function() {
        var bs = new b.ByteString();
        ok(bs instanceof b.ByteString, 'should be instanceof ByteString');
        ok(bs instanceof b.Binary, 'should be instanceof Binary');
        ok(bs instanceof Object, 'should be instanceof Object');
    },
    testByteStringConstructor_empty: function() {
        var bs = new b.ByteString();
        equal(bs.length, 0, 'should be length 0');
    },
    testByteStringConstructor_index: function() {
        var bs = new b.ByteString([1, 2, 3]);
        var a = bs[1];
        equal(a.length, 1, 'should be length 1');
        equal(a.codeAt(0), 2, 'should be 2');
        ok(a instanceof b.ByteString, 'should be instanceof ByteString');
        ok(a instanceof b.Binary, 'should be instanceof Binary');
        ok(a instanceof Object, 'should be instanceof Object');
    },
    
    testByteStringConstructor_toByteArray: function() {
        var bs = new b.ByteString([1, 2, 3]);
        var ba = new b.ByteArray(bs);
        equal(ba.length, 3, 'should be length 3');
        ok(ba instanceof b.ByteArray, 'should be instanceof ByteArray');
        equal(1, ba[0]);
        equal(2, ba[1]);
        equal(3, ba[2]);
    },
    
    testByteStringConstructor_join: function() {
        var bs = b.ByteString.join([1,2,3], 0);
        equal(bs.length, 5, 'should be length 5');
        equal(1, bs.codeAt(0));
        equal(0, bs.codeAt(1));
        equal(2, bs.codeAt(2));
        equal(0, bs.codeAt(3));
        equal(3, bs.codeAt(4));
    },
});
};
