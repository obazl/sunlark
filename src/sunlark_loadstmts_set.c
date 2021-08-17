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

#include "sunlark_loadstmts_set.h"

/*
  (pkg :load ref)
  (pkg :load ref :args)
  (pkg :load ref :arg ref)
  (pkg :load ref :@@)
  (pkg :load ref :@ ref)
 */

/* **************************************************************** */
/* already :load, so self must be pkg? */
s7_pointer
sunlark_set_load(s7_scheme *s7, struct node_s *pkg,
                            s7_pointer get_path, s7_pointer selector,
                            s7_pointer newval)
{
#ifdef DEBUG_TRACE
    log_debug(">> sunlark_set_load: get_path %s, selector %s, newval %s",
              s7_object_to_c_string(s7, get_path),
              s7_object_to_c_string(s7, selector),
              s7_object_to_c_string(s7, newval));
#endif

    assert(pkg->tid == TK_Package);

    if (s7_is_null(s7, get_path)) { // sel: int, kwint, or string
        errno = 0;
        int idx = sunlark_kwindex_to_int(s7, selector);
        if (errno == 0) { // op == int or kwint
            struct node_s *loadstmt
                = sealark_pkg_loadstmt_for_int(pkg, idx);
            if (loadstmt) {
                if ( s7_is_c_object(newval) ) {
                    sealark_loadstmt_replace(loadstmt,
                                             s7_c_object_value(newval));
                    return s7_unspecified(s7);
                } else {
                    if (s7_is_list(s7, newval)) {
                        log_debug("splicing at %d", idx);
                        _loadstmt_splice_list(s7, pkg, idx, newval);
                        return s7_unspecified(s7);
                    } else {
                        if (s7_is_vector(newval)) {
                            log_debug("splicing after %d", idx);
                            _loadstmt_splice_vector(s7, pkg, idx, newval);
                            return s7_unspecified(s7);
                        } else {
                            log_error("Invalid newval for set! load");
                            return handle_errno(s7, EINVALID_LOAD_NEWVAL, newval);
                        }
                    }
                }
            } else {
                return handle_errno(s7, errno, selector);
            }
        } else {
            if (s7_is_string(selector)) {
                if ( s7_is_c_object(newval) ) {
                    errno = 0;
                    struct node_s *loadstmt
                        = sealark_pkg_loadstmt_for_key(pkg,
                                                       s7_string(selector));
                    if (loadstmt) {
                        sealark_loadstmt_replace(loadstmt,
                                                 s7_c_object_value(newval));
                        return s7_unspecified(s7);
                    } else {
                        return handle_errno(s7, errno, selector);
                    }
                } else {
                    errno = 0;
                    idx = sealark_pkg_loadstmt_idx_for_key(pkg,
                                                       s7_string(selector));
                    if (idx < 0)
                        return handle_errno(s7, errno, selector);

                    if (s7_is_list(s7, newval)) {
                        log_debug("splicing before %s",
                                  s7_object_to_c_string(s7, selector));
                        _loadstmt_splice_list(s7, pkg, idx, newval);
                        return s7_unspecified(s7);
                    } else {
                        if (s7_is_vector(newval)) {
                            log_debug("splicing after %d", idx);
                            _loadstmt_splice_vector(s7, pkg, idx, newval);
                            return s7_unspecified(s7);
                        } else {
                            log_error("Invalid newval for set! load");
                            return handle_errno(s7, EINVALID_LOAD_NEWVAL, newval);
                        }
                    }
                }
            }
            return handle_errno(s7, errno, selector);
        }
        log_error("Bad arg 8435");
        return handle_errno(s7, EINVALID_ARG, selector);
    }

    s7_pointer op = s7_car(get_path);
    int idx = sunlark_kwindex_to_int(s7, op);
    if (errno == 0) { // op == int or kwint
        struct node_s *loadstmt
            = sealark_pkg_loadstmt_for_int(pkg, idx);
        if (loadstmt) {
            sunlark_loadstmt_set_dispatcher(s7, loadstmt,
                                            s7_cdr(get_path), selector,
                                            newval);
            return NULL;
        } else {
            return handle_errno(s7, errno, selector);
        }
    } else {
        log_error("unexpected");
        return handle_errno(s7, EUNEXPECTED_STATE, get_path);
   }

    log_error("Unhandled tid %d %s", pkg->tid, TIDNAME(pkg));
    errno = ENOT_IMPLEMENTED;
    return NULL;
}

