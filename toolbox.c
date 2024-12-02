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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <regex.h>

#include "php.h"
#include "php_version.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_toolbox.h"

ZEND_DECLARE_MODULE_GLOBALS(toolbox);
zend_object_handlers toolbox_obj_handlers;
zend_class_entry *toolbox_ce;

/* True global resources - no need for thread safety here */
static int le_toolbox;


PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("toolbox.php_src",  "", PHP_INI_ALL, OnUpdateString, php_src, zend_toolbox_globals, toolbox_globals)
PHP_INI_END()


PHP_FUNCTION(getToolboxVersion)
{
    zval *zv, zstr;

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    } 

#if PHP_VERSION_ID < 70100
    zv = zend_hash_str_find(&toolbox_ce->constants_table, "VERSION", sizeof("VERSION") - 1);
#else
    zend_class_constant *zcc = (zend_class_constant *)zend_hash_str_find(&toolbox_ce->constants_table, "VERSION", sizeof("VERSION") - 1);
    zv = (zval *)zcc->value.value.zv;
#endif

    ZVAL_STR_COPY(&zstr, Z_STR_P(zv));

    RETURN_STR(Z_STR_P(&zstr));
}


PHP_GINIT_FUNCTION(toolbox)
{
    toolbox_globals->php_src = NULL;
    toolbox_globals->module_table = NULL;
    toolbox_globals->opcode_table = NULL;
}


ZEND_BEGIN_ARG_INFO_EX(arginfo_toolbox_getToolboxVersion, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_toolbox___construct, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_toolbox_getPHPModuleByFunction, 0, 0, 0)
    ZEND_ARG_INFO(0, fname)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_toolbox_getZendVMOpcodes, 0, 0, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_toolbox_setWebsite, 0, 0, 1)
    ZEND_ARG_INFO(0, site)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO_EX(arginfo_toolbox_getWebsite, 0, 0, 0)
ZEND_END_ARG_INFO()



zend_function_entry toolbox_methods[] = {
	PHP_ME(toolbox, __construct, arginfo_toolbox___construct, ZEND_ACC_PUBLIC | ZEND_ACC_CTOR)
	PHP_ME(toolbox, getPHPModuleByFunction, arginfo_toolbox_getPHPModuleByFunction, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(toolbox, getZendVMOpcodes, arginfo_toolbox_getZendVMOpcodes, ZEND_ACC_PUBLIC | ZEND_ACC_STATIC)
	PHP_ME(toolbox, setWebsite, arginfo_toolbox_setWebsite, ZEND_ACC_PUBLIC)
	PHP_ME(toolbox, getWebsite, arginfo_toolbox_getWebsite, ZEND_ACC_PUBLIC)
    PHP_ME_END
};


PHP_METHOD(toolbox, __construct) 
{
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    } 

    toolbox_object *box = Z_TOOLBOX_OBJECT_P(getThis());
    box->website = zend_string_init(ZEND_STRL("http://www.phpcreeper.com"), 0);
}


PHP_METHOD(toolbox, getPHPModuleByFunction) 
{
	zend_string *fname = NULL, *key = NULL;
    zend_long h, count;
    zval *val, *zextension;
    zval zarray1, zarray2;
    zend_function *func;
    zend_module_entry *entry;
    /*php_extend_data *extend_data, *head;*/

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "|S", &fname) == FAILURE) {
		return;
	}

	if (fname) {
        zend_string *lcname = zend_string_tolower(fname);
        zextension = zend_hash_find(EG(function_table), lcname);
        zend_string_release(lcname);
        if(!zextension) RETURN_STRING("**Unknown Module**");
        func = (zend_function*)Z_PTR_P(zextension);
        entry = func->internal_function.module;
        RETURN_STRING(entry->name);
	}

    //important: GC_TYPE(TOOLBOX_G(module_table)) == IS_ARRAY
    if(TOOLBOX_G(module_table) && GC_TYPE(TOOLBOX_G(module_table)) == IS_ARRAY){
        ZVAL_ARR(&zarray1, TOOLBOX_G(module_table));
        Z_ADDREF_P(&zarray1); //we need to increase the reference count here
        RETURN_ARR(TOOLBOX_G(module_table));
    }

    ZVAL_NEW_ARR(&zarray1);
    zend_array *ht = Z_ARR_P(&zarray1);
    zend_hash_init(ht, 8, NULL, ZVAL_PTR_DTOR, 0);

    /*count = EG(function_table)->nNumOfElements;*/
    /*head = extend_data = (php_extend_data *)emalloc(sizeof(php_extend_data) * count);*/
    /*memset(extend_data, 0, sizeof(php_extend_data) * count);*/

    ZEND_HASH_FOREACH_KEY_VAL(EG(function_table), h, key, val){
        func = (zend_function *)(val->value.func);
        if(ZEND_INTERNAL_FUNCTION != func->type) continue;
        entry = (zend_module_entry*)(func->internal_function.module);
        /*php_printf("h = %d|key=%s|name=%s|entry=%s\n", h, ZSTR_VAL(key), ZSTR_VAL(func->common.function_name), entry->name);*/
        zend_string *module_name = zend_string_init(entry->name, strlen(entry->name), 0);
        zval zstr1, zstr2;
        ZVAL_STR_COPY(&zstr1, func->common.function_name);  //note reference count
        ZVAL_NEW_STR(&zstr2, module_name);                  //note reference count
        ZVAL_NEW_ARR(&zarray2);                             //note reference count
        zend_array *ht2 = Z_ARR_P(&zarray2);
        zend_hash_init(ht2, 16, NULL, ZVAL_PTR_DTOR, 0);
        zend_hash_update(ht, func->common.function_name, &zarray2);
        zend_hash_index_update(ht2, 0, &zstr1);
        zend_hash_index_update(ht2, 1, &zstr2);
    }ZEND_HASH_FOREACH_END();

    TOOLBOX_G(module_table) = ht;

    RETURN_ARR(ht);
}


