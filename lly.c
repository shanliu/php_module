#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <Zend/zend_interfaces.h>
#include <Zend/zend_smart_str.h>
#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_lly.h"

/*
 *. 函数定义 入参-出参 调用
 *. 类[属性 方法 (修饰)]定义 继承 接口 静态方法 调用 参数 调用
 *. 对象建立 调用
 *. 字符串处理
 *. 数组-遍历-修改-操作
 *. zval 转换 convert_* 系列函数 更多操作类参见 zend_operators.h
 *. 资源句柄封装
 *. 全局变量
 */

//线程安全全局变量和INI使用
//1 需要一个默认模块变量结构 由 ZEND_BEGIN_MODULE_GLOBALS 定义 一般在头文件中
// ZEND_BEGIN_MODULE_GLOBALS(lly)
//2 根据上面定义结构定义全局变量
ZEND_DECLARE_MODULE_GLOBALS(lly)
//3 定义INI修改时回调函数 可无
PHP_INI_MH(OnUpdateSeparator) {
    LLY_G(global_string) = ZSTR_VAL(new_value);
    return SUCCESS;
}
//4 关联INI 全局变量关系 及设置默认值
PHP_INI_BEGIN()
    STD_PHP_INI_ENTRY("global_string","testval",  PHP_INI_ALL, OnUpdateString, global_string, zend_lly_globals, lly_globals)
PHP_INI_END();
//5 在初始化时注册 REGISTER_INI_ENTRIES 关闭时需要取消注册
//5 未关联INI的全局变量通过回调函数初始化
static void php_lly_init_globals(zend_lly_globals *aaa_globals)
{
    aaa_globals->no_ini_key = 1000;
}
static void php_lly_remove_globals(zend_lly_globals *aaa_globals)
{
    //移除时调用
}
//php_lly_init_globals 和 php_lly_remove_globals的默认注册调用
//static PHP_GINIT_FUNCTION(lly)
//{
//	lly_globals->global_string = "ccccc";
//}
//static PHP_GSHUTDOWN_FUNCTION(lly)
//{
//
//}


//接口
//参数定义名,占位,是否返回引用,必要参数数量
//ZEND_BEGIN_ARG_INFO_EX(arg_say_hello, 0,0,1)
//    //是否是引用,参数名
//    ZEND_ARG_INFO(1, str) //使用引用传参
//ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_INFO(arg_say_hello, 0)
	ZEND_ARG_INFO(0, name)
ZEND_END_ARG_INFO()

zend_class_entry *i_myinterface_ce;
//结构方法 跟抽象方法定义一致
static zend_function_entry i_myinterface_method[] = {
	ZEND_ABSTRACT_ME(i_myinterface, hello, arg_say_hello)
	PHP_FE_END
};
//类
zend_class_entry  *ab_class_ce,*parent_class_ce, *sub_class_ce;

//定义函数的参数构成
ZEND_BEGIN_ARG_INFO(arg_say_hello1, 0)
    ZEND_ARG_OBJ_INFO(0, otherHash, ddddd\\parent_class, 0)
ZEND_END_ARG_INFO()


//抽象方法定义
static zend_function_entry ab_class_ce_entry[] = {
	ZEND_ABSTRACT_ME(ab_class_ce, hello1, arg_say_hello1)
	PHP_FE_END
};


ZEND_METHOD(parent_class, hello)
{
	zend_string* str;
	zval *object;
	if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OS",&object,parent_class_ce,&str) == FAILURE) {
		RETURN_FALSE;
	}

	php_printf("%s",ZSTR_VAL(str));
	char * text="fddd";
//	zend_string * out;
//	out=zend_string_init(text,strlen(text),0);
//	RETURN_STR(out);
	RETURN_STRING(text);
}