/* **************************************************************** */
s7_pointer sunlark_loadstmt_set_dispatcher(s7_scheme *s7,
                                           struct node_s *loadstmt,
                                           s7_pointer get_path,
                                           s7_pointer selector,
                                           s7_pointer newval)
{
#ifdef DEBUG_TRACE
    log_debug(">> sunlark_loadstmt_set_dispatcher: path %s, selector %s",
              s7_object_to_c_string(s7, get_path),
              s7_object_to_c_string(s7, selector));
#endif

    assert(loadstmt->tid == TK_Load_Stmt);

    if (s7_is_null(s7, get_path)) {
        /* ?? (set! (pkg :load) ...) */
        log_error("Unexpected args");
        errno = EUNEXPECTED_STATE;
        return NULL;
    }

    s7_pointer op = s7_car(get_path);
    if (op == KW(arg)) {
        /* (set! (pkg :load ref :arg ref) newval) */
        int idx = sunlark_kwindex_to_int(s7, selector);
        if (errno == 0) { // op == int or kwint
            if (s7_is_string(newval)) {
                /* (set! (pkg :load i) "str") */
                errno = 0;
                sealark_loadstmt_replace_arg_at_int(loadstmt, idx,
                                                    s7_string(newval));
                if (errno == 0)
                    return s7_unspecified(s7);
                else
                    return handle_errno(s7, errno, selector);
            } else {
                if (s7_is_list(s7, newval)) {
                    errno = 0;
                    _loadstmt_args_splice_list(s7, loadstmt, idx, newval);
                    if (errno == 0)
                        return s7_unspecified(s7);
                    else
                        return handle_errno(s7, errno, selector);
                } else {
                    if (s7_is_vector(newval)) {
                        errno = 0;
                        _loadstmt_args_splice_vector(s7,
                                                     loadstmt, idx, newval);
                        if (errno == 0)
                            return s7_unspecified(s7);
                        else
                            return handle_errno(s7, errno, selector);
                    } else {
                        log_error("Invalid arg: expected string, list or vector; got %s", s7_object_to_c_string(s7, newval));
                        errno = EINVALID_LOAD_ARG_VAL;
                        return NULL;
                    }
                }
            }
        } else {
            if (s7_is_string(selector)) {
                if (s7_is_string(newval)) {
                    log_debug("replacing string");
                    errno = 0;
                    sealark_loadstmt_replace_arg_at_str(loadstmt,
                                                        s7_string(selector),
                                                        s7_string(newval));
                    if (errno == 0)
                        return s7_unspecified(s7);
                    else
                        return handle_errno(s7, errno, selector);
                } else {
                    idx = sealark_loadstmt_arg_idx_for_str(loadstmt,
                                                     s7_string(selector));
                    log_debug("key: %s, idx: %d", s7_string(selector), idx);
                    if (idx < 0)
                        return handle_errno(s7, errno, selector);

                    if (s7_is_list(s7, newval)) {
                        log_debug("splicing list at %d", idx);
                        errno = 0;
                        _loadstmt_args_splice_list(s7, loadstmt, idx, newval);
                        if (errno == 0)
                            return s7_unspecified(s7);
                        else
                            return handle_errno(s7, errno, selector);
                    } else {
                        if (s7_is_vector(newval)) {
                            log_debug("splicing vector after %d", idx);
                            errno = 0;
                            _loadstmt_args_splice_vector(s7,
                                                         loadstmt, idx, newval);
                            if (errno == 0)
                                return s7_unspecified(s7);
                            else
                                return handle_errno(s7, errno, selector);
                        } else {
                            log_error("Invalid arg: expected string, list or vector; got %s", s7_object_to_c_string(s7, newval));
                            errno = EINVALID_LOAD_ARG_VAL;
                            return NULL;
                        }
                    }
                }
            } else {
                log_error("Invalid arg: expected int, kwint, or string; got %s", s7_object_to_c_string(s7, selector));
                return handle_errno(s7, EINVALID_LOAD_ARG, selector);
            }
        }
    }

    if (op == KW(@) || op == KW(attr) || op == KW(binding)) {
        /* (set! (pkg :load ref :@ ref) newval) */
        /* newval: binding or list or vector of bindings */
        /*  or: string, e.g (set! (pkg :load :0 :@ :0 :key) "newkey") */
        if (s7_is_string(newval)) {
            /* must be setting :key or :value */
            if (selector == KW(key) || selector == KW(value)) {
                log_debug("replacing %s of binding", s7_object_to_c_string(s7, selector));
                errno = 0;
                _loadstmt_binding_replace_k_or_v(s7, loadstmt,
                                                 s7_cdr(get_path),
                                                 selector, newval);
                if (errno == 0)
                    return s7_unspecified(s7);
                else
                    return handle_errno(s7, errno, get_path);
            } else {
                log_error("string newval only allowed for binding :key or :value");
                return handle_errno(s7, EINVALID_SET_ATTR_NEWVAL, newval);
            }
        }

        if ( !s7_is_c_object(newval) ) {
            if ( !s7_is_list(s7, newval) ) {
                if ( !s7_is_vector(newval) ) {
                    log_error("newval for set! attr must be binding or list or vector of bindings");
                    return handle_errno(s7, EINVALID_SET_ATTR_NEWVAL, newval);
                }
            }
        }

        int idx = sunlark_kwindex_to_int(s7, selector);
        if (errno == 0) { // (pkg :load x :@ int-or-kwint ...
            if (s7_is_c_object(newval)) {
                /* (set! (pkg :load i :@ b) <binding>) */
                if (sunlark_node_tid(s7,newval) == TK_Binding) {
                    log_debug("0 NEW BINDING: %s",
                              s7_object_to_c_string(s7, newval));
                    errno = 0;
                    sealark_loadstmt_replace_attr_at_int(loadstmt,
                                                            idx,
                                         s7_c_object_value(newval));
                    if (errno == 0)
                        return s7_unspecified(s7);
                    else
                        return handle_errno(s7, errno, selector);
                } else {
                    log_error("Invalid newval for set! binding");
                   return handle_errno(s7, EINVALID_SET_ATTR_NEWVAL, newval);
                }
                /* errno = 0; */
                /* sealark_loadstmt_replace_arg_at_int(loadstmt, idx, */
                /*                                     s7_string(newval)); */
                /* if (errno == 0) */
                /*     return s7_unspecified(s7); */
                /* else */
                /*     return handle_errno(s7, errno, selector); */
            } else {
                if (s7_is_list(s7, newval)) {
log_debug("0 xxxxxxxxxxxxxxxx");
                    errno = 0;
                    _loadstmt_attrs_splice_list(s7, loadstmt, idx, newval);
                    if (errno == 0)
                        return s7_unspecified(s7);
                    else
                        return handle_errno(s7, errno, selector);
                } else {
                    if (s7_is_vector(newval)) {
                        errno = 0;
                        _loadstmt_attrs_splice_vector(s7,
                                                     loadstmt, idx, newval);
                        if (errno == 0)
                            return s7_unspecified(s7);
                        else
                            return handle_errno(s7, errno, selector);
                    } else {
                        log_error("Invalid arg: expected string, list or vector; got %s", s7_object_to_c_string(s7, newval));
                        errno = EINVALID_LOAD_ARG_VAL;
                        return NULL;
                    }
                }
            }
        } else {
            if (s7_is_symbol(selector)) {
                // (pkg :load x :@ c-obj ...)
                // newval already vetted above
                idx = sealark_loadstmt_attr_idx_for_str(loadstmt,
                                               s7_symbol_name(selector));
                log_debug("key: %s, idx: %d",
                          s7_symbol_name(selector), idx);
                if (idx < 0)
                    return handle_errno(s7, errno, selector);

                if (s7_is_c_object(newval)) {
                        log_debug("replacing attr at %d", idx);
                        sealark_loadstmt_replace_attr_at_int(loadstmt,
                                                             idx,
                                                s7_c_object_value(newval));
                        if (errno == 0)
                            return s7_unspecified(s7);
                        else
                            return handle_errno(s7, errno, selector);
                } else {
                    if (s7_is_list(s7, newval)) {
                        log_debug("splicing list at %d", idx);
                        errno = 0;
                        _loadstmt_attrs_splice_list(s7, loadstmt, idx, newval);
                        if (errno == 0)
                            return s7_unspecified(s7);
                        else
                            return handle_errno(s7, errno, selector);
                    } else {
log_debug("1 xxxxxxxxxxxxxxxx");
                        if (s7_is_vector(newval)) {
                            log_debug("splicing vector after %d", idx);
                            errno = 0;
                            _loadstmt_attrs_splice_vector(s7,
                                                         loadstmt, idx, newval);
                            if (errno == 0)
                                return s7_unspecified(s7);
                            else
                                return handle_errno(s7, errno, selector);
                        } else {
                            log_error("Invalid arg: expected string, list or vector; got %s", s7_object_to_c_string(s7, newval));
                            errno = EINVALID_LOAD_ARG_VAL;
                            return NULL;
                        }
                    }
                }
            } else {
                log_error("Invalid arg: expected int, kwint, or string; got %s", s7_object_to_c_string(s7, selector));
                return handle_errno(s7, EINVALID_LOAD_ARG, selector);
            }
        }
    }
log_debug("9 xxxxxxxxxxxxxxxx");
    return NULL;
}

