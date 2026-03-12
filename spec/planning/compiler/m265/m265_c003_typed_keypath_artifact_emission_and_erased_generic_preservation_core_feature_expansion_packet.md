# M265-C003 Packet

Issue: `M265-C003`

Objective:
- implement live artifact emission for the validated single-component typed key-path subset so the native path emits stable descriptor handles, retained descriptor sections, and preserved erased-generic replay evidence

Code anchors:
- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`

Spec anchors:
- `docs/objc3c-native.md`
- `spec/ABSTRACT_MACHINE_AND_SEMANTIC_CORE.md`
- `spec/ATTRIBUTE_AND_SYNTAX_CATALOG.md`
- `spec/PART_3_TYPES_NULLABILITY_OPTIONALS_GENERICS_KEYPATHS.md`

Acceptance detail:
- the native lowering packet truthfully flips the validated typed key-path subset from deferred to live artifact emission
- typed key-path literals receive deterministic descriptor ordinals and lower to stable nonzero handles
- emitted IR carries per-descriptor globals and an aggregate rooted in `objc3.runtime.keypath_descriptors`
- discovery-root retention includes the key-path descriptor aggregate
- erased-generic replay evidence survives in both the generic metadata ABI packet and the typed-keypath emission comment
- the positive fixture proves compile, object inspection, link, and execution with exit code `7`
- deterministic evidence lands under `tmp/reports/m265/M265-C003/`

Non-goals:
- no broader multi-component typed key-path runtime yet
- no mutation-capable writable key-path runtime yet
- no runtime helper API closeout beyond the emitted descriptor-handle slice
