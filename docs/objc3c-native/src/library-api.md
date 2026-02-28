# libobjc3c_frontend Library API (Embedding Contract)

This document defines the current public embedding API exposed by `native/objc3c/src/libobjc3c_frontend/api.h`.

## Public Surface

- Primary header: `native/objc3c/src/libobjc3c_frontend/api.h`
- Version header: `native/objc3c/src/libobjc3c_frontend/version.h`
- Optional C shim header: `native/objc3c/src/libobjc3c_frontend/c_api.h`

`api.h` exposes a C ABI usable from C and C++ with an opaque context type (`objc3c_frontend_context_t`).

## Stability

- stability boundary: exported symbols, enums, and struct layouts in `api.h`.
- ABI growth rule: append-only struct evolution; reserved fields remain reserved.
- Embedding rule: zero-initialize option/result structs to keep reserved fields deterministic.
- Current compile behavior is stable and intentional for now: compile entrypoints are scaffolded and return `OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR` after argument validation.

## Compatibility and versioning

- Semantic version source: `OBJC3C_FRONTEND_VERSION_{MAJOR,MINOR,PATCH}`.
- Current version string macro: `OBJC3C_FRONTEND_VERSION_STRING` (`"0.1.0"` in this workspace).
- ABI version source: `OBJC3C_FRONTEND_ABI_VERSION` (`1u` in this workspace).
- Compatibility window (inclusive): `OBJC3C_FRONTEND_MIN_COMPATIBILITY_ABI_VERSION` through `OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION`.
- Startup check: call `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)` before invoking compile entrypoints.

```c
#include "libobjc3c_frontend/api.h"

int objc3c_frontend_startup_check(void) {
  if (!objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)) {
    return 0;
  }

  const objc3c_frontend_version_t v = objc3c_frontend_version();
  return v.abi_version == objc3c_frontend_abi_version();
}
```

## M224 integration/release-readiness (1.0 ABI/version gates)

- Gate intent: fail closed on ABI/version drift before a 1.0 cut.
- Required startup/version invariants:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)` must pass before compile entrypoints are used.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()` must remain true.
  - `OBJC3C_FRONTEND_ABI_VERSION` must stay inside the inclusive compatibility window
    `OBJC3C_FRONTEND_MIN_COMPATIBILITY_ABI_VERSION` through
    `OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION`.
- Deterministic M224 integration gate:
  - `npm run check:objc3c:m224-integration-release-readiness`
  - This gate chains existing deterministic checks for M222 compatibility migration,
    library/CLI parity golden replay, and M224 tooling wiring.

## M225 integration roadmap seeding

- Gate intent: export ABI/version and deterministic gate evidence into 1.1/1.2 planning intake.
### 1.1 ABI/version continuity planning intake
- Preserve these intake invariants as seeded evidence for 1.1 planning:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)` remains the required startup guard.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()` remains true.
  - `OBJC3C_FRONTEND_ABI_VERSION` stays within
    `OBJC3C_FRONTEND_MIN_COMPATIBILITY_ABI_VERSION` through
    `OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain the planning intake anchors.
### 1.2 Gate-evidence planning intake
- Deterministic export gate:
  - `npm run check:objc3c:m225-roadmap-seeding`
- Exported evidence chain for 1.2 planning intake:
  - Replays `check:objc3c:m224-integration-release-readiness` as the baseline deterministic ABI/version gate.
  - Runs M225 roadmap-seeding contracts for frontend, sema/type, lowering/runtime, validation/perf, and integration wiring:
    `tests/tooling/test_objc3c_m225_frontend_roadmap_seed_contract.py`,
    `tests/tooling/test_objc3c_m225_sema_roadmap_seed_contract.py`,
    `tests/tooling/test_objc3c_m225_lowering_roadmap_seed_contract.py`,
    `tests/tooling/test_objc3c_m225_validation_roadmap_seed_contract.py`,
    `tests/tooling/test_objc3c_m225_integration_roadmap_seed_contract.py`.

## M221 integration GA blocker burn-down

- Gate intent: fail closed on unresolved GA blockers by chaining release-readiness and M221 lane contracts.
### 1.1 GA blocker integration chain
- Deterministic blocker gate:
  - `npm run check:objc3c:m221-ga-blocker-burndown`
- Chain order:
  - replays `check:objc3c:m225-roadmap-seeding`.
  - enforces all M221 lane contract surfaces:
    `tests/tooling/test_objc3c_m221_frontend_ga_blocker_contract.py`,
    `tests/tooling/test_objc3c_m221_sema_ga_blocker_contract.py`,
    `tests/tooling/test_objc3c_m221_lowering_ga_blocker_contract.py`,
    `tests/tooling/test_objc3c_m221_validation_ga_blocker_contract.py`,
    `tests/tooling/test_objc3c_m221_integration_ga_blocker_contract.py`.
### 1.2 ABI/version continuity constraints
- Keep startup/version invariants unchanged while burning down GA blockers:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain release anchors.

## M220 integration public-beta triage loop

- Gate intent: formalize public-beta intake/triage/patch loop with deterministic lane-contract replay.
### 1.1 Public-beta integration chain
- Deterministic triage gate:
  - `npm run check:objc3c:m220-public-beta-triage`
- Chain order:
  - replays `check:objc3c:m221-ga-blocker-burndown`.
  - enforces all M220 lane contracts:
    `tests/tooling/test_objc3c_m220_frontend_public_beta_contract.py`,
    `tests/tooling/test_objc3c_m220_sema_public_beta_contract.py`,
    `tests/tooling/test_objc3c_m220_lowering_public_beta_contract.py`,
    `tests/tooling/test_objc3c_m220_validation_public_beta_contract.py`,
    `tests/tooling/test_objc3c_m220_integration_public_beta_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve release guard invariants through beta loop execution:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain integration anchors.

