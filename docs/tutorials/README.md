# Tutorials And Migration Guides

This directory is the live learning-path and migration boundary.

Use it for reader-facing material that teaches someone how to:

- get productive with the repo,
- understand the current runnable subset,
- map Objective-C 2 habits onto Objective-C 3 surfaces,
- compare Objective-C 3 choices against Swift and C++ interop expectations,
- and follow the checked-in showcase examples without reading internal implementation notes first.

## Start Here By Goal

Choose the shortest route that matches the question you actually have:

| If you want to... | Start here | Runnable proof to keep open |
| --- | --- | --- |
| prove the toolchain is alive | `docs/tutorials/getting_started.md` | `showcase/auroraBoard/main.objc3` |
| understand the full example map | `showcase/README.md` | `showcase/portfolio.json` |
| migrate familiar ObjC2 habits | `docs/tutorials/objc2_to_objc3_migration.md` | `showcase/auroraBoard/main.objc3` |
| compare ObjC3 against Swift or C++ expectations | `docs/tutorials/objc2_swift_cpp_comparison.md` | `showcase/signalMesh/main.objc3` and `showcase/patchKit/main.objc3` |
| follow the exact compile run verify flow | `docs/tutorials/build_run_verify.md` | `showcase/tutorial_walkthrough.json` |

## Capability-Backed Routes

Use the example that best matches the capability you care about:

- `auroraBoard`
  - use it for categories, reflection, and synthesized behaviors
  - pair it with `docs/tutorials/getting_started.md`
  - pair it with `docs/tutorials/objc2_to_objc3_migration.md` when the question starts from ObjC2 habits
- `signalMesh`
  - use it for actors, status bridging, and runtime messaging
  - pair it with `docs/tutorials/guided_walkthrough.md`
  - pair it with `docs/tutorials/objc2_swift_cpp_comparison.md` when the question is "what does this look like relative to Swift-style concurrency expectations?"
- `patchKit`
  - use it for derives, macros, property behaviors, and interop
  - pair it with `docs/tutorials/objc2_swift_cpp_comparison.md`
  - use it when the question is about feature shape rather than the smallest possible first compile

## Learning Paths

- evaluator path:
  - start in `README.md`
  - verify the repo with `Fresh Setup` and `First Working Session`
  - then use `showcase/README.md` and `showcase/`
- tutorial path:
  - start with `docs/tutorials/getting_started.md`
  - use `docs/tutorials/build_run_verify.md` for the exact command and artifact flow
  - use `docs/tutorials/guided_walkthrough.md` for the ordered example pass
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
  - `docs/tutorials/build_run_verify.md`
  - `docs/tutorials/guided_walkthrough.md`
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
  - `docs/tutorials/build_run_verify.md`
  - `docs/tutorials/guided_walkthrough.md`
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

Reader-facing downstream work should keep one rule in mind: route from a user
question to a checked-in example first, then to the comparison or migration
text that explains that example.

## Explicit Non-Goals

- no milestone-local tutorial wrappers or sidecar indexes
- no machine-owned paths such as `tmp/` or `artifacts/` treated as tutorials
- no screenshots or slide-deck-only teaching material with no checked-in source
- no historical redirect material used as the primary learning path
- no separate command aliases when the public package-script surface already exists
