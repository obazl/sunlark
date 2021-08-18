
(define (test-targets-set pkg)
  (display "test-target-set")
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
