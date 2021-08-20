# sunlark
Scheme binding for [Sealark](https://github.com/obazl/sealark) (Starlark parser)

## quickstart

[WORKSPACE.bazel]
```
load("@bazel_tools//tools/build_defs/repo:git.bzl", "git_repository")

git_repository(
    name = "sunlark",
    remote = "https://github.com/obazl/sunlark",
    branch = "main",
)
load("@sunlark//:WORKSPACE.bzl", "cc_fetch_repos")
cc_fetch_repos() ## fetches sealark, rules_cc, rules_foreign_cc

## The following load statement sets up some common toolchains for building targets.
## Needed for sealark; for more details, please see:
##  https://bazelbuild.github.io/rules_foreign_cc/0.5.1/flatten.html#rules_foreign_cc_dependencies
load("@rules_foreign_cc//foreign_cc:repositories.bzl", "rules_foreign_cc_dependencies")

rules_foreign_cc_dependencies(
    ## register toolchains for native build tools installed on exec host?
    # register_preinstalled_tools=False,
    ## register toolchains that build tools from source?
    # register_built_tools=False
)

```

Run a repl: `$ bazel run @sunlark//repl`

Batch process a file:

1. Create `.sunlark.d/edit.scm`, containing a procedure named `sunlark-ast-handler` that takes one AST node argument; for example:

[.sunlark.d/edit.scm]
```
(define (sunlark-ast-handler pkg)
  (display "running local sunlark-ast-handler proc") (newline)
  (display (format #f "scheme load path: ~S" *load-path*)) (newline)
  (display (format #f "(sunlark-node? pkg) => ~A" (sunlark-node? pkg))) (newline)
  (display (format #f "(pkg :package?) => ~A" (pkg :package?))) (newline)
  (newline)
  (display (sunlark->string pkg :starlark)) (newline)
  ;; ... edit operations ...
  (remove-trailing-commas pkg) ;; optional
  (pkg :format) ;; optional - does some (primitive) fixing of line/col stuff
  (pkg :save)   ;; optional; overwrites original

```

2. Run the edit: `$ bazel run @sunlark//edit -- -f path/to/BUILD.bazel`

##  Scheme API

The Scheme dialect used by Sunlark is (roughly) documented at [https://ccrma.stanford.edu/software/snd/snd/s7.html](https://ccrma.stanford.edu/software/snd/snd/s7.html).  According to that, it should be compatible with R5RS and R7Rs.

You do not need to know much Scheme to use Sunlark to do basic inspection and alteration of BUILD files.

You can find many examples of API usage in [test/scm](test/scm). A good way to explore it hands-on is to copy the code there into your private `.sunlark.d` directory and run it using `bazel run @sunlark//edit`.

You can also find many examples in the unit tests in
[test/unit/sunlark](test/unit/sunlark), although those will be harder
to decipher, since they are written in C.

File [test/scm/buildozer.scm](test/scm/buildozer.scm) contains some examples showing how [Buildozer](https://github.com/bazelbuild/buildtools/blob/master/buildozer/README.md) commands can be translated into Sunlark.

### Path DSL

Sunlark supports a kind of micro-DSL loosely analogous to the path
language of XSL (and CSS). To access or update a node in an AST, you
write a "path" expression leading from the AST root to the node of
interest. The general principle is that each "step" in the path
selects something from the result of the previous step. So a path
expression can be thought of as a kind of pipeline.

Examples, using `pkg` as the AST root node:

* First load statement in file: `(pkg :load :0)` or `(pkg :load 0)` (Sunlark
  supports integers and keywordized integers (e.g. `0` or `:0`) for
  indexing.)
* List of all load statements: `(pkg :loads)`
* Last target in file: `(pkg :target :-1)` or `(pkg :> :-1)`
* All targets: `(pkg :targets)` or `(pkg :>>)`

* The `deps` attribute of the `"hello-world"` target: `(pkg :>
  "hello-world" :@ 'deps)`.

Attributes (which Sunlark calls 'bindings') are (key,val) pairs. So, assuming we have

```
cc_binary(
    name = "hello-world",
    srcs = ["hello-world.cc"],
    deps = [":hello-lib", "//fr/bonjour-lib"],
)
```

* The key of the `deps` attribute of the `"hello-world"` target: `(pkg :>
  "hello-world" :@ 'deps :key)` => `deps`
* The value of the `deps` attribute of the `"hello-world"` target: `(pkg :>
  "hello-world" :@ 'deps :value)` => `[":hello-lib", "//fr/bonjour-lib"]`
* The first element of the list value of the `deps` attribute: `(pkg :>
  "hello-world" :@ 'deps :value :0)` => `":hello-lib"`
* The last element of the list value of the `deps` attribute: `(pkg :>
  "hello-world" :@ 'deps :value :-1)` => `"//fr/bonjour-lib"`

**WARNING**: With only a few exceptions, Sunlark operators return either
Sunlark (AST) nodes or lists of Sunlark nodes. In the last two preceding examples, the print representation of the result is a string, but the actual value returnd is an AST node of type TK_STRING. See below for more information.

See [doc/operators.md](doc/operators.md) for a complete list of operators.

#### abbreviations

As a convenience:

* `:>>` == `:targets`
* `:>` == `:target`
* `:@@` == `:bindings` == `:attrs`
* `:@` == `:binding` == `:attr`

### node types and properties

A Sunlark node is a Scheme wrapper around a C `struct node_s*`. Each
node has a (numeric) node type, which is exposed in Scheme as a
keyword predicate; for example, target attributes have type
`TK_Binding`, whose corresponding predicate is `:binding?`. Node type
properties:

* `(nd <t?>)` - where <t?> is a type keyword, e.g. `:package`, `:call-expr`, `:binding`, etc. A complete list is at ... Special cases:
  * `(nd :target?)` - true if `(nd :call-expr?)` is true (i.e. targets are call expressions in the AST, but we support `:target?` as a convenience
  * `(nd symbol?)` - true if `(nd :id?)` is true (i.e. Starlark identifiers are represented as Scheme symbols)
* `(nd :node?)` - #t for Sunlark nodes
* `(nd :tid)` - the node type ID (int)
* `(nd :tid->kw)` - the keyword node type ID (e.g. :binding)
* `(nd :tid->string)` - the node type as a string (e.g. "binding")

 Nodes also have several other predicates and properties:

* `(nd :printable?)` - #t if `nd` is directly printable. For example, node types `:lbrack`, `:rbrack`, `:comma`, `:string`, `:id` etc. are printable, but higher-level structs like `:binding` and `:list-expr` are not. Example: `((pkg :> 0 :@ :0) :printable?)` => `#f`, but `((pkg :> 0 :@ :0 :key) :printable?)` => `#t`.
* `(nd :line)` - the line number of the node in the source file
* `(nd :col)` - the column number of the node in the source file
* `(nd :length)` - for nodes that have subnodes, this gives the number of (semantic) subnodes, e.g. the number of elements in a `:list-expr`.
* `(nd :subnode-count)` - the number of AST subnodes of `nd`; this includes delims (like `:lbrack`, `:rbrack`) and punctuation (e.g. `:comma`).
* `(nd :subnode-count-recursive)` - returns the total number of subnode descendents of `nd`.
* `(nd :printable-subnode-count-recursive)` - counts the total number of descendent subnodes that satisfy `:printable?`.

### filtering

Target lists may be formed by filtering on the :rule property of
target nodes.  A filter is a (Scheme) symbol or list of symbols; if the symbol ends with `*` it will be treated as a glob expression. Examples:

* `(pkg :targets 'cc_binary)` - list only targets with `:rule` == `cc_binary`
* `(pkg :targets '(cc_binary cc_library))` - list targets whose :rule is either `cc_binary` or `cc_library`
* `(pkg :targets 'cc_*)` - list all targets whose `:rule` starts with `cc_`

The result of such filtering expression is a Scheme list.

### mutation

Mutation of the AST is accomplished using Scheme's generic `set!`
operator. The first argument must be a path expression yielding the
value to be set, and the second is the expression to be used to update
it.  Some examples:

* Change the rule of target "hello" to `cc_library`:
  `(set! (pkg :> "hello" :rule) 'cc_library)`

* Append "//foo/bar" to the `deps` list attribute of target "hello":
  `(set! (pkg :> "hello" :@ 'deps :-1) (vector "//foo/bar"))`

* To delete something, just `set!` it to `:null`: `(set! (pkg :>
  "hello" :@ :-1) :null)` removes the last attribute of the "hello"
  target.

**IMPORTANT** Adding uses _splice_ operations: splicing a `list` inserts the values of the list _before_ the splice point; splicing a `vector` inserts the elements of the vector _after_ the splice point. So the preceding example splices "//foo/bar" into the `deps` list after the last (`:-1`) element. If the new value were instead `(list "//foo/bar")`, then it would be spliced _before_ the last element.

Using `set!` like this works the same everywhere; you use the same
technique to add/replace/remove load statments, load statement args
and bindings, targets, bindings, etc.

### constructors

Sometimes you may need to construct new AST nodes. Sunlark provides the following:

* `make-load`
* `make-target`
* `make-binding` (where "binding" is synonymous with "attribute")
* `make-ast-node`
* `sunlark-make-string`

**IMPORTANT** if you need a string usually you can use a Scheme
string; but Starlark supports variants: single- or double-quoted,
using one or three quotes, in plain, raw, binary, or raw binary
encoding. For those you need to use `sunlark-make-string`; see the
tests for examples.

### display and serialization

Sunlark can serialize a Starlark AST in a variety of ways:

* display representation - this is the serialization format used for display in the REPL
* display "readable" representation - uses Scheme syntax that can be "read" in to recreate the same AST
* Starlark syntax
* AST outline syntax

**WARNING** The display represention used in the REPL is experimental;
currently it is a kind of hybrid notation mixing Scheme and Starlark
syntax. The `:readable` syntax is not yet implemented, pending
stabilization of the API.

To print a node `nd` in Starlark syntax: `(sunlark->string nd
:starlark)`. An optional argument is supported: `:squeeze` will
collapse consecutive blank lines into one, and `:crush` will remove
blank lines.

During development it can be useful to inspect the AST, which you can print using: `(sunlark->string nd :ast)`.


## misc