## M219 integration cross-platform parity matrix

- Gate intent: enforce deterministic cross-platform parity replay across Windows, Linux, and macOS.
### 1.1 Cross-platform integration chain
- Deterministic parity gate:
  - `npm run check:objc3c:m219-cross-platform-parity`
- Chain order:
  - replays `check:objc3c:m220-public-beta-triage`.
  - enforces all M219 lane contracts:
    `tests/tooling/test_objc3c_m219_frontend_cross_platform_contract.py`,
    `tests/tooling/test_objc3c_m219_sema_cross_platform_contract.py`,
    `tests/tooling/test_objc3c_m219_lowering_cross_platform_contract.py`,
    `tests/tooling/test_objc3c_m219_validation_cross_platform_contract.py`,
    `tests/tooling/test_objc3c_m219_integration_cross_platform_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through parity runs:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain parity anchors.

## M218 integration RC automation and provenance

- Gate intent: enforce deterministic RC automation and provenance attestation chain across all lanes.
### 1.1 RC integration chain
- Deterministic RC gate:
  - `npm run check:objc3c:m218-rc-provenance`
- Chain order:
  - replays `check:objc3c:m219-cross-platform-parity`.
  - enforces all M218 lane contracts:
    `tests/tooling/test_objc3c_m218_frontend_rc_provenance_contract.py`,
    `tests/tooling/test_objc3c_m218_sema_rc_provenance_contract.py`,
    `tests/tooling/test_objc3c_m218_lowering_rc_provenance_contract.py`,
    `tests/tooling/test_objc3c_m218_validation_rc_provenance_contract.py`,
    `tests/tooling/test_objc3c_m218_integration_rc_provenance_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through RC automation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain provenance anchors.

## M217 integration differential testing matrix

- Gate intent: enforce deterministic differential testing chain against baseline toolchains.
### 1.1 Differential integration chain
- Deterministic differential gate:
  - `npm run check:objc3c:m217-differential-parity`
- Chain order:
  - replays `check:objc3c:m218-rc-provenance`.
  - enforces all M217 lane contracts:
    `tests/tooling/test_objc3c_m217_frontend_differential_contract.py`,
    `tests/tooling/test_objc3c_m217_sema_differential_contract.py`,
    `tests/tooling/test_objc3c_m217_lowering_differential_contract.py`,
    `tests/tooling/test_objc3c_m217_validation_differential_contract.py`,
    `tests/tooling/test_objc3c_m217_integration_differential_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through differential replay:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain differential anchors.

## M216 integration conformance suite v1

- Gate intent: enforce deterministic Objective-C 3 conformance suite v1 mapping against spec sections.
### 1.1 Conformance integration chain
- Deterministic conformance gate:
  - `npm run check:objc3c:m216-conformance-suite-v1`
- Chain order:
  - replays `check:objc3c:m217-differential-parity`.
  - enforces all M216 lane contracts:
    `tests/tooling/test_objc3c_m216_frontend_conformance_contract.py`,
    `tests/tooling/test_objc3c_m216_sema_conformance_contract.py`,
    `tests/tooling/test_objc3c_m216_lowering_conformance_contract.py`,
    `tests/tooling/test_objc3c_m216_validation_conformance_contract.py`,
    `tests/tooling/test_objc3c_m216_integration_conformance_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through conformance suite execution:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain conformance anchors.

## M215 integration SDK/toolchain packaging

- Gate intent: enforce deterministic SDK/toolchain packaging evidence for IDE consumption.
### 1.1 SDK packaging integration chain
- Deterministic SDK gate:
  - `npm run check:objc3c:m215-sdk-packaging`
