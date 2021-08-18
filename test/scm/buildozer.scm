;; $ bazel run edit -- -f test/buildozer/pkg/BUILD.bazel

(define (test-buildozer-add-dep pkg)
  (display "test-buildozer-add-dep")
  (newline)

  ;; # Edit //pkg:rule and //pkg:rule2, and add a dependency on //base
  ;; buildozer 'add deps //base' //pkg:rule //pkg:rule2
  ;; test data, test/buildozer/pkg/BUILD.bazel, targets: hello-world,
  ;; goodbye-world

  (let* (
         (v (pkg :> "hello-world" :@ 'deps :value :-1))
         )
    ;; splice (append) a single value into the attr value list:
    ;; (set! (pkg :> "hello-world" :@ 'deps :value :-1) (vector "//base"))
    ;; (set! (pkg :> "goodbye-world" :@ 'deps :value :-1) (vector "//base"))

    ;; or: splice (prepend)
    (set! (pkg :> "hello-world" :@ 'deps :value :0) (list "//base"))
    (set! (pkg :> "goodbye-world" :@ 'deps :value :0) (list "//base"))
    (display (sunlark->string pkg :starlark :crush))
    (newline)
    ))

(define (test-buildozer-replace-dep pkg)
  (display "test-buildozer-replace-dep")
  (newline)

  ;; # Replace the dependency on pkg_v1 with a dependency on pkg_v2
  ;; buildozer 'replace deps //pkg_v1 //pkg_v2' //pkg:rule

  (set! (pkg :> "hello-world" :@ 'deps :value ":hello-lib") "//pkg_v2")
  (display (sunlark->string pkg :starlark :crush))
  (newline)
  )

(define (test-buildozer-replace-deps-regex pkg)
  (display "test-buildozer-replace-deps-regex")
  (newline)

  ;; # Replace all dependencies using regular expressions.
  ;; buildozer 'substitute deps //old/(.*) //new/${1}' //pkg:rule

  ;; test: 'srcs from all bindings (:@@) of all targets (:>>)
  ;; (display (pkg :>> :@@ 'srcs)) (newline)

  (for-each (lambda (binding)
              (display (sunlark->string binding :starlark :crush))
              (let ((v (binding :value)))
                (display (format #f "val: ~A" v))
                (newline))
              )
            (pkg :>> :@@ 'srcs))

  ;; (remove-trailing-commas pkg)
  ;; (display (sunlark->string pkg :starlark :crush))
  ;; (newline)
  )

(define (test-buildozer-replace-dep-for-rule pkg)
  (display "test-buildozer-replace-dep-for-rule")
  (newline)

  ;; # Delete the dependency on foo in every cc_library in the package
  ;; buildozer 'remove deps foo' //pkg:%cc_library

  ;; we'll remove the "common.c" src of each cc_library target:
  (for-each (lambda (t)
              ;; (display (sunlark->string t :starlark :crush)) (newline)
              ;; (display (sunlark->string t :ast)) (newline)
              (set! (t :@ 'srcs :value "common.c") :null)
              )
            (pkg :>> 'cc_library))

  (remove-trailing-commas pkg)
  (display (sunlark->string pkg :starlark :crush))
  (newline)
  )

(define (test-buildozer-add-bindings pkg)
  (display "test-buildozer-add-bindings")
  (newline)

  ;; # Set two attributes in the same rule
  ;; buildozer 'set compile 1' 'set srcmap 1' //pkg:rule

  ;; NB: buildozer does not say what "set" means. add? replace?
  ;; prepend, append?

  ;; we'll append a few attrs to target "hello-lib"
  ;; we could prepend them by using a list instead of a vector
  (set! (pkg :> "hello-lib" :@ -1)
        (vector (make-binding 'newattr1 "newval1")
                (make-binding 'newattr2 '("a" "b" "c"))))

  (remove-trailing-commas pkg)
  (display (sunlark->string pkg :starlark :crush))
  (newline)
  )

(define (test-buildozer-set-if-absent pkg)
  (display "test-buildozer-set-if-absent")
  (newline)

  ;; # Make a default explicit in all soy_js rules in a package
  ;;   buildozer 'set_if_absent allowv1syntax 1' //pkg:%soy_j

  ;; to demo we'll do this for cc_library targets
  (for-each (lambda (t)
              (let ((a (t :@ 'allowv1syntax)))
                (if (null? a)
                    (set! (t :@ -1) ;; append after last attr
                          (vector (make-binding 'allowv7syntax 1))))))
            (pkg :>> 'cc_library))

  (remove-trailing-commas pkg)
  (display (sunlark->string pkg :starlark :crush))
  (newline)
  )

(define (test-buildozer-add-load pkg)
  (display "test-buildozer-add-load")
  (newline)

  ;; # A load for a skylark file in //pkg
  ;; buildozer 'new_load //tools/build_rules:build_test.bzl build_test' //pkg:__pkg__

  (let* (
         (new_load (make-load "//tools/build_rules:build_test.bzl"
                              :args '("build_test")
                              :attrs '()))
         )
    ;; splice (append) a single load stmt:
    (set! (pkg :load :-1) (vector new_load))

    (display (sunlark->string pkg :starlark :crush))
    (newline)
    ))

(define (test-buildozer-replace-load pkg)
  (display "test-buildozer-replace-load")
  (newline)

  ;; no idea what buildozer means by this:
  ;; # Replaces existing loads for build_test in //pkg
  ;; buildozer 'replace_load @rules_build//build:defs.bzl build_test' //pkg:__pkg__

  ;; maybe it means: find all load statements having "build_test" as an arg?
  ;; but there could only be one of those

  ;; (let* (
  ;;        (ldstmt (pkg :load :? :arg "build_test"))
  ;;        ;; or:
  ;;        (loadstmt (sunlark-loadstmt-for-sym pkg 'build_test))
  ;;        )
  ;;   ;; splice (append) a single load stmt:
  ;;   (set! (loadstmt :key) "@rules_build//build:defs.bzl build_test")

  ;;   (display (sunlark->string pkg :starlark :crush))
  ;;   (newline)
  ;;   )
  )

(define (test-buildozer-rm-attr-from-all pkg)
  (display "test-buildozer-rm-attr-from-all")
  (newline)

  ;; # Delete the testonly attribute in every rule in the package
  ;; buildozer 'remove testonly' '//pkg:*'

  ;; sunlark: iterate over targets in pkg
  (for-each (lambda (t)
              (set! (t :@ 'testonly) :null)
              )
            (pkg :>>))

  (remove-trailing-commas pkg)
  (display (sunlark->string pkg :starlark :crush))
  (newline)
  )

(define (test-buildozer-replace-tgt-rules pkg)
  (display "test-buildozer-replace-tgt-rules")
  (newline)

  ;; # Change all gwt_module targets to java_library in the package //pkg
  ;; buildozer 'set kind java_library' //pkg:%gwt_module

  ;; test: replace :rule for one target
  ;; (set! (pkg :> "hello-world" :rule) 'cc_foobar)

  ;; change all cc_library targets to foo_bar
  (for-each (lambda (t)
              ;; (display (sunlark->string t :starlark :crush)) (newline)
              ;; (display (sunlark->string t :ast)) (newline)
              (set! (t :rule) 'foo_bar)
              )
            (pkg :>> 'cc_library))

  (remove-trailing-commas pkg)
  (display (sunlark->string pkg :starlark :crush))
  (newline)
  )

(define (test-buildozer-splice-tgt-before pkg)
  (display "test-buildozer-splice-tgt-before")
  (newline)

  ;; # Add a cc_binary rule named new_bin before the rule named tests
  ;; buildozer 'new cc_binary new_bin before tests' //:__pkg__

  ;; we'll splice new_bin before "hello-lib"
  (let ((newbin (make-target :rule 'cc_binary :name "new_bin"
                              :attrs '())))
    (display (sunlark->string newbin :starlark :crush))
    (newline)
    (set! (pkg :> 2) newbin))

  (remove-trailing-commas pkg)
  (display (sunlark->string pkg :starlark :crush))
  (newline)
  )