//类函数定义
ZEND_METHOD(parent_class, __construct)
{
    zend_string* str;
    zval *object;
    //类方法获取参数
    if (zend_parse_method_parameters(ZEND_NUM_ARGS() TSRMLS_CC, getThis(), "OS",&object,parent_class_ce,&str) == FAILURE) {
        RETURN_FALSE;
    }

   // php_printf("%s",ZSTR_VAL(str));
}



//!!! 内部调用返回值需要先申请存放数据空间
//EG(v);//全局变量 executor_globals 如$_GLOBALS[EG(symbol_table),地址:EG(active_symbol_table)
//PG(v);//核心变量 php_core_globals 如:$_GET $_POST .. PG(http_globals)[TRACK_VARS_*],INI信息
//SG(v);//SAPI变量 请求数据 sapi_globals_struct 如:HTTP原始请求变量 sapi_request_info
//CG(v);//编译变量 compiler_globals 可以得到函数表,类表
//EX(v);//当前执行数据 zend_execute_data 可以获取到当前执行的函数,类,OPCODE等 EG()元素
//OG(v);//输出变量 output_globals 结构:ZEND_BEGIN_MODULE_GLOBALS(output)

ZEND_METHOD(sub_class, call_hello)
{


	zval val;
	//返回==&val
    zend_call_method_with_0_params(getThis(), sub_class_ce, NULL, "hello", &val);
    zend_string_release(Z_STR(val));//字符串用完,需要手动清理掉


	//函数调用 内外函数均可
    zval retval,handle,params[2];
    ZVAL_STRING(&handle,"substr_count");
    ZVAL_STRING(&params[0],"1111100011111");
    ZVAL_STRING(&params[1],"1");
    if(call_user_function(EG(function_table), NULL, &handle, &retval, 2, params)== SUCCESS){
        php_printf("%d",Z_LVAL_P(&retval));
    }
    zend_string_release(Z_STR(handle));
    zend_string_release(Z_STR(params[0]));
    zend_string_release(Z_STR(params[1]));


    //  RETURN_FALSE;



//  字符串加减 非常方便..
//    smart_str buf = {0};
//    smart_str_appends(&buf, "Array\n");
//    smart_str_0(&buf);
//    //buf.s;//zend_string;
//    //buf.a//string length
//    zend_string *str = buf.s;
//    php_printf(ZSTR_VAL(str));
//    smart_str_free(&buf);//释放内存

    //数组操作 下面有更详细示例
	zval ret;
	array_init(&ret);//会申请内存,使用完清理掉
	//zend_array_destroy(Z_ARR(ret));//清理整个数组数据
	zend_string * out=zend_string_init("aaa",strlen("aaa"),0);
	zend_string * out1=zend_string_init("aaaaaa",strlen("aaaaaa"),0);
	add_next_index_str(&ret,out);
	add_assoc_str(&ret,"bbb",out1);

    zend_string_release(out);
    zend_string_release(out1);

// php_var_dump // var_dump
//	zend_print_zval_r(&ret,0);//print_r

//遍历数组 下面有通过宏的方式遍历 建议用下面方式
//	int i=0, count = zend_hash_num_elements(Z_ARRVAL(ret));
//	zend_hash_internal_pointer_reset(Z_ARRVAL(ret));
//	zval *z_item;
//	for (i = 0; i < count; i ++) {
//		zend_string* key;
//		zend_ulong idx;
//		z_item=zend_hash_get_current_data(Z_ARRVAL(ret));
//		convert_to_string_ex(z_item);
//		if (zend_hash_get_current_key(Z_ARRVAL(ret), &key, &idx) == HASH_KEY_IS_STRING) {
//			php_printf("array[%s] = %s\n", ZSTR_VAL(key), ZSTR_VAL(Z_STR_P(z_item)));
//		} else {
//			php_printf("array[%d] = %s\n", idx, Z_STRVAL_P(z_item));
//		}
//		zend_hash_move_forward(Z_ARRVAL(ret));
//	}
	RETURN_ZVAL(&ret,1,1);
}

