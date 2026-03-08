# M253-D003 Expectations

## Objective

Close the remaining lane-D metadata discovery gaps after `M253-D002` by making the public linker/discovery symbols translation-unit-stable, by ensuring metadata-only library objects do not export a colliding public `objc3c_entry`, and by publishing one canonical merged discovery/response artifact pair for multi-archive static-link orchestration.

## Required outcomes

1. The native IR must publish the continued object-level linker-retention handoff through:
   - `; runtime_metadata_linker_retention = ...`
   - `!objc3.objc_runtime_linker_retention`
2. The native IR must also publish the D003 archive/static-link discovery handoff through:
   - `; runtime_metadata_archive_static_link_discovery = ...`
   - `!objc3.objc_runtime_archive_static_link_discovery`
3. The linker-anchor hash seed must include translation-unit identity in addition to module/metadata replay inputs.
4. Successful object emission must continue writing:
   - `module.runtime-metadata-linker-options.rsp`
   - `module.runtime-metadata-discovery.json`
5. Object-level discovery JSON must now include:
   - `translation_unit_identity_model`
   - `translation_unit_identity_key`
6. Metadata-only library objects compiled from distinct translation units with identical module/metadata surface must produce distinct:
   - `objc3_runtime_metadata_link_anchor_<hash>`
   - `objc3_runtime_metadata_discovery_root_<hash>`
7. Metadata-only translation units without a source `main` must no longer export a duplicate public `objc3c_entry` in multi-archive links.
8. A canonical merge utility must produce:
   - `module.merged.runtime-metadata-linker-options.rsp`
   - `module.merged.runtime-metadata-discovery.json`
9. The merge utility must fail closed on collisions or malformed discovery inputs.
10. The positive multi-archive proof must show:
    - plain link drops metadata sections,
    - merged-response link retains metadata sections,
    - merged retained metadata is larger than the single-archive retained baseline.

## Non-goals

- No runtime registration/startup bootstrap yet.
- No class/protocol/property runtime execution semantics yet.
- No archive-builder integration inside the native compiler driver yet.

## Evidence

- `tmp/reports/m253/M253-D003/archive_and_static_link_metadata_discovery_behavior_summary.json`
