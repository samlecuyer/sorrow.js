/*
 copyright 2012 sam l'ecuyer
*/

var assert = require('assert');
var cons = require('console');
var test = require('test');
var b    = require('binary');
var io   = require('io');
var fs   = require('fs');
var dir  = fs.directory(module.uri);

function resolve(path) {
    return fs.join(dir, path);
}

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
    TestCaps: function() {
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
    testByteArrayConstructors_copyFromArray: function() {
        var a = new b.ByteArray([1,2,3,4,5]);
        var c = new b.ByteArray(a);
        assert.equal(a.length, 5, 'should be length 5');
        for (var i = 0; i < 5; i++) {
            assert.equal(a[i], c[i], 'all values should be the same');
        }
    },
    testByteArrayConstructors_copyFromString: function() {
        var a = new b.ByteString([1,2,3,4,5]);
        var c = new b.ByteArray(a);
        assert.equal(a.length, 5, 'should be length 5');
        for (var i = 0; i < 5; i++) {
            assert.equal(a.codeAt(i), c[i], 'all values should be the same');
        }
    },
    testByteArrayConstructors_copyFromStringMustBeCopy: function() {
        var a = new b.ByteString([1,2,3,4,5]);
        var c = new b.ByteArray(a);
        assert.equal(a.length, 5, 'should be length 5');
        //change all the calues of the array and make sure that didn't change the BS
        for (var i = 0; i < 5; i++) { c[i] = 17; }
        for (var i = 0; i < 5; i++) {
            assert.notEqual(a.codeAt(i), c[i], 'all values should not be the same');
        }
    },
    testByteArrayConstructors_ArrayOfNonBytes: function() {
        assert.throws(function() {
            var a = new b.ByteString([1,2,-3,4,270]); // -3, 270 is not a byte
        }, 'should throw an expection when it hits -3 or 270');
    },
    testByteArrayConstructors_stringCharset: function() {
        assert.ok(new b.ByteArray('Hello, world', 'us-ascii'), 'should support ascii');
        assert.ok(new b.ByteArray('こんにちは 世界', 'utf-8'), 'should support utf-8');
        assert.throws(function() {
            new b.ByteArray('Καλημέρα κόσμε', 'ebcdic');
        }, 'TODO: implement transcoding!');
    },
    testByteArray_LengthMustBeMutable: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        a.length = 10;
        assert.equal(a.length, 10, 'length must be mutable');
        for (var i = 0; i < 5; i++) {
            assert.equal(a[i], data[i], 'all values should still be data');
        }
        for (var i = 5; i < 10; i++) {
            assert.equal(a[i], 0, 'all values should be 0');
        }
    },
    testByteArray_IndexMustBeMutable: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        assert.equal(a[1], 2, 'uhhh....');
        a[1] = 12;
        assert.equal(a[1], 12, 'uhhh....');
    },
    testByteArray_toArray: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        var t = a.toArray();
        assert.equal(t.length, 5);
    },
    testByteArray_toString: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        assert.equal(a.toString(), '[ByteArray 5]');
    },
    testByteArray_toSource: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        assert.equal(a.toSource(), 'ByteArray([1,2,3,4,5])');
    },
    testByteArray_toByteArray: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        var c = a.toByteArray();
        assert.ok(c instanceof b.ByteArray, 'must be ByteArray');
        assert.equal(a.length, c.length, 'length of copy should be the same');
    },
    testByteArray_toByteString: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        var c = a.toByteString();
        assert.ok(c instanceof b.ByteString, 'must be ByteString');
        assert.equal(a.length, c.length, 'length of copy should be the same');
    },
    testByteArray_byteAt: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        var c = a.byteAt(1);
        assert.ok(c instanceof b.ByteString, 'must be ByteString')
        assert.equal(a.length, c.length, 'length of copy should be the same');
    },
    testByteArray_valueAt: function() {
        var data = [1,2,3,4,5];
        var a = new b.ByteArray(data);
        assert.equal(a.byteAt(1), a.valueAt(1), 'valueAt should be the same as ByteAt');
    },
    
    testByteArray_getCodeAt: function() {
        var a = new b.ByteArray([1,2,3,4,5]);
        assert.equal(a[3], a.get(3), 'accessors should behave the same');
        assert.equal(a[3], a.codeAt(3), 'accessors should behave the same');
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
    testByteArray_concat: function() {
        var a = new b.ByteArray(10);
        var c = new b.ByteArray([13]);
        var d = a.concat(c);
        assert.ok(d instanceof b.ByteString, 'must be ByteString');
        assert.equal(d.length, 11);
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
        var stream = new io.Stream(resolve('driver.js'), 'rb');
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
        var stream = new io.Stream(resolve('driver.js'), 'rb');
        assert.equal(0, stream.position, 'should be at start');
        stream.skip(10);
        assert.equal(10, stream.position, 'should have skipped 10');
    },
    
    testRawStreamConstructor_readWayTooMuch: function() {
        var stream = new io.Stream(resolve('driver.js'), 'rb');
        var bs = stream.read(100000);
        assert.ok(bs.length < 100000, 'shouldnt be 100000 bytes long');
        assert.ok(bs instanceof b.ByteString, 'should be instanceof ByteString');
    },
    
    testByteStringConstructor_readDecode: function() {
        var stream = new io.Stream(resolve('driver.js'), 'rb');
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
            var ins = new io.Stream(resolve('driver.js'), 'rb');
            var outs = new io.Stream(resolve('driver_2.js'), 'rb');
            ins.copy(outs);
        }, 'should have failed');
    },
   
    testTextStreamConstructor: function() {
        var raw = new io.Stream(resolve('driver.js'), 'r');
        var text = new io.TextStream(raw, {});
        
        assert.equal(raw, text.raw, 'should be equal');
    }, 
    
    testFilesystem_copy: function() {
        var inf = fs.open(resolve('data/test_data1'), 'r');
        var outf = fs.open(resolve('data/tmp_test_data2'), 'w');
        inf.copy(outf);
    }, 
    
    testFilesystem_pwd: function() {
        assert.ok(fs.cwd, 'There should be a cwd');
    }, 
    
    testFilesystem_splitJoin: function() {
        var path = '/web/home/sam';
        var splitPath = fs.split(path);
        var joinPath  = fs.join.apply(this,splitPath);
        assert.equal(path, joinPath, 'the paths should be identical');
    },
    
    testFilesystem_normal1: function() {
        var path = '/web/home/sam';
        var splitPath = fs.split(path);
        var joinPath  = fs.join.apply(this,splitPath);
        assert.equal(path, joinPath, 'the paths should be identical');
    },
    
    testModule_require: function() {
        var yo = require('./a').yo('This shouldnt throw an exception');
        assert.equal(5, yo, 'Should be 5');
    }
});