PHP_METHOD(toolbox, getZendVMOpcodes) 
{
    zval zarray1;
    FILE *file;
    char real_header_file[PATH_MAX];

    if (zend_parse_parameters_none() == FAILURE) {
        return;
    } 

    //important: GC_TYPE(TOOLBOX_G(opcode_table)) == IS_ARRAY
    if(TOOLBOX_G(opcode_table) && GC_TYPE(TOOLBOX_G(opcode_table)) == IS_ARRAY){
        ZVAL_ARR(&zarray1, TOOLBOX_G(opcode_table));
        Z_ADDREF_P(&zarray1); //we need to increase the reference count here
        RETURN_ARR(TOOLBOX_G(opcode_table));
    }

    ZVAL_NEW_ARR(&zarray1);
    zend_array *ht = Z_ARR_P(&zarray1);
    zend_hash_init(ht, 256, NULL, ZVAL_PTR_DTOR, 0);

    size_t len = strlen(TOOLBOX_G(php_src)) + strlen(ZEND_VM_OPCODES_PATH) + 1;
    char *filename = emalloc(sizeof(char*) * len);
    memcpy(filename, TOOLBOX_G(php_src), strlen(TOOLBOX_G(php_src)));
    memcpy(filename + strlen(TOOLBOX_G(php_src)), ZEND_VM_OPCODES_PATH, strlen(ZEND_VM_OPCODES_PATH));
    filename[len-1] = '\0';
    file = fopen(filename, "r");

    if (NULL == file) {
        memset(real_header_file, 0, sizeof(real_header_file));
        char *default_header_file = ZEND_VM_OPCODES_PATH_DEFAULT; 
        realpath(default_header_file, real_header_file);
        file = fopen(real_header_file, "r");
        if (NULL == file) {
            perror("****** Failed to open file: zend_vm_opcodes.h");
            php_printf("****** Failed to open file: zend_vm_opcodes.h: " 
                    "or consider to set `ini_set('toolbox.php_src', $php_%s_src_path)` in php script\n", PHP_VERSION);
            php_printf("****** Failed to open file: zend_vm_opcodes.h: " 
                    "or consider to set `toolbox.php_src=php_%s_src_path` in php.ini\n", PHP_VERSION);
            efree(filename);
            efree(default_header_file);
            zend_array_destroy(ht);
            RETURN_NULL();
        }
        efree(default_header_file);
    }

    char *line = NULL;
    size_t length = 0;
    ssize_t read1, read2;
    const char *pattern = "^#define\\s*([a-zA-Z_]+)\\s+([0-9]+)\\s*$";

    while ((read1 = getline(&line, &length, file)) != -1) {
        if(!strstr(line, "ZEND_NOP")) continue;
        writeIntoOpcodeHashTable(pattern, line, ht);
        while ((read2 = getline(&line, &length, file)) != -1) {
            writeIntoOpcodeHashTable(pattern, line, ht);
        }
    }

    fclose(file);
    free(line);
    efree(filename);
    TOOLBOX_G(opcode_table) = ht;

    RETURN_ARR(ht);
}

PHP_METHOD(toolbox, setWebsite) 
{
    zend_string *site;

	if (zend_parse_parameters(ZEND_NUM_ARGS(), "S", &site) == FAILURE) {
		return;
	}

    toolbox_object *box = Z_TOOLBOX_OBJECT_P(getThis());

    if(box->website){
        zend_string_release(box->website);
    }

    box->website = site; 
}