ZEND_METHOD(sub_class, hello)
{
	RETURN_STRING("DDDD");
}
//类函数定义
static zend_function_entry parent_class_method[] = {
	ZEND_ME(parent_class, hello, arg_say_hello, ZEND_ACC_PUBLIC)
    ZEND_ME(parent_class, __construct, arg_say_hello, ZEND_ACC_PUBLIC)
	PHP_FE_END
};
//类函数定义
static zend_function_entry sub_class_method[]={
	ZEND_ME(sub_class, call_hello, NULL, ZEND_ACC_PUBLIC)
	ZEND_ME(sub_class, hello, arg_say_hello, ZEND_ACC_PUBLIC)
	PHP_FE_END
};


//两个参数 EX(call),zval(null) EX结构为 _zend_execute_data
PHP_FUNCTION(confirm_lly_compiled) // 等于 zif_confirm_lly_compiled(_zend_execute_data call,zval retval)
{
// 函数方式得到参数
//	zend_string * str;
//	if (zend_parse_parameters(ZEND_NUM_ARGS() TSRMLS_CC, "S",&str) == FAILURE) {
//		return;
//	}
// 宏方式得到参数 官方建议尽量用这个方式
//	zend_string *parameter;
//	zend_long options = 0;
//	ZEND_PARSE_PARAMETERS_START(1, 2)
//			Z_PARAM_STR(str)
//			Z_PARAM_OPTIONAL
//			Z_PARAM_LONG(options)
//	ZEND_PARSE_PARAMETERS_END();

//	php_printf("%s",ZSTR_VAL(str));


    zval object_zval;
    zend_function *constructor;
    if (UNEXPECTED(object_init_ex(&object_zval, parent_class_ce) != SUCCESS)) {//对象生成 类似NEW 但属性不会有 也不会调用构造函数
        RETURN_FALSE;
    }
    //Z_OBJ_HT(object_zval) handlers 魔术构造函数的原型指针 可以用来判断是否存在
    constructor = Z_OBJ_HT(object_zval)->get_constructor(Z_OBJ(object_zval));
    if (constructor == NULL) {
     //   php_printf("no controller");
    }else {
		zval retval;
		zval param;
		ZVAL_STRING(&param,"??");
		zend_string_release(Z_STR(param));
		//传入 fn_proxy 后 function_name 被忽略
		zend_call_method_with_1_params(&object_zval, parent_class_ce, &parent_class_ce->constructor, ZEND_CONSTRUCTOR_FUNC_NAME, &retval,&param);
	}

    //更新或设置对象的属性 类似:$obj->var=aa;
	zend_update_property_string(parent_class_ce,&object_zval,"public_var",strlen("public_var"),"dddddddddd" TSRMLS_DC);
	zval *p;

	//读取对象属性 类似:$a=$obj->var;
	//参数:类,对象,属性名,属性名长度,不存在属性等是否不出错误提示,__get函数时zval存放
	p=zend_read_property(parent_class_ce,&object_zval,"public_var",strlen("public_var"),1,NULL TSRMLS_DC);
	php_printf(ZSTR_VAL(Z_STR_P(p)));

//	RETURN_ZVAL(&object_zval,1,1);

    //3个参数以内的方法对象方法调用有封装好的快捷函数
 //   zval param;
 //   ZVAL_STRING(&param,"----");
	//zend_call_method_with_1_params(&object_zval, parent_class_ce, NULL, "hello",&return_value,&param);


//   如果 参数为`回调函数` 可以如下方式调用
//    zend_fcall_info fci;
//    zend_fcall_info_cache fci_cache;
//    if (zend_parse_parameters(ZEND_NUM_ARGS(), "f*", &fci, &fci_cache, &fci.params, &fci.param_count) == FAILURE) {
//        return;
//    }
//    zval retval;
//    fci.retval = &retval;
//    if (zend_call_function(&fci, &fci_cache) == SUCCESS && Z_TYPE(retval) != IS_UNDEF) {
//        ZVAL_COPY_VALUE(return_value, &retval);
//    } else {
//        RETURN_FALSE;
//    }


    //普通函数调用
//    zval retval1;
//    zval params1[1];
//    ZVAL_STRING(&params1[0],"FASDFSAD");
//    zend_fcall_info fcif;
//    fcif.size = sizeof(fcif);
//    fcif.object = NULL;//必须为NULL
//    fcif.retval = &retval1;//返回,必须有,不能为NULL
//    fcif.param_count = 1;//参数数量
//    fcif.params = params1;//参数列表
//    fcif.no_separation = 1;
//    ZVAL_STRINGL(&fcif.function_name, "md5", strlen("md5"));
//    if (zend_call_function(&fcif, NULL) == FAILURE) {
//        zend_error_noreturn(E_CORE_ERROR, "call function error");
//    }
//    zend_string_release(Z_STR(params1[0]));
//    zval_ptr_dtor(&fcif.function_name);
//    //php_printf("%s",ZSTR_VAL(Z_STR(retval1)));
//    zval_ptr_dtor(&retval1);



    //调用对象方法
//    zval retval;
//    zval params[2];
//    ZVAL_BOOL(&params[0],1);
//    ZVAL_BOOL(&params[1],0);
//    zend_fcall_info fci;
//    fci.size = sizeof(fci);
//    fci.object =  Z_OBJ(object_zval);//调用对象
//    fci.retval = &retval;//返回值,必须有 先申请内存,不能为NULL
//    fci.param_count = 1;//参数数量
//    fci.params = params;//参数列表
//    fci.no_separation = 1;
//    zend_fcall_info_cache fcic;
//    ZVAL_UNDEF(&fci.function_name);//未使用
//    fcic.initialized = 1;//初始化
//    fcic.function_handler = &parent_class_ce->constructor;//调用函数,以下通过函数名查找
////    fcic.function_handler = zend_hash_str_find_ptr(
////            &lsorm_entity_class_entry_ptr->function_table, "entityname", strlen("entityname"));
////    if (fcic.function_handler == NULL) {
////        zend_error_noreturn(E_CORE_ERROR, "Couldn't find  method %s", "entityname");
////    }
//    fcic.calling_scope = parent_class_ce;//调用类
//    fcic.called_scope = Z_OBJCE(object_zval);//实际类
//    fcic.object = Z_OBJ(object_zval);//调用对象
//    if (zend_call_function(&fci, &fcic) == FAILURE) {
//        zend_error_noreturn(E_CORE_ERROR, "call function error");
//    }
//    zval_ptr_dtor(&retval);

    //调用类的静态方法
//    zval retval;
//    zend_fcall_info fci;
//    fci.size = sizeof(fci);
//    fci.object =  NULL;
//    fci.retval = &retval;
//    fci.param_count = 0;
//    fci.params = NULL;
//    fci.no_separation = 1;
//    ZVAL_UNDEF(&fci.function_name);
//    zend_fcall_info_cache fcic;
//    fcic.initialized = 1;//初始化
//    fcic.function_handler = zend_hash_str_find_ptr(
//            &parent_class_ce->function_table, "labels", strlen("labels"));
//    if (fcic.function_handler == NULL) {
//        zend_error_noreturn(E_CORE_ERROR, "Couldn't find  method %s", "entityname");
//    }
//    fcic.calling_scope = parent_class_ce;//调用类
//    fcic.called_scope = parent_class_ce;//实际类
//    fcic.object = NULL;//调用对象
//    if (zend_call_function(&fci,&fcic) == FAILURE) {
//        zend_error_noreturn(E_CORE_ERROR, "call entity constructor method fail");
//    }
//    zval_ptr_dtor(&retval);


}
/* }}} */


