# M254 Registrar Implementation and Image Walk Core Feature Implementation Expectations (D002)

Contract ID: `objc3c-runtime-bootstrap-registrar-image-walk/m254-d002-v1`

## Goal

Land the first live runtime registrar/image-walk path that consumes the emitted
registration table at startup and stages the discovered metadata roots for later
realization without widening the frozen D001 public runtime API.

## Canonical Surface

- Semantic surface path:
  `frontend.pipeline.semantic_surface.objc_runtime_bootstrap_registrar_contract`
- Private bootstrap header:
  `native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h`
- Private staging hook symbol:
  `objc3_runtime_stage_registration_table_for_bootstrap`
- Private image-walk snapshot symbol:
  `objc3_runtime_copy_image_walk_state_for_testing`
- Image-walk model:
  `registration-table-roots-validated-and-staged-before-realization`
- Discovery-root validation model:
  `linker-anchor-must-point-at-discovery-root`
- Selector-pool interning model:
  `canonical-selector-pool-preinterned-during-startup-image-walk`
- Realization staging model:
  `registration-table-roots-retained-for-later-realization`

## Acceptance

- The emitted init stub stages the registration table before calling the frozen
  D001 `objc3_runtime_register_image` entrypoint.
- The runtime validates and walks the staged registration table rather than
  trusting manifest-only descriptor counts.
- The runtime preinterns emitted selector-pool entries during the startup image
  walk.
- `module.runtime-registration-manifest.json` publishes the registrar/image-walk
  handoff fields.
- Deterministic evidence lands at
  `tmp/reports/m254/M254-D002/registrar_image_walk_summary.json`.
