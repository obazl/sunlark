(display "loading test/scm/loadstmts_set.scm")
(newline)

(define (test-make-load)
  (display "test-make-load")
  (newline)
  (let* ((ld (make-load "@repo_x//pkg_x:target_x.bzl"
                        :args '("a" "b")
                        :attrs (list #@(key1 "val1")
                                     #@(key2 "val2"))))
         )
    ;; (display (sunlark->string ld :starlark :crush))
    (display (sunlark->string ld :ast))
    (newline)
    ))

(define (test-loadstmts-set pkg)
  (display "test-loadstmts-set")
  (newline)
  (let* (
         ;; data: test/buildfiles/BUILD.loadstmts

          ;; (v (pkg :loadstmts))

          ;; (v (pkg :load "@repoa//pkga:targeta.bzl"))
          ;; (v (pkg :load "@repoc//pkgc:targetc.bzl"))
          ;; (v (pkg :load "@repoc//pkgc:targetc.bzl" :args))
          ;; (v (pkg :load "@repoc//pkgc:targetc.bzl" :arg :0))
          ;; (v (pkg :load "@repoc//pkgc:targetc.bzl" :arg "arg0c"))
         ;; (v (pkg :load :0 :arg 0))
         )
    ;; set! args:

    ;; (set! (pkg :> :0 :@ 1) (make-binding 'key1 "hello")) ;; first by int
    (set! (pkg :load :0 :arg 0) "hello") ;; first by int

    ;; (set! (pkg :load :0 :arg :0) :null) ;; first by kwint
    ;; (set! (pkg :load :0 :arg "arg0a") :null) ;; first by str

    ;;(set! (pkg :load :2 :arg -1) :null) ;; last by int
    ;;(set! (pkg :load :2 :arg :-1) :null) ;; last by kwint
    ;;(set! (pkg :load :2 :arg :5) :null) ;; index out of bound
    ;;(set! (pkg :load :2 :arg "arg2c") :null) ;; last by str
    ;; (set! (pkg :load :2 :arg "Xyz") :null) ;; not found

    ;; (set! (pkg :load 1 :binding :0) :null) ;; rm one binding

    ;; remove first loadstmt:
    ;; (set! (pkg :load 0) :null) ;; by int index
    ;; (set! (pkg :load :0) :null) ;; by kwint
    ;; (set! (pkg :load "@repoa//pkga:targeta.bzl") :null) ;; by key

    ;; remove last loadstmt
    ;; (set! (pkg :load -1) :null) ;; by int
    ;; (set! (pkg :load :-1) :null) ;; by kwint
    ;; (set! (pkg :load "@rules_cc//cc:defs.bzl") :null) ;; by key

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
    ;; (set! (pkg :loadstmts :*) :null)

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
    ;; (set! (pkg :load "@rules_cc//cc:defs.bzl" :arg 0) :null)
    ;; (set! (pkg :load :0 :arg :0) :null) ;; first by kwint
    ;; (set! (pkg :load :0 :arg "arg0a") :null) ;; first by str

    ;;(set! (pkg :load :2 :arg -1) :null) ;; last by int
    ;;(set! (pkg :load "@repoc//pkgc:targetc.bzl" :arg 0) :null)
    ;;(set! (pkg :load :2 :arg :-1) :null) ;; last by kwint
    ;;(set! (pkg :load :2 :arg :5) :null) ;; index out of bound
    ;;(set! (pkg :load :2 :arg "arg2c") :null) ;; last by str
    ;; (set! (pkg :load :2 :arg "Xyz") :null) ;; not found

    ;; remove bindings/attrs:
    ;; (set! (pkg :load :2 :attrs) :null) ;; rm all args

    (set! (pkg :load :2 :@ 0) :null) ;; first by int
    ;; (set! (pkg :load :2 :@ :0) :null) ;; first by kwint
    ;; (set! (pkg :load :2 :@ 'key0c) :null) ;; first by sym
    ;; (set! (pkg :load "@rules_cc//cc:defs.bzl" :arg 0) :null)
    ;; (set! (pkg :load :0 :arg :0) :null) ;; first by kwint
    ;; (set! (pkg :load :0 :arg "arg0a") :null) ;; first by str


    ;; (set! (pkg :load 1 :binding :0) :null) ;; rm one binding

    ;;(display v) (newline)
    (display (sunlark->string pkg :starlark :crush))
    (newline)
    ))