void aaa(zval *as){
    //打印zval用的函数 调试时使用非常方便 就是print_r 实现
    zend_print_zval_r(as,0);
}


PHP_FUNCTION(mytest111)
{
    //数组(hashtable)的使用
	zval ret;
	array_init(&ret);
	zend_string * out=zend_string_init(ZEND_STRL("bbb"),0);
	zend_string * out1=zend_string_init(ZEND_STRL("?????"),0);
	zend_string * out3=zend_string_init(ZEND_STRL("++++++"),0);
	//往数组添加数据
    add_assoc_string(&ret,"ssss","bbbb");
	add_assoc_str(&ret,"bbb",out1);
	add_assoc_str(&ret,"bbb1",out3);
    add_assoc_str(&ret,"bbbss",out);
    //直接操作HASH表方式
    zval htm;
    ZVAL_STRING(&htm,"00000000");
    zend_hash_str_add(Z_ARR(ret), "b22", sizeof("b22")-1,&htm);
    //修改hashtable数据 zend_api没在进一步封装这个
    zval t;
    ZVAL_STRING(&t,"update");
    //zend_hash_update(Z_ARR(ret),out,&t);// !!!千万不要出现把同个zend_string指针同时设置为KEY和VALUE的情况
    zend_hash_str_update(Z_ARR(ret), "bbb1", sizeof("bbb1")-1,&t);
    //删除hashtable数据 zend_api没在进一步封装这个
    zend_hash_str_del(Z_ARR(ret),ZEND_STRL("bbbss"));//注意:删除会连带释放对应的zval 如果删除 bbb1 下面出错
    zend_print_zval_r(&ret,0);
    if(!zend_hash_str_exists(Z_ARR(ret),ZEND_STRL("bbbss"))){
        php_printf("\n--------del----------\n");
    }
    //获取hashtable数据
    zval *s=zend_hash_str_find(Z_ARR(ret),ZEND_STRL("bbb1"));
    if(s)zend_print_zval_r(s,0);

    php_printf("\n---------------------\n");

    //遍历hashtable数据 都有相关的宏
    zval *val;
    ZEND_HASH_FOREACH_VAL(Z_ARR(ret), val) {// 遍历val :key需要是字符串
        zend_print_zval_r(val,0);
        php_printf("\n");
    } ZEND_HASH_FOREACH_END();
    php_printf("---------------------\n");
    zend_string * key;
    ZEND_HASH_FOREACH_STR_KEY(Z_ARR(ret), key) {//遍历KEY
        php_printf("%s",ZSTR_VAL(key));
        php_printf("\n");
    } ZEND_HASH_FOREACH_END();
    php_printf("---------------------\n");
    ZEND_HASH_FOREACH_STR_KEY_VAL(Z_ARR(ret),key,val) {//遍历KEY
        php_printf("key:%s\nval:",ZSTR_VAL(key));
        zend_print_zval_r(val,0);
        php_printf("\n");
    } ZEND_HASH_FOREACH_END();
    php_printf("---------------------\n");

    //使用完清理
	zend_array_destroy(Z_ARR(ret));


    zval ret1;
    array_init(&ret1);
    add_index_string(&ret1,10,"aaaa");//类似$a[]的方式添加数据
    add_next_index_string(&ret1,"aaaa");
    add_next_index_string(&ret1,"aaaa");
    add_next_index_string(&ret1,"aaaa");
    add_next_index_string(&ret1,"aaaa");
    add_next_index_string(&ret1,"aaaa");
    add_next_index_string(&ret1,"aaaa");
    add_next_index_string(&ret1,"aaaa");
    add_next_index_string(&ret1,"aaaa");
    zend_print_zval_r(&ret1,0);
    php_printf("%d",zend_hash_num_elements(Z_ARR(ret1)));
    zend_array_destroy(Z_ARR(ret1));

}


