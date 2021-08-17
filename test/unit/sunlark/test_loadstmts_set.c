#include "log.h"
#include "utarray.h"
#include "utstring.h"
#include "unity.h"
#include "s7.h"

/* we need both APIs for test validation */
#include "sealark.h"
#include "sunlark.h"

#include "test_loadstmts_set.h"

char *build_file = "test/unit/sunlark/BUILD.loadstmts_set";

static s7_pointer loadstmts;

static s7_pointer ld2;
static s7_pointer k;

LOCAL enum splice_type {LIST, VECTOR};

int main(void) {
    UNITY_BEGIN();

    /* replacing loadstmts */
    // (set! (pkg :load <ref>) newload)
    RUN_TEST(test_replace_first_loadstmt_by_int);   //ref  -1
    RUN_TEST(test_replace_first_loadstmt_by_kwint); //ref :-1
    RUN_TEST(test_replace_first_loadstmt_by_str); //"@rules_cc//cc:defs.bzl"

    RUN_TEST(test_replace_loadstmt_by_int);   //ref  2
    RUN_TEST(test_replace_loadstmt_by_kwint); //ref :2
    RUN_TEST(test_replace_loadstmt_by_str); //ref "@repoc//pkgc:targetc.bzl"

    RUN_TEST(test_replace_last_loadstmt_by_int);   //ref  -1
    RUN_TEST(test_replace_last_loadstmt_by_kwint); //ref :-1
    RUN_TEST(test_replace_last_loadstmt_by_str); //"@rules_cc//cc:defs.bzl"

    /* splicing loadsmts */
    RUN_TEST(test_splice_loadstmt_before_first_by_int);   //ref  0
    RUN_TEST(test_splice_loadstmt_before_first_by_kwint); //ref  :0
    RUN_TEST(test_splice_loadstmt_before_first_by_str); // "@repoa//pkga:targeta.bzl"

    RUN_TEST(test_splice_loadstmt_after_last_by_int);   //ref  -1
    RUN_TEST(test_splice_loadstmt_after_last_by_kwint);   //ref  :-1
    RUN_TEST(test_splice_loadstmt_after_last_by_str);   //"@rules_cc//cc:defs.bzl"

    /* replace args */
    RUN_TEST(test_loadstmt_replace_first_arg_by_int);   //ref 0
    RUN_TEST(test_loadstmt_replace_first_arg_by_kwint); //ref :0
    RUN_TEST(test_loadstmt_replace_first_arg_by_str);   //ref "arg0c"

    RUN_TEST(test_loadstmt_replace_last_arg_by_int);   //ref 0
    RUN_TEST(test_loadstmt_replace_last_arg_by_kwint); //ref :0
    RUN_TEST(test_loadstmt_replace_last_arg_by_str);   //ref "arg2c"

    /* splice args */
    RUN_TEST(test_splice_args_before_first_by_int);
    RUN_TEST(test_splice_args_before_first_by_kwint);
    RUN_TEST(test_splice_args_before_first_by_str);

    RUN_TEST(test_splice_args_after_last_by_int);
    RUN_TEST(test_splice_args_after_last_by_kwint);
    RUN_TEST(test_splice_args_after_last_by_str);

    return UNITY_END();
}

/* **************************************************************** */
LOCAL void _load_first_precheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    ld2 = s7_eval_c_string(s7, "(pkg :load 0)");
    struct node_s *ld2_nd = s7_c_object_value(ld2);
    TEST_ASSERT( ld2_nd->tid == TK_Load_Stmt);
    s7_pointer k = s7_apply_function(s7, ld2,
                                      s7_list(s7, 2,
                                              s7_make_keyword(s7, "key"),
                                              s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"@repoa//pkga:targeta.bzl\"",
                             s7_string(k) );
    args = s7_apply_function(s7, ld2,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "args")));
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );

    attrs = s7_apply_function(s7, ld2,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "attrs")));
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 0, s7_list_length(s7, attrs) );
}

LOCAL void _load_first_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    ld2 = s7_eval_c_string(s7, "(pkg :load 0)");
    struct node_s *ld2_nd = s7_c_object_value(ld2);
    TEST_ASSERT( ld2_nd->tid == TK_Load_Stmt);
    k = s7_apply_function(s7, ld2, s7_list(s7, 2,
                                           s7_make_keyword(s7, "key"),
                                           s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"@repo_x//pkg_x:target_x.bzl\"",
                              s7_string(k) );

    args = s7_apply_function(s7, ld2,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "args")));
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );

    attrs = s7_apply_function(s7, ld2,
                              s7_list(s7, 1,
                                      s7_make_keyword(s7, "attrs")));
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, attrs) );
}

