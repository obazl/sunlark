#include "log.h"
#include "utarray.h"
#include "utstring.h"
#include "unity.h"
#include "s7.h"

/* we need both APIs for test validation */
#include "sealark.h"
#include "sunlark.h"

#include "test_loadstmts_set_attrs.h"

char *build_file = "test/unit/sunlark/BUILD.loadstmts_set";

static s7_pointer loadstmts;

static s7_pointer ld2;
static s7_pointer k;

LOCAL enum splice_type {LIST, VECTOR};

int main(void) {
    UNITY_BEGIN();

    /* replace first attr key */
    RUN_TEST(test_loadstmt_first_attr_by_int_replace_key);
    RUN_TEST(test_loadstmt_first_attr_by_kwint_replace_key);
    RUN_TEST(test_loadstmt_first_attr_by_sym_replace_key);

    /* replace last attr key */
    RUN_TEST(test_loadstmt_last_attr_by_int_replace_key);
    RUN_TEST(test_loadstmt_last_attr_by_kwint_replace_key);
    RUN_TEST(test_loadstmt_last_attr_by_sym_replace_key);

    /* replace first attr val */
    RUN_TEST(test_loadstmt_first_attr_by_int_replace_val);
    RUN_TEST(test_loadstmt_first_attr_by_kwint_replace_val);
    RUN_TEST(test_loadstmt_first_attr_by_sym_replace_val);

    /* /\* replace last attr val *\/ */
    RUN_TEST(test_loadstmt_last_attr_by_int_replace_val);
    RUN_TEST(test_loadstmt_last_attr_by_kwint_replace_val);
    RUN_TEST(test_loadstmt_last_attr_by_sym_replace_val);

    /* replace entire attr */
    RUN_TEST(test_loadstmt_replace_first_attr_by_int);   //ref 0
    RUN_TEST(test_loadstmt_replace_first_attr_by_kwint); //ref :0
    RUN_TEST(test_loadstmt_replace_first_attr_by_str);   //ref "arg0c"

    RUN_TEST(test_loadstmt_replace_last_attr_by_int);   //ref 0
    RUN_TEST(test_loadstmt_replace_last_attr_by_kwint); //ref :0
    RUN_TEST(test_loadstmt_replace_last_attr_by_str);   //ref "arg2c"

    /* splice attrs */
    RUN_TEST(test_splice_attrs_before_first_by_int);
    RUN_TEST(test_splice_attrs_before_first_by_kwint);
    RUN_TEST(test_splice_attrs_before_first_by_str);

    RUN_TEST(test_splice_attrs_after_last_by_int);
    RUN_TEST(test_splice_attrs_after_last_by_kwint);
    RUN_TEST(test_splice_attrs_after_last_by_str);

    return UNITY_END();
}

