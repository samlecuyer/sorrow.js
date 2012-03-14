/*
 copyright 2012 sam l'ecuyer
*/

(function(internals) {
    var global = this;
    var makeGlobal = function(k, v) { global[k] = v; };
    makeGlobal('print', internals.print);
        
    function Lib(id) {
        this.id = id;
        this.filename = id + '.js';
        this.exports = {};
        this.loaded = false;
    }
    
    Lib._sources = internals.stdlib;
    Lib._cache = {};
    
    Lib.require = function(ident) {
        if (ident == '_impl') 
            return internals;
        if (ident == 'stdlib') 
            return Lib;
            
        var lib = Lib.getCached(ident);
        if (lib) {
            return lib.exports;
        }
        
        lib = new Lib(ident);
        lib.cache();
        lib.load();
        lib.cache();
        return lib.exports;
    }
    
    Lib.prototype.cache = function() {
        Lib._cache[this.id] = this;
    };
    
    Lib.getCached = function(id) {
        return Lib._cache[id];
    };
    
    Lib.exists = function(id) {
        return !!Lib._sources[id];
    };
    
    Lib.wrap = function(script) {
        return '(function(exports, module, require) {'+script +'\n});';
    };
    
    Lib.prototype.load = function() {
        var script = Lib._sources[this.id];
        var wrappedScript = Lib.wrap(script);
        var fn = internals.compile(wrappedScript, this.filename);
        fn.call(this, this.exports, {id: this.id}, Lib.require);
        this.loaded = true;
    };
    
    var Module = Lib.require('module').Module;
    var fs     = Lib.require('fs');
    
    var prog = internals.arg;
    Module.runProg(prog);
    
});