/* ******************************************* */
void test_replace_first_loadstmt_by_int(void)
{
    _parse_pkg();
    _load_first_precheck();

    /* make change */
    s7_pointer ld1 = _make_load1();
    char *getter_s = "'(pkg :load 0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                      s7_list(s7, 2, getter, ld1));
    _load_first_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_replace_first_loadstmt_by_kwint(void)
{
    _parse_pkg();
    _load_first_precheck();

    /* make change */
    s7_pointer ld1 = _make_load1();
    char *getter_s = "'(pkg :load :0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                      s7_list(s7, 2, getter, ld1));
    _load_first_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_replace_first_loadstmt_by_str(void)
{
    _parse_pkg();
    _load_first_precheck();

    /* make change */
    s7_pointer ld1 = _make_load1();
    char *getter_s = "'(pkg :load \"@repoa//pkga:targeta.bzl\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                      s7_list(s7, 2, getter, ld1));
    _load_first_postcheck();
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
LOCAL void _load_c_precheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    ld2 = s7_eval_c_string(s7, "(pkg :load 2)");
    struct node_s *ld2_nd = s7_c_object_value(ld2);
    TEST_ASSERT( ld2_nd->tid == TK_Load_Stmt);
    s7_pointer k = s7_apply_function(s7, ld2,
                                      s7_list(s7, 2,
                                              s7_make_keyword(s7, "key"),
                                              s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"@repoc//pkgc:targetc.bzl\"",
                              s7_string(k) );
    args = s7_apply_function(s7, ld2,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "args")));
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    attrs = s7_apply_function(s7, ld2,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "attrs")));
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, attrs) );
}

LOCAL void _load_c_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    ld2 = s7_eval_c_string(s7, "(pkg :load 2)");
    struct node_s *ld2_nd = s7_c_object_value(ld2);
    TEST_ASSERT( ld2_nd->tid == TK_Load_Stmt);
    k = s7_apply_function(s7, ld2, s7_list(s7, 2,
                                           s7_make_keyword(s7, "key"),
                                           s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"@repo_x//pkg_x:target_x.bzl\"",
                              s7_string(k) );

    args = s7_apply_function(s7, ld2,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "args")));
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );

    attrs = s7_apply_function(s7, ld2,
                              s7_list(s7, 1,
                                      s7_make_keyword(s7, "attrs")));
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, attrs) );
}

/* **************************************************************** */
void test_replace_loadstmt_by_int(void)
{
    // (set! (pkg :load 2) ld1)
    _parse_pkg();
    _load_c_precheck();

    /* make change */
    s7_pointer ld1 = _make_load1();
    char *getter_s = "'(pkg :load 2)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                      s7_list(s7, 2, getter, ld1));
    _load_c_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_replace_loadstmt_by_kwint(void)
{
    // (set! (pkg :load 2) ld1)
    _parse_pkg();
    _load_c_precheck();

    /* make change */
    s7_pointer ld1 = _make_load1();
    char *getter_s = "'(pkg :load :2)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                      s7_list(s7, 2, getter, ld1));
    _load_c_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_replace_loadstmt_by_str(void)
{
    // (set! (pkg :load 2) ld1)
    _parse_pkg();
    _load_c_precheck();

    /* make change */
    s7_pointer ld1 = _make_load1();
    char *getter_s = "'(pkg :load \"@repoc//pkgc:targetc.bzl\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                      s7_list(s7, 2, getter, ld1));
    _load_c_postcheck();
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
LOCAL void _load_last_precheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    ld2 = s7_eval_c_string(s7, "(pkg :load -1)");
    struct node_s *ld2_nd = s7_c_object_value(ld2);
    TEST_ASSERT( ld2_nd->tid == TK_Load_Stmt);
    s7_pointer k = s7_apply_function(s7, ld2,
                                      s7_list(s7, 2,
                                              s7_make_keyword(s7, "key"),
                                              s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"@rules_cc//cc:defs.bzl\"",
                              s7_string(k) );
    args = s7_apply_function(s7, ld2,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "args")));
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    attrs = s7_apply_function(s7, ld2,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "attrs")));
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 0, s7_list_length(s7, attrs) );
}

