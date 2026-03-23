# M270-C001 Packet: Actor Lowering And Metadata Contract - Contract And Architecture Freeze

- Milestone: `M270`
- Lane: `C`
- Issue: `M270-C001`
- Contract ID: `objc3c-part7-actor-lowering-and-metadata-contract/m270-c001-v1`
- Surface path: `frontend.pipeline.semantic_surface.objc_part7_actor_lowering_and_metadata_contract`
- Dependencies:
  - `M270-A002`
  - `M270-B002`
  - `M270-B003`
- Scope:
  - lower the actor source/enforcement/hazard packets into one deterministic actor lowering handoff
  - publish actor metadata record counts, actor isolation-thunk counts, and actor hop-artifact counts
  - thread the replay-stable actor lowering metadata into emitted LLVM IR frontend metadata
- Deferred:
  - live actor thunk bodies
  - mailbox runtime entrypoints
  - runnable cross-actor scheduling
