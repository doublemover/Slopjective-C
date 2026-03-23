# M271 Borrowed-Pointer Escape Analysis Expectations (B003)

Contract ID: `objc3c-part8-borrowed-pointer-escape-analysis/m271-b003-v1`

## Required outcomes

- The semantic pipeline must publish one deterministic packet at `frontend.pipeline.semantic_surface.objc_part8_borrowed_pointer_escape_analysis`.
- The packet must consume the already-landed `M271-B002` sema packet instead of inventing another source boundary.
- Live sema must fail closed for:
  - borrowed pointers passed to call boundaries not proven non-escaping
  - borrowed pointers captured by escaping blocks
  - borrowed returns that do not name a valid owner parameter with `objc_returns_borrowed(owner_index=...)`
- The current truth boundary must remain explicit: retainable-family legality, lowering, and runtime behavior remain later `M271` work.
- The happy-path proof for this issue must run through `objc3c-frontend-c-api-runner.exe` with `--no-emit-ir --no-emit-object`.
- Validation evidence must land under `tmp/reports/m271/M271-B003/`.
