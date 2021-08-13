#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "log.h"
#include "utarray.h"

#include "s7.h"

#include "sunlark_loadstmts_remove.h"

/* **************************************************************** */
/* already :load */
s7_pointer sunlark_loadstmts_remove(s7_scheme *s7, struct node_s *pkg_node,
                                    s7_pointer get_path, s7_pointer selector)
{
#ifdef DEBUG_TRACE
    log_debug(">> sunlark_loadstmts_remove: path %s, selector %s",
              s7_object_to_c_string(s7, get_path),
              s7_object_to_c_string(s7, selector));
#endif

    assert(pkg_node->tid == TK_Package);

    int path_len = s7_list_length(s7, get_path);

    struct node_s *result;

    if (get_path == s7_nil(s7)) {
        /* case: (set! (pkg :load :0) :null) - delete one loadstmt */
        int idx = sunlark_kwindex_to_int(s7, selector);
        if (errno == 0) {
            result = sealark_pkg_remove_loadstmt_at_int(pkg_node,
                                                        idx);
            if (result)
                return sunlark_new_node(s7, result);
            else
                return handle_errno(s7, errno, selector);
        }

        if (s7_is_string(selector)) {
            result
                = sealark_pkg_remove_loadstmt_at_key(
                                                     pkg_node,
                                                     s7_string(selector));
            if (result)
                return sunlark_new_node(s7, result);
            else
                return handle_errno(s7, errno, selector);
        }

        if (selector == KW(loadstmts)) {
            /* if (update_val == KW(null)) { */
            result = sealark_pkg_remove_all_loadstmts(pkg_node);
            return sunlark_new_node(s7, result);
            /* } else { */
            /*     log_error("Only action for selector :loads is :null"); */
            /*     return handle_errno(s7, EINVALID_ACTION, update_val); */
            /* } */
        }
        log_error("UNHANDLED selector: %s",
                  s7_object_to_c_string(s7, selector));
        return NULL;

        /* other singleton selectors on pkg: :loadstmts, etc. */
    }

    /* special cases */
    if (path_len == 1) {
        int idx = sunlark_kwindex_to_int(s7, s7_car(get_path));
        if (errno == 0) {
            struct node_s *loadstmt
                = sealark_pkg_loadstmt_for_int(pkg_node, idx);
            if (loadstmt) {
                if (selector == KW(args)) {
                    sealark_loadstmt_rm_args(loadstmt);
                }
            }
            return NULL;
        }
        /* if (s7_car(get_path) == KW(load)) { */
            if (selector == s7_make_keyword(s7, "*")) {
                /* (set! (pkg :load :*) :null): rm all args and bindings */
                /* if (update_val == KW(null)) { */
                result = sealark_pkg_remove_all_loadstmts(pkg_node);
                return sunlark_new_node(s7, result);
                /* } else { */
                /*     log_error("Only action in this context for selector :load is :null"); */
                /*     return handle_errno(s7, EINVALID_ACTION, update_val); */
                /* } */
            } else {
                errno = 0;
                int idx = sunlark_kwindex_to_int(s7, selector);
                if (errno == 0) {
                    result = sealark_pkg_remove_loadstmt_at_int(pkg_node,
                                                                idx);
                    if (result)
                        return sunlark_new_node(s7, result);
                    else
                        return handle_errno(s7, errno, selector);
                } else {
                    /* selector not a kw int, must be a string */
                    if (s7_is_string(selector)) {
                        result
                            = sealark_pkg_remove_loadstmt_at_key(
                                                                 pkg_node,
                                                                 s7_string(selector));
                        if (result)
                            return sunlark_new_node(s7, result);
                        else
                            return handle_errno(s7, errno, selector);
                    } else {
                        log_error("In this context :load must be followed by an int (or kw int) or string key");
                        return handle_errno(s7, EINVALID_ARG,
                                            selector);
                    }
                }
                log_error("special case: path == :load");
                return NULL;
            }
        /* } */
        log_error("special case len(get_path) == 1 ...");
    }

    if (path_len == 2) {
        s7_pointer op1 = s7_car(get_path);
        s7_pointer op2 = s7_cadr(get_path);
        int idx = sunlark_kwindex_to_int(s7, op1); // s7_car(get_path));
        if (errno == 0) {
            struct node_s *loadstmt
                = sealark_pkg_loadstmt_for_int(pkg_node, idx);
            if (loadstmt) {
                if (op2 == KW(arg)) {
                    errno = 0;
                    idx = sunlark_kwindex_to_int(s7, selector);
                    if (errno == 0) {
                        sealark_loadstmt_rm_arg_at_int(loadstmt, idx);
                        if (errno == 0)
                            return s7_unspecified(s7);
                        else
                            return NULL;
                    } else {
                    if (s7_is_string(selector)) {
                        const char *key = s7_string(selector);
                        log_debug("loadstmt rm arg at %s", key);
                        errno = 0;
                        sealark_loadstmt_rm_arg_at_str(loadstmt, key);
                        if (errno == 0)
                            return s7_unspecified(s7);
                        else
                            return NULL;

                        return NULL;
                    }
                    log_error("Bad arg: %s", s7_object_to_c_string(s7, selector));
                    }
                }
                if (s7_is_string(op2)) {
                        log_error("loadstmt rm arg at %s", s7_object_to_c_string(s7, op2));
                        return NULL;
                }
                /*     log_error("Bad arg: %s", s7_object_to_c_string(s7, selector)); */
                /*     return NULL; */
                /* } */

                if (op2 == KW(args)) {
                    sealark_loadstmt_rm_args(loadstmt);
                }
                if (op2 == KW(binding) || op2 == KW(@)) {
                    log_debug("accessing attr");
                    /* (:i :@) + selector */
                    /* selector must be int, kwint, or sym (binding key) */
                    idx = sunlark_kwindex_to_int(s7, selector);
                    if (errno == 0) { // int or kwint
                        log_debug("removing attr at %d", idx);
                        errno == 0;
                        sealark_loadstmt_rm_attr_at_int(loadstmt, idx);
                        if (errno == 0)
                            return s7_unspecified(s7);
                        else
                            return NULL;
                    } else {
                        log_debug("indexing by %s", s7_object_to_c_string(s7, selector));
                        if (s7_is_symbol(selector)) {
                            errno == 0;
                            sealark_loadstmt_rm_attr_at_sym(loadstmt,
                                                s7_symbol_name(selector));
                            if (errno == 0)
                                return s7_unspecified(s7);
                            else
                                return NULL;
                        } else {
                            log_error("Help!");
                        }
                    }
                }
            }
            return NULL;
        }
        /* op1 not int nor kwint */
        if (s7_is_string(op1)) {
            log_error("IDX BY KEY");
            struct node_s *loadstmt
                = sealark_pkg_loadstmt_for_key(pkg_node, s7_string(op1));
            //FIXME: following is same as for int/kwint deref
            if (loadstmt) {
                if (op2 == KW(arg)) {
                    errno = 0;
                    idx = sunlark_kwindex_to_int(s7, selector);
                    if (errno == 0) {
                        sealark_loadstmt_rm_arg_at_int(loadstmt, idx);
                        if (errno == 0)
                            return s7_unspecified(s7);
                        else
                            return NULL;
                    } else {
                    if (s7_is_string(selector)) {
                        const char *key = s7_string(selector);
                        log_debug("loadstmt rm arg at %s", key);
                        errno = 0;
                        sealark_loadstmt_rm_arg_at_str(loadstmt, key);
                        if (errno == 0)
                            return s7_unspecified(s7);
                        else
                            return NULL;

                        return NULL;
                    }
                    log_error("Bad arg: %s", s7_object_to_c_string(s7, selector));
                    }
                } else {
                    if (s7_is_string(op2)) {
                        log_debug("loadstmt rm arg at %s", s7_object_to_c_string(s7, op2));
                        return NULL;
                    }
                    log_error("Bad arg: %s", s7_object_to_c_string(s7, selector));
                    return NULL;
                }

                if (op2 == KW(args)) {
                    log_debug("1 xxxxxxxxxxxxxxxx");
                    sealark_loadstmt_rm_args(loadstmt);
                }
                if (op2 == KW(binding) || op2 == KW(@)) {
                    sealark_loadstmt_rm_args(loadstmt);
                }
            }

        }

        /* errno = 0; */
        /* idx = sunlark_kwindex_to_int(s7, selector); */
        /* if (errno == 0) { */
        /*     result = sealark_pkg_remove_loadstmt_at_int(pkg_node, */
        /*                                                 idx); */
        /*     if (result) */
        /*         return sunlark_new_node(s7, result); */
        /*     else */
        /*         return handle_errno(s7, errno, selector); */
        /* } else { */
        /*     /\* selector not a kw int, must be a string *\/ */
        /*     if (s7_is_string(selector)) { */
        /*         result */
        /*             = sealark_pkg_remove_loadstmt_at_key( */
        /*                                                  pkg_node, */
        /*                                                  s7_string(selector)); */
        /*         if (result) */
        /*             return sunlark_new_node(s7, result); */
        /*         else */
        /*             return handle_errno(s7, errno, selector); */
        /*     } else { */
        /*         log_error("In this context :load must be followed by an int (or kw int) or string key"); */
        /*         return handle_errno(s7, EINVALID_ARG, */
        /*                             selector); */
        /*     } */
        log_error("special case: path == :load");
        return NULL;
    }
    /* /\* } *\/ */
    /*     log_error("special case len(get_path) == 1 ..."); */
    /* } */
 
    /* /\* case: (set! (pkg :load :0 :args :0) :null) - delete one arg *\/ */

}
