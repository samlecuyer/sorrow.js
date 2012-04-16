var MySql = require('./mysql').MySql;

var conn = new MySql();
conn.realConnect('localhost', 'grubhub', 'sywtbmc', 'grubhub');

var results = conn.realQuery('select * from customer limit 3');

var row = results.fetchRow();
while(row) {
    require('console').log(row.uid, row.city);
    row = results.fetchRow();
}