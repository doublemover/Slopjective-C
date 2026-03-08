# M253-E002 Cross-Lane Object Emission Gate And Closeout Packet

Packet: `M253-E002`
Milestone: `M253`
Lane: `E`
Issue: `#7100`

## Objective

Gate the milestone on source graph closure, layout policy, emitted sections,
and linker retention proof so later startup work can trust the objects.

## Dependencies

- `M253-A002`
- `M253-B003`
- `M253-C006`
- `M253-D003`
- `M253-E001`

## Required implementation

1. Add a canonical lane-E expectations document for the integrated closeout.
2. Add this packet, a deterministic checker, tooling tests, and a direct
   readiness runner:
   - `scripts/check_m253_e002_cross_lane_object_emission_gate_and_closeout.py`
   - `tests/tooling/test_check_m253_e002_cross_lane_object_emission_gate_and_closeout.py`
   - `scripts/run_m253_e002_lane_e_readiness.py`
3. Add lane-E `M253-E002` anchor text to:
   - `docs/objc3c-native.md`
   - `native/objc3c/src/ARCHITECTURE.md`
   - `spec/LOWERING_AND_RUNTIME_CONTRACTS.md`
   - `spec/MODULE_METADATA_AND_ABI_TABLES.md`
   - `native/objc3c/src/lower/objc3_lowering_contract.h`
   - `native/objc3c/src/lower/objc3_lowering_contract.cpp`
   - `native/objc3c/src/ir/objc3_ir_emitter.cpp`
   - `native/objc3c/src/io/objc3_process.cpp`
4. Require the checker to load the canonical `M253-E001` summary and fail
   closed if it is missing or no longer reports `ok: true`.
5. Require the checker to load the canonical `M253-A002`, `M253-B003`,
   `M253-C006`, and `M253-D003` summaries and fail closed if any drift or
   disappear.
6. Require real native object-emission probes over:
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_class_protocol_property_ivar.objc3`
   - `tests/tooling/fixtures/native/m251_runtime_metadata_source_records_category_protocol_property.objc3`
   - `tests/tooling/fixtures/native/execution/positive/message_send_runtime_shim.objc3`
   - `tests/tooling/fixtures/native/m252_b004_missing_interface_property.objc3`
7. Require the positive probes to verify on the same output path:
   - manifest graph closure where applicable,
   - `llvm-direct` object emission,
   - IR publication of the lane-E emission-gate and closeout metadata,
   - binary section inventories and tracked symbol continuity,
   - linker-response and discovery sidecar continuity.
8. Require the negative probe to verify deterministic `O3S206` diagnostics and
   the absence of object/discovery sidecars.
9. Require an integrated fan-in proof that the positive class and category
   linker/discovery sidecars publish distinct anchor symbols, discovery roots,
   linker flags, and translation-unit identity keys.
10. Wire `package.json` so lane-E readiness points at the direct runner.

## Determinism And Fail-Closed Rules

- `M253-E002` is a closeout gate only; it does not land new runtime startup
  registration or executable class registration behavior.
- The packet therefore fails closed when dependency summaries drift, integrated
  native probes regress, sidecar fan-in uniqueness is lost, or docs/runner
  wiring drifts.

## Validation plan

The checker runs integrated native probes plus dependency-summary validation,
and the lane-E readiness runner executes the direct chain once.

## Evidence

- `tmp/reports/m253/M253-E002/cross_lane_object_emission_gate_and_closeout_summary.json`
