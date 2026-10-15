/*
   ----------------------------------------------------------------------------
   | mg-dbx-napi.node                                                         |
   | Description: A ngnix/QuickJS interface to InterSystems Cache/IRIS        |
   |              and YottaDB                                                 |
   | Author:      Chris Munt cmunt@mgateway.com                               |
   |                         chris.e.munt@gmail.com                           |
   | Copyright (c) 2025-2026 MGateway Ltd                                     |
   | Surrey UK.                                                               |
   | All rights reserved.                                                     |
   |                                                                          |
   | http://www.mgateway.com                                                  |
   |                                                                          |
   | Licensed under the Apache License, Version 2.0 (the "License"); you may  |
   | not use this file except in compliance with the License.                 |
   | You may obtain a copy of the License at                                  |
   |                                                                          |
   | http://www.apache.org/licenses/LICENSE-2.0                               |
   |                                                                          |
   | Unless required by applicable law or agreed to in writing, software      |
   | distributed under the License is distributed on an "AS IS" BASIS,        |
   | WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. |
   | See the License for the specific language governing permissions and      |
   | limitations under the License.                                           |
   |                                                                          |
   ----------------------------------------------------------------------------
*/

#ifndef MG_QJS_H
#define MG_QJS_H

#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <stdlib.h>
#include <time.h>

#include <quickjs.h>

#include "mg_dba.h"

#define MAJORVERSION             1
#define MINORVERSION             0
#define MAINTVERSION             1
#define BUILDNUMBER              1

#define MGQJS_VERSION_MAJOR     "1"
#define MGQJS_VERSION_MINOR     "0"
#define MGQJS_VERSION_BUILD     "1"

#define MGQJS_VERSION            MGQJS_VERSION_MAJOR "." MGQJS_VERSION_MINOR "." MGQJS_VERSION_BUILD
#define MGQJS_COMPANYNAME        "M/Gateway Developments Ltd\0"
#define MGQJS_FILEDESCRIPTION    "ngnix/QuickJS interface to InterSystems IRIS/Cache and YottaDB\0"
#define MGQJS_FILEVERSION        MGQJS_VERSION
#define MGQJS_INTERNALNAME       "mg_qjs\0"
#define MGQJS_LEGALCOPYRIGHT     "Copyright 2025-2026, M/Gateway Developments Ltd\0"
#define MGQJS_ORIGINALFILENAME   "mg_qjs\0"
#define MGQJS_PLATFORM           PROCESSOR_ARCHITECTURE
#define MGQJS_PRODUCTNAME        "mg_qjs\0"
#define MGQJS_PRODUCTVERSION     MGQJS_VERSION
#define MGQJS_BUILD              MGQJS_VERSION

#if defined(_WIN32)

#define BUILDING_NODE_EXTENSION     1
#if defined(_MSC_VER)
/* Check for MS compiler later than VC6 */
#if (_MSC_VER >= 1400)
#define _CRT_SECURE_NO_DEPRECATE    1
#define _CRT_NONSTDC_NO_DEPRECATE   1
#endif
#endif
#endif

typedef struct tagMGQJSResult {
   int            cmnd;
   short          dtype;
   short          dsort;
   int            data_len;
   unsigned char  *data;
} MGQJSRES, *PMGQJSRES;


int      mgqjs_pack_args      (JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, DBXSTR *block, int cmnd, int index);
int      mgqjs_strt           (DBXSTR *block);
int      mgqjs_exit           (DBXSTR *block);
int      mgqjs_result         (DBXSTR *block, MGQJSRES *result);
int      mgqjs_inspect_buffer (char *obuffer, unsigned char *ibuffer, int len);
size_t   mgqjs_getstacksize   ();

#endif

