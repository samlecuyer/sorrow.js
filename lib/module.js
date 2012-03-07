function Module(id) {
    this.id = id;
    this.filename = null;
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

Module.load = function(id) {
    var modul = Module._cache[id];
    if (modul) {
        return modul.exports;
    }
    
    if (false) {
    
    }
}