/* **************************************************************** */
LOCAL void _loadstmt_splice_list(s7_scheme *s7, struct node_s *pkg,
                                 int index, s7_pointer splice)
{
#if defined(DEBUG_TRACE) || defined(DEBUG_MUTATE)
    log_debug("_loadstmt_splice_list at %d", index);
#endif
    assert(pkg->tid == TK_Package);

    if (!s7_is_list(s7, splice)) {
        log_error("Splice expected list, got %s",
                  s7_object_to_c_string(s7, splice));
        errno = EINVALID_LOAD_NEWVAL;
        return;
    }

    /* index = sealark_normalize_index(bindings, index); */
    /* normalize index - cannot use util routine since there is no
       args node type */
    int loadstmt_ct = sealark_pkg_loadstmt_count(pkg);
    log_debug("loadstmt_ct: %d", loadstmt_ct);
    /* reverse indexing */
    if (index < 0) {
        if (abs(index) > loadstmt_ct) {
            log_error("abs(%d) > loadstmt_ct", index, loadstmt_ct);
            errno = EINDEX_OUT_OF_BOUNDS;
            return;
        } else {
            index = loadstmt_ct + index;
            /* log_debug("recurring..."); */
            /* return sealark_vector_item_for_int(node, index); */
        }
    }

    if (index > loadstmt_ct-1) {
        log_error("index > tattret count");
        errno = EINDEX_OUT_OF_BOUNDS;
        return;
    }
#if defined (DEBUG_TRACE) || defined(DEBUG_VECTORS)
    log_debug("\tidx normalized: %d", index);
#endif

    int len = s7_list_length(s7, splice);
    struct node_s *new_loadstmt_nd;

    int i = 0;
    s7_pointer iter = s7_make_iterator(s7, splice);
    s7_pointer new_loadstmt = s7_iterate(s7, iter);
    while ( ! s7_iterator_is_at_end(s7, iter) ) {
        log_debug("splicing item %d", i);
        if ( !s7_is_c_object(new_loadstmt)) {
            log_error("Loadstmt :splice only accepts :loadstmt nodes; got: %s of type %s",
                      s7_object_to_c_string(s7, new_loadstmt),
                      s7_object_to_c_string(s7, s7_type_of(s7, new_loadstmt)));
            errno = ESPLICE_LOADSTMT_ARG;
            return;
        }

        /* log_debug("splicing loadstmt %d: %s", */
        /*           i, s7_object_to_c_string(s7, new_loadstmt)); */
        new_loadstmt_nd = s7_c_object_value(new_loadstmt);
        new_loadstmt_nd->line = -1; // mark as unformatted
        new_loadstmt_nd->col  = -1; // mark as unformatted
        /* log_debug("new_loadstmt_nd %d %s", */
        /*           new_loadstmt_nd->tid, TIDNAME(new_loadstmt_nd)); */
        /* item must be loadstmt */
        if (new_loadstmt_nd->tid == TK_Load_Stmt) {
            errno = 0;
            sealark_pkg_insert_loadstmt_at_int(pkg, index + i,
                                               new_loadstmt_nd);
            if (errno != 0) {
                log_error("Error splicing...%d", errno);
                errno = ESPLICE_LOADSTMT_ARG;
                return;
            }
        } else {
            log_error("Trying to splice non-loadstmt into loadstmts: %d %s",
                      new_loadstmt_nd->tid, TIDNAME(new_loadstmt_nd));
            errno = ESPLICE_LOADSTMT_ARG;
            return;
        }
        i++;
        new_loadstmt = s7_iterate(s7, iter);
    }
}