void print_hashtable(zend_array * ht){
    //打印hashtable 方法 前提是hashtable里存放的是zval 调试用
    zval a;
    ZVAL_ARR(&a,ht);
    zend_print_zval_r(&a,0);
}


PHP_FUNCTION(mytest112)
{
    //hashtable 独立使用 如果hashtable里存放非zval 通过以下方式使用
    zend_array *ht;
    ALLOC_HASHTABLE(ht);//必须先分配内存
    zend_hash_init(ht,0,NULL,ZVAL_PTR_DTOR,0);
    char * a=(char*)emalloc(100);//非ZVAL得变量
    zend_hash_str_add_ptr(ht, "b22", sizeof("b22")-1,&a);
    zval *s=zend_hash_str_find_ptr(ht,ZEND_STRL("b22"));
    if(a==Z_PTR_P(s))php_printf("ddd");
    efree(Z_PTR_P(s));
    zend_array_destroy(ht);//释放 OR
//    FREE_HASHTABLE(ht);
//    efree(a);
}


//返回资源列表句柄
static int le_result;
ZEND_RSRC_DTOR_FUNC(myfile_dtor)
{//统一释放资源回调函数
    FILE *fp = (FILE *) res->ptr;
    fclose(fp);
}
PHP_FUNCTION(mytest113)
{
    FILE *fp=fopen("/tmp/aa","ab");
    zval *zv;
    //把资源注册进PHP
    zv = zend_list_insert(fp, le_result);
    RETURN_ZVAL(zv,0,0);
}
//使用资源句柄参数
PHP_FUNCTION(mytest114)
{
    zval *result;
    ZEND_PARSE_PARAMETERS_START(1,1)
            Z_PARAM_RESOURCE(result)
    ZEND_PARSE_PARAMETERS_END_EX(RETURN_FALSE);
    FILE *f;
    //根据zend_resource得到实际的资源句柄
    if ((f = (FILE *)zend_fetch_resource(Z_RES_P(result), "result", le_result)) == NULL)
    {
        RETURN_FALSE;
    }
    fwrite(ZEND_STRL("DDDD"),1,f);
    //把资源从资源列表中删除
    if (zend_list_delete(Z_RES_P(result)) == FAILURE) {
        RETURN_FALSE;
    }
    RETURN_TRUE;
}