- Chain order:
  - replays `check:objc3c:m216-conformance-suite-v1`.
  - enforces all M215 lane contracts:
    `tests/tooling/test_objc3c_m215_frontend_sdk_packaging_contract.py`,
    `tests/tooling/test_objc3c_m215_sema_sdk_packaging_contract.py`,
    `tests/tooling/test_objc3c_m215_lowering_sdk_packaging_contract.py`,
    `tests/tooling/test_objc3c_m215_validation_sdk_packaging_contract.py`,
    `tests/tooling/test_objc3c_m215_integration_sdk_packaging_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through SDK packaging validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain packaging anchors.

## M214 integration daemonized compiler/watch mode

- Gate intent: enforce deterministic daemon/watch mode evidence across all lanes.
### 1.1 Daemonized integration chain
- Deterministic daemonized gate:
  - `npm run check:objc3c:m214-daemonized-watch`
- Chain order:
  - replays `check:objc3c:m215-sdk-packaging`.
  - enforces all M214 lane contracts:
    `tests/tooling/test_objc3c_m214_frontend_daemonized_contract.py`,
    `tests/tooling/test_objc3c_m214_sema_daemonized_contract.py`,
    `tests/tooling/test_objc3c_m214_lowering_daemonized_contract.py`,
    `tests/tooling/test_objc3c_m214_validation_daemonized_contract.py`,
    `tests/tooling/test_objc3c_m214_integration_daemonized_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through daemonized/watch validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain daemonized anchors.

## M213 integration debug-info fidelity

- Gate intent: enforce deterministic debug-info fidelity evidence across all lanes.
### 1.1 Debug-fidelity integration chain
- Deterministic debug-fidelity gate:
  - `npm run check:objc3c:m213-debug-fidelity`
- Chain order:
  - replays `check:objc3c:m214-daemonized-watch`.
  - enforces all M213 lane contracts:
    `tests/tooling/test_objc3c_m213_frontend_debug_fidelity_contract.py`,
    `tests/tooling/test_objc3c_m213_sema_debug_fidelity_contract.py`,
    `tests/tooling/test_objc3c_m213_lowering_debug_fidelity_contract.py`,
    `tests/tooling/test_objc3c_m213_validation_debug_fidelity_contract.py`,
    `tests/tooling/test_objc3c_m213_integration_debug_fidelity_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through debug-fidelity validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain debug-fidelity anchors.

## M212 integration refactor/code-action engine

- Gate intent: enforce deterministic code-action/refactor evidence across all lanes.
### 1.1 Code-action integration chain
- Deterministic code-action gate:
  - `npm run check:objc3c:m212-code-action`
- Chain order:
  - replays `check:objc3c:m213-debug-fidelity`.
  - enforces all M212 lane contracts:
    `tests/tooling/test_objc3c_m212_frontend_code_action_contract.py`,
    `tests/tooling/test_objc3c_m212_sema_code_action_contract.py`,
    `tests/tooling/test_objc3c_m212_lowering_code_action_contract.py`,
    `tests/tooling/test_objc3c_m212_validation_code_action_contract.py`,
    `tests/tooling/test_objc3c_m212_integration_code_action_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through code-action validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain code-action anchors.

## M211 integration LSP semantic tokens and navigation

- Gate intent: enforce deterministic LSP semantic-token/navigation evidence across all lanes.
### 1.1 LSP integration chain
- Deterministic LSP gate:
  - `npm run check:objc3c:m211-lsp-semantics`
- Chain order:
  - replays `check:objc3c:m212-code-action`.
  - enforces all M211 lane contracts:
    `tests/tooling/test_objc3c_m211_frontend_lsp_contract.py`,
    `tests/tooling/test_objc3c_m211_sema_lsp_contract.py`,
    `tests/tooling/test_objc3c_m211_lowering_lsp_contract.py`,
    `tests/tooling/test_objc3c_m211_validation_lsp_contract.py`,
    `tests/tooling/test_objc3c_m211_integration_lsp_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through LSP semantic validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain LSP anchors.

## M210 integration performance budgets and regression gates

- Gate intent: enforce deterministic performance-budget and regression-gate evidence across all lanes.
### 1.1 Performance-regression integration chain
- Deterministic performance-regression gate:
  - `npm run check:objc3c:m210-performance-regression`
- Chain order:
  - replays `check:objc3c:m211-lsp-semantics`.
  - enforces all M210 lane contracts:
    `tests/tooling/test_objc3c_m210_frontend_perf_regression_contract.py`,
    `tests/tooling/test_objc3c_m210_sema_perf_regression_contract.py`,
    `tests/tooling/test_objc3c_m210_lowering_perf_regression_contract.py`,
    `tests/tooling/test_objc3c_m210_validation_perf_regression_contract.py`,
    `tests/tooling/test_objc3c_m210_integration_perf_regression_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through performance-regression validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain regression-gate anchors.