LOCAL void _load_last_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    ld2 = s7_eval_c_string(s7, "(pkg :load -1)");
    struct node_s *ld2_nd = s7_c_object_value(ld2);
    TEST_ASSERT( ld2_nd->tid == TK_Load_Stmt);
    k = s7_apply_function(s7, ld2, s7_list(s7, 2,
                                           s7_make_keyword(s7, "key"),
                                           s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"@repo_x//pkg_x:target_x.bzl\"",
                              s7_string(k) );

    args = s7_apply_function(s7, ld2,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "args")));
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );

    attrs = s7_apply_function(s7, ld2,
                              s7_list(s7, 1,
                                      s7_make_keyword(s7, "attrs")));
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, attrs) );
}

/* **************************************************************** */
void test_replace_last_loadstmt_by_int(void)
{
    // (set! (pkg :load -1) ld1)
    _parse_pkg();
    _load_last_precheck();

    /* make change */
    s7_pointer ld1 = _make_load1();
    char *getter_s = "'(pkg :load -1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                      s7_list(s7, 2, getter, ld1));
    _load_last_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_replace_last_loadstmt_by_kwint(void)
{
    // (set! (pkg :load 2) ld1)
    _parse_pkg();
    _load_last_precheck();

    /* make change */
    s7_pointer ld1 = _make_load1();
    char *getter_s = "'(pkg :load :-1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                      s7_list(s7, 2, getter, ld1));
    _load_last_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_replace_last_loadstmt_by_str(void)
{
    // (set! (pkg :load 2) ld1)
    _parse_pkg();
    _load_last_precheck();

    /* make change */
    s7_pointer ld1 = _make_load1();
    char *getter_s = "'(pkg :load \"@rules_cc//cc:defs.bzl\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_apply_function(s7, set_bang,
                      s7_list(s7, 2, getter, ld1));
    _load_last_postcheck();
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
/* **************************************************************** */
LOCAL void _splice_before_first_precheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    ld2 = s7_eval_c_string(s7, "(pkg :load 0)");
    struct node_s *ld2_nd = s7_c_object_value(ld2);
    TEST_ASSERT( ld2_nd->tid == TK_Load_Stmt);
    s7_pointer k = s7_apply_function(s7, ld2,
                                      s7_list(s7, 2,
                                              s7_make_keyword(s7, "key"),
                                              s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"@repoa//pkga:targeta.bzl\"",
                              s7_string(k) );
    args = s7_apply_function(s7, ld2,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "args")));
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );

    attrs = s7_apply_function(s7, ld2,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "attrs")));
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 0, s7_list_length(s7, attrs) );
}

LOCAL void _splice_before_first_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 6, s7_list_length(s7, loadstmts) );

    s7_pointer ld0 = s7_eval_c_string(s7, "(pkg :load 0)");
    struct node_s *ld0_nd = s7_c_object_value(ld0);
    TEST_ASSERT( ld0_nd->tid == TK_Load_Stmt);
    s7_pointer k0 = s7_apply_function(s7, ld0, s7_list(s7, 2,
                                           s7_make_keyword(s7, "key"),
                                           s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"@repo_x//pkg_x:target_x.bzl\"",
                              s7_string(k0) );

    args = s7_apply_function(s7, ld0,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "args")));
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );

    attrs = s7_apply_function(s7, ld0,
                              s7_list(s7, 1,
                                      s7_make_keyword(s7, "attrs")));
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, attrs) );

    /* ******************************** */
    s7_pointer ld1 = s7_eval_c_string(s7, "(pkg :load 1)");
    struct node_s *ld1_nd = s7_c_object_value(ld1);
    TEST_ASSERT( ld1_nd->tid == TK_Load_Stmt);
    s7_pointer k1 = s7_apply_function(s7, ld1,
                                       s7_list(s7, 2,
                                               s7_make_keyword(s7, "key"),
                                               s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"@repo_y//pkg_y:target_y.bzl\"",
                              s7_string(k1) );

    args = s7_apply_function(s7, ld1,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "args")));
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );

    attrs = s7_apply_function(s7, ld1,
                              s7_list(s7, 1,
                                      s7_make_keyword(s7, "attrs")));
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, attrs) );

    /* ******************************** */
    s7_pointer ld2 = s7_eval_c_string(s7, "(pkg :load 2)");
    struct node_s *ld2_nd = s7_c_object_value(ld2);
    TEST_ASSERT( ld2_nd->tid == TK_Load_Stmt);
    s7_pointer k2 = s7_apply_function(s7, ld2, s7_list(s7, 2,
                                           s7_make_keyword(s7, "key"),
                                           s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"@repoa//pkga:targeta.bzl\"",
                              s7_string(k2) );

}

