#include "log.h"
#include "utarray.h"
#include "utstring.h"
#include "unity.h"
#include "s7.h"

#include "sealark.h"
#include "sunlark.h"

#include "test_loadstmts_rm.h"

char *build_file = "test/unit/sunlark/BUILD.loadstmts_rm";

s7_scheme *s7;

/**************/
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_rm_all_loadstmts);
    RUN_TEST(test_rm_all_loadstmts_star);

    RUN_TEST(test_rm_first_loadstmt_by_int);
    RUN_TEST(test_rm_first_loadstmt_by_kwint);
    RUN_TEST(test_rm_first_loadstmt_by_key);

    RUN_TEST(test_rm_last_loadstmt_by_int);
    RUN_TEST(test_rm_last_loadstmt_by_kwint);
    RUN_TEST(test_rm_last_loadstmt_by_key);

    /* ******** rm args from load stmts ******** */
    RUN_TEST(test_loadstmt_rm_all_args);
    RUN_TEST(test_loadstmt_rm_all_args_star);

    RUN_TEST(test_loadstmt_int_rm_first_arg_by_int);
    RUN_TEST(test_loadstmt_int_rm_first_arg_by_kwint);
    RUN_TEST(test_loadstmt_int_rm_first_arg_by_str);

    RUN_TEST(test_loadstmt_kwint_rm_first_arg_by_int);
    RUN_TEST(test_loadstmt_kwint_rm_first_arg_by_kwint);
    RUN_TEST(test_loadstmt_kwint_rm_first_arg_by_str);

    RUN_TEST(test_loadstmt_str_rm_first_arg_by_int);
    RUN_TEST(test_loadstmt_str_rm_first_arg_by_kwint);
    RUN_TEST(test_loadstmt_str_rm_first_arg_by_str);

    /* **************** */
    RUN_TEST(test_loadstmt_int_rm_last_arg_by_int);
    RUN_TEST(test_loadstmt_int_rm_last_arg_by_kwint);
    RUN_TEST(test_loadstmt_int_rm_last_arg_by_str);

    RUN_TEST(test_loadstmt_kwint_rm_last_arg_by_int);
    RUN_TEST(test_loadstmt_kwint_rm_last_arg_by_kwint);
    RUN_TEST(test_loadstmt_kwint_rm_last_arg_by_str);

    RUN_TEST(test_loadstmt_str_rm_last_arg_by_int);
    RUN_TEST(test_loadstmt_str_rm_last_arg_by_kwint);
    RUN_TEST(test_loadstmt_str_rm_last_arg_by_str);

    /* ******** rm ATTRS from load stmts ******** */
    /* RUN_TEST(test_loadstmt_rm_all_attrs); */
    /* RUN_TEST(test_loadstmt_rm_all_attrs_star); */

    RUN_TEST(test_loadstmt_int_rm_first_attr_by_int);
    RUN_TEST(test_loadstmt_int_rm_first_attr_by_kwint);
    RUN_TEST(test_loadstmt_int_rm_first_attr_by_sym);

    RUN_TEST(test_loadstmt_kwint_rm_first_attr_by_int);
    RUN_TEST(test_loadstmt_kwint_rm_first_attr_by_kwint);
    RUN_TEST(test_loadstmt_kwint_rm_first_attr_by_sym);

    RUN_TEST(test_loadstmt_str_rm_first_attr_by_int);
    RUN_TEST(test_loadstmt_str_rm_first_attr_by_kwint);
    RUN_TEST(test_loadstmt_str_rm_first_attr_by_sym);

    /* **************** */
    RUN_TEST(test_loadstmt_int_rm_last_attr_by_int);
    RUN_TEST(test_loadstmt_int_rm_last_attr_by_kwint);
    RUN_TEST(test_loadstmt_int_rm_last_attr_by_sym);

    RUN_TEST(test_loadstmt_kwint_rm_last_attr_by_int);
    RUN_TEST(test_loadstmt_kwint_rm_last_attr_by_kwint);
    RUN_TEST(test_loadstmt_kwint_rm_last_attr_by_sym);

    RUN_TEST(test_loadstmt_str_rm_last_attr_by_int);
    RUN_TEST(test_loadstmt_str_rm_last_attr_by_kwint);
    RUN_TEST(test_loadstmt_str_rm_last_attr_by_sym);

    return UNITY_END();
}

