/*
 copyright 2012 sam l'ecuyer
*/

var assert = require('assert');
var cons = require('console');
var test = require('test');
var b    = require('binary');
var io   = require('io');
var fs   = require('fs');

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
        assert.ok(new b.ByteArray('Hello, world', 'us-ascii'), 'should support ascii');
        assert.ok(new b.ByteArray('こんにちは 世界', 'utf-8'), 'should support utf-8');
        assert.throws(function() {
            new b.ByteArray('Καλημέρα κόσμε', 'ebcdic');
        }, 'should not support ebcdic');
    },
    testByteArray_toByteArray: function() {
        var a = new b.ByteArray([1,2,3,4,5]);
        var c = a.toByteArray();
        assert.equal(a.length, 5, 'should be length 10');
        for (var i = 0; i < 5; i++) {
            assert.equal(a[i], c[i], 'all values should be the same');
        }
    },
    testByteArray_getCodeAt: function() {
        var a = new b.ByteArray([1,2,3,4,5]);
        assert.equal(a[3], a.get(3), 'accessors should bahave the same');
        assert.equal(a[3], a.codeAt(3), 'accessors should bahave the same');
    },
    testByteArray_copy: function() {
        var c = new b.ByteArray([1,2,3,4,5]);
        var d = new b.ByteArray(10);
        
        c.copy(d, 0, 4, 3);
        
        assert.equal(   0, d[0]);
        assert.equal(   0, d[1]);
        assert.equal(   0, d[2]);
        assert.equal(c[0], d[3]);
        assert.equal(c[1], d[4]);
        assert.equal(c[2], d[5]);
        assert.equal(c[3], d[6]);
        assert.equal(   0, d[7]);
        assert.equal(   0, d[8]);
        assert.equal(   0, d[9]);
    },
    testByteArray_fillNumber: function() {
        var d = new b.ByteArray(10);
        d.fill(7);
        for (var i = 0; i < 10; i++) {
            assert.equal(d[i], 7, 'all values should be the same');
        }
    },
    testByteArray_fillByteArray: function() {
        var a = new b.ByteArray(10);
        var d = new b.ByteArray([13]);
        a.fill(d);
        for (var i = 0; i < 10; i++) {
            assert.equal(a[i], 13, 'all values should be the same');
        }
    },
    testByteArray_toString: function() {
        var d = new b.ByteArray(10);
        assert.equal(d.toString(), '[ByteArray 10]');
    },
    
    testByteStringIsBinary: function() {
        var bs = new b.ByteString();
        assert.ok(bs instanceof b.ByteString, 'should be instanceof ByteString');
        assert.ok(bs instanceof b.Binary, 'should be instanceof Binary');
        assert.ok(bs instanceof Object, 'should be instanceof Object');
    },
    testByteStringConstructor_empty: function() {
        var bs = new b.ByteString();
        assert.equal(bs.length, 0, 'should be length 0');
    },
    testByteStringConstructor_index: function() {
        var bs = new b.ByteString([1, 2, 3]);
        var a = bs[1];
        assert.equal(a.length, 1, 'should be length 1');
        assert.equal(a.codeAt(0), 2, 'should be 2');
        assert.ok(a instanceof b.ByteString, 'should be instanceof ByteString');
        assert.ok(a instanceof b.Binary, 'should be instanceof Binary');
        assert.ok(a instanceof Object, 'should be instanceof Object');
    },
    
    testByteStringConstructor_toByteArray: function() {
        var bs = new b.ByteString([1, 2, 3]);
        var ba = new b.ByteArray(bs);
        assert.equal(ba.length, 3, 'should be length 3');
        assert.ok(ba instanceof b.ByteArray, 'should be instanceof ByteArray');
        assert.equal(1, ba[0]);
        assert.equal(2, ba[1]);
        assert.equal(3, ba[2]);
    },
    
    testByteStringConstructor_join: function() {
        var bs = b.ByteString.join([1,2,3], 0);
        assert.equal(bs.length, 5, 'should be length 5');
        assert.equal(1, bs.codeAt(0));
        assert.equal(0, bs.codeAt(1));
        assert.equal(2, bs.codeAt(2));
        assert.equal(0, bs.codeAt(3));
        assert.equal(3, bs.codeAt(4));
    },
    
    testRawStreamConstructor_read: function() {
        var stream = new io.Stream('test/driver.js', 'rb');
        // read 10 bytes
        var bs = stream.read(10);
        assert.equal(bs.length, 10);
        assert.ok(bs instanceof b.ByteString, 'should be instanceof ByteString');
        // read the whole file
        bs = stream.read();
        assert.ok(bs instanceof b.ByteString, 'should be instanceof ByteString');
        //this is reading at eof
        bs = stream.read(10);
        assert.equal(bs.length, 0);
        assert.ok(bs instanceof b.ByteString, 'should be instanceof ByteString');
    },
    
    testRawStreamConstructor_skip: function() {
        var stream = new io.Stream('test/driver.js', 'rb');
        assert.equal(0, stream.position, 'should be at start');
        stream.skip(10);
        assert.equal(10, stream.position, 'should have skipped 10');
    },
    
    testRawStreamConstructor_readWayTooMuch: function() {
        var stream = new io.Stream('test/driver.js', 'rb');
        var bs = stream.read(10000);
        assert.ok(bs.length < 10000, 'shouldnt be 10000 bytes long');
        assert.ok(bs instanceof b.ByteString, 'should be instanceof ByteString');
    },
    
    testByteStringConstructor_readDecode: function() {
        var stream = new io.Stream('test/driver.js', 'rb');
        var bs = stream.read(10);
        assert.ok(bs instanceof b.ByteString, 'should be instanceof ByteString');
        assert.equal(typeof bs.decodeToString(), 'string', 'should be a String');
    }, 
    
    testStdStreamsShouldBeTextStreams: function() {
        assert.ok(io.stdin instanceof io.TextStream);
        assert.ok(io.stdout instanceof io.TextStream);
    },
    
    testRawStreamConstructor_copyShouldFailOnReadTarget: function() {
        assert.throws(function() {
            var ins = new io.Stream('driver.js', 'rb');
            var outs = new io.Stream('driver_2.js', 'rb');
            ins.copy(outs);
        }, 'should have failed');
    },
   
    testTextStreamConstructor: function() {
        var raw = new io.Stream('test/driver.js', 'r');
        var text = new io.TextStream(raw, {});
        
        assert.equal(raw, text.raw, 'should be equal');
    }, 
    
    testFilesystem_copy: function() {
        var inf = fs.open('test/data/test_data1', 'r');
        var outf = fs.open('test/data/tmp_test_data2', 'w');
        inf.copy(outf);
    }, 
    
    testFilesystem_pwd: function() {
        assert.ok(fs.cwd, 'There should be a cwd');
    }, 
    
    testModule_require: function() {
        var yo = require('./a').yo('This shouldnt throw an exception');
        assert.equal(5, yo, 'Should be 5');
    }
});

assert.equal(results, 0, 'all tests should pass');

var cwd = fs.cwd;
print(cwd);
