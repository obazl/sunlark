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
    # register toolchains for native build tools installed on exec host?
    register_preinstalled_tools=False,
    # register toolchains that build tools from source?
    register_built_tools=False
)

```

Run a repl: `$ bazel run @sunlark//repl`

Batch process a file:

1. Create `.sunlark.d/edit.scm`, containing a procedure named `sunlark-ast-handler` that takes one AST node argument; for example:

[.sunlark.d/edit.scm]
```
(define (sunlark-ast-handler pkg)
  (display (string-append "running local sunlark-ast-handler proc"))
  (newline)
  (display (format #f "scheme load path: ~S" *load-path*))
  (newline)
  (display (format #f "(sunlark-node? pkg) => ~A"
                   (sunlark-node? pkg)))
  (newline)
  (display (format #f "(pkg :package?) => ~A"
                   (pkg :package?)))
  (newline) (newline)
  (display (sunlark->string pkg :starlark))
  (newline))

```

Add edit operations to this routine, then reformat and save changes:
`(pkg :format) (pkg :save)`.

2. Run the edit: `$ bazel run @sunlark//edit -- -f path/to/BUILD.bazel`




## misc