/* **************************************************************** */
void test_rm_all_loadstmts(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);

    char *s = "'(:loadstmts)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    char *getter_s = "'(pkg :loadstmts)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                      s7_list(s7, 2,
                              getter,
                              s7_make_keyword(s7,"null")));

    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 0, s7_list_length(s7, loadstmts) );
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
void test_rm_all_loadstmts_star(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:loadstmts)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    char *getter_s = "'(pkg :loadstmts :*)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 0, s7_list_length(s7, loadstmts) );
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
void test_rm_first_loadstmt_by_int(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:loadstmts)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    char *getter_s = "'(pkg :load 0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, loadstmts) );
    sunlark_dispose(s7,pkg);
}

void test_rm_first_loadstmt_by_kwint(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:loadstmts)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    char *getter_s = "'(pkg :load :0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, loadstmts) );
    sunlark_dispose(s7,pkg);
}

void test_rm_first_loadstmt_by_key(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:loadstmts)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    char *getter_s = "'(pkg :load \"@repoa//pkga:targeta.bzl\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, loadstmts) );
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
void test_rm_last_loadstmt_by_int(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:loadstmts)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    char *getter_s = "'(pkg :load -1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, loadstmts) );
    sunlark_dispose(s7,pkg);
}

void test_rm_last_loadstmt_by_kwint(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:loadstmts)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    char *getter_s = "'(pkg :load :-1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, loadstmts) );
    sunlark_dispose(s7,pkg);
}

void test_rm_last_loadstmt_by_key(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:loadstmts)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    char *getter_s = "'(pkg :load \"@rules_cc//cc:defs.bzl\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(loadstmts) );
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, loadstmts) );
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
void test_loadstmt_rm_all_args(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load :2 :args)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 0, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_rm_all_args_star(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load :2 :arg :*)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
}

