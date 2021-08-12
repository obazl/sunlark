;; //edit/scm:edit.scm

(define (test-1 pkg)
  ;; print the AST
  (newline)
  (display (sunlark->string pkg :ast))
  (newline)

  ;; print starlark - :crush removes blank lines
  (display (sunlark->string pkg :starlark :crush))
  (newline)
  )

(define (sunlark-ast-handler pkg)
  (display (string-append "running default sunlark-ast-handler proc"))
  (newline)
  (display (string-append "to run your own proc, put it in .sunlark.d/edit.scm"))
  (newline)
  (display (format #f "load path: ~S" *load-path*))
  (newline)

  ;; (set! *load-path* (cons "test/scm" *load-path*))
  ;; (load "myscript.scm")

  (test-1 pkg)
  )

