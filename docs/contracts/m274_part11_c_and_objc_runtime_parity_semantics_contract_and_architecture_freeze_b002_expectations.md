# M274 Part 11 C And Objective-C Runtime Parity Semantics Contract And Architecture Freeze Expectations (B002)

Issue: `#7364`
Contract ID: `objc3c-part11-c-and-objc-runtime-parity-semantics/m274-b002-v1`
Semantic surface: `frontend.pipeline.semantic_surface.objc_part11_c_and_objc_runtime_parity_semantics`

## Required outcomes

- the semantic model remains a deterministic freeze over Part 11 C and Objective-C runtime parity interactions
- the surface classifies the live callability split across C foreign callables, Objective-C method foreign callables, import-module annotations, and runtime-parity callables
- foreign declarations remain declaration-only / extern-only when marked foreign
- `objc_import_module` remains fail-closed unless `objc_foreign` is also present
- implementation methods and categories fail closed on Part 11 foreign/import annotations
- ABI lowering and runtime bridge generation remain deferred

## Canonical packet expectations

The emitted manifest packet is expected to publish the following fields:

- counts:
  - `foreign_callable_sites`
  - `c_foreign_callable_sites`
  - `objc_method_foreign_callable_sites`
  - `import_module_annotation_sites`
  - `import_module_foreign_callable_sites`
  - `objc_runtime_parity_callable_sites`
  - `foreign_definition_rejection_sites`
  - `import_without_foreign_rejection_sites`
  - `implementation_annotation_rejection_sites`
- booleans:
  - `dependency_required`
  - `declaration_only_foreign_c_enforced`
  - `import_module_requires_foreign_enforced`
  - `implementation_annotations_fail_closed`
  - `objc_runtime_parity_classified`
  - `ffi_abi_lowering_deferred`
  - `runtime_bridge_generation_deferred`
  - `deterministic`
  - `ready_for_lowering_and_runtime`

## Diagnostics

- `O3S331` foreign function must be declaration-only / extern surface
- `O3S332` `objc_import_module` requires `objc_foreign`
- `O3S333` implementation methods/categories cannot use Part 11 foreign/import annotations

## Fixture policy

- one positive fixture exercises the live compiler surface with a deterministic manifest-only compile
- at least three negative fixtures isolate the three diagnostics above
- the positive packet shape should be roughly:
  - 2 foreign free functions
  - 2 foreign Objective-C method declarations across interface/protocol surfaces
  - 2 import-module annotations that also use `objc_foreign`
  - 3 Objective-C runtime-parity callable sites
  - zero rejection sites
