# M274 Part 11 Swift Metadata And Isolation Mapping Completion Expectations (B004)

Contract ID: `objc3c-part11-swift-metadata-and-isolation-mapping-completion/m274-b004-v1`

Issue: `#7366`

## Required outcomes

- the frontend manifest publishes `frontend.pipeline.semantic_surface.objc_part11_swift_metadata_and_isolation_mapping`
- Swift-facing metadata fails closed when `objc_swift_private` is not paired with `objc_swift_name(named("..."))`
- Swift-facing metadata fails closed on actor-owned callable surfaces, `objc_nonisolated` callable surfaces, and implementation surfaces
- diagnostics `O3S337`, `O3S338`, `O3S339`, and `O3S340` remain deterministic
- ABI lowering and runnable Swift-facing isolation export are not claimed here
- evidence lands at `tmp/reports/m274/M274-B004/part11_swift_metadata_and_isolation_mapping_summary.json`
