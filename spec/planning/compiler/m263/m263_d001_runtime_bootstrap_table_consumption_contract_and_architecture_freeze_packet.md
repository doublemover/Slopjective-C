# M263-D001 Runtime Bootstrap Table Consumption Contract And Architecture Freeze Packet

Packet: `M263-D001`

Milestone: `M263`

Lane: `D`

Issue: `#7228`

Dependencies: `M263-C002`, `M254-D002`

## Summary

Freeze the already-live runtime-side bridge that consumes staged registration tables, rejects duplicate bootstrap identities before state advance, and publishes authoritative image-walk state for probes and closeout evidence.

Contract ID: `objc3c-runtime-bootstrap-table-consumption-freeze/m263-d001-v1`

## Acceptance criteria

- publish an explicit `M263-D001` contract in docs, specs, runtime anchors, and the emitted runtime registration manifest
- prove the next public `objc3_runtime_register_image` call consumes the staged registration table exactly once
- prove duplicate identity rejection does not advance registered-image counters or ordinals
- prove bootstrap-visible image state remains available through `objc3_runtime_copy_image_walk_state_for_testing`
- land deterministic evidence at `tmp/reports/m263/M263-D001/runtime_bootstrap_table_consumption_contract_summary.json`

## Inputs

- `tests/tooling/fixtures/native/m254_c002_runtime_bootstrap_metadata_library.objc3`
- `tests/tooling/fixtures/native/m254_c003_runtime_bootstrap_category_library.objc3`
- `tests/tooling/runtime/m263_d001_runtime_bootstrap_table_consumption_probe.cpp`

## Outputs

- emitted runtime registration manifests with `M263-D001` proof fields
- runtime probe evidence for startup consumption and duplicate rejection
- deterministic issue-local summary JSON under `tmp/reports/m263/M263-D001/`

## Next issue

`M263-D002`
