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

/*
   Development Diary (in brief):

Version 1.0.1 15 October 2026:
   First release.

*/

#include "mg_dbasys.h"
#include "mg_dba.h"
#include "mg_qjs.h"

#include <stdlib.h>
#include <pthread.h>

#define countof(x) (sizeof(x) / sizeof((x)[0]))

int mgqjs_init = 0;
//char mgqjs_gbuffer[CACHE_MAXLOSTSZ + 7];
char mgqjs_gbuffer[1024];
//char *mgqjs_gbuffer;

#if defined(_WIN32)
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpReserved)
{
   switch (fdwReason)
   {
   case DLL_PROCESS_ATTACH:
      mg_init_critical_section((void*) &dbx_global_mutex);
      break;
   case DLL_THREAD_ATTACH:
      break;
   case DLL_THREAD_DETACH:
      break;
   case DLL_PROCESS_DETACH:
      mg_delete_critical_section((void*) &dbx_global_mutex);
      break;
   }
   return TRUE;
}
#endif /* #if defined(_WIN32) */



static JSValue mgqjs_version(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   JSValue result;
   char buffer[32];

#if defined(_WIN32)
   sprintf_s(buffer, CACHE_MAXLOSTSZ, "%s", MGQJS_VERSION);
#else
   sprintf(buffer, "%s", MGQJS_VERSION);
#endif

   result = JS_NewString(ctx, buffer);
   return result;
}

static JSValue mgqjs_dbversion(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   int index;
   JSValue result;
   char buffer[256];

   index = 0;
   dbx_version(index, buffer, 250);

   result = JS_NewString(ctx, buffer);
   return result;
}

static JSValue mgqjs_open(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   int index;
   DBXSTR block;
   MGQJSRES res;
   JSValue result;

   if (!mgqjs_init) {
      dbx_init();
      mgqjs_init ++;
   }
   index = 0;
   res.cmnd = DBX_CMND_OPEN;
   if (!mgqjs_strt(&block)) {
      return JS_ThrowTypeError(ctx, "mg_qjs failure");
   }

   index = mgqjs_pack_args(ctx, this_val, argc, argv, &block, res.cmnd, index);
   dbx_open(block.buf_addr, block.buf_addr);
   mgqjs_result(&block, &res);
   result = JS_NewString(ctx, res.data);
   mgqjs_exit(&block);
   return result;
}

static JSValue mgqjs_close(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   int index;
   DBXSTR block;
   MGQJSRES res;
   JSValue result;

   if (!mgqjs_init) {
      return JS_ThrowTypeError(ctx, "DB connection not initialised");
   }
   index = 0;
   res.cmnd = DBX_CMND_CLOSE;
   if (!mgqjs_strt(&block)) {
      return JS_ThrowTypeError(ctx, "mg_qjs failure");
   }

   mgqjs_pack_args(ctx, this_val, argc, argv, &block, res.cmnd, index);
   dbx_close(block.buf_addr, block.buf_addr);
   mgqjs_result(&block, &res);
   result = JS_NewString(ctx, res.data);
   mgqjs_exit(&block);
   return result;
}

static JSValue mgqjs_namespace(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   int index;
   DBXSTR block;
   MGQJSRES res;
   JSValue result;

   if (!mgqjs_init) {
      return JS_ThrowTypeError(ctx, "DB connection not initialised");
   }
   index = 0;
   res.cmnd = DBX_CMND_NSSET;
   if (!mgqjs_strt(&block)) {
      return JS_ThrowTypeError(ctx, "mg_qjs failure");
   }

   mgqjs_pack_args(ctx, this_val, argc, argv, &block, res.cmnd, index);
   dbx_set(block.buf_addr, block.buf_addr);
   mgqjs_result(&block, &res);
   result = JS_NewString(ctx, res.data);
   mgqjs_exit(&block);
   return result;
}

