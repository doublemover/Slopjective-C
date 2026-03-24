# M273-C001 Packet: Expansion and Lowering Contract - Contract and Architecture Freeze

- Issue: `M273-C001`
- Milestone: `M273`
- Lane: `C`
- Contract ID: `objc3c-part10-expansion-lowering-contract/m273-c001-v1`
- Surface path: `frontend.pipeline.semantic_surface.objc_part10_expansion_and_lowering_contract`

## Summary

This packet freezes the first truthful Part 10 lowering boundary.

## Lowering surface

- derived selector inventory rows become replay-visible lowering facts
- macro package/provenance-admitted callables become replay-visible lowering facts
- synthesized property binding/getter/setter visibility becomes replay-visible lowering facts
- the emitted IR carries the lowering replay key and one dedicated metadata node

## Deferred boundary

- no runnable derive method bodies are emitted yet
- no runnable macro execution is emitted yet
- no runnable property-behavior hooks are emitted yet