/* **************************************************************** */
LOCAL void _loadstmt_args_splice_list(s7_scheme *s7,
                                      struct node_s *loadstmt,
                                      int index,
                                      s7_pointer splice)
{
#if defined(DEBUG_TRACE) || defined(DEBUG_MUTATE)
    log_debug(">> _loadstmt_args_splice_list at %d", index);
#endif
    assert(loadstmt->tid == TK_Load_Stmt);
    assert(s7_is_list(s7, splice));

    /* index = sealark_normalize_index(bindings, index); */
    /* normalize index - cannot use util routine since there is no
       args node type */
    int arg_ct = sealark_loadstmt_arg_count(loadstmt);
    log_debug("arg_ct: %d", arg_ct);
    /* reverse indexing */
    if (index < 0) {
        if (abs(index) > arg_ct) {
            log_error("abs(%d) > arg_ct", index, arg_ct);
            errno = EINDEX_OUT_OF_BOUNDS;
            return;
        } else {
            index = arg_ct + index;
            /* log_debug("recurring..."); */
            /* return sealark_vector_item_for_int(node, index); */
        }
    }

    if (index > arg_ct-1) {
        log_error("index > arg count");
        errno = EINDEX_OUT_OF_BOUNDS;
        return;
    }
#if defined (DEBUG_TRACE) || defined(DEBUG_VECTORS)
    log_debug("\tidx normalized: %d", index);
#endif

    int len = s7_list_length(s7, splice);
    struct node_s *new_arg_nd;

    int i = 0;
    s7_pointer iter = s7_make_iterator(s7, splice);
    s7_pointer new_arg = s7_iterate(s7, iter);
    while ( ! s7_iterator_is_at_end(s7, iter) ) {
        log_debug("splicing item %d", i);
        if ( !s7_is_string(new_arg)) {
            log_error("Loadstmt :arg splicing only accepts strings; got: %s of type %s",
                      s7_object_to_c_string(s7, new_arg),
                      s7_object_to_c_string(s7, s7_type_of(s7, new_arg)));
            errno = ESPLICE_LOADSTMT_ARG;
            return;
        }

        new_arg_nd = sealark_new_s_node(TK_STRING, s7_string(new_arg));
        /* new_arg_nd->line = -1; // mark as unformatted */
        /* new_arg_nd->col  = -1; // mark as unformatted */
        /* log_debug("new_arg_nd %d %s", */
        /*           new_arg_nd->tid, TIDNAME(new_arg_nd)); */
        /* item must be loadstmt */
        if (new_arg_nd->tid == TK_STRING) {
            errno = 0;
            sealark_loadstmt_insert_arg_at_int(loadstmt,
                                               index + i,
                                               new_arg_nd);
            if (errno != 0) {
                log_error("Error splicing...%d", errno);
                errno = ESPLICE_LOADSTMT_ARG;
                return;
            }
        } else {
            log_error("Trying to splice non-loadstmt into loadstmts: %d %s",
                      new_arg_nd->tid, TIDNAME(new_arg_nd));
            errno = ESPLICE_LOADSTMT_ARG;
            return;
        }
        i++;
        new_arg = s7_iterate(s7, iter);
    }
}

