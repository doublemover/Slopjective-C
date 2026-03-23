# M271 System-Extension Semantic Model Contract And Architecture Freeze Expectations (B001)

Contract ID: `objc3c-part8-system-extension-semantic-model/m271-b001-v1`

## Required outcomes

- The semantic pipeline must publish one deterministic packet at `frontend.pipeline.semantic_surface.objc_part8_system_extension_semantic_model`.
- The packet must consume the already-landed `M271-A001`, `M271-A002`, and `M271-A003` frontend/source packets instead of inventing another source boundary.
- The packet must preserve deterministic counts for cleanup hooks, resource locals, borrowed pointer spellings, borrowed-return relations, explicit block capture lists, and retainable-family declaration metadata.
- The current claim must stay truthful: this issue freezes one sema/accounting boundary only.
- Resource move legality, borrowed escape legality, retainable-family legality, lowering, and runtime behavior must remain explicitly deferred to later `M271` work.
- The happy-path proof for this issue must run through `objc3c-frontend-c-api-runner.exe` with `--no-emit-ir --no-emit-object`.
- Validation evidence must land under `tmp/reports/m271/M271-B001/`.
