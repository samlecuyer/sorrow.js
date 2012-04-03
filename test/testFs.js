/*
 copyright 2012 sam l'ecuyer
*/

var ok = require('assert').ok;
var equal = require('assert').equal;

var test = require('test');
var fs   = require('fs');
var dir  = fs.directory(module.uri);

function resolve(path) {
    return fs.join(dir, path);
}

exports.runTests =  function() {
test.run({
    
    testFilesystem_copy: function() {
        var inf = fs.open(resolve('data/test_data1'), 'r');
        var outf = fs.open(resolve('data/tmp_test_data2'), 'w');
        inf.copy(outf);
    }, 
    
    testFilesystem_cwd: function() {
        ok(fs.workingDirectory(), 'There should be a cwd');
    }, 
    
    testFilesystem_splitJoin: function() {
        var path = '/web/home/sam';
        var splitPath = fs.split(path);
        var joinPath  = fs.join.apply(this,splitPath);
        equal(path, joinPath, 'the paths should be identical');
    },
    
    testFilesystem_normal1: function() {
        var path = '/web/home/sam';
        var splitPath = fs.split(path);
        var joinPath  = fs.join.apply(this,splitPath);
        equal(path, joinPath, 'the paths should be identical');
    },
    
    testFilesystem_base: function() {
        var path = '/web/home/sam.js';
        equal('sam.js', fs.base(path));
    },

    testFilesystem_base2: function() {
        var path = '/web/home/sam.js';
        equal('sam', fs.base(path, 'js'));
    },
    
    testFilesystem_base3: function() {
        var path = '/web/home/sam.js.ext';
        equal('sam.js.ext', fs.base(path, 'js'));
    },
    
    testFilesystem_extension: function() {
        var path = '/web/home/sam.js.ext';
        equal('ext', fs.extension(path));
    },
    
    testFilesystem_extension: function() {
        var path = '/usr/bin';
        equal(path, fs.canonical(path));
    },
    
    testFilesystem_extension2: function() {
        var path = '/web/home/sam';
        equal('', fs.extension(path));
    },
    
    testFilesystem_Path: function() {
        var path = new fs.Path('/web/home/sam');
        equal('/web/home/sam', path);
    },
    
    testFilesystem_Path2: function() {
        var path = new fs.Path('/web/home', 'sam');
        equal('/web/home/sam', path);
    },

    
    testModule_require: function() {
        var yo = require('./a').yo('This shouldnt throw an exception');
        equal(5, yo, 'Should be 5');
    }
});
};
