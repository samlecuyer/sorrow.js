(function(internals) {
    var global = this;
    var makeGlobal = function(k, v) { global[k] = v; };
    makeGlobal('print', internals.print);
    
    var require = function(ident) {
        if (ident == 'internals') 
            return internals;
        var module = Module.getCached(ident);
        if (module) {
            return module.exports;
        }
        module = new Module(ident).load().cache();
        return module.exports;
    }
    
    // this is a super simple module system.
    // needs major work to be remotely ready for production
    function Module(id) {
        this.id = id;
        this.filename = id + '.js';
        this.exports = {};
        this.loaded = false;
    }
    
    Module._cache = {};
    
    Module.prototype.cache = function() {
        Module._cache[this.id] = this;
        return this;
    };
    
    Module.getCached = function(id) {
        return Module._cache[id];
    };
    
    Module.prototype.load = function() {
        if (!this.loaded) {
            var script = internals.read(this.filename);
            var fn = new Function(['exports', 'module', 'require'], script);
            fn.call(this, this.exports, {id: this.id}, require);
        }
        return this;
    };
    
    Module.prototype._loadNative = function(script) {
        if (!this.loaded) {
            var fn = new Function(['exports', 'module', 'require'], script);
            fn.call(this, this.exports, {id: this.id}, require);
        }
        return this;
    };
    
    for (id in internals.natives) {
        var lib = internals.natives[id];
        var module = Module.getCached(id);
        if (module) {
            return module.exports;
        }
        new Module(id)._loadNative(lib).cache();
    };
    
    makeGlobal('require', require);
    
});