/* **************************************************************** */
/* **************************************************************** */
LOCAL void _first_attr_precheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    attrs = s7_eval_c_string(s7, "(pkg :load :2 :@@)");
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, attrs) );

    attr = s7_eval_c_string(s7, "(pkg :load :2 :attr :0)");
    struct node_s *attr_nd = s7_c_object_value(attr);
    TEST_ASSERT(attr_nd->tid == TK_Binding);

    /* attr keys are syms (TK_ID) */
    key = s7_apply_function(s7, attr,
                                     s7_list(s7, 1,
                                             s7_make_keyword(s7, "key")));
    TEST_ASSERT( sunlark_node_tid(s7, key) == TK_ID );
    key = s7_apply_function(s7, key, s7_list(s7, 1,
                                         s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "key0c", s7_symbol_name(key));

    /* attr value - in this case, a string */
    val = s7_apply_function(s7, attr,
                            s7_list(s7, 1,
                                    s7_make_keyword(s7, "value")));
    TEST_ASSERT( sunlark_node_tid(s7, val) == TK_STRING );
    val = s7_apply_function(s7, val, s7_list(s7, 1,
                                             s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"val0c\"", s7_string(val));
}

/* **************************************************************** */
LOCAL void _last_attr_precheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    attrs = s7_eval_c_string(s7, "(pkg :load :2 :@@)");
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, attrs) );

    attr = s7_eval_c_string(s7, "(pkg :load :2 :attr :-1)");
    struct node_s *attr_nd = s7_c_object_value(attr);
    TEST_ASSERT(attr_nd->tid == TK_Binding);

    /* attr keys are syms (TK_ID) */
    key = s7_apply_function(s7, attr,
                            s7_list(s7, 1,
                                    s7_make_keyword(s7, "key")));
    TEST_ASSERT( sunlark_node_tid(s7, key) == TK_ID );
    key = s7_apply_function(s7, key, s7_list(s7, 1,
                                             s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "lastkey2c", s7_symbol_name(key));

    /* attr value - in this case, a string */
    val = s7_apply_function(s7, attr,
                            s7_list(s7, 1,
                                    s7_make_keyword(s7, "value")));
    TEST_ASSERT( sunlark_node_tid(s7, val) == TK_STRING );
    val = s7_apply_function(s7, val, s7_list(s7, 1,
                                             s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"lastval2c\"", s7_string(val));
}

/* **************************************************************** */
LOCAL void _replace_first_attr_key_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    attrs = s7_eval_c_string(s7, "(pkg :load :2 :@@)");
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, attrs) );

    attr = s7_eval_c_string(s7, "(pkg :load :2 :attr :0)");
    struct node_s *attr_nd = s7_c_object_value(attr);
    TEST_ASSERT(attr_nd->tid == TK_Binding);

    /* attr keys are syms (TK_ID) */
    key = s7_apply_function(s7, attr,
                            s7_list(s7, 1, s7_make_keyword(s7, "key")));
    TEST_ASSERT( sunlark_node_tid(s7, key) == TK_ID );
    key = s7_apply_function(s7, key, s7_list(s7, 1,
                                         s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "newkey", s7_symbol_name(key));

    /* attr value - in this case, a string */
    val = s7_apply_function(s7, attr,
                            s7_list(s7, 1,
                                    s7_make_keyword(s7, "value")));
    TEST_ASSERT( sunlark_node_tid(s7, val) == TK_STRING );
    val = s7_apply_function(s7, val, s7_list(s7, 1,
                                             s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"val0c\"", s7_string(val));
}

/* **************************************************************** */
void test_loadstmt_first_attr_by_int_replace_key(void)
{
    _parse_pkg();
    _first_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ 0 :key)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter, s7_make_string(s7, "newkey"));
    s7_apply_function(s7, set_bang, x);
    _replace_first_attr_key_postcheck();

    sunlark_dispose(s7,pkg);
}

void test_loadstmt_first_attr_by_kwint_replace_key(void)
{
    _parse_pkg();
    _first_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ :0 :key)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter, s7_make_string(s7, "newkey"));
    s7_apply_function(s7, set_bang, x);
    _replace_first_attr_key_postcheck();

    sunlark_dispose(s7,pkg);
}

void test_loadstmt_first_attr_by_sym_replace_key(void)
{
    _parse_pkg();
    _first_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ 'key0c :key)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter, s7_make_string(s7, "newkey"));
    s7_apply_function(s7, set_bang, x);
    _replace_first_attr_key_postcheck();

    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
LOCAL void _replace_last_attr_key_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    attrs = s7_eval_c_string(s7, "(pkg :load :2 :@@)");
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, attrs) );

    attr = s7_eval_c_string(s7, "(pkg :load :2 :attr :-1)");
    struct node_s *attr_nd = s7_c_object_value(attr);
    TEST_ASSERT(attr_nd->tid == TK_Binding);

    /* attr keys are syms (TK_ID) */
    key = s7_apply_function(s7, attr,
                            s7_list(s7, 1, s7_make_keyword(s7, "key")));
    TEST_ASSERT( sunlark_node_tid(s7, key) == TK_ID );
    key = s7_apply_function(s7, key, s7_list(s7, 1,
                                         s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "newkey", s7_symbol_name(key));

    /* attr value - in this case, a string */
    val = s7_apply_function(s7, attr,
                            s7_list(s7, 1,
                                    s7_make_keyword(s7, "value")));
    TEST_ASSERT( sunlark_node_tid(s7, val) == TK_STRING );
    val = s7_apply_function(s7, val, s7_list(s7, 1,
                                             s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"lastval2c\"", s7_string(val));
}

/* **************************************************************** */
void test_loadstmt_last_attr_by_int_replace_key(void)
{
    _parse_pkg();
    _last_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ -1 :key)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter, s7_make_string(s7, "newkey"));
    s7_apply_function(s7, set_bang, x);
    _replace_last_attr_key_postcheck();

    sunlark_dispose(s7,pkg);
}