/* **************************************************************** */
void test_loadstmt_int_rm_first_arg_by_int(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load by int 2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load 2 :arg 0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_int_rm_first_arg_by_kwint(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load by 2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load 2 :arg :0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_int_rm_first_arg_by_str(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load by int 2 =  "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load 2 :arg \"arg0c\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

/* **************** */
void test_loadstmt_kwint_rm_first_arg_by_int(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load :2 :arg 0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_kwint_rm_first_arg_by_kwint(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load :2 :arg :0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_kwint_rm_first_arg_by_str(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load :2 :arg \"arg0c\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

/* **************** */
void test_loadstmt_str_rm_first_arg_by_int(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load str "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load \"@repoc//pkgc:targetc.bzl\" :arg 0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_str_rm_first_arg_by_kwint(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load str "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load \"@repoc//pkgc:targetc.bzl\" :arg :0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_str_rm_first_arg_by_str(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load str "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load \"@repoc//pkgc:targetc.bzl\" :arg \"arg0c\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

/* **************** */
void test_loadstmt_int_rm_last_arg_by_int(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load 2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load 2 :arg -1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_int_rm_last_arg_by_kwint(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load 2 :arg :-1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_int_rm_last_arg_by_str(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load 2 :arg \"arg2c\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

/* **************** */
void test_loadstmt_kwint_rm_last_arg_by_int(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load :2 :arg -1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_kwint_rm_last_arg_by_kwint(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load :2 :arg :-1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_kwint_rm_last_arg_by_str(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load :2 :arg \"arg2c\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

/* **************** */
void test_loadstmt_str_rm_last_arg_by_int(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load str "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load \"@repoc//pkgc:targetc.bzl\" :arg -1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_str_rm_last_arg_by_kwint(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load \"@repoc//pkgc:targetc.bzl\" :arg :-1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_str_rm_last_arg_by_str(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :args)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load \"@repoc//pkgc:targetc.bzl\" :arg \"arg2c\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
void test_loadstmt_rm_all_attrs(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load :2 :args)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 0, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_rm_all_attrs_star(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load :2 :arg :*)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
}

/* **************************************************************** */
void test_loadstmt_int_rm_first_attr_by_int(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load 2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load 2 :@ 0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang, s7_list(s7, 2,
                                            getter,
                                            s7_make_keyword(s7,"null")));
    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_int_rm_first_attr_by_kwint(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load 2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load 2 :@ :0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang, s7_list(s7, 2,
                                            getter,
                                            s7_make_keyword(s7,"null")));
    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_int_rm_first_attr_by_sym(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load 2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load 2 :@ 'key0c)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang, s7_list(s7, 2,
                                            getter,
                                            s7_make_keyword(s7,"null")));
    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

/* **************** */
void test_loadstmt_kwint_rm_first_attr_by_int(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load :2 :@ 0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_kwint_rm_first_attr_by_kwint(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load :2 :@ :0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_kwint_rm_first_attr_by_sym(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load :2 :@ 'key0c)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

/* **************** */
void test_loadstmt_str_rm_first_attr_by_int(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load str "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load \"@repoc//pkgc:targetc.bzl\" :@ 0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_str_rm_first_attr_by_kwint(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load \"@repoc//pkgc:targetc.bzl\" :@ :0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_str_rm_first_attr_by_sym(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    /* load :2 == "@repoc//pkgc:targetc.bzl" */
    char *getter_s = "'(pkg :load \"@repoc//pkgc:targetc.bzl\" :@ 'key0c)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

/* **************** */
void test_loadstmt_int_rm_last_attr_by_int(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    char *getter_s = "'(pkg :load 2 :@ -1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_int_rm_last_attr_by_kwint(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    char *getter_s = "'(pkg :load 2 :@ :-1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_int_rm_last_attr_by_sym(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    char *getter_s = "'(pkg :load 2 :@ 'key0c)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

/* **************** */
void test_loadstmt_kwint_rm_last_attr_by_int(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    char *getter_s = "'(pkg :load :2 :@ -1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_kwint_rm_last_attr_by_kwint(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    char *getter_s = "'(pkg :load :2 :@ :-1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_kwint_rm_last_attr_by_sym(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    char *getter_s = "'(pkg :load :2 :@ 'key0c)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

/* **************** */
void test_loadstmt_str_rm_last_attr_by_int(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    char *getter_s = "'(pkg :load \"@repoc//pkgc:targetc.bzl\" :@ -1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_str_rm_last_attr_by_kwint(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    char *getter_s = "'(pkg :load \"@repoc//pkgc:targetc.bzl\" :@ :-1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_str_rm_last_attr_by_sym(void)
{
    _parse_pkg();
    char *s = "'(:load \"@repoc//pkgc:targetc.bzl\" :@@)";
    s7_pointer path = s7_eval_c_string(s7, s);
    s7_pointer args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    char *getter_s = "'(pkg :load \"@repoc//pkgc:targetc.bzl\" :@ 'key0c)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                                       s7_list(s7, 2,
                                               getter,
                                               s7_make_keyword(s7,"null")));

    args = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( !s7_is_c_object(args) );
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
LOCAL void _parse_pkg(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
}

void setUp(void) {
    s7 = sunlark_init();
    init_s7_syms(s7);

    /* trap error messages */
    old_port = s7_set_current_error_port(s7, s7_open_output_string(s7));
    if (old_port != s7_nil(s7))
        gc_loc = s7_gc_protect(s7, old_port);

    /* pkg = sunlark_parse_build_file(s7, */
    /*                                s7_list(s7, 1, */
    /*                                        s7_make_string(s7, build_file))); */
    /* s7_define_variable(s7, "pkg", pkg); */
}

void tearDown(void) {
    s7_quit(s7);
}