void list_dtr(void * ptr){
    //释放指向的字符串的内存
   efree(*((char**)ptr));
}
void list_call(void * ptr){
    //对每个元素进行回调
    php_printf(*((char**)ptr));
}

//内置列表使用
PHP_FUNCTION(mytest115)
{
//  备注:指针是可以内存拷贝的
//    char * t1 =(char*) malloc(4);
//    strlcpy(t1,"aaa",4);
//    void * t2 = malloc(sizeof(char *));
//    memcpy(t2,&t1,sizeof(char *));
//    char ** t3=(char**)t2;
//    printf("%s",*t3);
//    free(t1);
//    free(t2);

    //注意: llist 会对添加元素进行内存拷贝
    //以下是将存储字符串指针,复杂数据类型时可放结构体(非结构体指针)
    zend_llist l;
    zend_llist_init(&l,sizeof(char *),list_dtr,0);
    char * a=(char*)emalloc(sizeof(char)*10);
    strlcpy(a,"aaa",4);
    zend_llist_add_element(&l,&a);
    zend_llist_apply(&l,list_call);//
    zend_llist_destroy(&l);
}



//堆栈使用
PHP_FUNCTION(mytest116)
{
    zend_stack st;
    zend_stack_init(&st, sizeof(char *));
    char *t="fasdfas0";
    zend_stack_push(&st,&t);
    char *t1="fasdfas1";
    zend_stack_push(&st,&t1);//进栈
    void * outt=zend_stack_top(&st);//取栈顶
    char ** b=(char**)outt;
    zend_stack_del_top(&st);//出栈
    zend_stack_del_top(&st);//出栈
    printf("%s,%d",*b,zend_stack_count(&st));
    zend_stack_destroy(&st);
}

