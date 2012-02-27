/*
 copyright 2012 sam l'ecuyer
*/

var console = require('console');
exports.run = function(exports) {
    var pass = 0,
        fail = 0;
    for (x in exports) {
        var mthd = exports[x];
        if (typeof mthd != 'function') continue;
        try {
            mthd();
            pass++;
        } catch(e) {
            console.log('Failure: ' + x);
            console.log(e);
            fail++;
        }
    }
    console.log('Results: '+pass+' test pass, '+fail+' tests fail');
    return fail;
};