/*
 copyright 2012 sam l'ecuyer
*/

// a super dumb installer

var fs  = require('fs');
var sys = require('system');
var p   = require('util').printf;

function install(source, target) {
    var targetDir = fs.directory(target)
    if (!fs.isWriteable(targetDir)) {
        p('Cannot write to '+targetDir+'.  Are you sudo-ing? \n');
    } else {
        p('Installing sorrow.js \n');
        if (fs.exists(target))
            fs.remove(target);
        fs.copy(source, target);
        fs.changePermissions(target, 0755);
        p('Finished installing sorrow.js \n');
    }
}

if (module.id == require.main) {
    install('out/Release/sorrow', '/usr/bin/sorrow');
}