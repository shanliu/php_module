/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2018 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author:                                                              |
  +----------------------------------------------------------------------+
*/

/* $Id$ */

#ifndef PHP_LLY_H
#define PHP_LLY_H

// 声明模块的变量.在 lly.c 定义
extern zend_module_entry lly_module_entry;
// 定义模块的变量指针 方便使用,非必须
#define phpext_lly_ptr &lly_module_entry

// 模块的版本号.定义常量方便使用 在 lly.c 定义 lly_module_entry 使用到 非必须
#define PHP_LLY_VERSION "0.1.0"


// 定义函数头部定义 等于 ZEND_API .定义函数时最好用下面的定义
#ifdef PHP_WIN32
#	define PHP_LLY_API __declspec(dllexport)
#elif defined(__GNUC__) && __GNUC__ >= 4
#	define PHP_LLY_API __attribute__ ((visibility("default")))
#else
#	define PHP_LLY_API
#endif


// 线程安全全局变量
// CPP里可以 __thread 方式 但最好还是保持PHP C的风格
ZEND_BEGIN_MODULE_GLOBALS(lly)
	char *global_string;
	char *no_ini_key;
ZEND_END_MODULE_GLOBALS(lly)
//定义全局变量的获取宏,方便使用
#define LLY_G(v) ZEND_MODULE_GLOBALS_ACCESSOR(lly, v)



//线程安全时引入
#ifdef ZTS
#include "TSRM.h"
#endif
// 线程安全时必须要有
#if defined(ZTS) && defined(COMPILE_DL_LLY)
ZEND_TSRMLS_CACHE_EXTERN()
#endif

#endif	/* PHP_LLY_H */


/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
