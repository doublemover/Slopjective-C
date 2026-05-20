# objc3c Adoption And Legibility

## Working Boundary

This runbook defines the adoption, canonical conversion,
capability-comparison, onboarding, and evaluator-legibility boundary for
objc3c.

Use it when changing:

- README, site, tutorial, showcase, or canonicalization-guide entry points
- canonical conversion guidance from Objective-C 2, Swift, C++, and
  package-based projects
- capability-comparison wording against the support matrix and conformance data
- evaluator-facing public command discovery
- onboarding paths that depend on templates, package locks, canonical apps, or
  long-horizon support claims

Canonical checked-in boundary surfaces:

- `tests/tooling/fixtures/adoption_legibility/boundary_inventory.json`
- `README.md`
- `docs/tutorials/getting_started.md`
- canonicalization guide at `docs/tutorials/objc2_to_objc3_migration.md`
- `docs/tutorials/objc2_swift_cpp_comparison.md`
- `tests/tooling/fixtures/adoption_legibility/migration_analyzer_contract.json`
- `showcase/README.md`
- `site/index.md`
- `docs/runbooks/objc3c_public_command_surface.md`
- `docs/runbooks/objc3c_public_conformance_reporting.md`
- `docs/runbooks/objc3c_performance_governance.md`
- `docs/runbooks/objc3c_package_ecosystem.md`
- `docs/runbooks/objc3c_long_horizon_operations.md`

Replayable boundary inventory:

- covered by `npm run objc3c -- validate-adoption-legibility`

Helper implementations for this runbook are action-catalog-owned. Script paths
are not operator commands and are not part of the public adoption surface.

## Public Adoption And Conversion Claim Policy

The canonical public-claim policy is checked in at:

- `tests/tooling/fixtures/adoption_legibility/public_claim_policy.json`

Replay it with:

- `npm run objc3c -- validate-adoption-legibility`

Public adoption and conversion claims are intentionally narrower than the
available implementation surface. Any claim that describes production fit,
conversion safety, ecosystem maturity, performance, conformance, or support must
name its evidence class and must stay inside the support class published by the
release, package, conformance, performance, and long-horizon runbooks.
Conversion wording is evidence and onboarding prose only; it does not create
retired-source acceptance or alternate old-surface support.

Adoption copy may say that objc3c has a runnable evaluator path only when it
points to checked-in README, site, tutorial, showcase, public-command,
package, and support evidence. Conversion copy may describe incremental
Objective-C 2 conversion only when it stays same-major scoped and names package
locks, support windows, revert guidance, and deferred unsupported runtime
behavior.

The policy fails closed on:

- unsupported source-acceptance or performance-leadership claims
- cross-major conversion safety without generated conversion evidence
- package-manager, hosted registry, or IDE marketplace parity claims
- tutorial-only conversion guidance that bypasses support-window evidence
- adoption metrics that are hand-maintained instead of generated

## Capability Narrative And Comparison Semantics

The canonical comparison semantics contract is checked in at:

- `tests/tooling/fixtures/adoption_legibility/capability_comparison_semantics.json`

Replay it with:

- `npm run objc3c -- validate-adoption-legibility`

Comparison language must answer a concrete evaluator question, name the
Objective-C 2, Swift, or C++ comparison axis, then link to runnable examples and
evidence. It must not claim parity, superiority, or conversion safety unless the
claim is backed by conformance, performance, interop, package, release, and
support evidence. Unsupported or intentionally deferred behavior must remain
visible in the comparison text.

## Canonical Adoption Replay And Interop Guidance

The canonical adoption replay semantics contract is checked in at:

- `tests/tooling/fixtures/adoption_legibility/adoption_replay_semantics.json`

Replay it with:

- `npm run objc3c -- validate-adoption-legibility`

The public contract is canonical conversion over checked-in runnable examples
and support evidence. Adoption replay has four ordered phases: orient on public docs, compile the
showcase anchors, create or validate package/application workspace state, then
check support and revert evidence. Interop guidance is part of that path; it
must name the runnable example and runbook that prove the current Objective-C 2,
Swift-facing, or C++-facing boundary.

## Migration Analyzer And Rewrite Workflow

The canonical migration analyzer contract is checked in at:

- `tests/tooling/fixtures/adoption_legibility/migration_analyzer_contract.json`

Replay it with:

- `npm run objc3c -- validate-migration-workflow`

Public analyzer and rewrite entry points:

