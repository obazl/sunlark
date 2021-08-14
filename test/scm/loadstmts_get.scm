(display "loading test/scm/loadstmts_get.scm")
(newline)

(define (test-loadstmts-get pkg)
  (display "test-loadstmts-get")
  (newline)
  (let* (
         ;; data: test/buildfiles/BUILD.loadstmts

          ;; (v (pkg :loadstmts))

          (v (pkg :load "@repoa//pkga:targeta.bzl"))
          ;; (v (pkg :load "@repoc//pkgc:targetc.bzl"))
          ;; (v (pkg :load "@repoc//pkgc:targetc.bzl" :args))
          ;; (v (pkg :load "@repoc//pkgc:targetc.bzl" :arg :0))
          ;; (v (pkg :load "@repoc//pkgc:targetc.bzl" :arg "arg0c"))


         ;;(v (pkg :load "@rules_haskell//haskell:defs.bzl" :args))
         ;; (v (pkg :load "@rules_haskell//haskell:defs.bzl" :arg :0)) ;or 0
         ;; (v (pkg :load "@rules_haskell//haskell:defs.bzl"
         ;;         :arg "haskell_library"))
         ;; (v (pkg :load "@rules_haskell//haskell:defs.bzl"
         ;;         :arg "haskell_library" :$)) ;;=> string?

         )
    ;;(display v) (newline)
    (display (sunlark->string v :ast))
    ;; (display (sunlark->string pkg :starlark :crush))
    (newline)
    ))
