# M253 Cross-Lane Object Emission Gate And Closeout Expectations (E002)

Contract ID: `objc3c-runtime-cross-lane-object-emission-closeout/m253-e002-v1`

## Objective

Fail closed unless the same native object-emission outputs prove lane-A source
graph closure, lane-B object-format policy continuity, lane-C binary
inspection continuity, lane-D linker/discovery artifact continuity, and lane-E
closeout continuity on one integrated path.

## Dependency evidence

- `tmp/reports/m253/M253-A002/source_to_section_mapping_completeness_matrix_summary.json`
- `tmp/reports/m253/M253-B003/coff_elf_and_mach_o_metadata_policy_surface_core_feature_expansion_summary.json`
- `tmp/reports/m253/M253-C006/binary_inspection_harness_summary.json`
- `tmp/reports/m253/M253-D003/archive_and_static_link_metadata_discovery_behavior_summary.json`
- `tmp/reports/m253/M253-E001/metadata_emission_gate_summary.json`

## Required integrated cases

- `class-protocol-property-ivar-object-closeout`
- `category-protocol-property-object-closeout`
- `message-send-object-closeout`
- `negative-missing-interface-property-closeout`
- `fanin-distinct-linker-discovery-closeout`

## Fail-closed rules

- Positive cases must keep manifest graph closure, `llvm-direct` object
  emission, binary-inspection section inventories, and linker-response /
  discovery artifacts aligned on one output directory.
- Negative cases must preserve deterministic `O3S206` diagnostics and must not
  synthesize `module.ll`, `module.obj`, runtime-metadata sidecars, or
  linker/discovery sidecars.
- The direct runner is `scripts/run_m253_e002_lane_e_readiness.py`.
- Later startup-registration work may build on this gate but may not weaken it.

## Evidence

- `tmp/reports/m253/M253-E002/cross_lane_object_emission_gate_and_closeout_summary.json`