## M209 integration profile-guided optimization hooks

- Gate intent: enforce deterministic profile-guided optimization hook evidence across all lanes.
### 1.1 PGO integration chain
- Deterministic PGO hook gate:
  - `npm run check:objc3c:m209-pgo-hooks`
- Chain order:
  - replays `check:objc3c:m210-performance-regression`.
  - enforces all M209 lane contracts:
    `tests/tooling/test_objc3c_m209_frontend_pgo_contract.py`,
    `tests/tooling/test_objc3c_m209_sema_pgo_contract.py`,
    `tests/tooling/test_objc3c_m209_lowering_pgo_contract.py`,
    `tests/tooling/test_objc3c_m209_validation_pgo_contract.py`,
    `tests/tooling/test_objc3c_m209_integration_pgo_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through PGO-hook validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain PGO anchors.

## M208 integration whole-module optimization controls

- Gate intent: enforce deterministic whole-module optimization control evidence across all lanes.

## M146 integration @interface/@implementation grammar

- Integration gate:
  - `npm run check:objc3c:m146-interface-implementation`
- Gate coverage files:
  - `tests/tooling/test_objc3c_m146_frontend_interface_implementation_contract.py`
  - `tests/tooling/test_objc3c_m146_sema_interface_implementation_contract.py`
  - `tests/tooling/test_objc3c_m146_lowering_interface_implementation_contract.py`
  - `tests/tooling/test_objc3c_m146_validation_interface_implementation_contract.py`
  - `tests/tooling/test_objc3c_m146_integration_interface_implementation_contract.py`

## M147 integration @protocol/@category grammar

- Integration gate:
  - `npm run check:objc3c:m147-protocol-category`
- Gate coverage files:
  - `tests/tooling/test_objc3c_m147_frontend_protocol_category_contract.py`
  - `tests/tooling/test_objc3c_m147_sema_protocol_category_contract.py`
  - `tests/tooling/test_objc3c_m147_lowering_protocol_category_contract.py`
  - `tests/tooling/test_objc3c_m147_validation_protocol_category_contract.py`
  - `tests/tooling/test_objc3c_m147_integration_protocol_category_contract.py`

## M148 integration selector-normalized method declaration grammar

- Integration gate:
  - `npm run check:objc3c:m148-selector-normalization`
- Gate coverage files:
  - `tests/tooling/test_objc3c_m148_frontend_selector_normalization_contract.py`
  - `tests/tooling/test_objc3c_m148_sema_selector_normalization_contract.py`
  - `tests/tooling/test_objc3c_m148_lowering_selector_normalization_contract.py`
  - `tests/tooling/test_objc3c_m148_validation_selector_normalization_contract.py`
  - `tests/tooling/test_objc3c_m148_integration_selector_normalization_contract.py`

## M149 integration @property grammar and attribute parsing

- Integration gate:
  - `npm run check:objc3c:m149-property-attributes`
- Gate coverage files:
  - `tests/tooling/test_objc3c_m149_frontend_property_attribute_contract.py`
  - `tests/tooling/test_objc3c_m149_sema_property_attribute_contract.py`
  - `tests/tooling/test_objc3c_m149_lowering_property_attribute_contract.py`
  - `tests/tooling/test_objc3c_m149_validation_property_attribute_contract.py`
  - `tests/tooling/test_objc3c_m149_integration_property_attribute_contract.py`

## M150 integration object-pointer declarators, nullability, lightweight generics parse

- Integration gate:
  - `npm run check:objc3c:m150-object-pointer-nullability-generics`
- Gate coverage files:
  - `tests/tooling/test_objc3c_m150_frontend_object_pointer_nullability_generics_contract.py`
  - `tests/tooling/test_objc3c_m150_sema_object_pointer_nullability_generics_contract.py`
  - `tests/tooling/test_objc3c_m150_lowering_object_pointer_nullability_generics_contract.py`
  - `tests/tooling/test_objc3c_m150_validation_object_pointer_nullability_generics_contract.py`
  - `tests/tooling/test_objc3c_m150_integration_object_pointer_nullability_generics_contract.py`

## M151 integration symbol graph and scope resolution overhaul

- Integration gate:
  - `npm run check:objc3c:m151-symbol-graph-scope-resolution`
- Gate coverage files:
  - `tests/tooling/test_objc3c_m151_frontend_symbol_graph_scope_resolution_contract.py`
  - `tests/tooling/test_objc3c_m151_sema_symbol_graph_scope_resolution_contract.py`
  - `tests/tooling/test_objc3c_m151_lowering_symbol_graph_scope_resolution_contract.py`
  - `tests/tooling/test_objc3c_m151_validation_symbol_graph_scope_resolution_contract.py`
  - `tests/tooling/test_objc3c_m151_integration_symbol_graph_scope_resolution_contract.py`
### 1.1 WMO integration chain
- Deterministic WMO gate:
  - `npm run check:objc3c:m208-whole-module-optimization`
- Chain order:
  - replays `check:objc3c:m209-pgo-hooks`.
  - enforces all M208 lane contracts:
    `tests/tooling/test_objc3c_m208_frontend_wmo_contract.py`,
    `tests/tooling/test_objc3c_m208_sema_wmo_contract.py`,
    `tests/tooling/test_objc3c_m208_lowering_wmo_contract.py`,
    `tests/tooling/test_objc3c_m208_validation_wmo_contract.py`,
    `tests/tooling/test_objc3c_m208_integration_wmo_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through WMO validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain WMO anchors.