- `npm run objc3c -- analyze-migration-source <input.json>`
- `npm run objc3c -- rewrite-migration-source <input.json>`

Migration inputs are checked-in JSON contracts that name a source file,
target profile, required interop surfaces, Swift and C++ foreign interfaces,
and packaged execution evidence. The analyzer fails closed when required
surfaces are missing, source or package paths are not repository-owned, foreign
interfaces are incomplete, delimiters are malformed, or unsafe mixed-image
loading markers appear.

Rewrite output is split into safe automatic edits and manual migration steps.
Automatic edits are limited to token-boundary and import-line rewrites. Manual
steps are report surfaces only; they do not claim retired Objective-C 2 source
acceptance, Swift parity, C++ ABI parity, or alternate old-surface support.

## Artifact Contract

The canonical adoption and legibility artifact contract is checked in at:

- `tests/tooling/fixtures/adoption_legibility/artifact_contract.json`

Schema surface:

- `schemas/objc3c-adoption-legibility-evidence-v1.schema.json`
- registry owner: `scripts/objc3c_shared/schema_registry.py`

Replay it with:

- `npm run objc3c -- validate-adoption-legibility`

Generated machine-owned outputs stay in adoption-legibility artifact and report
roots selected by the public workflow.

No evaluator, conversion, comparison, onboarding, or adoption claim is canonical
unless it can be regenerated from checked-in contracts and validated through the
schema above.

## Evidence Generation

The checked-in evidence workflow is:

- `npm run objc3c -- validate-adoption-legibility`

It writes transient adoption-legibility outputs for the evidence artifact,
evaluator publication, and evidence summary.

The generator replays the boundary inventory, public claim policy, comparison
semantics, adoption replay semantics, and artifact contract summaries before
writing artifacts. Generated outputs are temporary; checked-in contracts,
docs, and action-catalog-owned implementations remain the owner inputs.

## Public Workflow Integration

The repo-scope adoption workflow is:

- `npm run objc3c -- validate-adoption-legibility`
- `npm run objc3c -- publish-adoption-legibility`

It maps to:

- `npm run objc3c -- validate-adoption-legibility`
- `npm run objc3c -- publish-adoption-legibility`

The public workflow validates the generated-output artifact shape, evaluator
entrypoints, adoption replay phases, comparison axes, onboarding workspaces, support
state, and claim-audit blockers.

Evaluator metadata publication emits transient publication artifacts and
publication summaries selected by the checked-in adoption contract.

## Current Boundary

Adoption work starts from existing public surfaces rather than inventing a new
marketing layer. The repo already has evaluator-facing docs, tutorials,
showcase projects, conformance reporting, package workflows, release operations,
performance governance, and long-horizon support evidence. This boundary
connects those surfaces into a coherent path for people deciding whether to
evaluate, convert code toward, or build with objc3c.

## Claim Boundary

Supported in this boundary:

- evaluator entry points that name the exact commands and docs to read first
- conversion guidance grounded in checked-in tutorials, package workflows, and
  runnable canonical application surfaces
- comparison wording tied to public conformance, performance, package, release,
  and support evidence
- onboarding paths that can be replayed through package, template, showcase, and
  public workflow commands
- generated adoption outputs selected by the checked-in adoption contracts

Not supported in this boundary:

- unsupported claims about source acceptance, performance leadership, or
  ecosystem maturity
- hand-maintained adoption counts without replayable queries
- tutorial-only conversion claims that bypass package locks or support-window
  evidence
- hosted service, registry, IDE marketplace, or community-program commitments
- private maintainer context as a prerequisite for external evaluation

## Substrate

This boundary consumes:

- public command discovery from `npm run objc3c -- build-public-command-surface`
- package workflow evidence from `docs/runbooks/objc3c_package_ecosystem.md`
- release and support-window evidence from `docs/runbooks/objc3c_release_operations.md`
- conformance scorecard evidence from
  `docs/runbooks/objc3c_public_conformance_reporting.md`
- performance claim governance from `docs/runbooks/objc3c_performance_governance.md`
- long-horizon support evidence from `docs/runbooks/objc3c_long_horizon_operations.md`
- canonical application replay from
  `docs/runbooks/objc3c_application_architecture_testing.md`

## Successor Pressure

This boundary feeds governance and ecosystem sustainability. Later claims about
extensions, ecosystem lifecycle, adoption readiness, or production stability must
consume generated adoption evidence rather than restating broad intent.
