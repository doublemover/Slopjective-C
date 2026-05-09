# Lowering And IR

Lowering and IR claims are valid only when linked from the capability matrix
with source and test evidence.

Current ownership:

- `native/objc3c/src/lower/` owns lowering contracts, primitive lowering ops,
  metadata helpers, and `runtime_metadata_layout_policy.cpp`.
- `native/objc3c/src/ir/` owns LLVM IR emission and module-level metadata
  output.
- `native/objc3c/src/pipeline/dispatch_surface_classification.*` normalizes
  dispatch-surface classification before lowering and IR emission consume it.

The strict runtime dispatch lowering claim remains bounded to
`objc3_runtime_dispatch_i32`. Supported live sends lower to the canonical runtime
dispatch family; rejected or reserved dispatch surfaces must fail before they
become an unowned IR path. Direct helper calls, fixture-specific probes, and
native build internals are evidence owners only, not public command surface.

The public replay command for the current lowering and IR behavior rows is
`npm run objc3c -- test-behavior-matrix`.
