#include "s7.h"

#include "sunlark_errors.h"

s7_pointer handle_errno(s7_scheme *s7, int errorno, s7_pointer path_args)
{
    switch(errorno) {
    case ENOT_IMPLEMENTED:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                                                      "Not yet implemented: ~A"),
                                path_args)));
        break;
    case ETOO_MANY_ARGS:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                                                      "Too many args: ~A"),
                                path_args)));
        break;
    case ETOO_MANY_ARGS_BINDINGS:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                                "Too many args: ~A - :bindings must be last arg."),
                                path_args)));
        break;
    case EMISSING_ARG_BINDING:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                        "Invalid: ~A - missing selector after :binding"),
                                path_args)));
        break;
    case EMISSING_ARG_SELECTOR:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                                                      "~A: missing selector after :arg"),
                                path_args)));
        break;
    case EMISSING_GET_SELECTOR:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 1, s7_make_string(s7,
                        "Getter is missing selector"))));
        break;
    case EINVALID_LOAD_ARG:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                        "Expected int, kwint, or string, got ~A"),
                                path_args)));
        break;
    case EINVALID_LOAD_CTOR_ARGLIST:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                        "make-load arglist: string key, :args list of strings, :attrs list of attrs; got ~A"),
                                path_args)));
        break;
    case EINVALID_LOAD_CTOR_KEY:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                        "First arg to make-load must be string key: ~A"),
                                path_args)));
        break;
    case EINVALID_LOAD_CTOR_ARGS:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                        "make-load :args must be list of strings: ~A"),
                                path_args)));
        break;
    case EINVALID_LOAD_CTOR_ATTRS:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                        "make-load :attrs must be list of bindings: ~A"),
                                path_args)));
        break;
    case EINVALID_LOAD_CTOR_ATTR:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                        "make-load found non binding in :attrs list: ~A"),
                                path_args)));
        break;
    case EINVALID_LOAD_SELECTOR:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                "Selector for :load must be int, kwint, or string: ~A"),
                                path_args)));
        break;
    case EINVALID_LOAD_NEWVAL:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                        "set! load newval must be a :load-stmt: ~A"),
                                path_args)));
        break;
    case ESPLICE_LOADSTMT_ARG:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                        "Splicing loadstmt args must be :load-stmt ~A"),
                                path_args)));
        break;
    case EINVALID_GET_SELECTOR:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                        "Selector ~A in getter expr is invalid  here."),
                                path_args)));
        break;
    case EINVALID_SERIALIZE_AST:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
       "Serialization with :ast only works for sunlark nodes; got arg of type: ~A; try (display obj) or (sunlark->string obj :write)?"),
                                path_args)));
        break;
    case EINVALID_SERIALIZE_STARLARK:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
       "Serialization with :starlark only works for sunlark nodes; got arg of type: ~A; try (display obj) or (sunlark->string obj :write)?"),
                                path_args)));
        break;
    case ENOT_FOUND:
        return(s7_error(s7, s7_make_symbol(s7, "not_found"),
                        s7_list(s7, 2, s7_make_string(s7,
                                                      "Not found: ~A"),
                                (path_args))));
        break;
    case ENOT_FOUND_BINDING:
        return(s7_error(s7, s7_make_symbol(s7, "binding_not_found"),
                        s7_list(s7, 2, s7_make_string(s7,
                           "Binding not found for: ~A"),
                                (path_args))));
        break;
    case EINDEX_OUT_OF_BOUNDS:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                           "Index out of bounds: ~A"),
                                (path_args))));
        break;
    case EINDEX_TYPE_ERR:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                        "Attempting to index a non-sequence: ~A"),
                                (path_args))));
        break;
    case EINVALID_INT_INDEX:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_index"),
                        s7_list(s7, 2, s7_make_string(s7,
                 "~A - integer indexing not supported, please use keywordized int, like :2"),
                                (path_args))));
        break;
    case EINVALID_REMOVE:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_remove"),
                        s7_list(s7, 2, s7_make_string(s7,
                 "Deletion (set! (...) :null) not allowed in this context: ~A"),
                                (path_args))));
        break;
    case EINVALID_UPDATE:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_update"),
                        s7_list(s7, 2, s7_make_string(s7,
                 "Invalid update value: ~A"),
                                (path_args))));
        break;
    case EINVALID_ACTION:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_action"),
                        s7_list(s7, 2, s7_make_string(s7,
                        "Action ~A for set! is invalid in this context."),
                                path_args)));
        break;
    case EINVALID_ARG:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                        "Invalid arg ~A"),
                                path_args)));
        break;
    case EINVALID_ARG_LOAD:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                                                      "Invalid arg ~A following :load"),
                                path_args)));
        break;
    case ENULL_BINDING_VAL:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                        "Cannot set :value of binding to null: ~A"),
                                path_args)));
        break;
    case ESPLICE_ARG:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
               ":splice must be followed by vector, got: ~A"),
                                path_args)));
        break;
    case ESPLICE_BINDING_ARG:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
               "Binding :splice only accepts :binding nodes; got ~A"),
                                path_args)));
        break;
    default:
        return(s7_error(s7, s7_make_symbol(s7, "invalid_argument"),
                        s7_list(s7, 2, s7_make_string(s7,
                                                      "Error: errno ~D on path_args: ~A"),
                                s7_make_integer(s7,errorno),
                                path_args)));
    }
}