static JSValue mgqjs_set(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   int index;
   DBXSTR block;
   MGQJSRES res;
   JSValue result;

   if (!mgqjs_init) {
      return JS_ThrowTypeError(ctx, "DB connection not initialised");
   }
   index = 0;
   res.cmnd = DBX_CMND_GSET;
   if (!mgqjs_strt(&block)) {
      return JS_ThrowTypeError(ctx, "mg_qjs failure");
   }

   mgqjs_pack_args(ctx, this_val, argc, argv, &block, res.cmnd, index);
   dbx_set(block.buf_addr, block.buf_addr);
   mgqjs_result(&block, &res);
   result = JS_NewString(ctx, res.data);
   mgqjs_exit(&block);
   return result;
}

static JSValue mgqjs_get(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   int index;
   DBXSTR block;
   MGQJSRES res;
   JSValue result;

   if (!mgqjs_init) {
      return JS_ThrowTypeError(ctx, "DB connection not initialised");
   }
   index = 0;
   res.cmnd = DBX_CMND_GGET;
   if (!mgqjs_strt(&block)) {
      return JS_ThrowTypeError(ctx, "mg_qjs failure");
   }

   mgqjs_pack_args(ctx, this_val, argc, argv, &block, res.cmnd, index);
   dbx_get(block.buf_addr, block.buf_addr);
   mgqjs_result(&block, &res);
   result = JS_NewString(ctx, res.data);
   mgqjs_exit(&block);
   return result;
}

static JSValue mgqjs_delete(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   int index;
   DBXSTR block;
   MGQJSRES res;
   JSValue result;

   if (!mgqjs_init) {
      return JS_ThrowTypeError(ctx, "DB connection not initialised");
   }
   index = 0;
   res.cmnd = DBX_CMND_GDELETE;
   if (!mgqjs_strt(&block)) {
      return JS_ThrowTypeError(ctx, "mg_qjs failure");
   }

   mgqjs_pack_args(ctx, this_val, argc, argv, &block, res.cmnd, index);
   dbx_delete(block.buf_addr, block.buf_addr);
   mgqjs_result(&block, &res);
   result = JS_NewString(ctx, res.data);
   mgqjs_exit(&block);
   return result;
}

static JSValue mgqjs_defined(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   int index;
   DBXSTR block;
   MGQJSRES res;
   JSValue result;

   if (!mgqjs_init) {
      return JS_ThrowTypeError(ctx, "DB connection not initialised");
   }
   index = 0;
   res.cmnd = DBX_CMND_GDEFINED;
   if (!mgqjs_strt(&block)) {
      return JS_ThrowTypeError(ctx, "mg_qjs failure");
   }

   mgqjs_pack_args(ctx, this_val, argc, argv, &block, res.cmnd, index);
   dbx_delete(block.buf_addr, block.buf_addr);
   mgqjs_result(&block, &res);
   result = JS_NewString(ctx, res.data);
   mgqjs_exit(&block);
   return result;
}

static JSValue mgqjs_next(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   int index;
   DBXSTR block;
   MGQJSRES res;
   JSValue result;

   if (!mgqjs_init) {
      return JS_ThrowTypeError(ctx, "DB connection not initialised");
   }
   index = 0;
   res.cmnd = DBX_CMND_GNEXT;
   if (!mgqjs_strt(&block)) {
      return JS_ThrowTypeError(ctx, "mg_qjs failure");
   }

   mgqjs_pack_args(ctx, this_val, argc, argv, &block, res.cmnd, index);
   dbx_next(block.buf_addr, block.buf_addr);
   mgqjs_result(&block, &res);
   result = JS_NewString(ctx, res.data);
   mgqjs_exit(&block);
   return result;
}