void test_loadstmt_last_attr_by_kwint_replace_key(void)
{
    _parse_pkg();
    _last_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ :-1 :key)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter, s7_make_string(s7, "newkey"));
    s7_apply_function(s7, set_bang, x);
    _replace_last_attr_key_postcheck();

    sunlark_dispose(s7,pkg);
}

void test_loadstmt_last_attr_by_sym_replace_key(void)
{
    _parse_pkg();
    _last_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ 'lastkey2c :key)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter, s7_make_string(s7, "newkey"));
    s7_apply_function(s7, set_bang, x);
    _replace_last_attr_key_postcheck();

    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
/*  REPLACE VAL */
/* **************************************************************** */
LOCAL void _replace_first_attr_val_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    attrs = s7_eval_c_string(s7, "(pkg :load :2 :@@)");
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, attrs) );

    attr = s7_eval_c_string(s7, "(pkg :load :2 :attr :0)");
    struct node_s *attr_nd = s7_c_object_value(attr);
    TEST_ASSERT(attr_nd->tid == TK_Binding);

    /* attr keys are syms (TK_ID) */
    key = s7_apply_function(s7, attr,
                            s7_list(s7, 1, s7_make_keyword(s7, "key")));
    TEST_ASSERT( sunlark_node_tid(s7, key) == TK_ID );
    key = s7_apply_function(s7, key, s7_list(s7, 1,
                                         s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "key0c", s7_symbol_name(key));

    /* attr value - in this case, a string */
    val = s7_apply_function(s7, attr,
                            s7_list(s7, 1,
                                    s7_make_keyword(s7, "value")));
    TEST_ASSERT( sunlark_node_tid(s7, val) == TK_STRING );
    val = s7_apply_function(s7, val, s7_list(s7, 1,
                                             s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"newval\"", s7_string(val));
}

/* **************************************************************** */
void test_loadstmt_first_attr_by_int_replace_val(void)
{
    _parse_pkg();
    _first_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ 0 :value)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter, s7_make_string(s7, "newval"));
    s7_apply_function(s7, set_bang, x);
    _replace_first_attr_val_postcheck();

    sunlark_dispose(s7,pkg);
}

void test_loadstmt_first_attr_by_kwint_replace_val(void)
{
    _parse_pkg();
    _first_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ :0 :value)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter, s7_make_string(s7, "newval"));
    s7_apply_function(s7, set_bang, x);
    _replace_first_attr_val_postcheck();

    sunlark_dispose(s7,pkg);
}

void test_loadstmt_first_attr_by_sym_replace_val(void)
{
    _parse_pkg();
    _first_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ 'key0c :value)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter, s7_make_string(s7, "newval"));
    s7_apply_function(s7, set_bang, x);
    _replace_first_attr_val_postcheck();

    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
LOCAL void _replace_last_attr_val_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    attrs = s7_eval_c_string(s7, "(pkg :load :2 :@@)");
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, attrs) );

    attr = s7_eval_c_string(s7, "(pkg :load :2 :attr :-1)");
    struct node_s *attr_nd = s7_c_object_value(attr);
    TEST_ASSERT(attr_nd->tid == TK_Binding);

    /* attr keys are syms (TK_ID) */
    key = s7_apply_function(s7, attr,
                            s7_list(s7, 1, s7_make_keyword(s7, "key")));
    TEST_ASSERT( sunlark_node_tid(s7, key) == TK_ID );
    key = s7_apply_function(s7, key, s7_list(s7, 1,
                                         s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "lastkey2c", s7_symbol_name(key));

    /* attr value - in this case, a string */
    val = s7_apply_function(s7, attr,
                            s7_list(s7, 1,
                                    s7_make_keyword(s7, "value")));
    TEST_ASSERT( sunlark_node_tid(s7, val) == TK_STRING );
    val = s7_apply_function(s7, val, s7_list(s7, 1,
                                             s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"newval\"", s7_string(val));
}

/* **************************************************************** */
void test_loadstmt_last_attr_by_int_replace_val(void)
{
    _parse_pkg();
    _last_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ -1 :value)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter, s7_make_string(s7, "newval"));
    s7_apply_function(s7, set_bang, x);
    _replace_last_attr_val_postcheck();

    sunlark_dispose(s7,pkg);
}

