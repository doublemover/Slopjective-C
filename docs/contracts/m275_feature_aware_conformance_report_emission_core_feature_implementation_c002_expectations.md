# M275 Feature-Aware Conformance Report Emission - Core Feature Implementation Expectations (C002)

Contract ID: `objc3c-part12-feature-aware-conformance-report-emission/m275-c002-v1`

## Required outcomes

- The frontend manifest publishes `frontend.pipeline.semantic_surface.objc_part12_feature_aware_conformance_report_emission`.
- The emitted versioned conformance report carries top-level `advanced_feature_reporting`.
- The emitted IR carries the matching Part 12 feature-aware report anchor.
- The advanced-feature payload stays bounded to the live Part 12 fix-it and migration surfaces.

## Dynamic proof

- Run the frontend on the positive fixture in canonical mode with migration assist enabled.
- Validate:
  - manifest packet readiness
  - top-level `advanced_feature_reporting` payload in the report artifact
  - `fixit_family_ids`
  - `canonical_mode_rejection_code = O3S216`
  - emitted IR Part 12 feature-aware report anchor