static JSValue mgqjs_previous(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   int index;
   DBXSTR block;
   MGQJSRES res;
   JSValue result;

   if (!mgqjs_init) {
      return JS_ThrowTypeError(ctx, "DB connection not initialised");
   }
   index = 0;
   res.cmnd = DBX_CMND_GPREVIOUS;
   if (!mgqjs_strt(&block)) {
      return JS_ThrowTypeError(ctx, "mg_qjs failure");
   }

   mgqjs_pack_args(ctx, this_val, argc, argv, &block, res.cmnd, index);
   dbx_previous(block.buf_addr, block.buf_addr);
   mgqjs_result(&block, &res);
   result = JS_NewString(ctx, res.data);
   mgqjs_exit(&block);
   return result;
}

static JSValue mgqjs_increment(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   int index;
   DBXSTR block;
   MGQJSRES res;
   JSValue result;

   if (!mgqjs_init) {
      return JS_ThrowTypeError(ctx, "DB connection not initialised");
   }
   index = 0;
   res.cmnd = DBX_CMND_GINCREMENT;
   if (!mgqjs_strt(&block)) {
      return JS_ThrowTypeError(ctx, "mg_qjs failure");
   }

   mgqjs_pack_args(ctx, this_val, argc, argv, &block, res.cmnd, index);
   dbx_increment(block.buf_addr, block.buf_addr);
   mgqjs_result(&block, &res);
   result = JS_NewString(ctx, res.data);
   mgqjs_exit(&block);
   return result;
}

static JSValue mgqjs_lock(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   int index;
   DBXSTR block;
   MGQJSRES res;
   JSValue result;

   if (!mgqjs_init) {
      return JS_ThrowTypeError(ctx, "DB connection not initialised");
   }
   index = 0;
   res.cmnd = DBX_CMND_GLOCK;
   if (!mgqjs_strt(&block)) {
      return JS_ThrowTypeError(ctx, "mg_qjs failure");
   }

   mgqjs_pack_args(ctx, this_val, argc, argv, &block, res.cmnd, index);
   dbx_lock(block.buf_addr, block.buf_addr);
   mgqjs_result(&block, &res);
   result = JS_NewString(ctx, res.data);
   mgqjs_exit(&block);
   return result;
}

static JSValue mgqjs_unlock(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   int index;
   DBXSTR block;
   MGQJSRES res;
   JSValue result;

   if (!mgqjs_init) {
      return JS_ThrowTypeError(ctx, "DB connection not initialised");
   }
   index = 0;
   res.cmnd = DBX_CMND_GUNLOCK;
   if (!mgqjs_strt(&block)) {
      return JS_ThrowTypeError(ctx, "mg_qjs failure");
   }

   mgqjs_pack_args(ctx, this_val, argc, argv, &block, res.cmnd, index);
   dbx_unlock(block.buf_addr, block.buf_addr);
   mgqjs_result(&block, &res);
   result = JS_NewString(ctx, res.data);
   mgqjs_exit(&block);
   return result;
}

static JSValue mgqjs_function(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv)
{
   int index;
   DBXSTR block;
   MGQJSRES res;
   JSValue result;

   if (!mgqjs_init) {
      return JS_ThrowTypeError(ctx, "DB connection not initialised");
   }
   index = 0;
   res.cmnd = DBX_CMND_FUNCTION;
   if (!mgqjs_strt(&block)) {
      return JS_ThrowTypeError(ctx, "mg_qjs failure");
   }

   mgqjs_pack_args(ctx, this_val, argc, argv, &block, res.cmnd, index);
   dbx_function(block.buf_addr, block.buf_addr);
   mgqjs_result(&block, &res);
   result = JS_NewString(ctx, res.data);
   mgqjs_exit(&block);
   return result;
}

