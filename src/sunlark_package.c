#include <assert.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "log.h"
#include "utarray.h"

#include "s7.h"

#include "sunlark_package.h"

/* **************************************************************** */
s7_pointer sunlark_pkg_remove(s7_scheme *s7, struct node_s *self_node,
                          s7_pointer get_path, s7_pointer selector)
{
#ifdef DEBUG_TRACE
    log_debug(">> sunlark_pkg_remove path: %s, selector %s",
              s7_object_to_c_string(s7, get_path),
              s7_object_to_c_string(s7, selector));
#endif

    /* case: (set! (pkg :>>) :null) - rm all targets */
    /* case: (set! (pkg :> :*) :null) - rm all targets */
    /* case: (set! (pkg :> "hello-lib") :null) - rm target */
    /* case: (set! (pkg :> :0) :null) - rm target */
    /* ditto for :load, :loads */
    /* case: (set! (pkg :load :0) :null) - rm loadstmt */
    if (s7_is_null(s7, selector)) {
        log_error("Getter must include a selector");
        return handle_errno(s7, EMISSING_GET_SELECTOR, s7_nil(s7));
        return NULL;
    }

    int path_len = s7_list_length(s7, get_path);

    struct node_s *result;

    /* if self == TK_Package */
    /*   call applicator only if len(get_path) > 1 */
    switch(self_node->tid) {
    case TK_Package:
    /* if (self_node->tid == TK_Package) { */
        /* getter (pkg :>>): get_path (), selector :>> */
        /* case: (set! (pkg :>>) :null) - delete all targets */
        /* case: (set! (pkg :>>) #>(mytgt ...)) - illegal replace */
        /* case: (set! (pkg :>>) '(tgt1 tgt2 ...)) - illegal splice */
        /* case: (set! (pkg :>>) #(tgt1 tgt2 ...)) - illegal splice */
        if (get_path == s7_nil(s7)) {
            if (selector == KW(>>)) {
                /* only allowed set action: :null */
                /* if (update_val == KW(null)) { */
                    result = sealark_pkg_remove_all_targets(self_node);
                    return sunlark_new_node(s7, result);
                /* } else { */
                /*     log_error("Only action for selector :>> is :null"); */
                /*     return handle_errno(s7, EINVALID_ACTION, update_val); */
                /* } */
            }
            if (selector == KW(loadstmts)) {
                /* if (update_val == KW(null)) { */
                    result = sealark_pkg_remove_all_loadstmts(self_node);
                    return sunlark_new_node(s7, result);
                /* } else { */
                /*     log_error("Only action for selector :loads is :null"); */
                /*     return handle_errno(s7, EINVALID_ACTION, update_val); */
                /* } */
            }

            if (selector == KW(>)) {
                log_error(":> may not be selector in getter");
                return handle_errno(s7, EINVALID_GET_SELECTOR, selector);
            }
            if (selector == KW(load)) {
                log_error(":load may not be selector in getter");
                return handle_errno(s7, EINVALID_GET_SELECTOR,
                                    selector);
            }
            log_error("UNHANDLED selector: %s",
                      s7_object_to_c_string(s7, selector));
            return NULL;

            /* other singleton selectors on pkg: :loadstmts, etc. */
        }
        /* special cases */
        if (path_len == 1) {
            if (s7_car(get_path) == KW(>)) {
                if (selector == s7_make_keyword(s7, "*")) {
                    /* (set! (pkg :> :*) :null) */
                    /* if (update_val == KW(null)) { */
                        result = sealark_pkg_remove_all_targets(self_node);
                        return sunlark_new_node(s7, result);
                    /* } else { */
                    /*     log_error("Only action for selector :* is :null"); */
                    /*     return handle_errno(s7, EINVALID_ACTION, update_val); */
                    /* } */
                } else {
                    /* if selector is :n */
                    log_warn("special case: path = :> sel");
                    errno = 0;
                    int idx = sunlark_kwindex_to_int(s7, selector);
                    if (errno == 0) {
                        result = sealark_pkg_rm_target_at_int(self_node,
                                                                  idx);
                        if (result)
                            return sunlark_new_node(s7, result);
                        return handle_errno(s7, errno, selector);
                    } else {
                        return handle_errno(s7, errno, s7_nil(s7));
                    }
                }
            }
            /* (set! (pkg :load :*) :null) */
            if (s7_car(get_path) == KW(load)) {
                if (selector == s7_make_keyword(s7, "*")) {
                    /* if (update_val == KW(null)) { */
                        result = sealark_pkg_remove_all_loadstmts(self_node);
                        return sunlark_new_node(s7, result);
                    /* } else { */
                    /*     log_error("Only action in this context for selector :load is :null"); */
                    /*     return handle_errno(s7, EINVALID_ACTION, update_val); */
                    /* } */
                } else {
                    errno = 0;
                    int idx = sunlark_kwindex_to_int(s7, selector);
                    if (errno == 0) {
                        result = sealark_pkg_remove_loadstmt_at_int(self_node,
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
                                                     self_node,
                                                     s7_string(selector));
                            if (result)
                                return sunlark_new_node(s7, result);
                            else
                                return handle_errno(s7, errno, selector);
                        } else {
                            log_error("In this context :load must be followe by an int (or kw int) or string key");
                            return handle_errno(s7, EINVALID_ARG,
                                                selector);
                        }
                    }
                    log_error("special case: path == :load");
                    return NULL;
                }
            }
            log_error("special case len(get_path) == 1 ...");
        }
        break;
    case TK_List_Expr:
        log_error("TK_List_Expr");
        return NULL;
        break;
    default:
        log_error("uncaught tid");
        return NULL;
    }
}

