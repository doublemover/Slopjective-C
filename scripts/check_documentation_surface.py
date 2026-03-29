#!/usr/bin/env python3
"""Validate the live reader-facing documentation surface and machine-appendix boundary."""

from __future__ import annotations

import sys
from pathlib import Path


ROOT = Path(__file__).resolve().parents[1]

README_PATH = ROOT / "README.md"
CONTRIBUTING_PATH = ROOT / "CONTRIBUTING.md"
SHOWCASE_README_PATH = ROOT / "showcase" / "README.md"
SITE_BODY_PATH = ROOT / "site" / "src" / "index.body.md"
SITE_POLICY_PATH = ROOT / "site" / "src" / "README.md"
NATIVE_OWNERSHIP_PATH = ROOT / "docs" / "objc3c-native" / "src" / "OWNERSHIP.md"
NATIVE_FRAGMENT_README_PATH = ROOT / "docs" / "objc3c-native" / "src" / "README.md"
MAINTAINER_WORKFLOW_PATH = ROOT / "docs" / "runbooks" / "objc3c_maintainer_workflows.md"
DEVELOPER_TOOLING_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_developer_tooling.md"
BONUS_EXPERIENCES_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_bonus_experiences.md"
PUBLIC_COMMAND_SURFACE_PATH = ROOT / "docs" / "runbooks" / "objc3c_public_command_surface.md"
TUTORIAL_README_PATH = ROOT / "docs" / "tutorials" / "README.md"
GETTING_STARTED_PATH = ROOT / "docs" / "tutorials" / "getting_started.md"
BUILD_RUN_VERIFY_PATH = ROOT / "docs" / "tutorials" / "build_run_verify.md"
GUIDED_WALKTHROUGH_PATH = ROOT / "docs" / "tutorials" / "guided_walkthrough.md"
MIGRATION_GUIDE_PATH = ROOT / "docs" / "tutorials" / "objc2_to_objc3_migration.md"
COMPARISON_README_PATH = ROOT / "docs" / "tutorials" / "objc2_swift_cpp_comparison.md"


def require_token(text: str, token: str, *, path: Path, errors: list[str]) -> None:
    if token not in text:
        errors.append(f"{path.relative_to(ROOT).as_posix()}: missing required token {token!r}")