/* **************************************************************** */
LOCAL void _loadstmt_args_splice_vector(s7_scheme *s7,
                                      struct node_s *loadstmt,
                                      int index,
                                      s7_pointer splice)
{
#if defined(DEBUG_TRACE) || defined(DEBUG_MUTATE)
    log_debug("_loadstmt_args_splice_vector at %d", index);
#endif
    assert(loadstmt->tid == TK_Load_Stmt);
    assert(s7_is_vector(splice));

    /* index = sealark_normalize_index(bindings, index); */
    /* normalize index - cannot use util routine since there is no
       args node type */
    int arg_ct = sealark_loadstmt_arg_count(loadstmt);
    log_debug("arg_ct: %d", arg_ct);
    /* reverse indexing */
    if (index < 0) {
        if (abs(index) > arg_ct) {
            log_error("abs(%d) > arg_ct", index, arg_ct);
            errno = EINDEX_OUT_OF_BOUNDS;
            return;
        } else {
            index = arg_ct + index;
            /* log_debug("recurring..."); */
            /* return sealark_vector_item_for_int(node, index); */
        }
    }

    if (index > arg_ct-1) {
        log_error("index > arg count");
        errno = EINDEX_OUT_OF_BOUNDS;
        return;
    }
#if defined (DEBUG_TRACE) || defined(DEBUG_VECTORS)
    log_debug("\tidx normalized: %d", index);
#endif

    index++; // splice after

    int len = s7_list_length(s7, splice);
    struct node_s *new_arg_nd;

    int i = 0;
    s7_pointer iter = s7_make_iterator(s7, splice);
    s7_pointer new_arg = s7_iterate(s7, iter);
    while ( ! s7_iterator_is_at_end(s7, iter) ) {
        log_debug("splicing item %d", i);
        if ( !s7_is_string(new_arg)) {
            log_error("Loadstmt :arg splicing only accepts strings; got: %s of type %s",
                      s7_object_to_c_string(s7, new_arg),
                      s7_object_to_c_string(s7, s7_type_of(s7, new_arg)));
            errno = ESPLICE_LOADSTMT_ARG;
            return;
        }

        new_arg_nd = sealark_new_s_node(TK_STRING, s7_string(new_arg));
        /* new_arg_nd->line = -1; // mark as unformatted */
        /* new_arg_nd->col  = -1; // mark as unformatted */
        /* log_debug("new_arg_nd %d %s", */
        /*           new_arg_nd->tid, TIDNAME(new_arg_nd)); */
        /* item must be loadstmt */
        if (new_arg_nd->tid == TK_STRING) {
            errno = 0;
            sealark_loadstmt_insert_arg_at_int(loadstmt,
                                               index + i,
                                               new_arg_nd);
            if (errno != 0) {
                log_error("Error splicing...%d", errno);
                errno = ESPLICE_LOADSTMT_ARG;
                return;
            }
        } else {
            log_error("Trying to splice non-loadstmt into loadstmts: %d %s",
                      new_arg_nd->tid, TIDNAME(new_arg_nd));
            errno = ESPLICE_LOADSTMT_ARG;
            return;
        }
        i++;
        new_arg = s7_iterate(s7, iter);
    }
}

