
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
    var s2 = p.load();
    
    system.stdout.writeLine('Enter a number: ');
    var n = system.stdin.readLine();
	system.stdout.writeLine(s2.getNum(n));
    
    p.close();
}