# M275 Feature-Specific Fix-It Synthesis - Core Feature Implementation Expectations (B002)

Contract ID: `objc3c-part12-feature-specific-fixit-synthesis/m275-b002-v1`

## Required outcomes

- The frontend manifest publishes `frontend.pipeline.semantic_surface.objc_part12_feature_specific_fixit_synthesis`.
- The packet truthfully freezes the currently live fix-it family set to:
  - migration canonicalization for legacy `YES` / `NO` / `NULL`
  - ownership ARC fix-it availability exported from the live semantic baseline
- The packet depends on `M275-B001` rather than inventing a new semantic source of truth.
- The positive fixture proves deterministic handoff with no emitted diagnostics.

## Dynamic proof

- Run the frontend C API runner against the positive fixture with:
  - `--objc3-compat-mode legacy`
  - `--objc3-migration-assist`
  - `--no-emit-ir`
  - `--no-emit-object`
- The emitted manifest packet must report:
  - `fixit_family_count = 2`
  - `migration_fixit_candidate_sites = 3`
  - `migrator_candidate_sites = 3`
  - `diagnostic_taxonomy_ready = true`
  - `deterministic_handoff = true`
  - `ready_for_lowering_and_runtime = true`