PHP_METHOD(toolbox, getWebsite) 
{
    zval zv, *result;
    if (zend_parse_parameters_none() == FAILURE) {
        return;
    }   

    toolbox_object *box = Z_TOOLBOX_OBJECT_P(getThis());

    if (box->website) {
        RETURN_STR(zend_string_copy(box->website));
    }    

    RETURN_EMPTY_STRING();
}


static inline zend_object *toolbox_new(zend_class_entry *ce)
{
	toolbox_object *box = emalloc(sizeof(toolbox_object) + zend_object_properties_size(ce));

	memset(box, 0, XtOffsetOf(toolbox_object, std));
	zend_object_std_init(&box->std, ce);
	box->std.handlers = &toolbox_obj_handlers;

	return &box->std;
}


static inline void toolbox_free(zend_object *object) 
{
	toolbox_object *box = toolbox_fetch_object(object);

    if(box->author){
        zend_string_release(box->author);
    }

    if(box->website){
        zend_string_release(box->website);
    }

    if(box->properties) {
        if(GC_DELREF(box->properties) == 0) {
            GC_REMOVE_FROM_BUFFER(box->properties);
            zend_array_destroy(box->properties);
        }
    }

    zend_object_std_dtor(object);
}


static zval *toolbox_read_property(TOOLBOX_OBJECT_T *obj, void *name, int type, void **cache_slot, zval *zv) 
{
    zval *retval;
	zend_string *member;
	toolbox_object *box = toolbox_fetch_object(TOOLBOX_ZOBJ(obj));

#if PHP_VERSION_ID < 80000
	if (UNEXPECTED(Z_TYPE_P((zval*)name) != IS_STRING)) {
		return &EG(uninitialized_zval);
	}
    member = Z_STR_P((zval*)name);
#else
	member = (zend_string*)name;
#endif

	if (UNEXPECTED(type == BP_VAR_W || type == BP_VAR_RW)) {
		return &EG(error_zval);
	}

	if (zend_string_equals_literal(member, "author")) {
		if (box->author) {
			ZVAL_STR_COPY(zv, box->author);
			return zv;
		}
    } else if (zend_string_equals_literal(member, "website")) {
         zend_error(E_NOTICE, "Cannot access private property: %s", Z_STRVAL_P((zval*)name));
         return &EG(uninitialized_zval);
	} else {
        zend_object_handlers *std_handler = (zend_object_handlers *)zend_get_std_object_handlers();
        retval = std_handler->read_property(obj, name, type, cache_slot, zv);
        return retval;
    }

	return &EG(uninitialized_zval);
}



static TOOLBOX_WHANDLER_T toolbox_write_property(TOOLBOX_OBJECT_T *obj, void *name, zval *value, void **cache_slot)
{
	zend_string *member;
	toolbox_object *box = toolbox_fetch_object(TOOLBOX_ZOBJ(obj));

#if PHP_VERSION_ID < 80000
	if (UNEXPECTED(Z_TYPE_P((zval*)name) != IS_STRING)) {
		TOOLBOX_WHANDLER_RETURN(value);
	}
    member = Z_STR_P((zval*)name);
#else
	member = (zend_string*)name;
#endif

	if (zend_string_equals_literal(member, "author")) {
		if (Z_TYPE_P(value) != IS_STRING) {
			TOOLBOX_WHANDLER_RETURN(value);
		}
		if (box->author) {
			zend_string_release(box->author);
		}
		box->author= zend_string_copy(Z_STR_P(value));
	} else if (zend_string_equals_literal(member, "website")) {
        zend_error(E_NOTICE, "Cannot access private property: %s", Z_STRVAL_P((zval*)name));
	} else {
        zend_object_handlers *std_handler = (zend_object_handlers *)zend_get_std_object_handlers();
        std_handler->write_property(obj, name, value, cache_slot);
    }

	TOOLBOX_WHANDLER_RETURN(value);
}


static HashTable *toolbox_get_properties(TOOLBOX_OBJECT_T *obj) 
{
	zval zv, zarray;
    HashTable *ht;
	toolbox_object *box = toolbox_fetch_object(TOOLBOX_ZOBJ(obj));

    if (!box->properties) {
        ZVAL_NEW_ARR(&zarray);
        box->properties = Z_ARR_P(&zarray);
        zend_hash_init(box->properties, 16, NULL, ZVAL_PTR_DTOR, 0);
        TOOLBOX_ALLOW_VIOLATION(box->properties);
	}

	if (box->author) {
		ZVAL_STR_COPY(&zv, box->author);
	} else {
		ZVAL_NULL(&zv);
	}
	zend_hash_str_update(box->properties, "author", sizeof("author") - 1, &zv);

	if (box->website) {
		ZVAL_STR_COPY(&zv, box->website);
	} else {
		ZVAL_NULL(&zv);
	}
	zend_hash_str_update(box->properties, "website:private", sizeof("website:private") - 1, &zv);


    zend_object_handlers *std_handler = (zend_object_handlers *)zend_get_std_object_handlers();
    ht = std_handler->get_properties(obj);
    zend_hash_copy(ht, box->properties, zval_add_ref);

    return ht;
}


