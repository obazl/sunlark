(display "loading test/scm/loadstmts_rm.scm")
(newline)

;; load this file in .sunlark.d/edit.scm and call test-loadstmts-rm;
;; then: $ bazel run edit -- -f test/unit/sunlark/BUILD.loadstmts_rm
(define (test-loadstmts-rm pkg)
  (display "test-loadstmts-rm")
  (newline)
  (let* (
         ;; data: test/buildfiles/BUILD.loadstmts
         )
    ;; rm all loadstmts:
    ;; (set! (pkg :loadstmts) :null)
    ;; (set! (pkg :loadstmts :*) :null)

    ;; remove first loadstmt:
    ;; (set! (pkg :load 0) :null) ;; by int index
    ;; (set! (pkg :load :0) :null) ;; by kwint
    ;; (set! (pkg :load "@repoa//pkga:targeta.bzl") :null) ;; by key

    ;; remove last loadstmt
    ;; (set! (pkg :load -1) :null) ;; by int
    ;; (set! (pkg :load :-1) :null) ;; by kwint
    ;; (set! (pkg :load "@rules_cc//cc:defs.bzl") :null) ;; by key

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;; REMOVE ARGS from load stmts:
    ;; (set! (pkg :load :0 :args) :null) ;; rm all args

    ;; (set! (pkg :load 3 :arg 0) :null) ;; first by int
    ;; (set! (pkg :load :3 :arg 0) :null) ;; first by int
    ;; (set! (pkg :load "@rules_cc//cc:defs.bzl" :arg 0) :null)

    ;; (set! (pkg :load 3 :arg :0) :null) ;; first by kwint
    ;; (set! (pkg :load :3 :arg :0) :null) ;; first by kwint
    ;; (set! (pkg :load "@rules_cc//cc:defs.bzl" :arg :0) :null)

    ;; (set! (pkg :load 3 :arg "cc_binary") :null) ;; first by str
    ;; (set! (pkg :load :3 :arg "cc_binary") :null) ;; first by str
    ;; (set! (pkg :load "@rules_cc//cc:defs.bzl" :arg "cc_binary") :null)

    ;;(set! (pkg :load :2 :arg -1) :null) ;; last by int
    ;;(set! (pkg :load "@repoc//pkgc:targetc.bzl" :arg 0) :null)
    ;;(set! (pkg :load :2 :arg :-1) :null) ;; last by kwint
    ;;(set! (pkg :load :2 :arg :5) :null) ;; index out of bound
    ;;(set! (pkg :load :2 :arg "arg2c") :null) ;; last by str
    ;; (set! (pkg :load :2 :arg "Xyz") :null) ;; not found

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ;; REMOVE BINDINGS/ATTRS: using :@ (or :attr or :binding), not :arg

    ;; (set! (pkg :load :2 :attrs) :null) ;; rm all args

    ;; (set! (pkg :load :2 :@ 0) :null) ;; first by int
    ;; (set! (pkg :load 2 :@ 0) :null) ;; first by int
    ;; (set! (pkg :load "@repoc//pkgc:targetc.bzl"
    ;;            :@ 0) :null)

    ;; (set! (pkg :load :2 :@ :0) :null) ;; first by kwint
    ;; (set! (pkg :load 2 :@ :0) :null) ;; first by kwint
    ;; (set! (pkg :load "@repoc//pkgc:targetc.bzl"
    ;;            :@ :0) :null)

    ;; (set! (pkg :load :2 :@ 'key0c) :null) ;; first by sym
    ;; (set! (pkg :load 2 :@ 'key0c) :null) ;; first by kwint
    ;; (set! (pkg :load "@repoc//pkgc:targetc.bzl"
    ;;            :@ 'key0c) :null)

    (set! (pkg :load 2 :@ -1) :null) ;; last by int
    ;; (set! (pkg :load :2 :@ -1) :null) ;; last by int
    ;; (set! (pkg :load "@repoc//pkgc:targetc.bzl"
    ;;            :@ -1) :null)

    ;;(set! (pkg :load :2 :@ :-1) :null) ;; last by kwint
    ;;(set! (pkg :load 2 :@ :-1) :null) ;; last by kwint
    ;; (set! (pkg :load "@repoc//pkgc:targetc.bzl"
    ;;            :@ :-1) :null)

    ;; (set! (pkg :load :2 :@ 'lastkey2c) :null) ;; last by sym
    ;; (set! (pkg :load 2 :@ 'lastkey2c) :null) ;; last by kwint
    ;; (set! (pkg :load "@repoc//pkgc:targetc.bzl"
    ;;            :@ 'lastkey2c) :null)

    ;;(display v) (newline)
    (display (sunlark->string pkg :starlark :crush))
    (newline)
    ))