//字符串 数字转换
PHP_FUNCTION(mytest117)
{
    int decpt,sign;
    //mode
    //0 1 保留有效数字并转为字符串
    //2 四舍五入取整数
    //3 指定小数点后位数并四舍五入取整数(ndigits 指定数量)
    //4 保留有效数字(ndigits 指定数量)
    //5 好像和3类似(ndigits 指定数量)
    //decpt 整数位数 为有效数字 这个值为9999
    char *p = zend_dtoa(500.1121545, 6, 4, &decpt, &sign, NULL);
    printf("%s,%d,%d\n",p,decpt,sign);

    //字符串转浮点数
    double ss=zend_bin_strtod("11",NULL);
    printf("%f\n",ss);

    zend_freedtoa(p);


    //注意:字符串跟整数转换用 zend_long.h 里面的宏

    //整数转字符串
    char t[5];
    ZEND_LTOA(1000,t,5);
    printf("%s\n",t);

    //字符串转整数
    int a;
    ZEND_ATOL(a,"888");
    printf("%d\n",a);
    printf("%lld\n",ZEND_STRTOL("1000",NULL,2));
}

//全局变量获取
PHP_FUNCTION(mytest118)
{
    //全局变量
    php_printf("%s,%d",LLY_G(global_string),LLY_G(no_ini_key));
}




//以下为模块的钩子函数定义
PHP_MINIT_FUNCTION(lly) //加载扩展时调用
{
    //建立一个用于存储的资源用的资源列表 第一个参数每次请求完成回调 第二个参数PHP退出后回调
    le_result = zend_register_list_destructors_ex(myfile_dtor,NULL,"standard-c-file", module_number);

    //注册常量
    REGISTER_STRING_CONSTANT("TTT","DDD", CONST_CS | CONST_PERSISTENT);

    //全局变量注册和初始化
    //初始化INI关联的全局变量
    REGISTER_INI_ENTRIES();
    //未关联INI的全局变量注册
    ZEND_INIT_MODULE_GLOBALS(lly, php_lly_init_globals, php_lly_remove_globals);



    //模块加载时候可以进行类注册 函数会自动注册

    //定义类变量
	zend_class_entry s_ce, a_ce,p_ce, i_ce;
	INIT_CLASS_ENTRY(i_ce, "i_myinterface", i_myinterface_method);//通过 i_myinterface_method 定义类 i_ce

    //注册接口
	i_myinterface_ce = zend_register_internal_interface(&i_ce TSRMLS_CC);//把i_ce 注册到全局类表 会拷贝一份 所以注册 i_ce 就没用了

	//注册类，实现接口用 :zend_class_implements
	INIT_CLASS_ENTRY(a_ce, "absclass", ab_class_ce_entry);
	ab_class_ce = zend_register_internal_class(&a_ce TSRMLS_CC);

    //继承类实现 初始化
	INIT_CLASS_ENTRY(p_ce, "parent_class", parent_class_method);
	//带命名空间类初始化
	INIT_NS_CLASS_ENTRY(p_ce,"ddddd","parent_class",parent_class_method);
	parent_class_ce = zend_register_internal_class(&p_ce TSRMLS_CC);

	//实现接口 注册
	zend_class_implements(parent_class_ce TSRMLS_CC, 1, i_myinterface_ce);

	//定义类属性
	zend_declare_property_string(parent_class_ce, "public_var", strlen("public_var"),"dddd", ZEND_ACC_PUBLIC TSRMLS_CC);
//	zend_declare_property_null(parent_class_ce, "public_var", strlen("public_var"), ZEND_ACC_PUBLIC TSRMLS_CC);
	zend_declare_class_constant_long(parent_class_ce, "aaa", 3,100 TSRMLS_DC);

	// 注册子类，使用zend_register_internal_class_ex函数
	INIT_CLASS_ENTRY(s_ce, "sub_class", sub_class_method);
	sub_class_ce = zend_register_internal_class_ex(&s_ce, parent_class_ce TSRMLS_CC);

	return SUCCESS;
}


