# M254-C003 Registration Table Emission and Image-Local Initialization Core Feature Expansion Packet

Packet: `M254-C003`
Milestone: `M254`
Lane: `C`
Issue: `#7107`
Contract: `objc3c-runtime-registration-table-image-local-initialization/m254-c003-v1`

## Objective

Expand the lowering-owned bootstrap materialization path so the emitted registration table is self-describing and later runtime image-walk issues can ingest one deterministic per-image boundary without reconstructing section roots or image-local initialization state from ad hoc object inspection.

## Required implementation surface

- `native/objc3c/src/ir/objc3_ir_emitter.cpp`
- `native/objc3c/src/ir/objc3_ir_emitter.h`
- `native/objc3c/src/pipeline/objc3_frontend_types.h`
- `native/objc3c/src/pipeline/objc3_frontend_artifacts.cpp`
- `native/objc3c/src/driver/objc3_objc3_path.cpp`
- `native/objc3c/src/io/objc3_process.h`
- `native/objc3c/src/io/objc3_process.cpp`
- `native/objc3c/src/libobjc3c_frontend/frontend_anchor.cpp`

## Acceptance checklist

- Emit one self-describing registration table with ABI version and pointer-field count.
- Emit one derived image-local init-state cell and retain it through the live bootstrap path.
- Guard the generated init stub with the image-local init-state cell before runtime registration.
- Preserve manifest authority and publish the exact derived image-local init-state symbol.
- Add fail-closed checker/test coverage for both the C002 metadata fixture and a category-bearing metadata-only library.
- Land stable evidence under `tmp/reports/m254/M254-C003/`.
