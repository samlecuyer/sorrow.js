/*
 copyright 2012 sam l'ecuyer
 */

var stdin = require('system').stdin;
var stdout = require('system').stdout;
var stderr = require('system').stderr;

function run() {
    stdout.writeLine('sorrow.js console');
    while (true) {
        stdout.writeLine('>> ', true);
        var line = stdin.readLine();
        if (line.trim() == '#break') break;
        try {
            var result = eval(line);
            stdout.writeLine(result);
        } catch (e) {
            stderr.writeLine(e);
        }
    }
}
exports.run = run;

if (module.id == require.main) {
    require('event').on('exit', function() {
        stdout.writeLine('goodbye.');
    });
	run();
}