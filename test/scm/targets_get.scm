
(define (test-targets-get pkg)
  ;; test/unit/sunlark/BUILD.targets
  (let* (
         ;; (v (pkg :>>)) ;; => scheme list of targets

         ;; (v (pkg :>> :@)) ;; error
         ;; (v (pkg :>> :name)) ;; error

         ;; (v (pkg :target 0))
         ;; (v (pkg :> 0))

         ;; (v (pkg :> 0 :rule))
         ;; using intermediate var:
         ;; (t (pkg :> 0)) ;; now t is a :target

         ;; (r (t :rule))  ;; the target's rule node
         ;; (v (r :$))
         ;; (v (r :rule?))  ;; #f - rule nodes are :id nodes

         ;; (v (r :id?)) ;; now v is #t
         ;; (v (r :$)) ;; now v is the Scheme value of the rule node: a symbol
         ;;(v (symbol? v)) ;; #t
         ;; (v (symbol? (r :$))) ;; #t

         ;; (v (equal? (r :$) 'cc_library)) ;; #t

         ;; (n (t :name))  ;; the target's name attr
         ;; (v n)
         ;; (v (pkg :> 0 :name :$))

         ;; (v (pkg :target :0)) ;; index by kwint
         ;; (v (pkg :> :0))

         ;; (v (pkg :target "hello-world")) ;; index by name string
         ;; (v (pkg :> "hello-world"))

         ;;(v (pkg "hello-lib")) ;; error
         ;; (v (pkg :> "hello-lib"))

         ;; (v (pkg :>> :@@)) ;; list all attrs of all targets
         ;; (v (pkg :> 1 :@@))
         ;; (v (pkg :> 1 :@)) ;; error
         ;; (v (pkg :> 1 :@ 'deps))
         ;; (v (pkg :> 1 :@ 2))

         ;;(v (pkg :> "vectors" :@ 'string_vec :value 1))
         ;;(v (pkg :> "vectors" :@ 1 :value 1))

         ;; (v (pkg :> "vectors"))
         ;; (v (v :@ 'string_vec :value 0))
         ;; (v (v :@ 1 :value 0))

         ;; (v (pkg :> "vectors" :@ 'string_vec))
         ;; (v (pkg :> "vectors" :@ 1))
         ;; (v (v :value 1))

         ;; (v (pkg :> "hello-lib" :@ 'srcs :key)) ; 'deps :value))
         ;;(v (v :print))
         ;; (v (pkg :> 1 :@ 'deps :val)) ;; error
         ;; (v (pkg :> 1 :@ 'deps 0)) ;; error

         ;; (v (pkg :> 2 :@ 'string_vec :value 1))

         ;; (v (pkg :>> :@@ :keys))

         ;; (vm (map (lambda (x)
         ;;            (display (format #f "~A" (sunlark->starlark x :crush)))
         ;;            (newline)
         ;;            (x :key)
         ;;            ;;98
         ;;           )
         ;;         v))
         )
    (display  v) (newline)
    ;; (display (sunlark->string v :starlark :crush))
    (newline)
    ))

(define (test-targets-filter pkg)
  (display "test-targets-filter")
  (newline)

  (let* (
         ;; (tlist (pkg :targets))

         ;;;;  multiple filters not supported by DSL, but programmable
         ;;;; (tlist (pkg :targets '(cc_binary 3 file*) '(cc_*))); No

         (tlist (pkg :>>)) ;; cc_library)))

         ;;;; symbols filter by target rule
         ;; (tlist (pkg :targets 'cc_binary)) ;; cc_library)))
         ;; (tlist (pkg :targets '(cc_test))) ;; cc_library)))
         ;; (tlist (pkg :targets '(cc_binary))) ;; cc_library)))
         ;; (tlist (pkg :targets '(cc_binary cc_library)))
         ;; (tlist (pkg :targets '(cc_library file*)))
         ;; (tlist (pkg :targets 'cc_*))

         ;;;; strings filter by target name
         ;; (tlist (pkg :targets "hello-lib"))
         ;; (tlist (pkg :targets '("hello-world" "hello-lib")))

         ;;;; integers, by index
         ;; (tlist (pkg :targets 0))
         ;; (tlist (pkg :targets '(0)))
         ;; (tlist (pkg :targets '(0 1)))
         ;; (tlist (pkg :targets '(0 4)))
         ;; currently out-of-bounds indices in a list are not caught
         ;; (tlist (pkg :targets '(-1 5)))

         ;; filter then select
         ;; (tlist (pkg :targets '(cc_binary cc_test))) ;; 0))
         ;;(tlist (pkg :targets '(cc_test)))
         ;; (tlist (pkg :targets '(cc_binary) :-1))
         ;; (tlist (pkg :targets 'cc_test :count)) ;; not supported
         ;; (tlist (pkg :targets '(cc_test f*)))
         ;; (tlist (pkg :targets '(cc_test foo*) :count))
         ;; (tlist (pkg :targets '(0 4) 0)) ;; nope

         ;;;; mixed
         ;; (tlist (pkg :targets '(file* "hello-world")))
         ;; (tlist (pkg :targets '("hello-world" cc_library)))
         ;; (tlist (pkg :targets '("hello-world" file*)))
         ;; (tlist (pkg :targets '("hello-world" 4)))
         ;; (tlist (pkg :targets '(1 "srcs")))
         ;; (tlist (pkg :targets '("srcs" 1))) ;; filter order doesn't matter
         ;; (tlist (pkg :targets '(1 file*)))
         ;; (tlist (pkg :targets '(cc_binary 3)))
         ;; (tlist (pkg :targets '(cc_binary 1))) ;; both refer to same target
         (i 0)
         )
    (for-each (lambda (t)
                (display i) (newline)
                (set! i (+ i 1))
                ;; (display (sunlark-node? t)) (newline)
                ;; (display (t :tid->kw)) (newline)
                (display t) (newline)
                )
              tlist)
    ;; (display (length tlist))
    ;; (newline)
    ;; (display ((pkg :> 0 :@ :0 :key) :printable?)) (newline)
    ;; (display tlist) (newline)
    ;; (display (sunlark->string pkg :ast))
    ;; (display (sunlark->string pkg :starlark :crush))
    ;; (display (sunlark->string (pkg :> 12) :starlark :crush))
    (newline)
  ))

(define (test-target-update pkg)
  (display "test-target-update")
  (newline)

  ;; (walk-all pkg handlers)
  ;; (newline)

  ;; (display (object->string pkg :readable))
  ;; (newline)

  ;; (display (object->string (pkg :targets) :readable))
  ;; (newline)

  ;; (display (length (pkg :targets)))
  ;; (newline)

  ;; (display (pkg :target "hello-lib"))
  ;; (newline)

  ;; (display (pkg :target "hello"))
  ;; (newline)

  ;; (display (pkg :load "@rules_cc//cc:defs.bzl"))
  ;; (newline)
  ;; (display (pkg :load "@rules_foo//foo:defs.bzl"))
  ;; (newline)

  ;; (display (format #f "(pkg :target ...):\n~A"
  ;;                  (object->string (pkg :target "b-lib")
  ;;                                  :readable)))
  ;; (newline)

  ;; (display (pkg :target "hello-world" :bindings))
  ;; (newline)

  ;; (display (pkg :target "hello" :bindings :deps))
  ;; (newline)

  ;;  by rule name, e.g. cc_library
  ;;  input: test/data/walk/BUILD.target3

  ;; (let* (
  ;;        (path '(:targets)); 'cc_test))
  ;;        ;; (tlen (length tlist))
  ;;        )
  ;;   (display (format #f "(sunlark->starlark ... )\n\n~A"
  ;;                    (sunlark->starlark
  ;;                     ;;pkg
  ;;                     (apply pkg path)
  ;;                     :crush))) (newline)
  ;;   ;; (display (format #f "(pkg :target ...)\n~A"
  ;;   ;;                  ;; (object->string tlist :readable)
  ;;   ;;                  ;; (sunlark->starlark tlist)
  ;;   ;;                  ;; (sunlark->starlark tlist :squeeze)
  ;;   ;;                  (sunlark->starlark tlist :crush)
  ;;   ;;                  ))
  ;;   (newline)
    ;; (display (format #f "target count: ~D" tlen))
    ;; (newline)
    ;; )

  ;; by target id == value of name binding. :target "foo" means
  ;; filter the list of targets by binding name="foo"
  ;;  result is one node

  ;; (let* ((tnode (pkg :target "b-lib" :bindings))
  ;;        ;; tnode is a list node, not a list. bindings are its subnodes
  ;;        (tlen (length (tnode :subnodes))))
  ;;   (display (format #f "(pkg :target ...)\n~A"
  ;;                    (object->string
  ;;                     tnode ;; a list node, not a list
  ;;                     :readable)))
  ;;   (newline)
  ;;   (display (format #f "binding count: ~D" tlen))
  ;;   (newline)
  ;;   )

  ;; returns :arg-named > :id :eq :list-expr > :lbrack, :expr-list, :rbrack
  ;;   :expr-list == :string, :comma ...
  ;; (let* ((tnode (pkg :target "b-lib" :bindings 'srcs)))
  ;;   (display (format #f "(pkg :target ...)\n~A"
  ;;                    (object->string
  ;;                     tnode
  ;;                     :readable)))
  ;;   (newline)
  ;;   )

  ;; value of 'srcs' binding is a string list, e.g. ["a", "b"]
  ;;  returns :list-expr > lbrack, :expr-list, :rbrack
  ;;    :expr-list == :string, :comma ...
  ;; (let* ((tnode (pkg :target "b-lib" :bindings 'srcs :value)))
  ;;   (display (format #f "(pkg :target ...)\n~A"
  ;;                    (object->string
  ;;                     tnode
  ;;                     :readable)))
  ;;   (newline)
  ;;   )

  ;; (let* ((tlist (pkg :target 'cc_test))
  ;;        (tlen (length tlist)))
  ;;   (display (format #f "(pkg :target 'cc_library)\n~A"
  ;;                    (object->string tlist
  ;;                                    :readable)))
  ;;   (newline)
  ;;   (display (format #f "target count: ~D" tlen)))
  ;; (newline)

  ;; but :load <symbol> not allowed
  ;; (display (pkg :load 'foo)) ;; meaning?
  )

