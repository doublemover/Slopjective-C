# M315-C002 Expectations

Contract ID: `objc3c-cleanup-artifact-authenticity-schema/m315-c002-v1`

`M315-C002` freezes the authenticity schema and evidence contract for replay, report,
fixture, sample, policy, and archive artifacts.

The contract must:
- normalize the broad `M315-A003` inventory classes into a stable authenticity-class
  set that later implementation work can enforce;
- distinguish proof-eligible generated replay artifacts from bridge-only legacy replay
  artifacts that still lack full provenance;
- define the required authenticity envelope fields for JSON and `.ll` transport
  surfaces;
- make synthetic fixtures and examples explicitly non-proof-bearing;
- require a regeneration recipe and generator surface id before any artifact may claim
  genuine replay evidence.

The frozen class set must cover:
- `synthetic_fixture`
- `sample_or_example_artifact`
- `generated_report`
- `generated_replay`
- `legacy_generated_replay_bridge`
- `schema_policy_contract`
- `historical_archive`

This issue must hand off live regeneration and provenance capture to `M315-C003` and
synthetic fixture relocation and labeling updates to `M315-C004`.
