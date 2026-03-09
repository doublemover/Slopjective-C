# M263 Runtime Bootstrap Table Consumption Contract And Architecture Freeze Expectations (D001)

Issue: `#7228`

Contract ID: `objc3c-runtime-bootstrap-table-consumption-freeze/m263-d001-v1`

Scope: `M263` lane-D freeze work that locks the already-live runtime path for consuming staged registration tables, rejecting duplicate bootstrap identities before state advance, and publishing bootstrap-visible image state for probes and closeout evidence.

## Required outcomes

1. The runtime must treat the staged registration table as a one-shot input to the next public `objc3_runtime_register_image` call.
2. Duplicate translation-unit identity rejection must happen before registered-image counters or ordinals advance.
3. Successful staged-table consumption must publish bootstrap-visible image state through `objc3_runtime_copy_image_walk_state_for_testing`.
4. The emitted runtime registration manifest must publish explicit `M263-D001` proof fields for table consumption, deduplication, and image-state publication.
5. Dynamic proof must compile native `.objc3` fixtures, link an issue-local runtime probe against the emitted object, and prove both startup success and duplicate rejection.
6. Code/spec/package anchors must remain explicit and deterministic.
7. Evidence must land at `tmp/reports/m263/M263-D001/runtime_bootstrap_table_consumption_contract_summary.json`.

## Required artifacts

- `docs/objc3c-native.md`
- `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
- `spec/MODULE_METADATA_AND_ABI_TABLES.md`
- `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- `native/objc3c/src/runtime/objc3_runtime.cpp`
- `native/objc3c/src/io/objc3_process.cpp`
- `tests/tooling/runtime/m263_d001_runtime_bootstrap_table_consumption_probe.cpp`
- `scripts/check_m263_d001_runtime_bootstrap_table_consumption_contract_and_architecture_freeze.py`

## Non-goals

- no new public runtime bootstrap entrypoints
- no new replay/reset behavior beyond the already-landed runtime surfaces
- no widening of the existing registration-table ABI