/* **************************************************************** */
LOCAL void _loadstmt_splice_vector(s7_scheme *s7, struct node_s *pkg,
                                   int index, s7_pointer splice)
{
#if defined(DEBUG_TRACE) || defined(DEBUG_MUTATE)
    log_debug("_loadstmt_splice_list at %d", index);
#endif
    assert(pkg->tid == TK_Package);

    if ( (!s7_is_list(s7, splice))
         &&  (!s7_is_vector(splice)) ) {
        log_error("Splice expects list or vector, got %s",
                  s7_object_to_c_string(s7, splice));
        errno = EINVALID_LOAD_NEWVAL;
        return;
    }

    /* index = sealark_normalize_index(bindings, index); */
    /* normalize index - cannot use util routine since there is no
       args node type */
    int loadstmt_ct = sealark_pkg_loadstmt_count(pkg);
    log_debug("loadstmt_ct: %d", loadstmt_ct);
    /* reverse indexing */
    if (index < 0) {
        if (abs(index) > loadstmt_ct) {
            log_error("abs(%d) > loadstmt_ct", index, loadstmt_ct);
            errno = EINDEX_OUT_OF_BOUNDS;
            return;
        } else {
            index = loadstmt_ct + index;
            /* log_debug("recurring..."); */
            /* return sealark_vector_item_for_int(node, index); */
        }
    }

    if (index > loadstmt_ct-1) {
        log_error("index > tattret count");
        errno = EINDEX_OUT_OF_BOUNDS;
        return;
    }

    index++; /* splice AFTER index */

#if defined (DEBUG_TRACE) || defined(DEBUG_VECTORS)
    log_debug("\tidx normalized: %d", index);
#endif

    int len = s7_vector_length(splice);
    struct node_s *new_loadstmt_nd;

    int i = 0;
    s7_pointer iter = s7_make_iterator(s7, splice);
    s7_pointer new_loadstmt = s7_iterate(s7, iter);
    while ( ! s7_iterator_is_at_end(s7, iter) ) {
        log_debug("splicing item %d", i);
        if ( !s7_is_c_object(new_loadstmt)) {
            log_error("Loadstmt :splice only accepts :loadstmt nodes; got: %s of type %s",
                      s7_object_to_c_string(s7, new_loadstmt),
                      s7_object_to_c_string(s7, s7_type_of(s7, new_loadstmt)));
            errno = ESPLICE_LOADSTMT_ARG;
            return;
        }

        /* log_debug("splicing loadstmt %d: %s", */
        /*           i, s7_object_to_c_string(s7, new_loadstmt)); */
        new_loadstmt_nd = s7_c_object_value(new_loadstmt);
        new_loadstmt_nd->line = -1; // mark as unformatted
        new_loadstmt_nd->col  = -1; // mark as unformatted
        /* log_debug("new_loadstmt_nd %d %s", */
        /*           new_loadstmt_nd->tid, TIDNAME(new_loadstmt_nd)); */
        /* item must be loadstmt */
        if (new_loadstmt_nd->tid == TK_Load_Stmt) {
            errno = 0;
            sealark_pkg_insert_loadstmt_at_int(pkg, index + i,
                                               new_loadstmt_nd);
            if (errno != 0) {
                log_error("Error splicing...%d", errno);
                errno = ESPLICE_LOADSTMT_ARG;
                return;
            }
        } else {
            log_error("Trying to splice non-loadstmt into loadstmts: %d %s",
                      new_loadstmt_nd->tid, TIDNAME(new_loadstmt_nd));
            errno = ESPLICE_LOADSTMT_ARG;
            return;
        }
        i++;
        new_loadstmt = s7_iterate(s7, iter);
    }
}

