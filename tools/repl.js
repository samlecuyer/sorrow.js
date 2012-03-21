/*
 copyright 2012 sam l'ecuyer
 */

var stdin = require('system').stdin;
var stdout = require('system').stdout;
var stderr = require('system').stderr;

function _help() {
    stdout.writeLine('This is a very simple read-eval-print loop for sorow');
    stdout.writeLine('Built in macros:');
    stdout.writeLine('\t#help: show this help message');
    stdout.writeLine('\t#break: break out of the loop (exit)');
}

function _run() {
    stdout.writeLine('sorrow.js console');
    stdout.writeLine('`#help` for help');
    while (true) {
        try {
            stdout.writeLine('>> ', true);
            var line = stdin.readLine();
            if (line.trim() == '#help') {
                _help();
            } else if (line.trim() == '#break') {
                break;
            } else {
                var result = eval(line);
                stdout.writeLine(result);
            }
        } catch (e) {
            if (e == 'EOF') break;
            stderr.writeLine(e);
        }
    }
}
exports.run = _run;

if (module.id == require.main) {
    require('event').on('exit', function() {
        stdout.writeLine('goodbye.');
    });
	_run();
}