int mgqjs_pack_args(JSContext *ctx, JSValueConst this_val, int argc, JSValueConst *argv, DBXSTR *block, int cmnd, int index)
{
   int n, offset;
   size_t len;
   const char * str;

   offset = 0;
   offset = mg_add_block_head(block, (unsigned long) block->len_alloc, (unsigned long) index);

   for (n = 0; n < argc; n ++) {
      str = JS_ToCStringLen(ctx, &len, argv[n]);
      offset = mg_add_block_data(block, (unsigned char *) str, (unsigned long) len, DBX_DSORT_DATA, DBX_DTYPE_STR);
   }
   offset = mg_add_block_data(block, (unsigned char *) "", 0, DBX_DSORT_EOD, DBX_DTYPE_STR);
   mg_add_block_head_size(block, (unsigned long) offset, cmnd);

   return offset;
}

int mgqjs_strt(DBXSTR *block)
{
   block->buf_addr = malloc(511); //mgqjs_gbuffer;
   block->len_alloc = 256;
   block->len_used = 0;
   return 1;
}

int mgqjs_exit(DBXSTR *block)
{
   free((void *) block->buf_addr); //mgqjs_gbuffer;
   block->buf_addr = NULL;
   block->len_alloc = 256;
   block->len_used = 0;
   return 0;
}

int mgqjs_result(DBXSTR *block,  MGQJSRES *result)
{
   result->data_len = (int) mg_get_block_size(block, (unsigned long) 0, &(result->dsort), &(result->dtype));
   result->data = block->buf_addr + 5;
   result->data[result->data_len] = '\0';
   return result->data_len;
}


int mgqjs_inspect_buffer(char *obuffer, unsigned char *ibuffer, int len)
{
   int n;
   char buffer[32];
   unsigned char c;

   sprintf(obuffer, "size=%d;", len);

   for (n = 0; n < len; n ++) {
      c = (unsigned char) ibuffer[n];
      sprintf(buffer, " %x", (unsigned char) c);
      strcat(obuffer, buffer);
   }
   return 0;
}

size_t mgqjs_getstacksize()
{
   int rc;
   size_t stack_size;
   pthread_attr_t attr;
   void *stack_addr;

   rc = pthread_getattr_np(pthread_self(), &attr);
   if (rc != 0) {
      stack_size = 0;
   }
   else {
      rc = pthread_attr_getstack(&attr, &stack_addr, &stack_size);
      rc = pthread_attr_destroy(&attr);
   }
   return stack_size;
}

/* Module function list */
static const JSCFunctionListEntry js_module_funcs[] = {
    JS_CFUNC_DEF("version", 0, mgqjs_version),
    JS_CFUNC_DEF("dbversion", 0, mgqjs_dbversion),
    JS_CFUNC_DEF("open", 0, mgqjs_open),
    JS_CFUNC_DEF("close", 0, mgqjs_close),
    JS_CFUNC_DEF("namespace", 0, mgqjs_namespace),
    JS_CFUNC_DEF("set", 0, mgqjs_set),
    JS_CFUNC_DEF("get", 0, mgqjs_get),
    JS_CFUNC_DEF("delete", 0, mgqjs_delete),
    JS_CFUNC_DEF("defined", 0, mgqjs_defined),
    JS_CFUNC_DEF("next", 0, mgqjs_next),
    JS_CFUNC_DEF("previous", 0, mgqjs_previous),
    JS_CFUNC_DEF("increment", 0, mgqjs_increment),
    JS_CFUNC_DEF("lock", 0, mgqjs_lock),
    JS_CFUNC_DEF("unlock", 0, mgqjs_unlock),
    JS_CFUNC_DEF("function", 0, mgqjs_function),
};

/* Module initialization */
static int js_module_init(JSContext *ctx, JSModuleDef *m)
{
   return JS_SetModuleExportList(ctx, m, js_module_funcs, countof(js_module_funcs));
}

/* Required entry point */
JSModuleDef * js_init_module(JSContext *ctx, const char *module_name)
{
   JSModuleDef  *m;

   m = JS_NewCModule(ctx, module_name, js_module_init);
   if (!m) {
      return NULL;
   }

   JS_AddModuleExportList(ctx, m, js_module_funcs, countof(js_module_funcs));

   return m;
}