/* **************************************************************** */
LOCAL void _loadstmt_attrs_splice_list(s7_scheme *s7,
                                      struct node_s *loadstmt,
                                      int index,
                                      s7_pointer splice)
{
#if defined(DEBUG_TRACE) || defined(DEBUG_MUTATE)
    log_debug("_loadstmt_attrs_splice_list at %d", index);
#endif
    assert(loadstmt->tid == TK_Load_Stmt);
    assert(s7_is_list(s7, splice));

    /* index = sealark_normalize_index(bindings, index); */
    /* normalize index - cannot use util routine since there is no
       attrs node type */
    int attr_ct = sealark_loadstmt_binding_count(loadstmt);
    log_debug("attr_ct: %d", attr_ct);
    /* reverse indexing */
    if (index < 0) {
        if (abs(index) > attr_ct) {
            log_error("abs(%d) > attr_ct", index, attr_ct);
            errno = EINDEX_OUT_OF_BOUNDS;
            return;
        } else {
            index = attr_ct + index;
            /* log_debug("recurring..."); */
            /* return sealark_vector_item_for_int(node, index); */
        }
    }

    if (index > attr_ct-1) {
        log_error("index > attr count");
        errno = EINDEX_OUT_OF_BOUNDS;
        return;
    }
#if defined (DEBUG_TRACE) || defined(DEBUG_VECTORS)
    log_debug("\tidx normalized: %d", index);
#endif

    int len = s7_list_length(s7, splice);
    struct node_s *new_attr_nd;

    int i = 0;
    s7_pointer iter = s7_make_iterator(s7, splice);
    s7_pointer new_attr = s7_iterate(s7, iter);
    while ( ! s7_iterator_is_at_end(s7, iter) ) {
        log_debug("splicing item %d", i);
        if ( !s7_is_c_object(new_attr)) {
            log_error("Loadstmt :attr splicing only accepts binding nodes; got: %s of type %s",
                      s7_object_to_c_string(s7, new_attr),
                      s7_object_to_c_string(s7, s7_type_of(s7, new_attr)));
            errno = ESPLICE_LOADSTMT_ATTR;
            return;
        }

        struct node_s *x = s7_c_object_value(new_attr);
        if ( sunlark_node_tid(s7, new_attr) != TK_Binding ) {
            log_error("Loadstmt :attr splicing only accepts binding nodes; got: %s of type %s",
                      s7_object_to_c_string(s7, new_attr),
                      s7_object_to_c_string(s7,
                                            s7_type_of(s7, new_attr)));
            errno = ESPLICE_LOADSTMT_ATTR;
            return;
        }

        struct node_s *new_attr_nd = s7_c_object_value(new_attr);
        errno = 0;
        sealark_loadstmt_insert_binding_at_int(loadstmt,
                                               index + i,
                                               new_attr_nd);
        if (errno != 0) {
            /* log_error("Error splicing...%d", errno); */
            /* return handle_errno(s7, errno, ); */
            return;
       }
        i++;
        new_attr = s7_iterate(s7, iter);
    }
}