/* **************************************************************** */
void test_splice_loadstmt_before_first_by_int(void)
{
    // (set! (pkg :load :0) (list ld1 ld2))
    _parse_pkg();
    _splice_before_first_precheck();

    /* make change */
    char *getter_s = "'(pkg :load 0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_splice(LIST);
    s7_pointer x = s7_list(s7, 2, getter, splice);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _splice_before_first_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_splice_loadstmt_before_first_by_kwint(void)
{
    _parse_pkg();
    _splice_before_first_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_splice(LIST);
    s7_apply_function(s7, set_bang,
                      s7_list(s7, 2, getter, splice));
    _splice_before_first_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_splice_loadstmt_before_first_by_str(void)
{
    _parse_pkg();
    _splice_before_first_precheck();

    /* make change */
    char *getter_s = "'(pkg :load \"@repoa//pkga:targeta.bzl\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_splice(LIST);
    s7_apply_function(s7, set_bang,
                      s7_list(s7, 2, getter, splice));
    _splice_before_first_postcheck();
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
/* **************************************************************** */
LOCAL void _splice_after_last_precheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    ld2 = s7_eval_c_string(s7, "(pkg :load 0)");
    struct node_s *ld2_nd = s7_c_object_value(ld2);
    TEST_ASSERT( ld2_nd->tid == TK_Load_Stmt);
    s7_pointer k = s7_apply_function(s7, ld2,
                                      s7_list(s7, 2,
                                              s7_make_keyword(s7, "key"),
                                              s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"@repoa//pkga:targeta.bzl\"",
                              s7_string(k) );
    args = s7_apply_function(s7, ld2,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "args")));
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );

    attrs = s7_apply_function(s7, ld2,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "attrs")));
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 0, s7_list_length(s7, attrs) );
}

LOCAL void _splice_after_last_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 6, s7_list_length(s7, loadstmts) );

    /* idx -3 was -1 before splicing */
    s7_pointer ld0 = s7_eval_c_string(s7, "(pkg :load -3)");
    struct node_s *ld0_nd = s7_c_object_value(ld0);
    TEST_ASSERT( ld0_nd->tid == TK_Load_Stmt);
    s7_pointer k0 = s7_apply_function(s7, ld0, s7_list(s7, 2,
                                           s7_make_keyword(s7, "key"),
                                           s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"@rules_cc//cc:defs.bzl\"",
                              s7_string(k0) );

    args = s7_apply_function(s7, ld0,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "args")));
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, args) );

    attrs = s7_apply_function(s7, ld0,
                              s7_list(s7, 1,
                                      s7_make_keyword(s7, "attrs")));
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 0, s7_list_length(s7, attrs) );

    /* ******************************** */
    s7_pointer ld1 = s7_eval_c_string(s7, "(pkg :load -2)");
    struct node_s *ld1_nd = s7_c_object_value(ld1);
    TEST_ASSERT( ld1_nd->tid == TK_Load_Stmt);
    s7_pointer k1 = s7_apply_function(s7, ld1,
                                       s7_list(s7, 2,
                                               s7_make_keyword(s7, "key"),
                                               s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"@repo_x//pkg_x:target_x.bzl\"",
                              s7_string(k1) );

    args = s7_apply_function(s7, ld1,
                             s7_list(s7, 1,
                                     s7_make_keyword(s7, "args")));
    TEST_ASSERT( s7_is_list(s7, args) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, args) );

    attrs = s7_apply_function(s7, ld1,
                              s7_list(s7, 1,
                                      s7_make_keyword(s7, "attrs")));
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 2, s7_list_length(s7, attrs) );

    /* ******************************** */
    s7_pointer ld2 = s7_eval_c_string(s7, "(pkg :load -1)");
    struct node_s *ld2_nd = s7_c_object_value(ld2);
    TEST_ASSERT( ld2_nd->tid == TK_Load_Stmt);
    s7_pointer k2 = s7_apply_function(s7, ld2, s7_list(s7, 2,
                                           s7_make_keyword(s7, "key"),
                                           s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"@repo_y//pkg_y:target_y.bzl\"",
                              s7_string(k2) );

}