void test_loadstmt_last_attr_by_kwint_replace_val(void)
{
    _parse_pkg();
    _last_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ :-1 :value)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter, s7_make_string(s7, "newval"));
    s7_apply_function(s7, set_bang, x);
    _replace_last_attr_val_postcheck();

    sunlark_dispose(s7,pkg);
}

void test_loadstmt_last_attr_by_sym_replace_val(void)
{
    _parse_pkg();
    _last_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ 'lastkey2c :value)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer x = s7_list(s7, 2, getter, s7_make_string(s7, "newval"));
    s7_apply_function(s7, set_bang, x);
    _replace_last_attr_val_postcheck();

    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
/*                     REPLACE ENTIRE ATTR
/* **************************************************************** */
LOCAL void _replace_first_attr_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    attrs = s7_eval_c_string(s7, "(pkg :load :2 :@@)");
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, attrs) );

    attr = s7_eval_c_string(s7, "(pkg :load :2 :attr :0)");
    struct node_s *attr_nd = s7_c_object_value(attr);
    TEST_ASSERT(attr_nd->tid == TK_Binding);

    /* attr keys are syms (TK_ID) */
    key = s7_apply_function(s7, attr,
                            s7_list(s7, 1, s7_make_keyword(s7, "key")));
    TEST_ASSERT( sunlark_node_tid(s7, key) == TK_ID );
    key = s7_apply_function(s7, key, s7_list(s7, 1,
                                         s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "newkey1", s7_symbol_name(key));

    /* attr value - in this case, a string */
    val = s7_apply_function(s7, attr,
                            s7_list(s7, 1,
                                    s7_make_keyword(s7, "value")));
    TEST_ASSERT( sunlark_node_tid(s7, val) == TK_STRING );
    val = s7_apply_function(s7, val, s7_list(s7, 1,
                                             s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"newval1\"", s7_string(val));
}

/* **************** */
/* FIXME: write unit tests to verify error checking, e.g.: */
    /* char *getter_s = "'(pkg :load :2 :@ 0)"; */
    /* s7_pointer getter = s7_eval_c_string(s7, getter_s); */
    /* s7_pointer x = s7_list(s7, 2, getter, */
    /*                        s7_make_string(s7, "hello")); */
    /* s7_apply_function(s7, set_bang, x); */
    /* errmsg = s7_get_output_string(s7, s7_current_error_port(s7)); */
    /* if ((errmsg) && (*errmsg)) { */
    /*     log_error("ERROR [%s]", errmsg); */
    /*     TEST_ASSERT( false ); */
    /* } */

/* **************************************************************** */
void test_loadstmt_replace_first_attr_by_int(void)
{
    _parse_pkg();
    _first_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ 0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    char *newval_s = "(make-binding 'newkey1 \"newval1\")";
    s7_pointer newval = s7_eval_c_string(s7, newval_s);
    s7_pointer x = s7_list(s7, 2, getter, newval);
    s7_apply_function(s7, set_bang, x);
    _replace_first_attr_postcheck();

    sunlark_dispose(s7,pkg);
}

void test_loadstmt_replace_first_attr_by_kwint(void)
{
    // (set! (pkg :load :2 :attr :0) "hello")
    _parse_pkg();
    _first_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ :0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    char *newval_s = "(make-binding 'newkey1 \"newval1\")";
    s7_pointer newval = s7_eval_c_string(s7, newval_s);
    s7_pointer x = s7_list(s7, 2, getter, newval);
    s7_apply_function(s7, set_bang, x);
    _replace_first_attr_postcheck();

    sunlark_dispose(s7,pkg);
}

void test_loadstmt_replace_first_attr_by_str(void)
{
    _parse_pkg();
    _first_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ 'key0c)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    char *newval_s = "(make-binding 'newkey1 \"newval1\")";
    s7_pointer newval = s7_eval_c_string(s7, newval_s);
    s7_pointer x = s7_list(s7, 2, getter, newval);
    s7_apply_function(s7, set_bang, x);

    _replace_first_attr_postcheck();
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
LOCAL void _replace_last_attr_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    attrs = s7_eval_c_string(s7, "(pkg :load :2 :@@)");
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 3, s7_list_length(s7, attrs) );

    attr = s7_eval_c_string(s7, "(pkg :load :2 :attr :-1)");
    struct node_s *attr_nd = s7_c_object_value(attr);
    TEST_ASSERT(attr_nd->tid == TK_Binding);

    /* attr keys are syms (TK_ID) */
    key = s7_apply_function(s7, attr,
                            s7_list(s7, 1, s7_make_keyword(s7, "key")));
    TEST_ASSERT( sunlark_node_tid(s7, key) == TK_ID );
    key = s7_apply_function(s7, key, s7_list(s7, 1,
                                         s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "newkey1", s7_symbol_name(key));

    /* attr value - in this case, a string */
    val = s7_apply_function(s7, attr,
                            s7_list(s7, 1,
                                    s7_make_keyword(s7, "value")));
    TEST_ASSERT( sunlark_node_tid(s7, val) == TK_STRING );
    val = s7_apply_function(s7, val, s7_list(s7, 1,
                                             s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"newval1\"", s7_string(val));
}

/* **************************************************************** */
void test_loadstmt_replace_last_attr_by_int(void)
{
    _parse_pkg();
    _last_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ -1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    char *newval_s = "(make-binding 'newkey1 \"newval1\")";
    s7_pointer newval = s7_eval_c_string(s7, newval_s);
    s7_pointer x = s7_list(s7, 2, getter, newval);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _replace_last_attr_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_replace_last_attr_by_kwint(void)
{
    _parse_pkg();
    _last_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :attr :-1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    char *newval_s = "(make-binding 'newkey1 \"newval1\")";
    s7_pointer newval = s7_eval_c_string(s7, newval_s);
    s7_pointer x = s7_list(s7, 2, getter, newval);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _replace_last_attr_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_loadstmt_replace_last_attr_by_str(void)
{
    _parse_pkg();
    _last_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :attr 'lastkey2c)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    char *newval_s = "(make-binding 'newkey1 \"newval1\")";
    s7_pointer newval = s7_eval_c_string(s7, newval_s);
    s7_pointer x = s7_list(s7, 2, getter, newval);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _replace_last_attr_postcheck();
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
/* **************************************************************** */
LOCAL void _splice_attrs_before_first_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    attrs = s7_eval_c_string(s7, "(pkg :load :2 :@@)");
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 5, s7_list_length(s7, attrs) );

    attr = s7_eval_c_string(s7, "(pkg :load :2 :attr :0)");
    struct node_s *attr_nd = s7_c_object_value(attr);
    TEST_ASSERT( attr_nd->tid == TK_Binding);

    /* attr keys are syms (TK_ID) */
    key = s7_apply_function(s7, attr,
                            s7_list(s7, 1, s7_make_keyword(s7, "key")));
    TEST_ASSERT( sunlark_node_tid(s7, key) == TK_ID );
    key = s7_apply_function(s7, key, s7_list(s7, 1,
                                         s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "newkey1", s7_symbol_name(key));

    /* attr value - in this case, a string */
    val = s7_apply_function(s7, attr,
                            s7_list(s7, 1,
                                    s7_make_keyword(s7, "value")));
    TEST_ASSERT( sunlark_node_tid(s7, val) == TK_STRING );
    val = s7_apply_function(s7, val, s7_list(s7, 1,
                                             s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"newval1\"", s7_string(val));
}

/* **************************************************************** */
void test_splice_attrs_before_first_by_int(void)
{
    _parse_pkg();
    _first_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ 0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_binding_splice(LIST);
    s7_pointer x = s7_list(s7, 2, getter, splice);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _splice_attrs_before_first_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_splice_attrs_before_first_by_kwint(void)
{
    _parse_pkg();
    _first_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :attr :0)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_binding_splice(LIST);
    s7_pointer x = s7_list(s7, 2, getter, splice);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _splice_attrs_before_first_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_splice_attrs_before_first_by_str(void)
{
    _parse_pkg();
    _first_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :attr 'key0c)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_binding_splice(LIST);
    s7_pointer x = s7_list(s7, 2, getter, splice);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _splice_attrs_before_first_postcheck();
    sunlark_dispose(s7,pkg);
}

/* **************************************************************** */
/* **************************************************************** */
LOCAL void _splice_attrs_after_last_postcheck(void)
{
    path = s7_eval_c_string(s7, "'(:loadstmts)");
    loadstmts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, loadstmts) );
    TEST_ASSERT_EQUAL_INT( 4, s7_list_length(s7, loadstmts) );

    attrs = s7_eval_c_string(s7, "(pkg :load :2 :@@)");
    TEST_ASSERT( s7_is_list(s7, attrs) );
    TEST_ASSERT_EQUAL_INT( 5, s7_list_length(s7, attrs) );

    attr = s7_eval_c_string(s7, "(pkg :load :2 :attr :-1)");
    struct node_s *attr_nd = s7_c_object_value(attr);
    TEST_ASSERT( attr_nd->tid == TK_Binding);

    /* attr keys are syms (TK_ID) */
    key = s7_apply_function(s7, attr,
                            s7_list(s7, 1, s7_make_keyword(s7, "key")));
    TEST_ASSERT( sunlark_node_tid(s7, key) == TK_ID );
    key = s7_apply_function(s7, key, s7_list(s7, 1,
                                         s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "newkey2", s7_symbol_name(key));

    /* attr value - in this case, a string */
    val = s7_apply_function(s7, attr,
                            s7_list(s7, 1,
                                    s7_make_keyword(s7, "value")));
    TEST_ASSERT( sunlark_node_tid(s7, val) == TK_STRING );
    val = s7_apply_function(s7, val, s7_list(s7, 1,
                                             s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"newval2\"", s7_string(val));
}

/* **************************************************************** */
void test_splice_attrs_after_last_by_int(void)
{
    _parse_pkg();
    _last_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :@ -1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_binding_splice(VECTOR);
    s7_pointer x = s7_list(s7, 2, getter, splice);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _splice_attrs_after_last_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_splice_attrs_after_last_by_kwint(void)
{
    _parse_pkg();
    _last_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :attr :-1)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_binding_splice(VECTOR);
    s7_pointer x = s7_list(s7, 2, getter, splice);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _splice_attrs_after_last_postcheck();
    sunlark_dispose(s7,pkg);
}

void test_splice_attrs_after_last_by_str(void)
{
    _parse_pkg();
    _last_attr_precheck();

    /* make change */
    char *getter_s = "'(pkg :load :2 :attr 'lastkey2c)";
    s7_pointer getter = s7_eval_c_string(s7, getter_s);
    s7_pointer splice = _make_binding_splice(VECTOR);
    s7_pointer x = s7_list(s7, 2, getter, splice);
    s7_apply_function(s7, set_bang, x);

    struct node_s *p = s7_c_object_value(pkg);
    _splice_attrs_after_last_postcheck();
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

LOCAL s7_pointer _make_binding_splice(int typ)
{
    char *b1s = "(make-binding 'newkey1 \"newval1\")";
    s7_pointer b1 = s7_eval_c_string(s7, b1s);
    char *b2s = "(make-binding 'newkey2 \"newval2\")";
    s7_pointer b2 = s7_eval_c_string(s7, b2s);

    /* 'splice' will be evaluated; we want it to eval to as list, not
       as a fncall; so we produce the list (list ld1 ld1) rather than
       the list (ld1 ld2) */
    s7_pointer splice;
    if (typ == LIST)
        splice = s7_list(s7, 3, s7_make_symbol(s7, "list"), b1, b2);
    else {
        splice = s7_make_vector(s7, 2);
        s7_vector_set(s7, splice, 0, b1);
        s7_vector_set(s7, splice, 1, b2);
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
    old_port = s7_set_current_error_port(s7, s7_open_output_string(s7));
    if (old_port != s7_nil(s7))
        gc_loc = s7_gc_protect(s7, old_port);

    s7_define_function(s7, "error-handler",
                       _error_handler, 1, 0, false,
                       "our error handler");
}

void tearDown(void) {
    s7_close_output_port(s7, s7_current_error_port(s7));
    s7_set_current_error_port(s7, old_port);
    if (gc_loc != -1)
        s7_gc_unprotect_at(s7, gc_loc);

    s7_quit(s7);
}
