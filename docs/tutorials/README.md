# Tutorials And Migration Guides

This directory is the live learning-path and migration boundary.

Use it for reader-facing material that teaches someone how to:

- get productive with the repo,
- understand the current runnable subset,
- map Objective-C 2 habits onto Objective-C 3 surfaces,
- compare Objective-C 3 choices against Swift and C++ interop expectations,
- and follow the checked-in showcase examples without reading internal implementation notes first.

## Learning Paths

- evaluator path:
  - start in `README.md`
  - verify the repo with `Fresh Setup` and `First Working Session`
  - then use `showcase/README.md` and `showcase/`
- tutorial path:
  - start with `docs/tutorials/getting_started.md`
  - then use `showcase/README.md` plus the checked-in example sources in `showcase/`
- migration path:
  - use this directory for ObjC2-to-ObjC3 and Swift/C++ comparison guides
  - keep examples compile-coupled to the live toolchain and showcase sources
  - start with `docs/tutorials/objc2_to_objc3_migration.md`
  - then use `docs/tutorials/objc2_swift_cpp_comparison.md` for broader comparison framing
- contributor path:
  - use `CONTRIBUTING.md` for repo-change instructions
  - use `docs/runbooks/objc3c_maintainer_workflows.md` only for maintainer-only workflow maps

## Canonical Inputs

The live teaching surface is intentionally narrow.

- onboarding and setup:
  - `README.md`
  - `site/src/index.body.md`
- tutorial and migration narrative:
  - `docs/tutorials/`
  - `docs/tutorials/getting_started.md`
  - `docs/tutorials/objc2_to_objc3_migration.md`
  - `docs/tutorials/objc2_swift_cpp_comparison.md`
- runnable walkthrough examples:
  - `showcase/README.md`
  - `showcase/portfolio.json`
  - `showcase/auroraBoard/main.objc3`
  - `showcase/signalMesh/main.objc3`
  - `showcase/patchKit/main.objc3`
- exact command appendix:
  - `docs/runbooks/objc3c_public_command_surface.md`

## Exact Live Paths For Downstream Work

Later issues in this milestone should edit these live paths directly:

- tutorial map and audience routing:
  - `docs/tutorials/README.md`
  - `docs/tutorials/getting_started.md`
  - `docs/tutorials/objc2_to_objc3_migration.md`
  - `docs/tutorials/objc2_swift_cpp_comparison.md`
  - `README.md`
  - `site/src/index.body.md`
- walkthrough and runnable teaching material:
  - `showcase/README.md`
  - `showcase/portfolio.json`
  - `showcase/*`
- maintainer boundary notes:
  - `docs/runbooks/objc3c_maintainer_workflows.md`
- documentation guardrails:
  - `scripts/check_documentation_surface.py`

## Explicit Non-Goals

- no milestone-local tutorial wrappers or sidecar indexes
- no machine-owned paths such as `tmp/` or `artifacts/` treated as tutorials
- no screenshots or slide-deck-only teaching material with no checked-in source
- no historical redirect material used as the primary learning path
- no separate command aliases when the public package-script surface already exists