/* **************************************************************** */
LOCAL void _loadstmt_attrs_splice_vector(s7_scheme *s7,
                                         struct node_s *loadstmt,
                                         int index,
                                         s7_pointer splice)
{
#if defined(DEBUG_TRACE) || defined(DEBUG_MUTATE)
    log_debug("_loadstmt_attrs_splice_vector at %d", index);
#endif
    assert(loadstmt->tid == TK_Load_Stmt);
    assert(s7_is_vector(splice));

    /* index = sealark_normalize_index(bindings, index); */
    /* normalize index - cannot use util routine since there is no
       attrs node type */
    int attr_ct = sealark_loadstmt_binding_count(loadstmt);
    log_debug("attr_ct: %d", attr_ct);
    /* reverse indexing */
    if (index < 0) {
        if (abs(index) > attr_ct) {
            log_error("abs(%d) > attr_ct", index, attr_ct);
            errno = EINDEX_OUT_OF_BOUNDS;
            return;
        } else {
            index = attr_ct + index;
            /* log_debug("recurring..."); */
            /* return sealark_vector_item_for_int(node, index); */
        }
    }

    if (index > attr_ct-1) {
        log_error("index > attr count");
        errno = EINDEX_OUT_OF_BOUNDS;
        return;
    }

    index++; // splice AFTER
#if defined (DEBUG_TRACE) || defined(DEBUG_VECTORS)
    log_debug("  normalized idx plus 1: %d", index);
#endif

    int len = s7_vector_length(splice);
    struct node_s *new_attr_nd;

    int i = 0;
    s7_pointer iter = s7_make_iterator(s7, splice);
    s7_pointer new_attr = s7_iterate(s7, iter);
    while ( ! s7_iterator_is_at_end(s7, iter) ) {
        log_debug("splicing item %d", i);
        if ( !s7_is_c_object(new_attr)) {
            log_error("Loadstmt :attr splicing only accepts binding nodes; got: %s of type %s",
                      s7_object_to_c_string(s7, new_attr),
                      s7_object_to_c_string(s7, s7_type_of(s7, new_attr)));
            errno = ESPLICE_LOADSTMT_ATTR;
            return;
        }

        if ( sunlark_node_tid(s7, new_attr) != TK_Binding ) {
            log_error("Loadstmt :attr splicing only accepts binding nodes; got: %s of type %s",
                      s7_object_to_c_string(s7, new_attr),
                      s7_object_to_c_string(s7, s7_type_of(s7, new_attr)));
            errno = ESPLICE_LOADSTMT_ATTR;
            return;
        }

        struct node_s *new_attr_nd = s7_c_object_value(new_attr);
        errno = 0;
        sealark_loadstmt_insert_binding_at_int(loadstmt,
                                               index + i,
                                               new_attr_nd);
        if (errno != 0) {
            log_error("Error splicing...%d", errno);
            errno = ESPLICE_LOADSTMT_ATTR;
            return;
        }
        i++;
        new_attr = s7_iterate(s7, iter);
    }
}

/* **************************************************************** */
LOCAL void _loadstmt_binding_replace_k_or_v(s7_scheme *s7,
                                            struct node_s *loadstmt,
                                            s7_pointer path,
                                            s7_pointer selector,
                                            s7_pointer newval)
{
#if defined(DEBUG_LOADS)
    log_debug(">> _loadstmt_binding_replace_k_or_v");
    log_debug("path: %s", s7_object_to_c_string(s7, path));
    log_debug("selector: %s", s7_object_to_c_string(s7, selector));
    log_debug("newval: %s", s7_object_to_c_string(s7, newval));
#endif

    if (s7_list_length(s7, path) != 1) {
        log_error("missing binding selector");
        errno = EINVALID_ARG;
        return;
    }
    s7_pointer indexer = s7_car(path);
    int idx = sunlark_kwindex_to_int(s7, indexer);
    if (errno == 0) { // int or kwint
            log_debug("derefing binding by int");
            /* selector has already been vetted, must be :key or :value */
            if (selector == KW(key)) {
                sealark_loadstmt_binding_for_int_replace_key(loadstmt,
                                                             idx,
                                                         s7_string(newval));
            } else {
                sealark_loadstmt_binding_for_int_replace_val(loadstmt,
                                                             idx,
                                                   s7_string(newval));
            }
    } else {
        if (s7_is_symbol(indexer)) {
            log_debug("derefing binding by keysym");
            if (selector == KW(key)) {
                sealark_loadstmt_binding_for_sym_replace_key(loadstmt,
                                                    s7_symbol_name(indexer),
                                                         s7_string(newval));
            } else {
                sealark_loadstmt_binding_for_sym_replace_val(loadstmt,
                                                    s7_symbol_name(indexer),
                                                         s7_string(newval));
            }
        } else {
            log_debug("binding indexer must be int, kwint, or symbol");
            errno = EINVALID_ARG;
            return;
        }
    }

    return;
}
