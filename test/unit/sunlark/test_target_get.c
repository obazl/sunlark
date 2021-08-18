#include "log.h"
#include "utarray.h"
#include "utstring.h"
#include "unity.h"
#include "s7.h"

/* we need both APIs for test validation */
#include "sealark.h"
#include "sunlark.h"

#include "test_target_get.h"

/* static s7_pointer tgt; */

char *build_file = "test/unit/sunlark/BUILD.target_get";

int main(void) {
    UNITY_BEGIN();

    RUN_TEST(test_pkg_targets_all);
    RUN_TEST(test_pkg_tgts_all);

    RUN_TEST(test_pkg_target_first_by_int);
    RUN_TEST(test_pkg_target_first_by_kwint);
    RUN_TEST(test_pkg_target_first_by_str);

    RUN_TEST(test_pkg_tgt_first_by_int);
    RUN_TEST(test_pkg_tgt_first_by_kwint);
    RUN_TEST(test_pkg_tgt_first_by_str);

    RUN_TEST(test_pkg_tgt_first_rule);
    RUN_TEST(test_pkg_tgt_first_name);

    return UNITY_END();
}

/* **************************************************************** */
/* **************************************************************** */
void test_pkg_targets_all(void)
{
    s7_pointer path = s7_eval_c_string(s7, "'(:targets)");
    s7_pointer tgts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, tgts) );
    TEST_ASSERT_EQUAL_INT( 6, s7_list_length(s7, tgts) );

    _tgt_first_check();
}

void test_pkg_tgts_all(void)
{
    s7_pointer path = s7_eval_c_string(s7, "'(:>>)");
    s7_pointer tgts = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, tgts) );
    TEST_ASSERT_EQUAL_INT( 6, s7_list_length(s7, tgts) );

    _tgt_first_check();
}

/* **************** */
void test_pkg_target_first_by_int(void) {
    s7_pointer path = s7_eval_c_string(s7, "'(:target 0)");
    s7_pointer tgt = s7_apply_function(s7, pkg, path);
    _tgt_first_check();
}
void test_pkg_target_first_by_kwint(void) {
    s7_pointer path = s7_eval_c_string(s7, "'(:target :0)");
    s7_pointer tgt = s7_apply_function(s7, pkg, path);
    _tgt_first_check();
}
void test_pkg_target_first_by_str(void) {
    s7_pointer path = s7_eval_c_string(s7, "'(:target \"hello-world\")");
    s7_pointer tgt = s7_apply_function(s7, pkg, path);
    _tgt_first_check();
}

void test_pkg_tgt_first_by_int(void) {
    s7_pointer path = s7_eval_c_string(s7, "'(:> 0)");
    s7_pointer tgt = s7_apply_function(s7, pkg, path);
    _tgt_first_check();
}
void test_pkg_tgt_first_by_kwint(void) {
    s7_pointer path = s7_eval_c_string(s7, "'(:> :0)");
    s7_pointer tgt = s7_apply_function(s7, pkg, path);
    _tgt_first_check();
}
void test_pkg_tgt_first_by_str(void) {
    s7_pointer path = s7_eval_c_string(s7, "'(:> \"hello-world\")");
    s7_pointer tgt = s7_apply_function(s7, pkg, path);
    _tgt_first_check();
}

/* **************** */
void test_pkg_tgt_first_rule(void) {
    s7_pointer path = s7_eval_c_string(s7, "'(:> :0 :rule :$)");
    s7_pointer x  = s7_apply_function(s7, pkg, path);
    TEST_ASSERT_EQUAL_STRING( "cc_library", s7_symbol_name(x));

    // using intermediate vars
    path = s7_eval_c_string(s7, "'(:> :0)");
    s7_pointer tgt  = s7_apply_function(s7, pkg, path);
    s7_pointer rule = s7_apply_function(s7, tgt,
                                        s7_eval_c_string(s7, "'(:rule)"));
    s7_pointer rule_v = s7_apply_function(s7, rule,
                                        s7_eval_c_string(s7, "'(:$)"));
    TEST_ASSERT_EQUAL_STRING( "cc_library", s7_symbol_name(rule_v));

}

/* **************** */
void test_pkg_tgt_first_name(void) {
    s7_pointer path = s7_eval_c_string(s7, "'(:> :0 :name :$)");
    s7_pointer x  = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_string(x) );
    TEST_ASSERT_EQUAL_STRING( "\"hello-lib\"", s7_string(x));

    // using intermediate vars
    path = s7_eval_c_string(s7, "'(:> :0)");
    s7_pointer tgt  = s7_apply_function(s7, pkg, path);
    s7_pointer rule = s7_apply_function(s7, tgt,
                                        s7_eval_c_string(s7, "'(:name)"));
    s7_pointer rule_v = s7_apply_function(s7, rule,
                                        s7_eval_c_string(s7, "'(:$)"));
    TEST_ASSERT_EQUAL_STRING( "\"hello-lib\"", s7_string(rule_v));
}

/* **************************************************************** */
//                  validations
/* **************************************************************** */
LOCAL void _tgt_first_check(void)
{
    path = s7_eval_c_string(s7, "'(:>>)");
    targets = s7_apply_function(s7, pkg, path);
    TEST_ASSERT( s7_is_list(s7, targets) );
    TEST_ASSERT_EQUAL_INT( 6, s7_list_length(s7, targets) );

    target = s7_eval_c_string(s7, "(pkg :> 0)");
    struct node_s *target_nd = s7_c_object_value(target);
    TEST_ASSERT( target_nd->tid == TK_Call_Expr);
    s7_pointer k = s7_apply_function(s7, target,
                                      s7_list(s7, 2,
                                              s7_make_keyword(s7, "name"),
                                              s7_make_keyword(s7, "$")));
    TEST_ASSERT_EQUAL_STRING( "\"hello-lib\"", s7_string(k) );
}

/* **************************************************************** */
void setUp(void) {
    s7 = sunlark_init();
    init_s7_syms(s7);
    pkg = sunlark_parse_build_file(s7,
                                   s7_list(s7, 1,
                                           s7_make_string(s7, build_file)));
    s7_define_variable(s7, "pkg", pkg);
}

void tearDown(void) {
    /* sealark_parse_state_free(parse_state); */
    s7_quit(s7);
}

