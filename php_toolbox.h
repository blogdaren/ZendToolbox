/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2016 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: 程序猿虎哥
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_TOOLBOX_H
#define PHP_TOOLBOX_H

extern zend_module_entry toolbox_module_entry;
#define phpext_toolbox_ptr &toolbox_module_entry

#define TOOLBOX_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(toolbox, v)

#if defined(ZTS) && defined(COMPILE_DL_TOOLBOX)
ZEND_TSRMLS_CACHE_EXTERN()
#endif


#define PHP_TOOLBOX_VERSION             "1.0" 
#define REGEXP_OPCODE_MAX_MATCHES       3 
#define ZEND_VM_OPCODES_PATH            "/Zend/zend_vm_opcodes.h"
#define ZEND_VM_OPCODES_PATH_EX         "/../../Zend/zend_vm_opcodes.h"
#define ZEND_VM_OPCODES_PATH_DEFAULT    getZendVMOpcodesDefaultPath()
#define Z_TOOLBOX_OBJECT_P(zv)          (toolbox_fetch_object)(Z_OBJ_P(zv))


#ifndef PHP_ME_END
#define PHP_ME_END  {NULL, NULL, NULL}
#endif


#ifdef PHP_WIN32
#	define PHP_TOOLBOX_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_TOOLBOX_API __attribute__ ((visibility("default")))
#else
#	define PHP_TOOLBOX_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif


#if PHP_VERSION_ID < 70400
#define TOOLBOX_WHANDLER_T             void
#define TOOLBOX_WHANDLER_RETURN(zv)    return
# if PHP_VERSION_ID < 70300
# define GC_ADDREF(gc)                 (++GC_REFCOUNT(gc))
# define GC_DELREF(gc)                 (--GC_REFCOUNT(gc))
# endif
#else
#define TOOLBOX_WHANDLER_T             zval *
#define TOOLBOX_WHANDLER_RETURN(zv)    return zv
#endif


#if PHP_VERSION_ID < 80000
#define TOOLBOX_OBJECT_T zval
#define TOOLBOX_ZOBJ(zv) Z_OBJ_P(zv)
#else
#define TOOLBOX_OBJECT_T zend_object
#define TOOLBOX_ZOBJ
#endif


#if PHP_VERSION_ID < 70200
#define TOOLBOX_ALLOW_VIOLATION(ht)
#else
#define TOOLBOX_ALLOW_VIOLATION(ht) do { \
        zend_hash_real_init(ht, 0);      \
        HT_ALLOW_COW_VIOLATION(ht);      \
} while (0)
#endif

#ifndef ZVAL_NEW_ARR    /* For PHP8 compality */
#define ZVAL_NEW_ARR(z) do {									\
		zval *__z = (z);										\
		zend_array *_arr =										\
		(zend_array *) emalloc(sizeof(zend_array));				\
		Z_ARR_P(__z) = _arr;									\
		Z_TYPE_INFO_P(__z) = IS_ARRAY_EX;						\
	} while (0)
#endif



ZEND_BEGIN_MODULE_GLOBALS(toolbox)
    char *php_src;
    zend_array *module_table;
    zend_array *opcode_table;
ZEND_END_MODULE_GLOBALS(toolbox)


typedef struct _toolbox_object{
    zend_string *author;
    zend_string *website;
    zend_array  *properties;
    zend_object std;
}toolbox_object;


/*
 *typedef struct _php_extend_data{
 *    zend_string *module_name;
 *    zend_string *function_name;
 *}php_extend_data;
 */


static zend_always_inline char *getZendVMOpcodesDefaultPath();
static inline zend_object *toolbox_new(zend_class_entry *ce);
static inline void toolbox_free(zend_object *object);
static zend_always_inline toolbox_object *toolbox_fetch_object(zend_object *object);
static HashTable *writeIntoOpcodeHashTable(const char *pattern, const char *text, HashTable *ht);

static zend_always_inline toolbox_object *toolbox_fetch_object(zend_object *object)
{
    return (toolbox_object*)((char*)(object) - XtOffsetOf(toolbox_object, std));
}

static zend_always_inline char *getZendVMOpcodesDefaultPath()
{
    char *path = (char*)emalloc(sizeof(char) * strlen(__FILE__) + strlen(ZEND_VM_OPCODES_PATH_EX));
    sprintf(path, "%s%s\0", dirname(strdup(__FILE__)), ZEND_VM_OPCODES_PATH_EX);

    return path;
}


PHP_MINIT_FUNCTION(toolbox);
PHP_MSHUTDOWN_FUNCTION(toolbox);
PHP_RINIT_FUNCTION(toolbox);
PHP_RSHUTDOWN_FUNCTION(toolbox);
PHP_MINFO_FUNCTION(toolbox);
PHP_GINIT_FUNCTION(toolbox);

/* function */
PHP_FUNCTION(getToolboxVersion);

/* method  */
PHP_METHOD(toolbox, __construct);
PHP_METHOD(toolbox, getPHPModuleByFunction);
PHP_METHOD(toolbox, getZendVMOpcodes);
PHP_METHOD(toolbox, setWebsite);
PHP_METHOD(toolbox, getWebsite); 


#endif	/* PHP_TOOLBOX_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
