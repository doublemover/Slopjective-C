# Explicit capture witnesses

These fixtures are authored and intentionally unexecuted during the development
span. They target the shared native Objective-C 3 compiler and GNUstep 2.2.

- `runtime.h` / `main.m`: creation snapshots, unused/contextual captures,
  explicit `__block` snapshots, ARC/MRC ownership, weak zeroing and nested
  weak-to-strong promotion, resource heap
  copies (including distinct heap Blocks copied from one stack literal), address
  cleanup, sentinels, templates, C++ object move/copy separation
  and capture evaluation order. ARC cases also move strong/weak Optional payloads
  and verify their source fields and weak registrations remain safe.
- `exceptions.mm`: a later C++ constructor fails during literal construction or
  heap copying, including a throwing move constructor; completed object/resource
  captures clean up exactly once.
- `sema.m`: parser and ownership diagnostics plus weak-self guard syntax and
  retain-cycle diagnostics. Its RUN line uses Clang's verify mechanism.
- `strict_resource.m`: strict-system consumed-state, reset, sentinel observation
  and selected negative cases (`CAPTURE_USE_AFTER_MOVE`,
  `CAPTURE_DOUBLE_CLEANUP`, `CAPTURE_SECOND_MOVE`, `CAPTURE_BODY_CLOSE`).
- `lowering.m`: emitted ownership operations and refcounted resource owners.
- `persistence.h` / `persistence.m`: source/PCH clients; define
  `CAPTURE_PERSISTENCE_PCH` when the header is supplied through `-include-pch`.

The maintained C++ witness in `../explicit_capture_lists_test.cpp` performs
semantic, AST import, PCH and ODR checks. Runtime source is suitable for C17 and
C++20/C++23 under ARC/MRC. The exception fixture additionally requires native
C++/Objective-C exception support and ARC exception cleanups. A PCH runtime run
uses `runtime.h` as the precompiled header and defines `CAPTURE_RUNTIME_PCH`.
