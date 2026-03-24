# M275-B002 Packet: Feature-specific fix-it synthesis - Core feature implementation

## Scope

- Publish one deterministic implementation packet over the currently live Part 12 fix-it families.
- Keep the packet truthful to current implementation status rather than implying generalized automated rewrites.

## Surface

- `frontend.pipeline.semantic_surface.objc_part12_feature_specific_fixit_synthesis`

## Dependencies

- `M275-B001` diagnostic taxonomy and portability contract
- live migration-canonicalization packet from `M275-A002`
- live ownership ARC fix-it baseline already exported through semantic handoff

## Required proof

- packet contract id is `objc3c-part12-feature-specific-fixit-synthesis/m275-b002-v1`
- packet exposes the two current fix-it families only
- migration and migrator counts stay aligned on the positive fixture
- ownership ARC availability is exported truthfully without inventing candidates

## Next issue

- `M275-B003` legacy-to-canonical migration semantics - Edge-case and compatibility completion