s7_pointer sunlark_package_dispatcher(s7_scheme *s7,
                                      s7_pointer data,
                                      s7_pointer path_args)
{
#if defined (DEBUG_TRACE) || defined(DEBUG_PATHS)
    log_debug(">> sunlark_package_dispatcher, args: %s",
              s7_object_to_c_string(s7, path_args));
#endif
    struct node_s *pkg = s7_c_object_value(data);
    assert(pkg->tid == TK_Package);

    int op_count = s7_list_length(s7, path_args);
    /* log_debug("op count: %d", op_count); */

    s7_pointer op = s7_car(path_args);
    //s7_pointer op2, op3, op4, op5;
    if ( !s7_is_keyword(op) ) {
        log_error("Path op %s not supported for :package nodes",
                  s7_object_to_c_string(s7, op));
        exit(EXIT_FAILURE);     /* FIXME */
    }

    s7_pointer result_list;

    if (op == KW(>>) || op == KW(targets)) {
        s7_pointer r
            = sunlark_forall_targets(s7, pkg, s7_cdr(path_args));
        return r;
    }

    if (op == KW(>) || op == KW(target)) {
        /* log_debug("dispatching on pkg for target"); */
        result_list //= sunlark_targets_for_buildfile(s7, pkg);
            = _pkg_target_dispatcher(s7, pkg, s7_cdr(path_args));
            return result_list;
    }

    if (op == KW(load)) {
        // :package > :stmt-list :smallstmt-list > load-expr,...
        errno = 0;
        s7_pointer loadstmt
            = sunlark_pkg_loadstmt_dispatch(s7, pkg, s7_cdr(path_args));
        /* /\* UT_array *loads = sealark_loadstmts(pkg); *\/ */
        if (loadstmt)
            return loadstmt;
        else
            return handle_errno(s7, errno, path_args);
    }

    if (op == KW(loadstmts)) {
        // :package > :stmt-list :smallstmt-list > load-expr,...
        /* result_list = sunlark_fetch_load_stmts(s7, pkg); */

        UT_array *loads = sealark_procs_for_id(pkg,
                                               "load");
        /* UT_array *loads = sealark_loadstmts(pkg); */
        if (loads)
            return nodelist_to_s7_list(s7, loads);
        else {
            log_debug("ERROR: ...fixme...");
        }
    }

    if (op == KW(package)) {
        UT_array *procs = sealark_procs_for_id(pkg,
                                               "package");
        if (utarray_len(procs) == 1)
            return sunlark_new_node(s7,
                                    utarray_eltptr(procs, 0));
        else
            return s7_nil(s7);
    }
    if (op == KW(directives)) {
        /* all procs and definitions */
        UT_array *directives = sealark_directives(pkg);
        return nodelist_to_s7_list(s7, directives);
        return NULL;
    }
    if (op == KW(definitions)) {
        /* sealark_debug_log_ast_outline(pkg, 0); */
        UT_array *defns = sealark_definitions(pkg);
        return nodelist_to_s7_list(s7, defns);
    }
    if (op == KW(vardefs)) {
        /* result_list = */
        UT_array *vardefs = sealark_vardefs(pkg);
        return nodelist_to_s7_list(s7, vardefs);
    }
    if (op == KW(procedures)) {
        UT_array *procs = sealark_procs(pkg);
        return nodelist_to_s7_list(s7, procs);
    }

    if (op == KW(format)) {
        sealark_pkg_format_force(pkg);
        return s7_unspecified(s7);
    }

    if (op == KW(filename)) {
        char *fname = sealark_pkg_filename(pkg);
        return s7_make_string(s7, fname);
    }

    if (op == KW(save)) {
        errno = 0;
        sealark_pkg_save(pkg);
        if (errno == 0)
            return s7_unspecified(s7);
        else
            return handle_errno(s7, errno, s7_make_string(s7, pkg->fname));
    }

    /* common properties */
    s7_pointer result = sunlark_common_property_lookup(s7, pkg, op);
    if (result) return result;

        log_error("Bad arg on buildfile: %s", s7_object_to_c_string(s7, op));
    return(s7_error(s7,
                    s7_make_symbol(s7, "invalid_argument"),
                    s7_list(s7, 2, s7_make_string(s7,
                 "Arg \"~A\" inadmissable here"),
                            op)));

}