## M207 integration dispatch-specific optimization passes

- Gate intent: enforce deterministic dispatch-specific optimization evidence across all lanes.
### 1.1 Dispatch-optimization integration chain
- Deterministic dispatch-optimization gate:
  - `npm run check:objc3c:m207-dispatch-optimizations`
- Chain order:
  - replays `check:objc3c:m208-whole-module-optimization`.
  - enforces all M207 lane contracts:
    `tests/tooling/test_objc3c_m207_frontend_dispatch_optimizations_contract.py`,
    `tests/tooling/test_objc3c_m207_sema_dispatch_optimizations_contract.py`,
    `tests/tooling/test_objc3c_m207_lowering_dispatch_optimizations_contract.py`,
    `tests/tooling/test_objc3c_m207_validation_dispatch_optimizations_contract.py`,
    `tests/tooling/test_objc3c_m207_integration_dispatch_optimizations_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through dispatch-optimization validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain dispatch-optimization anchors.

## M206 integration canonical optimization pipeline stage-1

- Gate intent: enforce deterministic canonical-optimization stage-1 evidence across all lanes.
### 1.1 Canonical-optimization stage-1 chain
- Deterministic canonical-optimization stage-1 gate:
  - `npm run check:objc3c:m206-canonical-optimization-stage1`
- Chain order:
  - replays `check:objc3c:m207-dispatch-optimizations`.
  - enforces all M206 lane contracts:
    `tests/tooling/test_objc3c_m206_frontend_canonical_optimization_contract.py`,
    `tests/tooling/test_objc3c_m206_sema_canonical_optimization_contract.py`,
    `tests/tooling/test_objc3c_m206_lowering_canonical_optimization_contract.py`,
    `tests/tooling/test_objc3c_m206_validation_canonical_optimization_contract.py`,
    `tests/tooling/test_objc3c_m206_integration_canonical_optimization_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through canonical-optimization validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain canonical-optimization anchors.

## M205 integration macro security policy enforcement

- Gate intent: enforce deterministic macro-security policy evidence across all lanes.
### 1.1 Macro-security integration chain
- Deterministic macro-security gate:
  - `npm run check:objc3c:m205-macro-security`
- Chain order:
  - replays `check:objc3c:m206-canonical-optimization-stage1`.
  - enforces all M205 lane contracts:
    `tests/tooling/test_objc3c_m205_frontend_macro_security_contract.py`,
    `tests/tooling/test_objc3c_m205_sema_macro_security_contract.py`,
    `tests/tooling/test_objc3c_m205_lowering_macro_security_contract.py`,
    `tests/tooling/test_objc3c_m205_validation_macro_security_contract.py`,
    `tests/tooling/test_objc3c_m205_integration_macro_security_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through macro-security validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain macro-security anchors.

## M204 integration macro diagnostics and provenance

- Gate intent: enforce deterministic macro-diagnostics/provenance evidence across all lanes.
### 1.1 Macro-diagnostics integration chain
- Deterministic macro-diagnostics gate:
  - `npm run check:objc3c:m204-macro-diagnostics`
- Chain order:
  - replays `check:objc3c:m205-macro-security`.
  - enforces all M204 lane contracts:
    `tests/tooling/test_objc3c_m204_frontend_macro_diagnostics_contract.py`,
    `tests/tooling/test_objc3c_m204_sema_macro_diagnostics_contract.py`,
    `tests/tooling/test_objc3c_m204_lowering_macro_diagnostics_contract.py`,
    `tests/tooling/test_objc3c_m204_validation_macro_diagnostics_contract.py`,
    `tests/tooling/test_objc3c_m204_integration_macro_diagnostics_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through macro-diagnostics validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain macro-diagnostics anchors.

