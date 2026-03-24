# M271 System Extension Lowering Contract And Architecture Freeze Expectations (C001)

Contract ID: `objc3c-part8-system-extension-lowering-contract/m271-c001-v1`

## Required Surface

- Publish `frontend.pipeline.semantic_surface.objc_part8_system_extension_lowering_contract` in emitted frontend manifests.
- Derive the lowering packet from the already-landed Part 8 semantic summaries:
  - `objc_part8_system_extension_semantic_model`
  - `objc_part8_resource_move_and_use_after_move_semantics`
  - `objc_part8_borrowed_pointer_escape_analysis`
  - `objc_part8_capture_list_and_retainable_family_legality_completion`
- Emit replay-stable Part 8 lowering metadata into LLVM IR frontend metadata.

## Truth Boundary

- This issue freezes lowering carriage and emitted metadata only.
- This issue does not claim:
  - live cleanup-runtime carriers
  - borrowed lifetime runtime interop
  - runnable retainable-family execution behavior

## Positive Proof

- A positive Part 8 fixture must compile through `objc3c-frontend-c-api-runner` and emit:
  - `module.manifest.json`
  - `module.ll`
- The manifest packet and IR metadata must agree on:
  - cleanup hook/resource counts
  - cleanup-owned/move-capture counts
  - borrowed boundary counts
  - explicit capture / retainable-family callable counts
  - zero guard-blocked and zero contract-violation counts on the happy path