def main() -> int:
    errors: list[str] = []

    readme = README_PATH.read_text(encoding="utf-8")
    for token in (
        "## Start Here",
        "## Fresh Setup",
        "## First Working Session",
        "## Public Command Surface",
        "## Spec Structure",
        "## Superclean Boundary",
        "published site",
        "CONTRIBUTING.md",
        "docs/tutorials/",
        "showcase/README.md",
        "docs/runbooks/objc3c_public_command_surface.md",
        "Canonical roots:",
        "Explicit non-goals for cleanup work:",
    ):
        require_token(readme, token, path=README_PATH, errors=errors)

    contributing = CONTRIBUTING_PATH.read_text(encoding="utf-8")
    for token in (
        "## Contributor Surface",
        "## Repo Boundary",
        "README.md",
        "CONTRIBUTING.md",
        "docs/tutorials/README.md",
        "docs/tutorials/getting_started.md",
        "docs/tutorials/build_run_verify.md",
        "docs/tutorials/guided_walkthrough.md",
        "docs/tutorials/objc2_to_objc3_migration.md",
        "docs/tutorials/objc2_swift_cpp_comparison.md",
        "docs/runbooks/objc3c_public_command_surface.md",
        "docs/runbooks/objc3c_maintainer_workflows.md",
        "showcase/",
        "native/objc3c/",
        "scripts/",
        "tests/",
        "tmp/",
        "artifacts/",
        "npm run check:repo:surface",
    ):
        require_token(contributing, token, path=CONTRIBUTING_PATH, errors=errors)

    showcase_readme = SHOWCASE_README_PATH.read_text(encoding="utf-8")
    for token in (
        "# Showcase Examples",
        "## Portfolio Boundary",
        "showcase/portfolio.json",
        "showcase/auroraBoard/main.objc3",
        "showcase/signalMesh/main.objc3",
        "showcase/patchKit/main.objc3",
        "scripts/check_showcase_surface.py",
        "tmp/artifacts/showcase/",
        "## Explicit Non-Goals",
    ):
        require_token(showcase_readme, token, path=SHOWCASE_README_PATH, errors=errors)

    tutorial_readme = TUTORIAL_README_PATH.read_text(encoding="utf-8")
    for token in (
        "# Tutorials And Migration Guides",
        "## Learning Paths",
        "## Canonical Inputs",
        "docs/tutorials/",
        "showcase/README.md",
        "showcase/portfolio.json",
        "## Exact Live Paths For Downstream Work",
        "scripts/check_documentation_surface.py",
        "## Explicit Non-Goals",
    ):
        require_token(tutorial_readme, token, path=TUTORIAL_README_PATH, errors=errors)

    getting_started = GETTING_STARTED_PATH.read_text(encoding="utf-8")
    for token in (
        "# Getting Started With The Runnable Subset",
        "## Teaching Model",
        "## Step 1 Verify The Toolchain",
        "npm run build:objc3c-native",
        "npm run test:fast",
        "## Step 2 Compile One Runnable Example",
        "showcase/auroraBoard/main.objc3",
        "## Step 3 Use The Showcase Surface As The Tutorial Backbone",
        "npm run check:showcase:surface",
        "npm run test:showcase",
        "## Step 4 Choose The Next Learning Path",
        "docs/runbooks/objc3c_public_command_surface.md",
        "## Canonical Inputs",
        "## Exact Live Paths For Downstream Work",
        "scripts/check_documentation_surface.py",
        "## Explicit Non-Goals",
    ):
        require_token(getting_started, token, path=GETTING_STARTED_PATH, errors=errors)

    build_run_verify = BUILD_RUN_VERIFY_PATH.read_text(encoding="utf-8")
    for token in (
        "# Tutorial Build Run And Verify Surface",
        "## Workflow Boundary",
        "## Build",
        "npm run build:objc3c-native",
        "## Run The First Compile",
        "npm run compile:objc3c -- showcase/auroraBoard/main.objc3",
        "## Verify The Portfolio Surface",
        "npm run check:showcase:surface",
        "npm run test:showcase",
        "npm run test:showcase:e2e",
        "## Validation Surface",
        "scripts/check_getting_started_surface.py",
        "scripts/check_getting_started_integration.py",
        "npm run test:getting-started",
        "## Artifact And Report Expectations",
        "tmp/artifacts/showcase/<example-id>/",
        "tmp/reports/showcase/",
        "## Canonical Inputs",
        "## Exact Live Paths For Downstream Work",
        "scripts/objc3c_public_workflow_runner.py",
        "## Explicit Non-Goals",
    ):
        require_token(build_run_verify, token, path=BUILD_RUN_VERIFY_PATH, errors=errors)

    guided_walkthrough = GUIDED_WALKTHROUGH_PATH.read_text(encoding="utf-8")
    for token in (
        "# Guided Showcase Walkthrough",
        "## Walkthrough Boundary",
        "## Walkthrough Steps",
        "npm run build:objc3c-native",
        "npm run compile:objc3c -- showcase/auroraBoard/main.objc3",
        "npm run compile:objc3c -- showcase/signalMesh/main.objc3",
        "npm run compile:objc3c -- showcase/patchKit/main.objc3",
        "npm run check:showcase:surface",
        "npm run test:showcase",
        "## Walkthrough Assets",
        "showcase/tutorial_walkthrough.json",
        "scripts/check_getting_started_surface.py",
        "scripts/check_getting_started_integration.py",
        "npm run test:getting-started",
        "## Canonical Inputs",
        "## Exact Live Paths For Downstream Work",
        "scripts/check_showcase_surface.py",
        "## Explicit Non-Goals",
    ):
        require_token(guided_walkthrough, token, path=GUIDED_WALKTHROUGH_PATH, errors=errors)

    migration_guide = MIGRATION_GUIDE_PATH.read_text(encoding="utf-8")
    for token in (
        "# ObjC2 To ObjC3 Migration Guide",
        "## Migration Boundary",
        "## Step 1 Keep The Familiar ObjC Shape, Drop The Implicit Assumptions",
        "showcase/auroraBoard/main.objc3",
        "## Step 3 Treat Swift-Facing Async And Imported Hooks As Explicit Interop Contracts",
        "showcase/signalMesh/main.objc3",
        "showcase/patchKit/main.objc3",
        "npm run check:showcase:surface",
        "npm run test:showcase",
        "## Recommended Reading Order",
        "## Canonical Inputs",
        "## Exact Live Paths For Downstream Work",
        "scripts/check_documentation_surface.py",
        "## Explicit Non-Goals",
    ):
        require_token(migration_guide, token, path=MIGRATION_GUIDE_PATH, errors=errors)

    comparison_readme = COMPARISON_README_PATH.read_text(encoding="utf-8")
    for token in (
        "# ObjC2 Swift And C++ Comparison Surface",
        "## Comparison Boundary",
        "## Canonical Inputs",
        "showcase/patchKit/main.objc3",
        "showcase/signalMesh/main.objc3",
        "## Exact Live Paths For Downstream Work",
        "scripts/check_documentation_surface.py",
        "## Explicit Non-Goals",
    ):
        require_token(comparison_readme, token, path=COMPARISON_README_PATH, errors=errors)

    site_body = SITE_BODY_PATH.read_text(encoding="utf-8")
    for token in (
        "## At a Glance {#toc-status-scope-note}",
        "## Quick Routes {#toc-quick-routes}",
        "## Reader Promises {#toc-reader-promises}",
        "## Specification Map {#toc-front-matter}",
        "## Language Parts {#toc-parts}",
        "[README.md](../README.md)",
        "[docs/tutorials/README.md](../docs/tutorials/README.md)",
        "[docs/tutorials/getting_started.md](../docs/tutorials/getting_started.md)",
        "[docs/tutorials/build_run_verify.md](../docs/tutorials/build_run_verify.md)",
        "[docs/tutorials/guided_walkthrough.md](../docs/tutorials/guided_walkthrough.md)",
        "[docs/tutorials/objc2_to_objc3_migration.md](../docs/tutorials/objc2_to_objc3_migration.md)",
        "[docs/tutorials/objc2_swift_cpp_comparison.md](../docs/tutorials/objc2_swift_cpp_comparison.md)",
        "[docs/objc3c-native.md](../docs/objc3c-native.md)",
        "[legacy spec redirects](../docs/reference/legacy_spec_anchor_index.md#legacy-files)",
    ):
        require_token(site_body, token, path=SITE_BODY_PATH, errors=errors)

    site_policy = SITE_POLICY_PATH.read_text(encoding="utf-8")
    require_token(site_policy, "## Tone and Accessibility Rules", path=SITE_POLICY_PATH, errors=errors)

    native_ownership = NATIVE_OWNERSHIP_PATH.read_text(encoding="utf-8")
    require_token(
        native_ownership,
        "## Public Doc Style And Accessibility Rules",
        path=NATIVE_OWNERSHIP_PATH,
        errors=errors,
    )

    native_fragment_readme = NATIVE_FRAGMENT_README_PATH.read_text(encoding="utf-8")
    for token in (
        "## Canonical Naming And Path Rules",
        "user-facing package entrypoints come from `package.json`",
        "transient outputs stay under `tmp/`",
        "published binaries and libraries stay under `artifacts/`",
    ):
        require_token(
            native_fragment_readme,
            token,
            path=NATIVE_FRAGMENT_README_PATH,
            errors=errors,
        )

    maintainer_workflow = MAINTAINER_WORKFLOW_PATH.read_text(encoding="utf-8")
    for token in (
        "## Superclean Working Boundary",
        "implementation roots:",
        "generated checked-in outputs:",
        "docs/tutorials/",
        "CONTRIBUTING.md",
        "showcase/",
        "docs/runbooks/objc3c_developer_tooling.md",
        "docs/runbooks/objc3c_bonus_experiences.md",
        "Do not add milestone-specific wrappers, sidecar compatibility files, or",
    ):
        require_token(
            maintainer_workflow,
            token,
            path=MAINTAINER_WORKFLOW_PATH,
            errors=errors,
        )

    developer_tooling_runbook = DEVELOPER_TOOLING_RUNBOOK_PATH.read_text(encoding="utf-8")
    for token in (
        "# objc3c Developer Tooling Boundary",
        "## Working Boundary",
        "## Exact Live Implementation Paths",
        "native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp",
        "native/objc3c/src/io/objc3_cli_reporting_output_contract_scaffold.h",
        "scripts/build_objc3c_native.ps1",
        "scripts/objc3c_public_workflow_runner.py",
        "native/objc3c/src/runtime/objc3_runtime.cpp",
        "scripts/check_objc3c_library_cli_parity.py",
        "scripts/check_objc3c_runtime_acceptance.py",
        "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        "## Explainability And Introspection Surface",
        "## Exact Live Artifact And Output Paths",
        "artifacts/bin/objc3c-frontend-c-api-runner.exe",
        "tmp/artifacts/objc3c-native/repo_superclean_source_of_truth.json",
        "tmp/artifacts/compilation/objc3c-native/",
        "tmp/reports/objc3c-public-workflow/compile-observability.json",
        "tmp/reports/objc3c-public-workflow/runtime-inspector.json",
        "tmp/reports/objc3c-public-workflow/capability-explorer.json",
        "tmp/reports/objc3c-public-workflow/compile-stage-trace.json",
        "## Exact Live Commands",
        "npm run build:objc3c-native:contracts",
        "artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/hello.objc3",
        "python scripts/objc3c_public_workflow_runner.py inspect-compile-observability",
        "npm run inspect:objc3c:observability",
        "python scripts/objc3c_public_workflow_runner.py inspect-runtime-inspector",
        "npm run inspect:objc3c:runtime",
        "python scripts/objc3c_public_workflow_runner.py inspect-capability-explorer",
        "npm run inspect:objc3c:capabilities",
        "python scripts/objc3c_public_workflow_runner.py trace-compile-stages",
        "npm run trace:objc3c:stages",
        "python scripts/objc3c_public_workflow_runner.py validate-developer-tooling",
        "npm run test:objc3c:developer-tooling",
        "## Runtime Introspection Primitives",
        "objc3_runtime_copy_arc_debug_state_for_testing",
        "runtime_metadata_object_inspection_uses_llvm_objdump",
        "## Explicit Non-Goals",
    ):
        require_token(
            developer_tooling_runbook,
            token,
            path=DEVELOPER_TOOLING_RUNBOOK_PATH,
            errors=errors,
        )

    bonus_experiences_runbook = BONUS_EXPERIENCES_RUNBOOK_PATH.read_text(encoding="utf-8")
    for token in (
        "# objc3c Bonus Experiences Boundary",
        "## Working Boundary",
        "interactive playground flows",
        "visual runtime inspection and capability-explorer flows",
        "starter-template and project-generator flows",
        "## Current Truthful Portfolio",
        "scripts/objc3c_public_workflow_runner.py",
        "package.json",
        "native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp",
        "scripts/build_objc3c_native.ps1",
        "scripts/package_objc3c_runnable_toolchain.ps1",
        "showcase/portfolio.json",
        "showcase/auroraBoard/main.objc3",
        "docs/tutorials/getting_started.md",
        "docs/runbooks/objc3c_developer_tooling.md",
        "scripts/check_showcase_integration.py",
        "scripts/check_getting_started_integration.py",
        "## Exact Playground Inspector And Template Paths",
        "tests/tooling/fixtures/native/hello.objc3",
        "showcase/tutorial_walkthrough.json",
        "tests/tooling/runtime/arc_debug_instrumentation_probe.cpp",
        "## Exact Live Artifact And Output Paths",
        "artifacts/bin/objc3c-frontend-c-api-runner.exe",
        "tmp/artifacts/showcase/",
        "tmp/reports/developer-tooling/integration-summary.json",
        "python scripts/objc3c_public_workflow_runner.py inspect-capability-explorer",
        "npm run inspect:objc3c:capabilities",
        "tmp/reports/tutorials/",
        "## Exact Live Commands",
        "python scripts/objc3c_public_workflow_runner.py compile-objc3c showcase/auroraBoard/main.objc3",
        "npm run compile:objc3c -- showcase/auroraBoard/main.objc3",
        "python scripts/objc3c_public_workflow_runner.py validate-showcase",
        "python scripts/objc3c_public_workflow_runner.py validate-runnable-showcase",
        "python scripts/objc3c_public_workflow_runner.py validate-getting-started",
        "## Feasibility And Working Model",
        "## Working Rules For Downstream Issues",
        "## Explicit Non-Goals",
    ):
        require_token(
            bonus_experiences_runbook,
            token,
            path=BONUS_EXPERIENCES_RUNBOOK_PATH,
            errors=errors,
        )

    command_surface = PUBLIC_COMMAND_SURFACE_PATH.read_text(encoding="utf-8")
    for token in (
        "operator-facing appendix",
        "## Operator Notes",
        "Treat this file as a generated machine-facing appendix",
        "Canonical user-facing command names come from `package.json`",
        "`native/objc3c/`, `scripts/`, and `tests/` are the live implementation roots",
    ):
        require_token(
            command_surface,
            token,
            path=PUBLIC_COMMAND_SURFACE_PATH,
            errors=errors,
        )

    if errors:
        print("documentation-surface: FAIL", file=sys.stderr)
        for error in errors:
            print(f"- {error}", file=sys.stderr)
        return 1

    print("documentation-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
