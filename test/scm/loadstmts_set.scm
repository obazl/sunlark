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
         ;; data: test/buildfiles/BUILD.loadstmts_set
          ;; (v (pkg :loadstmts))
          ;; (v (pkg :load "@repoa//pkga:targeta.bzl"))
         (ld1 (make-load "@repo_x//pkg_x:target_x.bzl"
                                    :args '("a" "b")
                                    :attrs (list #@(key1 "val1")
                                                 #@(key2 "val2"))))
         (ld2 (make-load "@repo_y//pkg_y:target_y.bzl"
                                    :args '("c" "d")
                                    :attrs (list #@(key3 "val3")
                                                 #@(key4 "val4"))))
         )
    ;; replace loadstmts
    ;; (set! (pkg :load 2) ld1)
    ;; (set! (pkg :load :2) ld1)
    ;; (set! (pkg :load "@repoc//pkgc:targetc.bzl") ld1)
    ;; replace last:
    ;; (set! (pkg :load -1) ld1)
    ;; (set! (pkg :load :-1) ld1)

    ;; splice in new loadstmts
    ;; first
    ;; (set! (pkg :load 0) (list ld1 ld2)) ;; splice at
    ;; (set! (pkg :load :0) (vector ld1 ld2)) ;; splice after
    ;; (set! (pkg :load "@repoa//pkga:targeta.bzl") (list ld1 ld2))
    ;; (set! (pkg :load "@repoa//pkga:targeta.bzl") (vector ld1 ld2))

    ;; last
    ;; (set! (pkg :load 0) (list ld1 ld2)) ;; splice after
    ;; (set! (pkg :load 0) (vector ld1 ld2)) ;; splice after
    ;; (set! (pkg :load "@rules_cc//cc:defs.bzl") (list ld1 ld2))
    ;; (set! (pkg :load "@rules_cc//cc:defs.bzl") (vector ld1 ld2))

    ;;(display v) (newline)
    (pkg :format)
    (display (sunlark->string pkg :starlark :squeeze))
    ;; (display (sunlark->string pkg :ast))
    (newline)
    ))

(define (test-loadstmt-args-set pkg)
  (display "test-loadstmt-args-set")
  (newline)
  (let* (
         ;; data: test/buildfiles/BUILD.loadstmts
         ;; (b1 (make-binding 'key1 "hello"))
         ;; (b2 (make-binding 'key2 "goodbye"))
         ;; (v (pkg :load :2 :arg 2))
         ;; (v (v :$))
         ;; (v (pkg :load :2 :@ 0 :key))
         )
    ;; (set! (pkg :load :0 :arg 0) "hello") ;; replace
    ;; (set! (pkg :load :0 :arg :0) "hello") ;; replace
    ;; (set! (pkg :load :0 :arg "arg0a") "hello") ;; replace
    ;; (set! (pkg :load :0 :arg 99) "hello") ;; index out of bounds
    ;; (set! (pkg :load :0 :arg (list "x")) "hello") ;; invalid arg
    ;; (set! (pkg :load :0 :arg :foo) "hello") ;; invalid arg

    ;; (set! (pkg :load :2 :arg -1) "hello") ;; replace
    ;; (set! (pkg :load :2 :arg -99) "hello") ;; out of bounds
    ;; (set! (pkg :load :2 :arg :-1) "hello") ;; replace
    ;; (set! (pkg :load :2 :arg :-99) "hello") ;; out of bounds
    ;; (set! (pkg :load :2 :arg "arg2c") "hello") ;; replace

    ;; splicing
    ;; (set! (pkg :load :2 :arg :0) (list "hi" "howdy")) ;; splice at
    ;; (set! (pkg :load :2 :arg :-1) (list "hi" "howdy")) ;; splice at
    ;; (set! (pkg :load :2 :arg "arg0c") (list "hi" "howdy"))

    ;; (set! (pkg :load :0 :arg :0) (vector "hi" "howdy")) ;; splice after
    ;; (set! (pkg :load :0 :arg :-1) (vector "hi" "howdy")) ;; splice after
    ;; (set! (pkg :load :1 :arg :-1) (vector "hi" "howdy")) ;; splice after
    ;; (set! (pkg :load :2 :arg "arg2c") (vector "hi" "howdy")) ;; splice after

    ;; replace attrs
    ;; (set! (pkg :load :2 :@ :0) "hello") ;; type error
    ;; (set! (pkg :load :2 :@ -1) (make-binding 'newkey "hello"))
    ;; (set! (pkg :load :2 :@ 0) (make-binding 'newkey "hello"))
    ;; (set! (pkg :load :2 :@ 'key0c) (make-binding 'newkey "hello"))
    ;; (set! (pkg :load :2 :@ 'key0c) (make-binding 'newkey "hello"))

    ;; splice (insert) new attrs
    ;; splice before first
    ;; (set! (pkg :load :2 :@ 0) ;; by int
    ;;       (list (make-binding 'newkey0 "newval0")
    ;;             (make-binding 'newkey1 "newval1")))
    ;; (set! (pkg :load :2 :@ :0) ;; by kwint
    ;;       (list (make-binding 'newkey0 "newval0")
    ;;             (make-binding 'newkey1 "newval1")))
    ;; (set! (pkg :load :2 :@ 'key0c) ;; by sym key
    ;;       (list (make-binding 'newkey0 "newval0")
    ;;             (make-binding 'newkey1 "newval1")))

    ;; splice before last
    ;; (set! (pkg :load :2 :@ -1) ;; by int
    ;;       (list (make-binding 'newkey0 "newval0")
    ;;             (make-binding 'newkey1 "newval1")))
    ;; (set! (pkg :load :2 :@ :-1) ; by kwint
    ;;       (list (make-binding 'newkey0 "newval0")
    ;;             (make-binding 'newkey1 "newval1")))
    ;; (set! (pkg :load :2 :@ 'lastkey2c) ;; by key sym
    ;;       (list (make-binding 'newkey0 "newval0")
    ;;             (make-binding 'newkey1 "newval1")))

    ;; splice after last
    ;; (set! (pkg :load :2 :@ -1) ;; by int
    ;;       (vector (make-binding 'newkey0 "newval0")
    ;;               (make-binding 'newkey1 "newval1")))
    ;; (set! (pkg :load :2 :@ 'lastkey2c) ;; by key sym
    ;;       (vector (make-binding 'newkey0 "newval0")
    ;;               (make-binding 'newkey1 "newval1")))

    ;; set attr keys and vals
    ;; (set! (pkg :load :2 :@ 0 :key) "newkey")
    ;; (set! (pkg :load :2 :@ 0 :value) "newval")

    ;; (set! (pkg :load :2 :@ :0 :key) "newkey")
    ;; (set! (pkg :load :2 :@ :0 :value) "newval")

    ;; (set! (pkg :load :2 :@ 'key0c :key) "newkey")
    ;; (set! (pkg :load :2 :@ 'key0c :value) "newval")

    ;; (set! (pkg :load :2 :@ -1 :key) "newkey")
    ;; (set! (pkg :load :2 :@ -1 :value) "newval")

    ;; (set! (pkg :load :2 :@ :-1 :key) "newkey")
    ;; (set! (pkg :load :2 :@ :-1 :value) "newval")

    ;; (set! (pkg :load :2 :@ 'lastkey2c :key) "newkey")
    ;; (set! (pkg :load :2 :@ 'lastkey2c :value) "newval")

    (pkg :format)
    ;; (display v) (newline)
    (display (sunlark->string pkg :starlark :crush))
    (newline)
    ))