## M203 integration compile-time evaluation engine

- Gate intent: enforce deterministic compile-time-eval evidence across all lanes.
### 1.1 Compile-time-eval integration chain
- Deterministic compile-time-eval gate:
  - `npm run check:objc3c:m203-compile-time-eval`
- Chain order:
  - replays `check:objc3c:m204-macro-diagnostics`.
  - enforces all M203 lane contracts:
    `tests/tooling/test_objc3c_m203_frontend_compile_time_eval_contract.py`,
    `tests/tooling/test_objc3c_m203_sema_compile_time_eval_contract.py`,
    `tests/tooling/test_objc3c_m203_lowering_compile_time_eval_contract.py`,
    `tests/tooling/test_objc3c_m203_validation_compile_time_eval_contract.py`,
    `tests/tooling/test_objc3c_m203_integration_compile_time_eval_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through compile-time-eval validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain compile-time-eval anchors.

## M202 integration derive/synthesis pipeline

- Gate intent: enforce deterministic derive/synthesis evidence across all lanes.
### 1.1 Derive/synthesis integration chain
- Deterministic derive/synthesis gate:
  - `npm run check:objc3c:m202-derive-synthesis`
- Chain order:
  - replays `check:objc3c:m203-compile-time-eval`.
  - enforces all M202 lane contracts:
    `tests/tooling/test_objc3c_m202_frontend_derive_synthesis_contract.py`,
    `tests/tooling/test_objc3c_m202_sema_derive_synthesis_contract.py`,
    `tests/tooling/test_objc3c_m202_lowering_derive_synthesis_contract.py`,
    `tests/tooling/test_objc3c_m202_validation_derive_synthesis_contract.py`,
    `tests/tooling/test_objc3c_m202_integration_derive_synthesis_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through derive/synthesis validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain derive/synthesis anchors.

## M201 integration macro expansion architecture and isolation

- Gate intent: enforce deterministic macro-expansion architecture/isolation evidence across all lanes.
### 1.1 Macro-expansion architecture integration chain
- Deterministic macro-expansion architecture gate:
  - `npm run check:objc3c:m201-macro-expansion-arch`
- Chain order:
  - replays `check:objc3c:m202-derive-synthesis`.
  - enforces all M201 lane contracts:
    `tests/tooling/test_objc3c_m201_frontend_macro_expansion_contract.py`,
    `tests/tooling/test_objc3c_m201_sema_macro_expansion_contract.py`,
    `tests/tooling/test_objc3c_m201_lowering_macro_expansion_contract.py`,
    `tests/tooling/test_objc3c_m201_validation_macro_expansion_contract.py`,
    `tests/tooling/test_objc3c_m201_integration_macro_expansion_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through macro-expansion architecture validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain macro-expansion architecture anchors.

## M200 integration interop integration suite and packaging

- Gate intent: enforce deterministic interop integration suite/packaging evidence across all lanes.
### 1.1 Interop integration suite packaging chain
- Deterministic interop integration suite packaging gate:
  - `npm run check:objc3c:m200-interop-packaging`
- Chain order:
  - replays `check:objc3c:m201-macro-expansion-arch`.
  - enforces all M200 lane contracts:
    `tests/tooling/test_objc3c_m200_frontend_interop_packaging_contract.py`,
    `tests/tooling/test_objc3c_m200_sema_interop_packaging_contract.py`,
    `tests/tooling/test_objc3c_m200_lowering_interop_packaging_contract.py`,
    `tests/tooling/test_objc3c_m200_validation_interop_packaging_contract.py`,
    `tests/tooling/test_objc3c_m200_integration_interop_packaging_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through interop integration suite packaging validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain interop integration suite packaging anchors.

## M199 integration foreign type import diagnostics

- Gate intent: enforce deterministic foreign type import diagnostics evidence across all lanes.
### 1.1 Foreign type import diagnostics integration chain
- Deterministic foreign type import diagnostics gate:
  - `npm run check:objc3c:m199-foreign-type-diagnostics`
