
var Plugin = require('plugins').Plugin;
var system = require('system');
var fs   = require('fs');
var dir  = fs.directory(module.uri);

function resolve(path) {
    return fs.join(dir, path);
}

function getPlugin() {
    var path = resolve('seventy2.dylib');
    var plugin = new Plugin(path);
    return plugin;
}

if (module.id == require.main) {
    var p = getPlugin();
	system.stdout.writeLine(p.load().getNum());
    p.close();
}