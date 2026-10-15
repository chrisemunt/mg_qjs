# mg_qjs

High speed access to InterSystems Cache/IRIS and YottaDB from QuickJS (via Nginx).

Chris Munt <cmunt@mgateway.com>  
15 October 2026, MGateway Ltd [http://www.mgateway.com](http://www.mgateway.com)

**NOTE!!! This project is in its experimental phase - Only recommended for those who have support aggreements with us.**

* Two connectivity models to the InterSystems or YottaDB database are provided: High performance via the local database API or network based.
* [Release Notes](#relnotes) can be found at the end of this document.

Contents

* [What Is mg_qjs?](#whatis)
* [Installing mg_qjs](#install)
* [Using mg_qjs](#using)
* [Connecting to the database](#connect)
* [Invocation of database commands](#dbcommands)
* [Invocation of database functions](#dbfunctions)
* [License](#license)

## <a name="whatis">What Is *mg_qjs*?</a>

This package provides **QuickJS** (embedded in the **Nginx** engine) applications with extremely high-performance access to InterSystems database products and the YottaDB database.

## <a name="install">Installing mg_qjs</a>

This guide assumes that you have an **Nginx** installation with QuickJS built in.  The **Nginx/QuickJS** documentation can be found here:

https://nginx.org/en/docs/njs/

It is recommended that you get the simple **QuickJS** examples working before attempting to install **mg\_qjs**

The first step is to build the **mg\_qjs** module (**mg\_qjs.so**) in the /src directory

Edit the the lines at the top of the **Makefile** file to match the paths for your installation.

       # Edit the following three lines to match the paths in your installation 
       # /path/to/installation/location/
       MGQJSDIR=/opt/nginx1314/modules/
       # -I/path/to/quickjs/
       MGQJSINC=-I/opt/quickjs/
       # -L/path/to/quickjs/
       MGQJSLIB=-L/opt/quickjs/

Now build the module:

       make

Install the module in its working directory (The hosting **Nginx** server must be shutdown before you do this):
 
       make install

Now, configure Nginx to use the **mg\_qjs** module.  Edit the **Nginx** configuration file (**nginx.conf**) as follows.

Add the following line to the global section:

        # Comment: js_load_http_native_module /path/to/mg_qjs.so;
        js_load_http_native_module /opt/nginx1314/modules/mg_qjs.so;

Add the following lines to the http section.  As an example, we assume that you're using the simple test found in the /samples directory (**mg\_qjs**).:

        http {
           js_engine qjs;
           # Comment: js_load_http_native_module /path/to/mg_qjs.js;
           js_import /opt/nginx1314/html/js/mg_qjs.js;

Finally, in the server section add a location from you wish to launch your JavaScript code:

	         location /qjs {
               js_content mg_qjs.handler;
	         }

In the top of the test code (**mg\_qjs.js**) you will find a range of connectivity options.  It is recommended that you start with the TCP based model using the **mgsi** superserver.
To use the API based models you should make sure that Nginx worker processes possess the necesary permissions to access the target database.

Start the **Nginx** server and run the test form:

            curl http::/localhost/qjs/test

Using the test provided, you should see the following returned:

            Result: 1.0.1
            Open database (TCP) ...
            Set records 1, 2 and 3 ...
            Retrieve record (1): Chris Munt
            Retrieve record (9):
            Parse record set ...
            Record : 1 : Chris Munt
            Record : 2 : Rob Tweed
            Record : 3 : John Smith
            Is record (3) defined?: 1
            Delete record (3): 0
            Is record (3) defined?: 0
            Parse record set (reverse order) ...
            Record : 2 : Rob Tweed
            Record : 1 : Chris Munt
            Close database ...


## <a name="using">Using mg\_qjs</a>

**mg\_qjs** is loaded into a **QuickJS** module as follows:

        import * as mg from 'mg_qjs.so';

## <a name="databases">Supported Databases</a>

**mg\_qjs** is designed for use with the following databases:

- [InterSystems IRIS](https://www.intersystems.com/uk/data-platform/)
- [InterSystems Cache](https://www.intersystems.com/uk/cache/)
- [YottaDB](https://yottadb.com/)

**mg\_qjs** can connect in two ways to these databases:

- networked connection, where the **Nginx/QuickJS** system and database can be on separate servers.
- in-process API connection, where the **Nginx/QuickJS** system and database reside on the same physical server.

Networked connections provide the most all-round platform compatibility (eg **Nginx/QuickJS** running on a Windows system can connect to YottaDB which only runs on Linux), and highest levels of security.  In order to use a networked connection, you need to install our DB Superserver on the database system: see next section below.

The in-process API connection mode provides the very highest levels of performance, reaching near in-memory performance access to the respective database persistent storage.  Note that in this mode, you need to be aware of the platform availability of the database products:

- InterSystems' Cache and IRIS database versions are available for both Windows and Linux (x64 and ARM64)
- YottaDB is only available on Linux (x64 and ARM64)

The in-process API connection does **not** require installation of our DB Superserver on the database system, unless:

- you want to use SQL to access the data held on the database
- you want to use the Merge command with the YottaDB database
- 

### Installing the M support routines (also known as the DB Superserver)

The M support routines are required for:

* Network based access to databases.
* Direct access to SQL (either via the API or via the network).

If none of the above apply you do not need to install these routines - proceed to  [Connecting to the database](#connect). 

Two M routines need to be installed (%zmgsi and %zmgsis).  These can be found in the *Service Integration Gateway* (**mgsi**) GitHub source code repository ([https://github.com/chrisemunt/mgsi](https://github.com/chrisemunt/mgsi)).  Note that it is not necessary to install the whole *Service Integration Gateway*, just the two M routines held in that repository.

#### Installation for InterSystems Cache/IRIS

Log in to the %SYS Namespace and install the **zmgsi** routines held in **/isc/zmgsi\_isc.ro**.

       do $system.OBJ.Load("/isc/zmgsi_isc.ro","ck")

Change to your development UCI and check the installation:

       do ^%zmgsi

       MGateway Ltd - Service Integration Gateway
       Version: 4.5; Revision 37 (9 March 2025)


#### Installation for YottaDB

The instructions given here assume a standard 'out of the box' installation of **YottaDB** (version 1.38) deployed in the following location:

       /usr/local/lib/yottadb/r138

The primary default location for routines:

       /root/.yottadb/r1.38_x86_64/r

Copy all the routines (i.e. all files with an 'm' extension) held in the GitHub **/yottadb** directory to:

       /root/.yottadb/r1.38_x86_64/r

Change directory to the following location and start a **YottaDB** command shell:

       cd /usr/local/lib/yottadb/r138
       ./ydb

Link all the **zmgsi** routines and check the installation:

       do ylink^%zmgsi

       do ^%zmgsi

       MGateway Ltd - Service Integration Gateway
       Version: 4.5; 4.5; Revision 37 (9 March 2025)

Note that the version of **zmgsi** is successfully displayed.

Finally, add the following lines to the interface file (**zmgsi.ci** in the example used in the mg.open() method).

       sqlemg: ydb_string_t * sqlemg^%zmgsis(I:ydb_string_t*, I:ydb_string_t *, I:ydb_string_t *)
       sqlrow: ydb_string_t * sqlrow^%zmgsis(I:ydb_string_t*, I:ydb_string_t *, I:ydb_string_t *)
       sqldel: ydb_string_t * sqldel^%zmgsis(I:ydb_string_t*, I:ydb_string_t *)
       ifc_zmgsis: ydb_string_t * ifc^%zmgsis(I:ydb_string_t*, I:ydb_string_t *, I:ydb_string_t*)

A copy of this file can be downloaded from the **/unix** directory of the  **mgsi** GitHub repository [here](https://github.com/chrisemunt/mgsi)

### Starting the DB Superserver (for network based connectivity only)

The default TCP server port for **zmgsi** is **7041**.  If you wish to use an alternative port then modify the following instructions accordingly.

* For InterSystems DB servers the concurrent TCP service should be started in the **%SYS** Namespace.

Start the DB Superserver using the following command:

       do start^%zmgsi(0) 

To use a server TCP port other than 7041, specify it in the start-up command (as opposed to using zero to indicate the default port of 7041).

* For YottaDB, as an alternative to starting the DB Superserver from the command prompt, Superserver processes can be started via the **xinetd** daemon.  Instructions for configuring this option can be found in the **mgsi** repository [here](https://github.com/chrisemunt/mgsi)


## <a name="connect">Connecting to the database</a>

Example:

        import * as mg from 'mg_qjs.so';
        function handler(r) {
           let result = mg.version() + "\r\n";
           mg.open("IRIS", "", "localhost", "7041", "_SYSTEM", "SYS", "USER");
           // we should be connected now

           result = result + mg.get("CM", 1);

           // ..do more stuff, then finally:

           mg.close();
           r.return(200, `Result: ${result}\n`);
        }

        export default { handler };

### Open a connection to the database

In the following examples, modify all paths (and any user names and passwords) to match those of your own installation.

#### InterSystems Cache

##### API based connectivity

Assuming Cache is installed under **/opt/cache20261/**

           mg.open("Cache", "/opt/cache20181/mgr", "", "", "_SYSTEM", "SYS", "USER");

##### Network based connectivity

Assuming Cache is accessed via **localhost** listening on TCP port **7041**

           mg.open("Cache", "", "localhost", "7041", "_SYSTEM", "SYS", "USER");

#### InterSystems IRIS

##### API based connectivity

Assuming IRIS is installed under **/opt/IRIS20181/**

           mg.open("IRIS", "/opt/IRIS20181/mgr", "", "", "_SYSTEM", "SYS", "USER");
           
##### Network based connectivity

Assuming IRIS is accessed via **localhost** listening on TCP port **7041**

           mg.open("IRIS", "", "localhost", "7041", "_SYSTEM", "SYS", "USER");

#### YottaDB

##### API based connectivity

Assuming an 'out of the box' YottaDB installation under **/usr/local/lib/yottadb/r138**.

           let envvars = "";
           envvars = envvars + "ydb_dir=/root/.yottadb\n"
           envvars = envvars + "ydb_rel=r1.38_x86_64\n"
           envvars = envvars + "ydb_gbldir=/root/.yottadb/r1.38_x86_64/g/yottadb.gld\n"
           envvars = envvars + "ydb_routines=/root/.yottadb/r1.38_x86_64/o*(/root/.yottadb/r1.38_x86_64/r /root/.yottadb/r) /usr/local/lib/yottadb/r138/libyottadbutil.so\n"
           envvars = envvars + "ydb_ci=/usr/local/lib/yottadb/r138/cm.ci\n"
           envvars = envvars + "\n"
           mg.open("YottaDB", "/usr/local/lib/yottadb/r138", "", "", "", "", "", envvars);

Note that the actual YottaDB environment variables that you should use will depend on your YottaDB configuration, and, in particular, the Globals and Routines that you may need to access from **Nginx/QuickJS**.

##### Network based connectivity

Assuming YottaDB is accessed via **localhost** listening on TCP port **7041**

           mg.open("YottaDB", "", "localhost", "7041");

### Return the version of mg\_qjs

       let result = mg.version();


### Returning (and optionally changing) the current directory (or Namespace)

       current_namespace = mg.namespace([<new_namespace>]);

Example 1 (Get the current Namespace): 

       let nspace = mg.namespace();

* Note this will return the current Namespace for InterSystems databases and the value of the current global directory for YottaDB (i.e. $ZG).

Example 2 (Change the current Namespace): 

       let new_nspace = mg.namespace("SAMPLES");

* If the operation is successful this method will echo back the new Namespace name.  If not successful, the method will return the name of the current (unchanged) Namespace.

### Close database connection

       mg.close();
 
## <a name="dbcommands">Invocation of database commands</a>

Database commands can invoked directly from a database object.

### Set a record

       let result = mg.set(<global name>, <key>, <data>);
      
Example:

       mg.set("Person", 1, "John Smith");


### Get a record

       let result = mg.get(<global name>, <key>);
      
Example:

       let name = mg.get("Person", 1);


### Delete a record

       let result = mg.delete(<global name>, <key>);
      
Example:

       let result = mg.delete("Person", 1);


### Check whether a record is defined

       let result = mg.defined(<global name>, <key>);
      
Example:

       let result = mg.defined("Person", 1);


### Parse a set of records (in order)

       let result = mg.next(<global name>, <key>);
      
Example:

       let result = "";
       let key = "";
       while ((key = mg.next("CM", key)) != "") {
          result = result + "Record : " + key + ' : ' + mg.get("CM", key) + "\r\n";
       }


### Parse a set of records (in reverse order)

       let result = mg.previous(<global name>, <key>);
      
Example:

       let result = "";
       let key = "";
       while ((key = mg.previous("CM", key)) != "") {
          result = result + "Record : " + key + ' : ' + mg.get("CM", key) + "\r\n";
       }


### Increment the value of a global node

       let result = mg.increment(<global name>, <key>, <increment_value>);
      
Example (increment the value of the "counter" node by 1.5 and return the new value):

       let result = mg.increment("Person", "counter", 1.5);


### Lock a global node

       let result = mg.lock(<global name>, <key>, <timeout>);
      
Example (lock global node '1' with a timeout of 30 seconds):

       let result = mg.lock("Person", 1, 30);

* Note: Specify the timeout value as '-1' for no timeout (i.e. wait until the global node becomes available to lock).


### Unlock a (previously locked) global node

       let result = mg.unlock(<global name>, <key>);
      
Example (unlock global node '1'):

       let result = mg.unlock("Person", 1);


## <a name="dbfunctions">Invocation of database functions</a>

       let result = mg.function(<function>, <parameters>);
      
Example:

M routine called 'math':

       add(a, b) ; Add two numbers together
                 quit (a+b)

JavaScript invocation:

      let result = mg.function("add^math", 2, 3);

## <a name="license">License</a>

Copyright (c) 2021-2026 MGateway Ltd,
Surrey UK.                                                      
All rights reserved.
 
http://www.mgateway.com                                                  
Email: cmunt@mgateway.com
 
 
Licensed under the Apache License, Version 2.0 (the "License"); you may not use this file except in compliance with the License. You may obtain a copy of the License at

      http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the License for the specific language governing permissions and limitations under the License.      

## <a name="relnotes">Release Notes</a>

### v1.0.1 (15 October 2026)

* Initial Release
