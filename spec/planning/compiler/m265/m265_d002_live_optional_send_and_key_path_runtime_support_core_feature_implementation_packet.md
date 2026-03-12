# M265-D002 Packet

Issue: `M265-D002`

Objective:
- implement the first truthful live runtime tranche for optional sends and typed key paths without over-claiming full key-path evaluation

Code anchors:
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `native/objc3c/src/io/objc3_process.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`

Spec anchors:
- `docs/objc3c-native.md`
- `docs/objc3c-native/src/20-grammar.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `native/objc3c/src/runtime/README.md`
- `native/objc3c/src/ARCHITECTURE.md`

Acceptance detail:
- preserve optional sends on the existing public lookup/dispatch ABI
- extend the emitted registration table with `keypath_descriptor_root`
- consume retained key-path descriptors into a private runtime registry during image registration
- expose deterministic testing helpers for validated single-component typed key-path handles
- keep the semantic-surface packet truthful and current
- prove one happy-path optional executable and one happy-path typed key-path executable
- prove the runtime registry state through a native C++ probe
- keep multi-component key-path execution explicitly deferred
- deterministic evidence lands under `tmp/reports/m265/M265-D002/`

Non-goals:
- no public stable runtime API for full typed key-path execution
- no multi-component typed key-path traversal
- no cross-module ambiguity-resolution redesign beyond truthful registry accounting
