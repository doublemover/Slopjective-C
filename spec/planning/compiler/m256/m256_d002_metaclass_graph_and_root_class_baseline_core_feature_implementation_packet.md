# M256-D002 Metaclass Graph and Root-Class Baseline Core Feature Implementation Packet

Packet: `M256-D002`
Issue: `#7140`
Milestone: `M256`
Lane: `D`
Wave: `W48`

## Summary
Implement runtime realization of class/metaclass graphs and establish the minimal root-class baseline needed for native object programs.

## Contract
- Contract ID: `objc3c-runtime-metaclass-graph-root-class-baseline/m256-d002-v1`
- Realized graph model: `runtime-owned-realized-class-nodes-bind-receiver-base-identities-to-class-and-metaclass-records`
- Root-class baseline model: `root-classes-realize-with-null-superclass-links-and-live-instance-plus-class-dispatch`
- Fail-closed model: `missing-receiver-bindings-or-broken-realized-superclass-links-fall-closed-to-compatibility-dispatch`

## Dependencies
- `M256-D001`
- `M256-C002`

## Anchors
- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/parse/objc3_parser.cpp`
- `native/objc3c/src/sema/objc3_semantic_passes.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`

## Required proof assets
- Fixture: `tests/tooling/fixtures/native/m256_d002_metaclass_graph_root_class_library.objc3`
- Probe: `tests/tooling/runtime/m256_d002_metaclass_graph_root_class_probe.cpp`
- Evidence: `tmp/reports/m256/M256-D002/metaclass_graph_and_root_class_baseline_summary.json`

## Happy-path proof
1. Realized graph publication exposes two realized classes and one root class.
2. Root class `RootObject` retains null superclass and super-metaclass links.
3. Subclass `Widget` retains `class:RootObject` and `metaclass:RootObject` as its realized super links.
4. Root class method dispatch succeeds for the root receiver.
5. Subclass class dispatch inherits the root class method through the metaclass graph.
6. Known-class and class-self receivers normalize to the same metaclass cache key.
7. Subclass instance dispatch inherits the root instance method and also resolves its own instance method.

## Non-goals
- object allocation
- instance storage or ivar layout
- protocol body execution
- category attachment runtime checks beyond the already frozen D001 surface
