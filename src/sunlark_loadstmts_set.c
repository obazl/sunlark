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

    if (s7_is_null(s7, get_path)) {
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
                struct node_s *loadstmt
                    = sealark_pkg_loadstmt_for_key(pkg, s7_string(selector));
                if (loadstmt) {
                    if ( !s7_is_c_object(newval) ) {
                        return handle_errno(s7, EINVALID_LOAD_NEWVAL, newval);
                    }
                    sealark_loadstmt_replace(loadstmt,
                                             s7_c_object_value(newval));
                    return s7_unspecified(s7);
                } else {
                    log_debug("732 failed to index load stmt");
                    return NULL;
                }
            } else {
                return handle_errno(s7, EINVALID_LOAD_SELECTOR, selector);
            }
            return NULL;
        }
    }

    s7_pointer op = s7_car(get_path);
    int idx = sunlark_kwindex_to_int(s7, op);
    if (errno == 0) { // op == int or kwint
        struct node_s *loadstmt
            = sealark_pkg_loadstmt_for_int(pkg, idx);
        if (loadstmt) {
            sunlark_loadstmt_set_dispatcher(s7, loadstmt,
                                            s7_cdr(get_path), selector);
            return NULL;
        } else {
            return handle_errno(s7, errno, selector);
        }
    } else {
        log_debug("unexpected");
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
                                           s7_pointer selector)
{
#ifdef DEBUG_TRACE
    log_debug(">> sunlark_loadstmt_set_dispatcher: path %s, selector %s",
              s7_object_to_c_string(s7, get_path),
              s7_object_to_c_string(s7, selector));
#endif

    assert(loadstmt->tid == TK_Load_Stmt);

    if (s7_is_null(s7, get_path)) {
        /* (set! (pkg :load ref) newval) */
        log_error("yyyyyyyyyyyyyyyy");
    }

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
