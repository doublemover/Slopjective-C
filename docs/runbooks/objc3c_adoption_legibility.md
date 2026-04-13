# objc3c Adoption And Legibility

## Working Boundary

This runbook defines the adoption, migration, capability-comparison, onboarding,
and evaluator-legibility boundary for objc3c.

Use it when changing:

- README, site, tutorial, showcase, or migration-guide entry points
- migration guidance from Objective-C 2, Swift, C++, and package-based projects
- capability-comparison wording against the support matrix and conformance data
- evaluator-facing public command discovery
- onboarding paths that depend on templates, package locks, canonical apps, or
  long-horizon support claims

Canonical checked-in boundary surfaces:

- `tests/tooling/fixtures/adoption_legibility/boundary_inventory.json`
- `README.md`
- `docs/tutorials/getting_started.md`
- `docs/tutorials/objc2_to_objc3_migration.md`
- `docs/tutorials/objc2_swift_cpp_comparison.md`
- `showcase/README.md`
- `site/index.md`
- `docs/runbooks/objc3c_public_command_surface.md`
- `docs/runbooks/objc3c_public_conformance_reporting.md`
- `docs/runbooks/objc3c_performance_governance.md`
- `docs/runbooks/objc3c_package_ecosystem.md`
- `docs/runbooks/objc3c_long_horizon_operations.md`

Replayable boundary inventory:

- `python scripts/build_adoption_legibility_boundary_inventory_summary.py`

## Current Boundary

Adoption work starts from existing public surfaces rather than inventing a new
marketing layer. The repo already has evaluator-facing docs, tutorials,
showcase projects, conformance reporting, package workflows, release operations,
performance governance, and long-horizon support evidence. This boundary
connects those surfaces into a coherent path for people deciding whether to
evaluate, migrate to, or build with objc3c.

## Claim Boundary

Supported in this boundary:

- evaluator entry points that name the exact commands and docs to read first
- migration guidance grounded in checked-in tutorials, package workflows, and
  runnable canonical application surfaces
- comparison wording tied to public conformance, performance, package, release,
  and support evidence
- onboarding paths that can be replayed through package, template, showcase, and
  public workflow commands
- generated adoption evidence under `tmp/reports/` and `tmp/artifacts/`

Not supported in this boundary:

- unsupported claims about source compatibility, performance leadership, or
  ecosystem maturity
- hand-maintained adoption counts without replayable queries
- tutorial-only migration claims that bypass package locks or compatibility
  support evidence
- hosted service, registry, IDE marketplace, or community-program commitments
- private maintainer context as a prerequisite for external evaluation

## Substrate

This boundary consumes:

- public command discovery from `scripts/render_objc3c_public_command_surface.py`
- package workflow evidence from `docs/runbooks/objc3c_package_ecosystem.md`
- release and compatibility evidence from `docs/runbooks/objc3c_release_operations.md`
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
