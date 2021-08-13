(display "loading test/scm/loadstmts.scm")
(newline)

(define (test-loadstmts pkg)
  (display "test-loadstmts")
  (newline)
  (let* (
         ;; data: test/buildfiles/BUILD.loadstmts

          ;; (v (pkg :loadstmts))

          ;; (v (pkg :load "@repoa//pkga:targeta.bzl"))
          ;; (v (pkg :load "@repoc//pkgc:targetc.bzl"))
          ;; (v (pkg :load "@repoc//pkgc:targetc.bzl" :args))
          (v (pkg :load "@repoc//pkgc:targetc.bzl" :arg :0))
          ;; (v (pkg :load "@repoc//pkgc:targetc.bzl" :arg "arg0c"))


         ;;(v (pkg :load "@rules_haskell//haskell:defs.bzl" :args))
         ;; (v (pkg :load "@rules_haskell//haskell:defs.bzl" :arg :0)) ;or 0
         ;; (v (pkg :load "@rules_haskell//haskell:defs.bzl"
         ;;         :arg "haskell_library"))
         ;; (v (pkg :load "@rules_haskell//haskell:defs.bzl"
         ;;         :arg "haskell_library" :$)) ;;=> string?

         )
    ;;(set! (pkg :loadstmts) :null) ;; rm all loadstmts

    ;; remove first loadstmt:
    ;; (set! (pkg :load 0) :null) ;; by int index
    ;; (set! (pkg :load :0) :null) ;; by kwint
    ;; (set! (pkg :load "@repoa//pkga:targeta.bzl") :null) ;; by key

    ;; remove last loadstmt
    ;; (set! (pkg :load -1) :null) ;; by int
    ;; (set! (pkg :load :-1) :null) ;; by kwint
    ;; (set! (pkg :load "@rules_cc//cc:defs.bzl") :null) ;; by key

    ;; remove args:
    ;; (set! (pkg :load :0 :args) :null) ;; rm all args

    ;; (set! (pkg :load :0 :arg 0) :null) ;; first by int
    ;; (set! (pkg :load :0 :arg :0) :null) ;; first by kwint
    ;; (set! (pkg :load :0 :arg "arg0a") :null) ;; first by str

    ;;(set! (pkg :load :2 :arg -1) :null) ;; last by int
    ;;(set! (pkg :load :2 :arg :-1) :null) ;; last by kwint
    ;;(set! (pkg :load :2 :arg :5) :null) ;; index out of bound
    ;;(set! (pkg :load :2 :arg "arg2c") :null) ;; last by str
    ;; (set! (pkg :load :2 :arg "Xyz") :null) ;; not found

    ;; (set! (pkg :load 1 :binding :0) :null) ;; rm one binding

    ;;(display v) (newline)
    (display (sunlark->string pkg :starlark :crush))
    (newline)
    ))

(define (test-loadstmt-set pkg)
  (display "test-loadstmt-set")
  (newline)
  (let* (
         ;; data: test/buildfiles/BUILD.loadstmts

         )
    ;; rm all loadstmts:
    ;; (set! (pkg :loadstmts) :null)

    ;; remove first loadstmt:
    ;; (set! (pkg :load 0) :null) ;; by int index
    ;; (set! (pkg :load :0) :null) ;; by kwint
    ;; (set! (pkg :load "@repoa//pkga:targeta.bzl") :null) ;; by key

    ;; remove last loadstmt
    ;; (set! (pkg :load -1) :null) ;; by int
    ;; (set! (pkg :load :-1) :null) ;; by kwint
    ;; (set! (pkg :load "@rules_cc//cc:defs.bzl") :null) ;; by key

    ;; remove args:
    ;; (set! (pkg :load :0 :args) :null) ;; rm all args

    ;; (set! (pkg :load :0 :arg 0) :null) ;; first by int
    ;; (set! (pkg :load :0 :arg :0) :null) ;; first by kwint
    ;; (set! (pkg :load :0 :arg "arg0a") :null) ;; first by str

    ;;(set! (pkg :load :2 :arg -1) :null) ;; last by int
    ;;(set! (pkg :load :2 :arg :-1) :null) ;; last by kwint
    ;;(set! (pkg :load :2 :arg :5) :null) ;; index out of bound
    ;;(set! (pkg :load :2 :arg "arg2c") :null) ;; last by str
    ;; (set! (pkg :load :2 :arg "Xyz") :null) ;; not found

    ;; (set! (pkg :load 1 :binding :0) :null) ;; rm one binding

    ;;(display v) (newline)
    (display (sunlark->string pkg :starlark :crush))
    (newline)
    ))
