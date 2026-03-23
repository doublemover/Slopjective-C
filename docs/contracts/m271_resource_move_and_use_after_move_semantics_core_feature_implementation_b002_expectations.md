# M271 Resource Move And Use-After-Move Semantics Expectations (B002)

Contract ID: `objc3c-part8-resource-move-use-after-move-semantics/m271-b002-v1`

## Required outcomes

- The semantic pipeline must publish one deterministic packet at `frontend.pipeline.semantic_surface.objc_part8_resource_move_and_use_after_move_semantics`.
- The packet must consume the already-landed `M271-B001` sema packet instead of inventing another source boundary.
- Live sema must fail closed for:
  - explicit `move` capture of a non-cleanup, non-resource local
  - use of a cleanup/resource-backed local after cleanup ownership was transferred by `move`
  - duplicate `move` transfer of the same cleanup/resource-backed local
- The current truth boundary must remain explicit: borrowed escape legality, retainable-family legality, lowering, and runtime behavior remain later `M271` work.
- The happy-path proof for this issue must run through `objc3c-frontend-c-api-runner.exe` with `--no-emit-ir --no-emit-object`.
- Validation evidence must land under `tmp/reports/m271/M271-B002/`.
