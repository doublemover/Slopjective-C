# M315-C003 Expectations

Contract ID: `objc3c-cleanup-genuine-replay-regeneration-and-provenance-capture/m315-c003-v1`

`M315-C003` implements the first truthful provenance bridge for replay artifacts.

This issue must:
- generate at least one fresh native replay artifact through the real frontend runner;
- capture a schema-compliant authenticity envelope for that fresh replay artifact;
- generate a bridge registry for the legacy committed replay corpus so those files are
  marked migration-only rather than treated as genuine proof;
- derive a concrete validation recipe for each legacy replay family from the existing
  validation test layout;
- preserve the `M315-C002` rule that only `generated_replay` with a regeneration
  recipe may claim genuine replay proof.

Expected closeout shape:
- one live replay capture under `tmp/reports/m315/M315-C003/live_replay_positive/`
- one legacy replay bridge registry under `tmp/reports/m315/M315-C003/`
- deterministic counts over the legacy replay corpus:
  - `76` legacy replay artifacts total
  - `30` with the generic frontend IR header
  - `46` still missing that header

This issue hands synthetic fixture relocation and labeling updates to `M315-C004`.