/* ************************************************ */
/* path resolved so far: :>
   next step options:
       () - invalid
       string - target_for_name
       int    - target_for_index
   (options after :>> or :targets :
       sym    - filter by rule (e.g. 'cc_library)
       list   - filter e.g. '(cc_library cc_test "hello-lib")
 */

LOCAL s7_pointer _pkg_target_dispatcher(s7_scheme *s7,
                                        struct node_s *pkg,
                                        s7_pointer path_args)
{
#if defined (DEBUG_TRACE) || defined(DEBUG_TARGETS)
    log_debug(">> _pkg_target_dispatcher: %s",
              s7_object_to_c_string(s7, path_args));
#endif

    assert(pkg->tid == TK_Package);

    int op_count = s7_list_length(s7, path_args);
    /* log_debug("op count: %d", op_count); */

    s7_pointer op = s7_car(path_args);
    /* s7_pointer op2 = s7_cadr(path_args); */

    s7_pointer result;

    if (op_count == 0) {
        /* log_debug("TARGET"); */
        /* UT_array *targets = sealark_targets_for_pkg(pkg); */
        /* return nodelist_to_s7_list(s7, targets); */
        return handle_errno(s7, EINVALID_ARG, path_args);
    }

    if (s7_is_string(op)) {     /* :> "hello-world") */
        errno = 0;
        struct node_s *tgt_node
            = sealark_target_for_name(pkg, s7_string(op));

        if (tgt_node == NULL) {
            /* errno = ENOT_FOUND; */
            return handle_errno(s7, errno, path_args);
        }

        if ( s7_is_null(s7, s7_cdr(path_args)) ) { /* e.g. (:> "mylib") */
            return sunlark_new_node(s7, tgt_node);
        } else {
            /* e.g. (:> "mylib" :@ ...), (:> "mylib" :rule), etc. */
            /* return sunlark_target_1(s7, tgt_node, s7_cdr(path_args)); */
            return sunlark_target_dispatcher(s7, tgt_node,
                                             s7_cdr(path_args));
        }

    }

    errno = 0;
    int idx = sunlark_kwindex_to_int(s7, op);
    if (errno == 0) {
        struct node_s *tgt_node = sealark_target_for_index(pkg,
                                                           idx);
        if ( s7_is_null(s7, s7_cdr(path_args)) ) { /* e.g. (:> 0) */
            return sunlark_new_node(s7, tgt_node);
        } else {
            /* e.g. (:> 1 :@ ...), (:> 1 :rule) */
            return sunlark_target_dispatcher(s7, tgt_node,
                                             s7_cdr(path_args));
            /* return sunlark_target_1(s7, tgt_node, */
            /*                           s7_cdr(path_args)); */
        }

    }
    log_error("Bad arg: %s after :target", s7_object_to_c_string(s7, op));
    return(s7_error(s7,
                    s7_make_symbol(s7, "invalid_argument"),
                    s7_list(s7, 2, s7_make_string(s7,
                     "Bad arg ~S after target (only string or int allowed)"),
                            op)));
}