/* **************************************************************** */
void test_splice_loadstmt_after_last_by_int(void)
{
    // (set! (pkg :load :0) (list ld1 ld2))
    _parse_pkg();
    _splice_after_last_precheck();

    /* make change */
    char *getter_s = "'(pkg :load -1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_splice(VECTOR);
    s7_pointer x = s7_list(s7, 2, getter, splice);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _splice_after_last_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_splice_loadstmt_after_last_by_kwint(void)
{
    _parse_pkg();
    _splice_after_last_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :-1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_splice(VECTOR);
    s7_apply_function(s7, set_bang,
                      s7_list(s7, 2, getter, splice));
    _splice_after_last_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_splice_loadstmt_after_last_by_str(void)
{
    _parse_pkg();
    _splice_after_last_precheck();

    /* make change */
    char *getter_s = "'(pkg :load \"@rules_cc//cc:defs.bzl\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_splice(VECTOR);
    s7_apply_function(s7, set_bang,
                      s7_list(s7, 2, getter, splice));
    _splice_after_last_postcheck();
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
/* **************************************************************** */
LOCAL void _replace_first_arg_precheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    arg = s7_eval_c_string(s7, "(pkg :load :2 :arg :0)");
    struct node_s *arg_nd = s7_c_object_value(arg);
    TEST_ASSERT( arg_nd->tid == TK_STRING);
    s7_pointer k = s7_apply_function(s7, arg,
                                      s7_list(s7, 1,
                                              s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"arg0c\"",
                              s7_string(k) );
}

LOCAL void _replace_first_arg_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    arg = s7_eval_c_string(s7, "(pkg :load :2 :arg :0)");
    struct node_s *arg_nd = s7_c_object_value(arg);
    TEST_ASSERT( arg_nd->tid == TK_STRING);
    s7_pointer k = s7_apply_function(s7, arg,
                                      s7_list(s7, 1,
                                              s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"hello\"",
                              s7_string(k) );
}

/* **************************************************************** */
void test_loadstmt_replace_first_arg_by_int(void)
{
    // (set! (pkg :load :2 :arg 0) "hello")
    _parse_pkg();
    _replace_first_arg_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :arg 0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter,
                           s7_make_string(s7, "hello"));
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _replace_first_arg_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_replace_first_arg_by_kwint(void)
{
    // (set! (pkg :load :2 :arg :0) "hello")
    _parse_pkg();
    _replace_first_arg_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :arg :0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter,
                           s7_make_string(s7, "hello"));
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _replace_first_arg_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_replace_first_arg_by_str(void)
{
    _parse_pkg();
    _replace_first_arg_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :arg \"arg0c\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter,
                           s7_make_string(s7, "hello"));
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _replace_first_arg_postcheck();
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
/* **************************************************************** */
LOCAL void _replace_last_arg_precheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    arg = s7_eval_c_string(s7, "(pkg :load :2 :arg :-1)");
    struct node_s *arg_nd = s7_c_object_value(arg);
    TEST_ASSERT( arg_nd->tid == TK_STRING);
    s7_pointer k = s7_apply_function(s7, arg,
                                      s7_list(s7, 1,
                                              s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"arg2c\"",
                              s7_string(k) );
}

LOCAL void _replace_last_arg_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    arg = s7_eval_c_string(s7, "(pkg :load :2 :arg :-1)");
    struct node_s *arg_nd = s7_c_object_value(arg);
    TEST_ASSERT( arg_nd->tid == TK_STRING);
    s7_pointer k = s7_apply_function(s7, arg,
                                      s7_list(s7, 1,
                                              s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"hello\"",
                              s7_string(k) );
}

/* **************************************************************** */
void test_loadstmt_replace_last_arg_by_int(void)
{
    _parse_pkg();
    _replace_last_arg_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :arg -1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter,
                           s7_make_string(s7, "hello"));
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _replace_last_arg_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_replace_last_arg_by_kwint(void)
{
    _parse_pkg();
    _replace_last_arg_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :arg :-1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter,
                           s7_make_string(s7, "hello"));
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _replace_last_arg_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_replace_last_arg_by_str(void)
{
    _parse_pkg();
    _replace_last_arg_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :arg \"arg2c\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter,
                           s7_make_string(s7, "hello"));
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _replace_last_arg_postcheck();
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
/* **************************************************************** */
LOCAL void _splice_args_before_first_precheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    arg = s7_eval_c_string(s7, "(pkg :load :2 :arg :-1)");
    struct node_s *arg_nd = s7_c_object_value(arg);
    TEST_ASSERT( arg_nd->tid == TK_STRING);
    s7_pointer k = s7_apply_function(s7, arg,
                                      s7_list(s7, 1,
                                              s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"arg2c\"",
                              s7_string(k) );
}

LOCAL void _splice_args_before_first_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    arg = s7_eval_c_string(s7, "(pkg :load :2 :arg :0)");
    struct node_s *arg_nd = s7_c_object_value(arg);
    TEST_ASSERT( arg_nd->tid == TK_STRING);
    s7_pointer k = s7_apply_function(s7, arg,
                                      s7_list(s7, 1,
                                              s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"hello\"",
                              s7_string(k) );

    arg = s7_eval_c_string(s7, "(pkg :load :2 :arg :1)");
    arg_nd = s7_c_object_value(arg);
    TEST_ASSERT( arg_nd->tid == TK_STRING);
    k = s7_apply_function(s7, arg,
                          s7_list(s7, 1,
                                  s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"goodbye\"",
                              s7_string(k) );

    arg = s7_eval_c_string(s7, "(pkg :load :2 :arg :2)");
    arg_nd = s7_c_object_value(arg);
    TEST_ASSERT( arg_nd->tid == TK_STRING);
    k = s7_apply_function(s7, arg,
                                      s7_list(s7, 1,
                                              s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"arg0c\"",
                              s7_string(k) );
}

/* **************************************************************** */
void test_splice_args_before_first_by_int(void)
{
    _parse_pkg();
    _splice_args_before_first_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :arg 0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = s7_list(s7, 3,
                                s7_make_symbol(s7, "list"),
                                s7_make_string(s7, "hello"),
                                s7_make_string(s7, "goodbye"));
    s7_pointer x = s7_list(s7, 2, getter, splice);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _splice_args_before_first_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_splice_args_before_first_by_kwint(void)
{
    _parse_pkg();
    _splice_args_before_first_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :arg :0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = s7_list(s7, 3,
                                s7_make_symbol(s7, "list"),
                                s7_make_string(s7, "hello"),
                                s7_make_string(s7, "goodbye"));
    s7_pointer x = s7_list(s7, 2, getter, splice);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _splice_args_before_first_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_splice_args_before_first_by_str(void)
{
    _parse_pkg();
    _splice_args_before_first_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :arg \"arg0c\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
   s7_pointer splice = s7_list(s7, 3,
                                s7_make_symbol(s7, "list"),
                                s7_make_string(s7, "hello"),
                                s7_make_string(s7, "goodbye"));
    s7_pointer x = s7_list(s7, 2, getter, splice);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _splice_args_before_first_postcheck();
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
/* **************************************************************** */
LOCAL void _splice_args_after_last_precheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    s7_pointer arg_ct = s7_eval_c_string(s7, "(pkg :load :2 :args)");
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, arg_ct) );

    arg = s7_eval_c_string(s7, "(pkg :load :2 :arg :-1)");
    s7_pointer k;
    k = s7_apply_function(s7, arg, s7_list(s7, 1, s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"arg2c\"", s7_string(k) );
}

LOCAL void _splice_args_after_last_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    s7_pointer arg_ct = s7_eval_c_string(s7, "(pkg :load :2 :args)");
    TEST_ASSERT_EQUAL_INT( 5, s7_list_length(s7, arg_ct) );

    arg = s7_eval_c_string(s7, "(pkg :load :2 :arg :-3)");
    s7_pointer k;
    k = s7_apply_function(s7, arg, s7_list(s7, 1, s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"arg2c\"", s7_string(k) );

    arg = s7_eval_c_string(s7, "(pkg :load :2 :arg :-2)");
    k = s7_apply_function(s7, arg, s7_list(s7, 1, s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"hello\"", s7_string(k) );

    arg = s7_eval_c_string(s7, "(pkg :load :2 :arg :-1)");
    k = s7_apply_function(s7, arg, s7_list(s7, 1, s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"goodbye\"", s7_string(k) );
}

s7_pointer _make_splice_vector(void)
{
    s7_pointer splice = s7_make_vector(s7, 2);
    s7_vector_set(s7, splice, 0, s7_make_string(s7, "hello"));
    s7_vector_set(s7, splice, 1, s7_make_string(s7, "goodbye"));
    return splice;
}

/* **************************************************************** */
void test_splice_args_after_last_by_int(void)
{
    _parse_pkg();
    _splice_args_after_last_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :arg -1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_splice_vector();
    s7_pointer x = s7_list(s7, 2, getter, splice);
    s7_apply_function(s7, set_bang, x);

    /* struct node_s *p = s7_c_object_value(pkg); */
    _splice_args_after_last_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_splice_args_after_last_by_kwint(void)
{
    _parse_pkg();
    _splice_args_after_last_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :arg :-1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_splice_vector();
    s7_pointer x = s7_list(s7, 2, getter, splice);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _splice_args_after_last_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_splice_args_after_last_by_str(void)
{
    _parse_pkg();
    _splice_args_after_last_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :arg \"arg2c\")";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_splice_vector();
    s7_pointer x = s7_list(s7, 2, getter, splice);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _splice_args_after_last_postcheck();
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
/* **************************************************************** */
LOCAL s7_pointer _make_load1(void)
{
    char *s =
        "(make-load \"@repo_x//pkg_x:target_x.bzl\" "
        "  :args '(\"a\" \"b\") "
        "  :attrs (list #@(key1 \"val1\") #@(key2 \"val2\")))";

    s7_pointer ld1 = s7_eval_c_string(s7, s);
    return ld1;
}

LOCAL s7_pointer _make_splice(int typ)
{
    char *s1 =
        "(make-load \"@repo_x//pkg_x:target_x.bzl\" "
        "  :args '(\"xa\" \"xb\") "
        "  :attrs (list (make-binding 'xkey1 \"xval1\") "
        "               (make-binding 'Xkey2 \"xval2\")))";
    s7_pointer ld1 = s7_eval_c_string(s7, s1);

    char *s2 =
        "(make-load \"@repo_y//pkg_y:target_y.bzl\" "
        "  :args '(\"ya\" \"yb\") "
        "  :attrs (list (make-binding 'ykey1 \"yval1\") "
        "         (make-binding 'ykey2 \"yval2\")))";
    s7_pointer ld2 = s7_eval_c_string(s7, s2);

    /* 'splice' will be evaluated; we want it to eval to as list, not
       as a fncall; so we produce the list (list ld1 ld1) rather than
       the list (ld1 ld2) */
    s7_pointer splice;
    if (typ == LIST)
        splice = s7_list(s7, 3, s7_make_symbol(s7, "list"),
                         ld1, ld2);
    else {
        splice = s7_make_vector(s7, 2);
        s7_vector_set(s7, splice, 0, ld1);
        s7_vector_set(s7, splice, 1, ld2);
    }
    return splice;
}

LOCAL void _parse_pkg(void)
{
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
}

/* **************************************************************** */
LOCAL s7_pointer _error_handler(s7_scheme *sc, s7_pointer args)
{
  fprintf(stdout, "error: %s\n", s7_string(s7_car(args)));
  return(s7_f(sc));
}

void setUp(void) {
    s7 = sunlark_init();
    init_s7_syms(s7);

    /* trap error messages */
    /*old_port = s7_set_current_error_port(s7, s7_open_output_string(s7));*/
    /* if (old_port != s7_nil(s7)) */
    /*     gc_loc = s7_gc_protect(s7, old_port); */

    /* s7_define_function(s7, "error-handler", */
    /*                    _error_handler, 1, 0, false, */
    /*                    "our error handler"); */
}

void tearDown(void) {
    /* s7_close_output_port(s7, s7_current_error_port(s7)); */
    /* s7_set_current_error_port(s7, old_port); */
    /* if (gc_loc != -1) */
    /*     s7_gc_unprotect_at(s7, gc_loc); */

    s7_quit(s7);
}
