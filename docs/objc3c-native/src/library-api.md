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
