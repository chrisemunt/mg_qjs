// A simple test for mg_qjs.  Write to and reads from the global: ^CM
//
// Import mg_qjs.so by filename
import * as mg from 'mg_qjs.so';

function handler(r) {
   let result = mg.version() + "\r\n";
   // set environment variables for YottaDB (only used for API connectivity)
   let envvars = "";
   envvars = envvars + "ydb_dir=/root/.yottadb\n"
   envvars = envvars + "ydb_rel=r1.38_x86_64\n"
   envvars = envvars + "ydb_gbldir=/root/.yottadb/r1.38_x86_64/g/yottadb.gld\n"
   envvars = envvars + "ydb_routines=/root/.yottadb/r1.38_x86_64/o*(/root/.yottadb/r1.38_x86_64/r /root/.yottadb/r) /usr/local/lib/yottadb/r138/libyottadbutil.so\n"
   envvars = envvars + "ydb_ci=/usr/local/lib/yottadb/r138/cm.ci\n"
   envvars = envvars + "\n"
   // options for connecting to the database
   //result = result + "Open database (YottaDB API) ..." + "\r\n";
   //mg.open("YottaDB", "/usr/local/lib/yottadb/r138", "", "", "", "", "", envvars);
   //result = result + "Open database (IRIS API) ..." + "\r\n";
   //mg.open("IRIS", "/opt/iris20261/mgr", "", "", "_SYSTEM", "SYS", "USER");
   result = result + "Open database (TCP) ..." + "\r\n";
   mg.open("IRIS", "", "localhost", "7041", "_SYSTEM", "SYS", "USER");
   // we should be connected now
   result = result + "Set records 1, 2 and 3 ..." + "\r\n";
   mg.set("CM", 1, "Chris Munt");
   mg.set("CM", 2, "Rob Tweed");
   mg.set("CM", 3, "John Smith");
   result = result + "Retrieve record (1): " + mg.get("CM", 1) + "\r\n";
   result = result + "Retrieve record (9): " + mg.get("CM", 9) + "\r\n";
   result = result + "Parse record set ..." + "\r\n";
   let key = "";
   while ((key = mg.next("CM", key)) != "") {
      result = result + "Record : " + key + ' : ' + mg.get("CM", key) + "\r\n";
   }
   result = result + "Is record (3) defined?: " + mg.defined("CM", 3) + "\r\n";
   result = result + "Delete record (3): " + mg.delete("CM", 3) + "\r\n";
   result = result + "Is record (3) defined?: " + mg.defined("CM", 3) + "\r\n";
   result = result + "Parse record set (reverse order) ..." + "\r\n";
   key = "";
   while ((key = mg.previous("CM", key)) != "") {
      result = result + "Record : " + key + ' : ' + mg.get("CM", key) + "\r\n";
   }
   result = result + "Close database ..." + "\r\n";
   mg.close();
   //let result = "";
   r.return(200, `Result: ${result}\n`);
}

export default { handler };
