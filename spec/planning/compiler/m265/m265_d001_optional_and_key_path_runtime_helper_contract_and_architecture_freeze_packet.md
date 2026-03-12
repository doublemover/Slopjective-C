# M265-D001 Packet

Issue: `M265-D001`

Objective:
- freeze the truthful runtime/helper boundary for the first runnable Part 3 type-surface slice without falsely claiming full typed key-path runtime evaluation

Code anchors:
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `native/objc3c/src/io/objc3_process.cpp`

Spec anchors:
- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`

Acceptance detail:
- publish one canonical semantic-surface contract for the runtime/helper boundary
- keep optional sends and optional-member access on the public selector lookup / dispatch path
- explicitly keep nil short-circuit semantics on the lowering side
- publish the retained typed key-path descriptor section and stable handle boundary as runtime-facing truth
- keep full typed key-path runtime evaluation helpers explicitly deferred to `M265-D002`
- prove the optional positive executable still returns `9`
- prove the typed key-path positive executable still returns `7`
- deterministic evidence lands under `tmp/reports/m265/M265-D001/`

Non-goals:
- no multi-component typed key-path runtime evaluation yet
- no new public runtime helper API
- no broader Part 3 closeout beyond the runtime/helper contract freeze
