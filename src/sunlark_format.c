#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include "log.h"

#include "s7.h"

#include "sunlark_format.h"

#if INTERFACE
#define SUNLARK_REMOVE_TRAILING_COMMAS_HELP "(remove-trailing-commas obj) removes all trailing commas from the AST."
/* #define SUNLARK_REMOVE_TRAILING_COMMAS_SIG s7_make_signature(s7, 2, s7_make_symbol(s7, "boolean?"), s7_t(s7)) */
#endif

/* called by Scheme 'node?'; internally, use c_is_sunlark_node (bool) */
s7_pointer sunlark_remove_trailing_commas(s7_scheme *s7, s7_pointer args)
{
#if defined(DEBUG_S7_API)
    log_debug(">>>>>>>>>>>>>>>> sunlark_remove_trailing_commas <<<<<<<<<<<<<<<<");
#endif

    if (s7_list_length(s7, args) > 1) {
        log_error("Too many args to remove-trailing-commas");
        return handle_errno(s7, ETOO_MANY_ARGS, args);
    }

    struct node_s *node = s7_c_object_value(s7_car(args));
    log_debug("0 xxxxxxxxxxxxxxxx");

    sealark_remove_trailing_commas(node);
    return s7_unspecified(s7);
}