/* {{{ PHP_MSHUTDOWN_FUNCTION
 */
PHP_MSHUTDOWN_FUNCTION(lly)
{
	//  取消注册INI 释放INI的相关内存
	UNREGISTER_INI_ENTRIES();

	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request start */
/* {{{ PHP_RINIT_FUNCTION
 */
PHP_RINIT_FUNCTION(lly)
{//请求开始调用
    //如开发框架时候 这里进行请求时的一些请求相关的赋值
#if defined(COMPILE_DL_LLY) && defined(ZTS)
	ZEND_TSRMLS_CACHE_UPDATE();
#endif
	return SUCCESS;
}
/* }}} */

/* Remove if there's nothing to do at request end */
/* {{{ PHP_RSHUTDOWN_FUNCTION
 */
PHP_RSHUTDOWN_FUNCTION(lly)
{
    //请求结束调用
    //如果有每次请求的相关变量 这里清理
	return SUCCESS;
}
/* }}} */

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(lly)
{
	php_info_print_table_start();
	php_info_print_table_header(2, "lly support", "enabled");
	php_info_print_table_end();
	/* 打印INI的信息
	DISPLAY_INI_ENTRIES();
	*/
}
/* }}} */

/* {{{ lly_functions[]
 * 注册扩展的函数 PHP_FE 第二个参数为参数定义
 * Every user visible function must have an entry in lly_functions[].
 */
const zend_function_entry lly_functions[] = {
	PHP_FE(confirm_lly_compiled,	NULL)		/* For testing, remove later. */
	PHP_FE(mytest111,	NULL)		/* For testing, remove later. */
	PHP_FE(mytest112,	NULL)		/* For testing, remove later. */
    PHP_FE(mytest113,	NULL)		/* For testing, remove later. */
    PHP_FE(mytest114,	NULL)		/* For testing, remove later. */
    PHP_FE(mytest115,	NULL)		/* For testing, remove later. */
    PHP_FE(mytest116,	NULL)		/* For testing, remove later. */
    PHP_FE(mytest117,	NULL)		/* For testing, remove later. */
    PHP_FE(mytest118,	NULL)		/* For testing, remove later. */
	PHP_FE_END	/* Must be the last line in lly_functions[] */
};
/* }}} */

// 定义扩展依赖 本扩展依赖那些扩展 这里定义
//static const zend_module_dep deps[] = {
//	ZEND_MOD_REQUIRED("pdo")
//	ZEND_MOD_END
//};


/* {{{ lly_module_entry
 */
zend_module_entry lly_module_entry = {
	STANDARD_MODULE_HEADER,// 如果要使用依赖 用这个代替=> STANDARD_MODULE_HEADER_EX, NULL, deps,
	"lly",
	lly_functions,
	PHP_MINIT(lly),  /* 扩展注册时调用*/
	PHP_MSHUTDOWN(lly), /*扩展卸载时调用*/
	PHP_RINIT(lly),		/* 请求开始时调用 没有定义为NULL */
	PHP_RSHUTDOWN(lly),	/* 请求结束时调用 没有定义为NULL*/
	PHP_MINFO(lly),
	PHP_LLY_VERSION,
//    PHP_MODULE_GLOBALS(lly),//作用:注册到全局变量中
//    PHP_GINIT(lly),//全局变量初始化时调用函数,一般为启动PHP时,可为NULL
//    PHP_GSHUTDOWN(lly),//全局变量销毁时调用函数,一般为关闭PHP时,可为NULL
//    NULL,
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

//这个必须保留 定义注册扩展时的调用函数
#ifdef COMPILE_DL_LLY
#ifdef ZTS
ZEND_TSRMLS_CACHE_DEFINE()
#endif
ZEND_GET_MODULE(lly)
#endif





/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
