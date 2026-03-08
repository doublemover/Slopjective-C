# M253 Linker Retention Anchors And Dead-Strip Resistance Core Feature Implementation Expectations (D002)

Contract ID: `objc3c-runtime-linker-retention-and-dead-strip-resistance/m253-d002-v1`

`M253-D002` turns the frozen `M253-D001` object boundary into one real archive/link retention capability.

## Required outcomes

1. The native IR path shall publish one public linker anchor rooted in one public discovery root over the retained metadata aggregates.
2. The emitted IR shall publish the boundary through:
   - `; runtime_metadata_linker_retention = ...`
   - `!objc3.objc_runtime_linker_retention`
3. Successful object emission shall also publish:
   - `module.runtime-metadata-linker-options.rsp`
   - `module.runtime-metadata-discovery.json`
4. The response artifact shall carry one current-format driver linker flag that can retain the archived metadata object:
   - COFF: `-Wl,/include:<symbol>`
   - ELF: `-Wl,--undefined=<symbol>`
   - Mach-O: `-Wl,-u,_<symbol>`
5. The positive proof must package `module.obj` into one archive/library, show that a plain link omits the metadata, then re-link with the emitted response artifact and prove the metadata sections survive dead stripping.
6. The negative proof must remain fail closed: compile failure emits no `module.obj`, no `module.object-backend.txt`, no `module.runtime-metadata-linker-options.rsp`, and no `module.runtime-metadata-discovery.json`.

## Non-goals

- No multi-archive fan-in proof yet.
- No cross-translation-unit anchor-merging proof yet.
- No startup registration/bootstrap behavior yet.

## Evidence

- `tmp/reports/m253/M253-D002/linker_retention_and_dead_strip_resistance_summary.json`
