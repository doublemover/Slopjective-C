# Explicit capture witnesses

These fixtures target the native Objective-C 3.0 compiler and GNUstep 2.2.
All 24 source/PCH configurations pass normally and under ASan/UBSan and TSan;
the installed SDK feature gate also passes. Exact inputs and scope are recorded
in the [integration receipt](../../../docs/support/linux_native_feature_integration_acceptance.json).

- `runtime.h` / `main.m`: creation snapshots, unused/contextual captures,
  explicit `__block` snapshots, ARC/MRC ownership, weak zeroing and nested
  weak-to-strong promotion, resource heap
  copies (including distinct heap Blocks copied from one stack literal), address
  cleanup, sentinels, templates, C++ object move/copy separation
  and capture evaluation order. ARC cases also move strong/weak Optional payloads
  and verify their source fields and weak registrations remain safe. Resource
  take/reset/discard operates through shared copied owners. Deferred actions
  observe committed fields before implicit release across nested scopes,
  return/goto exits and precise Block lifetime receiver regions.
  Nested resource moves hand off the shared outer payload to an independent
  owner and leave all old copies invalid; the exception fixture covers failed
  nested creation after that handoff.
- `exceptions.mm`: a later C++ constructor fails during literal construction or
  heap copying, including a throwing move constructor; completed object/resource
  captures clean up exactly once. It also covers throwing reset replacements,
  deferred resource discard, transferred handles, committed field unwind and
  temporary-destructor failure between field construction and lifetime commit,
  including `keepAlive`/`withLifetime` Block receivers.
- `sema.m`: parser and ownership diagnostics plus weak-self guard syntax and
  retain-cycle diagnostics. Its RUN line uses Clang's verify mechanism.
- `strict_resource.m`: strict-system consumed-state, reset, sentinel observation
  and selected negative cases (`CAPTURE_USE_AFTER_MOVE`,
  `CAPTURE_DOUBLE_CLEANUP`, `CAPTURE_SECOND_MOVE`, `CAPTURE_BODY_CLOSE`).
- `lowering.m`: emitted ownership operations and refcounted resource owners.
- `return_failures.h` / `return_failures.m`: ARC-only physical result ownership
  through throwing defers, native nested catches, checked errors and successful
  cleanup-local handlers. Retained/autoreleased and `id`-erased Block results
  are covered; compile as C17 or C++20/C++23. `exceptions.mm` also covers returned
  literals whose full-expression temporary destructor throws.
- `persistence.h` / `persistence.m`: source/PCH clients; define
  `CAPTURE_PERSISTENCE_PCH` when the header is supplied through `-include-pch`.

The maintained C++ witness in `../explicit_capture_lists_test.cpp` performs
semantic, AST import, PCH and ODR checks. Runtime source is suitable for C17 and
C++20/C++23 under ARC/MRC. The exception fixture additionally requires native
C++/Objective-C exception support and ARC exception cleanups. A PCH runtime run
uses `runtime.h` as the precompiled header and defines `CAPTURE_RUNTIME_PCH`.