/* **************************************************************** */
struct node_s *sunlark_pkg_target_mutate(s7_scheme *s7,
                                          struct node_s *pkg,
                                          s7_pointer selector,
                                          s7_pointer update_val)
{
#if defined (DEBUG_TRACE) || defined(DEBUG_TARGETS)
    log_debug(">> sunlark_pkg_target_mutate, sel: %s, newval: %s",
              s7_object_to_c_string(s7, selector),
              s7_object_to_c_string(s7, update_val));
#endif
    assert(pkg->tid == TK_Package);

    /* selector == target selector: int, kwint, or string key */

    /* update_val: target->replace; list->splice */

    struct node_s *result;
    int idx = sunlark_kwindex_to_int(s7, selector);
    if (errno == 0) {// we got an int
        if (s7_is_c_object(update_val)) {
            struct node_s *newval = s7_c_object_value(update_val);
            result = sealark_pkg_replace_target_at_int(pkg, idx,
                                                       newval);
            if (result)
                return result;
            else {
                errno = EUNEXPECTED_STATE;
                return NULL;
            }
        }
        if (s7_is_vector(update_val)) {
            /* log_debug("splicing after..."); */
            result = _pkg_splice_targets_at_int(s7, pkg,
                                                idx, update_val);
            if (result) {
                sealark_pkg_format(pkg);
                return result;
            } else {
                errno = EUNEXPECTED_STATE;
                return NULL;
            }
        }
        if (s7_is_list(s7, update_val)) {
            /* log_debug("splicing at..."); */
            result = _pkg_splice_targets_at_int(s7, pkg,
                                                idx, update_val);
            if (result) {
                sealark_pkg_format(pkg);
                return result;
            } else {
                errno = EUNEXPECTED_STATE;
                return NULL;
            }
        } else {
            log_error("37 FIXME xxxxxxxxxxxxxxxx");
        }
    } else {
        log_error("52 FIXME xxxxxxxxxxxxxxxx");
    }
    return NULL;
}

LOCAL struct node_s *_pkg_splice_targets_at_int(s7_scheme *s7,
                                                struct node_s *pkg,
                                                int _index,
                                                s7_pointer update_val)
{
#if defined (DEBUG_TRACE) || defined(DEBUG_TARGETS)
    log_debug(">> _pkg_splice_targets_at_int: %d", _index);
#endif

    assert(pkg->tid == TK_Package);

    //FIXME efficiency: sealark_normalize_index also counts targets
    int target_ct = sealark_pkg_targets_count(pkg);
    int index = sealark_normalize_index(pkg, _index);
    /* log_debug("normalized index: %d", index); */

    s7_pointer t;               /* target from list */
    struct node_s *tnode;

    if (s7_is_list(s7, update_val)) {
        /* log_debug("splicing list..."); */
        while ( !s7_is_null(s7, update_val) ) {
            t = s7_car(update_val);
            if ( !s7_is_c_object(t) ) {
                log_error("items in splice list must be c-objects (target nodes)");
                errno = EINVALID_ARG;
                return NULL;
            }
            tnode = s7_c_object_value(t);
            if (tnode->tid != TK_Call_Expr) {
                log_error("items in splice list must be target (call-expr) nodes");
                errno = EINVALID_ARG;
                return NULL;
            }
            struct node_s *result
                = sealark_pkg_splice_target_at_int(pkg, target_ct++,
                                                   index++, tnode);
        loop:
            update_val = s7_cdr(update_val);
            /* log_warn("loop %s", s7_object_to_c_string(s7, update_val)); */
        }
    }

    if (s7_is_vector(update_val)) {
        /* log_debug("splicing vector..."); */
        int len = s7_vector_length(update_val);
        for (int i = 0; i < len; i++) {
            t = s7_vector_ref(s7, update_val, i);
            if ( !s7_is_c_object(t) ) {
                log_error("items in splice list must be c-objects (target nodes): %s",
                          s7_object_to_c_string(s7, t));
                errno = EINVALID_ARG;
                return NULL;
            }
            tnode = s7_c_object_value(t);
            if (tnode->tid != TK_Call_Expr) {
                log_error("items in splice list must be target (call-expr) nodes");
                errno = EINVALID_ARG;
                return NULL;
            }
            struct node_s *result
                = sealark_pkg_splice_target_at_int(pkg,
                                                   /* we're adding a tgt */
                                                   target_ct++,
                                                   ++index,
                                                   tnode);
        }
    }
    return pkg;
}
