# objc3c Application Architecture And Testing

## Working Boundary

This runbook defines the live boundary for first-party testing, project
templates, and canonical application architecture.

Use it when changing:

- first-party test harness expectations and fixture layout
- project-template and workspace structure
- canonical medium-sized application guidance
- runnable template/app validation that reuses the existing public workflow

Canonical checked-in boundary surfaces:

- `tests/tooling/fixtures/application_architecture_testing/boundary_inventory.json`
- `showcase/portfolio.json`
- `stdlib/workspace.json`
- `scripts/materialize_objc3c_project_template.py`
- `scripts/materialize_objc3c_stdlib_workspace.py`

Replayable boundary inventory:

- `python scripts/build_application_architecture_testing_boundary_inventory_summary.py`
- `python scripts/build_application_architecture_testing_semantic_summary.py`
- `python scripts/build_application_architecture_template_workspace_summary.py`
- `python scripts/build_application_architecture_layering_summary.py`
- `python scripts/build_application_architecture_artifact_contract_summary.py`
- `python scripts/check_application_architecture_template_harness.py`

## Current Boundary

The current checked-in boundary is narrower than a full application framework.

- real structured workspaces already exist in:
  - `showcase/*/workspace.json`
  - `stdlib/workspace.json`
- machine-owned template/workspace materialization already exists in:
  - `scripts/materialize_objc3c_project_template.py`
  - `scripts/materialize_objc3c_stdlib_workspace.py`
- current runnable examples are still centered on:
  - showcase examples
  - stdlib program surfaces
  - developer-tooling workspace drills

That means this area starts from real runnable surfaces, but it still needs:

- first-party testing semantics that people can copy into real projects
- one canonical project-template/workspace structure
- one canonical medium-sized application layering model
- artifact contracts and runnable evidence that tie those surfaces back to the
  existing package/install/public-workflow path

## Canonical Semantics

Authoritative checked-in contracts for this milestone live under:

- `tests/tooling/fixtures/application_architecture_testing/`

The live meaning is:

- first-party testing reuses the existing public workflow runner and checked-in
  showcase, stdlib, developer-tooling, and documentation surfaces
- project templates are derived from checked-in showcase sources and are only
  claimable when they remain traceable through public workflow actions
- canonical application architecture is not a second example taxonomy; it is a
  layering model that unifies the checked-in showcase and stdlib surfaces into a
  copyable project shape
- machine-owned evidence for this milestone publishes under one shared report
  root and one shared schema/contract pair

## Non-Goals

- no second example tree outside `showcase/` and `stdlib/`
- no hosted package-manager or registry design in this milestone
- no screenshot-only demo architecture with no runnable workspace evidence
- no second package/install workflow outside the existing runnable toolchain bundle