- Chain order:
  - replays `check:objc3c:m200-interop-packaging`.
  - enforces all M199 lane contracts:
    `tests/tooling/test_objc3c_m199_frontend_foreign_type_diagnostics_contract.py`,
    `tests/tooling/test_objc3c_m199_sema_foreign_type_diagnostics_contract.py`,
    `tests/tooling/test_objc3c_m199_lowering_foreign_type_diagnostics_contract.py`,
    `tests/tooling/test_objc3c_m199_validation_foreign_type_diagnostics_contract.py`,
    `tests/tooling/test_objc3c_m199_integration_foreign_type_diagnostics_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through foreign type import diagnostics validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain foreign type import diagnostics anchors.

## M198 integration swift metadata bridge

- Gate intent: enforce deterministic Swift metadata-bridge evidence across all lanes.
### 1.1 Swift metadata-bridge integration chain
- Deterministic Swift metadata-bridge gate:
  - `npm run check:objc3c:m198-swift-metadata-bridge`
- Chain order:
  - replays `check:objc3c:m199-foreign-type-diagnostics`.
  - enforces all M198 lane contracts:
    `tests/tooling/test_objc3c_m198_frontend_swift_metadata_bridge_contract.py`,
    `tests/tooling/test_objc3c_m198_sema_swift_metadata_bridge_contract.py`,
    `tests/tooling/test_objc3c_m198_lowering_swift_metadata_bridge_contract.py`,
    `tests/tooling/test_objc3c_m198_validation_swift_metadata_bridge_contract.py`,
    `tests/tooling/test_objc3c_m198_integration_swift_metadata_bridge_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through Swift metadata-bridge validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain Swift metadata-bridge anchors.

## M197 integration C++ interop shim strategy

- Gate intent: enforce deterministic C++ interop-shim evidence across all lanes.
### 1.1 C++ interop-shim integration chain
- Deterministic C++ interop-shim gate:
  - `npm run check:objc3c:m197-cpp-interop-shim`
- Chain order:
  - replays `check:objc3c:m198-swift-metadata-bridge`.
  - enforces all M197 lane contracts:
    `tests/tooling/test_objc3c_m197_frontend_cpp_interop_shim_contract.py`,
    `tests/tooling/test_objc3c_m197_sema_cpp_interop_shim_contract.py`,
    `tests/tooling/test_objc3c_m197_lowering_cpp_interop_shim_contract.py`,
    `tests/tooling/test_objc3c_m197_validation_cpp_interop_shim_contract.py`,
    `tests/tooling/test_objc3c_m197_integration_cpp_interop_shim_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through C++ interop-shim validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain C++ interop-shim anchors.

## M196 integration C interop headers and ABI alignment

- Gate intent: enforce deterministic C-interop header/ABI-alignment evidence across all lanes.
### 1.1 C-interop header/ABI-alignment integration chain
- Deterministic C-interop header/ABI-alignment gate:
  - `npm run check:objc3c:m196-c-interop-headers-abi`
- Chain order:
  - replays `check:objc3c:m197-cpp-interop-shim`.
  - enforces all M196 lane contracts:
    `tests/tooling/test_objc3c_m196_frontend_c_interop_headers_abi_contract.py`,
    `tests/tooling/test_objc3c_m196_sema_c_interop_headers_abi_contract.py`,
    `tests/tooling/test_objc3c_m196_lowering_c_interop_headers_abi_contract.py`,
    `tests/tooling/test_objc3c_m196_validation_c_interop_headers_abi_contract.py`,
    `tests/tooling/test_objc3c_m196_integration_c_interop_headers_abi_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through C-interop header/ABI-alignment validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain C-interop header/ABI-alignment anchors.

## M195 integration system-extension conformance and policy

- Gate intent: enforce deterministic system-extension conformance/policy evidence across all lanes.
### 1.1 System-extension conformance/policy integration chain
- Deterministic system-extension conformance/policy gate:
  - `npm run check:objc3c:m195-system-extension-policy`
- Chain order:
  - replays `check:objc3c:m196-c-interop-headers-abi`.
  - enforces all M195 lane contracts:
    `tests/tooling/test_objc3c_m195_frontend_system_extension_policy_contract.py`,
    `tests/tooling/test_objc3c_m195_sema_system_extension_policy_contract.py`,
    `tests/tooling/test_objc3c_m195_lowering_system_extension_policy_contract.py`,
    `tests/tooling/test_objc3c_m195_validation_system_extension_policy_contract.py`,
    `tests/tooling/test_objc3c_m195_integration_system_extension_policy_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through system-extension conformance/policy validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain system-extension conformance/policy anchors.

## M194 integration atomics and memory-order mapping

- Gate intent: enforce deterministic atomics/memory-order mapping evidence across all lanes.
### 1.1 Atomics/memory-order integration chain
- Deterministic atomics/memory-order gate:
  - `npm run check:objc3c:m194-atomics-memory-order`
- Chain order:
  - replays `check:objc3c:m195-system-extension-policy`.
  - enforces all M194 lane contracts:
    `tests/tooling/test_objc3c_m194_frontend_atomics_memory_order_contract.py`,
    `tests/tooling/test_objc3c_m194_sema_atomics_memory_order_contract.py`,
    `tests/tooling/test_objc3c_m194_lowering_atomics_memory_order_contract.py`,
    `tests/tooling/test_objc3c_m194_validation_atomics_memory_order_contract.py`,
    `tests/tooling/test_objc3c_m194_integration_atomics_memory_order_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through atomics/memory-order validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain atomics/memory-order mapping anchors.

