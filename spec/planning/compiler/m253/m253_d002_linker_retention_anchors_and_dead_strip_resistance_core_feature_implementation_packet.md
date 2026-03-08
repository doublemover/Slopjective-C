# M253-D002 Linker Retention Anchors And Dead-Strip Resistance Core Feature Implementation Packet

Packet: `M253-D002`
Milestone: `M253`
Lane: `D`
Issue: `#7097`
Contract ID: `objc3c-runtime-linker-retention-and-dead-strip-resistance/m253-d002-v1`

## Objective

Keep emitted runtime metadata discoverable after `module.obj` is packaged into
one archive/library by publishing one public linker anchor, one public
discovery root, and one deterministic driver-friendly response artifact that
can force-retain the archived metadata object.

## Dependencies

- `M253-D001`
- `M253-C004`

## Required outcomes

- publish `Objc3RuntimeMetadataLinkerRetentionSummary`
- emit `!objc3.objc_runtime_linker_retention`
- emit `; runtime_metadata_linker_retention = ...`
- publish one hashed public linker-anchor symbol
  `objc3_runtime_metadata_link_anchor_<hash>`
- publish one hashed public discovery-root symbol
  `objc3_runtime_metadata_discovery_root_<hash>`
- emit `module.runtime-metadata-linker-options.rsp`
- emit `module.runtime-metadata-discovery.json`
- emit one current-format driver linker flag:
  - COFF: `-Wl,/include:<symbol>`
  - ELF: `-Wl,--undefined=<symbol>`
  - Mach-O: `-Wl,-u,_<symbol>`
- keep compile failure fail closed with no:
  - `module.obj`
  - `module.object-backend.txt`
  - `module.runtime-metadata-linker-options.rsp`
  - `module.runtime-metadata-discovery.json`

## Code anchors

- `native/objc3c/src/lower/objc3_lowering_contract.h`
- `native/objc3c/src/lower/objc3_lowering_contract.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/io/objc3_process.h`
- `native/objc3c/src/io/objc3_process.cpp`
- `native/objc3c/src/io/objc3_manifest_artifacts.h`
- `native/objc3c/src/io/objc3_manifest_artifacts.cpp`
- `native/objc3c/src/driver/objc3_objc3_path.cpp`
- `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`

## Validation notes

- Positive proof compiles a metadata-only Objective-C 3 fixture to
  `module.obj`, packages it into one archive/library, links a trivial C `main`
  without the emitted response file, proves no runtime metadata sections are
  present in the linked image, then re-links with
  `module.runtime-metadata-linker-options.rsp` and proves the metadata survives
  dead stripping.
- On COFF/PE the linked-image proof is section-name-truncated, so the positive
  retained case verifies the PE output now contains `objc3.ru*` sections while
  the plain link does not.
- Negative proof uses
  `tests/tooling/fixtures/native/m252_b004_missing_interface_property.objc3`
  and must remain fail closed.

## Acceptance boundaries

- `M253-D002` is intentionally bounded to the single-library retention proof.
- Multi-archive fan-in and cross-translation-unit anchor merging remain deferred
  to `M253-D003`.

## Evidence

- `tmp/reports/m253/M253-D002/linker_retention_and_dead_strip_resistance_summary.json`
