# M258-E002 Runnable Import And Module Execution Matrix Plus Docs Cross-Lane Integration Sync Packet

Packet: `M258-E002`  
Milestone: `M258`  
Lane: `E`  
Issue: `#7167`

## Objective

Close the M258 module/import tranche with one live runnable import/module
execution matrix that proves the current separate-compilation object-model path
is truthful end to end.

## Dependencies

- `M258-A002`
- `M258-B002`
- `M258-C002`
- `M258-D002`
- `M258-E001`

## Required outputs

- `docs/contracts/m258_runnable_import_and_module_execution_matrix_plus_docs_cross_lane_integration_sync_e002_expectations.md`
- `scripts/check_m258_e002_runnable_import_and_module_execution_matrix_plus_docs_cross_lane_integration_sync.py`
- `tests/tooling/test_check_m258_e002_runnable_import_and_module_execution_matrix_plus_docs_cross_lane_integration_sync.py`
- `scripts/run_m258_e002_lane_e_readiness.py`
- `tests/tooling/runtime/m258_e002_import_module_execution_matrix_probe.cpp`
- `tmp/reports/m258/M258-E002/runnable_import_module_execution_matrix_summary.json`

## Acceptance criteria

- Fail closed unless the `M258-A002` / `M258-B002` / `M258-C002` /
  `M258-D002` / `M258-E001` chain remains present, green, and contract-stable.
- Compile `m258_d002_runtime_packaging_provider.objc3` and
  `m258_d002_runtime_packaging_consumer.objc3` separately, then link the
  emitted objects through the emitted cross-module response file into one live
  runtime probe.
- The live probe must prove startup registration, realized-class lookup,
  selector lookup, method-cache resolution, protocol conformance, reset, and
  replay on the real integrated path.
- The live probe must prove that the current integrated dispatch path stays
  non-zero and replay-stable for `providerClassValue`,
  `importedProtocolValue`, and `localClassValue` without overstating source
  method-body binding.
- Code/spec anchors remain explicit and deterministic.
- `M259-A001` is the explicit next handoff after this matrix closes.

## Notes

- This issue broadens the frozen `M258-E001` gate into a real closeout matrix.
- This issue does not claim new parser, sema, lowering, or runtime features.
- This issue reuses the already landed D002 provider/consumer path and adds the
  live lane-E matrix and docs closeout around it.

Next issue: `M259-A001`
