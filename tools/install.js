/*
 copyright 2012 sam l'ecuyer
*/

// a super dumb installer

var fs  = require('fs');
var sys = require('system');
var p   = require('util').printf;

function install() {
    if (!fs.isWriteable('/usr/bin')) {
        p('Cannot write to /usr/bin.  Are you sudo-ing? \n');
    } else {
        p('Installing sorrow.js \n');
        fs.touch('/usr/bin/sorrow');
        fs.copy('out/Release/sorrow', '/usr/bin/sorrow');
        fs.changePermissions('/usr/bin/sorrow', 0755);
        p('Finished installing sorrow.js \n');
    }
}

if (module.id == require.main) {
    install();
}