## M193 integration SIMD/vector type lowering

- Gate intent: enforce deterministic SIMD/vector type lowering evidence across all lanes.
### 1.1 SIMD/vector type lowering integration chain
- Deterministic SIMD/vector type lowering gate:
  - `npm run check:objc3c:m193-simd-vector-lowering`
- Chain order:
  - replays `check:objc3c:m194-atomics-memory-order`.
  - enforces all M193 lane contracts:
    `tests/tooling/test_objc3c_m193_frontend_simd_vector_lowering_contract.py`,
    `tests/tooling/test_objc3c_m193_sema_simd_vector_lowering_contract.py`,
    `tests/tooling/test_objc3c_m193_lowering_simd_vector_lowering_contract.py`,
    `tests/tooling/test_objc3c_m193_validation_simd_vector_lowering_contract.py`,
    `tests/tooling/test_objc3c_m193_integration_simd_vector_lowering_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through SIMD/vector type lowering validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain SIMD/vector type lowering anchors.

## M142 integration CLI and C API parity harness

- Gate intent: enforce deterministic CLI/C API parity evidence across frontend, sema, lowering/runtime, validation, and integration lanes.
### 1.1 CLI/C API parity integration chain
- Deterministic CLI/C API parity gate:
  - `npm run check:objc3c:m142-cli-c-api-parity`
- Chain order:
  - replays `check:compiler-closeout:m142`.
  - enforces all M142 lane contracts:
    `tests/tooling/test_objc3c_m142_frontend_cli_c_api_parity_contract.py`,
    `tests/tooling/test_objc3c_m142_sema_cli_c_api_parity_contract.py`,
    `tests/tooling/test_objc3c_m142_lowering_cli_c_api_parity_contract.py`,
    `tests/tooling/test_objc3c_m142_validation_cli_c_api_parity_contract.py`,
    `tests/tooling/test_objc3c_m142_integration_cli_c_api_parity_contract.py`.
### 1.2 ABI/version guard continuity
- Preserve startup/version invariants through CLI/C API parity validation:
  - `objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)`.
  - `objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()`.
  - `OBJC3C_FRONTEND_VERSION_STRING` and `OBJC3C_FRONTEND_ABI_VERSION` remain CLI/C API parity anchors.

## Current call contract

- `objc3c_frontend_context_create()` returns `NULL` on allocation failure.
- `objc3c_frontend_context_destroy(ctx)` releases context resources.
- `objc3c_frontend_compile_file(...)` and `objc3c_frontend_compile_source(...)`:
  - Return `OBJC3C_FRONTEND_STATUS_USAGE_ERROR` when `context`, `options`, or `result` is `NULL`.
  - For non-null pointers, currently return `OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR` and set context error text to a scaffolded-entrypoint message.
- `objc3c_frontend_copy_last_error(ctx, buffer, buffer_size)`:
  - Returns required byte count including the trailing NUL.
  - Supports size probing with `buffer == NULL` or `buffer_size == 0`.
  - NUL-terminates written buffers when `buffer_size > 0`.

## Minimal embedding example (current API reality)

```c
#include <stdio.h>
#include <string.h>

#include "libobjc3c_frontend/api.h"

int main(void) {
  if (!objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)) {
    return 2;
  }

  objc3c_frontend_context_t *ctx = objc3c_frontend_context_create();
  if (ctx == NULL) {
    return 3;
  }

  objc3c_frontend_compile_options_t opts;
  memset(&opts, 0, sizeof(opts));
  opts.input_path = "example.objc3";
  opts.out_dir = "artifacts/compilation/objc3c-native";

  objc3c_frontend_compile_result_t result;
  memset(&result, 0, sizeof(result));

  const objc3c_frontend_status_t status = objc3c_frontend_compile_file(ctx, &opts, &result);
  if (status == OBJC3C_FRONTEND_STATUS_INTERNAL_ERROR) {
    char err[512];
    (void)objc3c_frontend_copy_last_error(ctx, err, sizeof(err));
    fprintf(stderr, "frontend unavailable yet: %s\n", err);
  }

  objc3c_frontend_context_destroy(ctx);
  return status == OBJC3C_FRONTEND_STATUS_OK ? 0 : 1;
}
```

For pure C environments that prefer `*_c_*` symbol names, use `c_api.h`; it forwards to the same underlying ABI and behavior.