static HashTable *writeIntoOpcodeHashTable(const char *pattern, const char *text, HashTable *ht) 
{
    zval zstr;
    regex_t regex;
    regmatch_t matches[REGEXP_OPCODE_MAX_MATCHES];
    char *opcode_name = NULL;
    char *opcode_value = NULL;
    int ret, i = 0, start = 0, end = 0;

    ret = regcomp(&regex, pattern, REG_EXTENDED);
    if (ret != 0) return NULL;

    while (1) {
        ret = regexec(&regex, text, REGEXP_OPCODE_MAX_MATCHES, matches, 0);
        if(ret != 0) return NULL;

        for (i = 0; i < REGEXP_OPCODE_MAX_MATCHES && start != -1; i++) {
            start = matches[i].rm_so;
            end = matches[i].rm_eo;
            if(i == 1) opcode_name = strndup(text + start, end - start);
            if(i == 2) opcode_value = strndup(text + start, end - start);
        }

        text += end;

        if(opcode_name && opcode_value){
            zend_string *oname = zend_string_init(opcode_name, strlen(opcode_name), 0);
            ZVAL_NEW_STR(&zstr, oname);
            zend_hash_index_update(ht, atoi(opcode_value), &zstr);
        }
    }
    regfree(&regex);

    return ht;
}


PHP_MINIT_FUNCTION(toolbox)
{
    REGISTER_INI_ENTRIES();

	zend_class_entry ce;
	INIT_CLASS_ENTRY(ce, "Toolbox", toolbox_methods);

	toolbox_ce = zend_register_internal_class_ex(&ce, NULL);
	toolbox_ce->create_object = toolbox_new;

	memcpy(&toolbox_obj_handlers, zend_get_std_object_handlers(), sizeof(zend_object_handlers));
	toolbox_obj_handlers.offset = XtOffsetOf(toolbox_object, std);
	toolbox_obj_handlers.clone_obj = NULL;
	toolbox_obj_handlers.free_obj = toolbox_free;
    toolbox_obj_handlers.get_properties = toolbox_get_properties;
    toolbox_obj_handlers.read_property = (zend_object_read_property_t)toolbox_read_property;
    toolbox_obj_handlers.write_property = (zend_object_write_property_t)toolbox_write_property;

    zend_declare_class_constant_string(toolbox_ce, "VERSION", sizeof("VERSION") - 1, "1.0");

	return SUCCESS;
}

PHP_MSHUTDOWN_FUNCTION(toolbox)
{
    UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}


PHP_RINIT_FUNCTION(toolbox)
{
#if defined(COMPILE_DL_TOOLBOX) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}


PHP_RSHUTDOWN_FUNCTION(toolbox)
{
	return SUCCESS;
}


PHP_MINFO_FUNCTION(toolbox)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "toolbox support", "enabled");
	php_info_print_table_end();

	DISPLAY_INI_ENTRIES();
}

const zend_function_entry toolbox_functions[] = {
	PHP_FE(getToolboxVersion,	arginfo_toolbox_getToolboxVersion)		
	PHP_FE_END	
};


zend_module_entry toolbox_module_entry = {
#if ZEND_MODULE_API_NO >= 20050922
    STANDARD_MODULE_HEADER_EX, 
    NULL,
    NULL,
#else
    STANDARD_MODULE_HEADER,
#endif  
	"toolbox",
	toolbox_functions,
	PHP_MINIT(toolbox),
	PHP_MSHUTDOWN(toolbox),
	PHP_RINIT(toolbox),		/* Replace with NULL if there's nothing to do at request start */
	PHP_RSHUTDOWN(toolbox),	/* Replace with NULL if there's nothing to do at request end */
	PHP_MINFO(toolbox),
	PHP_TOOLBOX_VERSION,
    PHP_MODULE_GLOBALS(toolbox),
    PHP_GINIT(toolbox),
    NULL,
    NULL,
	STANDARD_MODULE_PROPERTIES_EX
};


#ifdef COMPILE_DL_TOOLBOX
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(toolbox)
#endif

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
