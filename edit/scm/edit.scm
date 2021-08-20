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
  (display (string-append "to run your own proc, copy this to .sunlark.d/edit.scm and edit to taste"))
  (newline)
  (display (format #f "load path: ~S" *load-path*))
  (newline)

  ;; to add your own source dir to the load path:
  ;; (set! *load-path* (cons "path/to/scm" *load-path*))
  ;; (load "myscript.scm") ;; loads first matching file in load path

  (test-1 pkg)
  )

