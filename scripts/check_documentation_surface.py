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
PERFORMANCE_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_performance.md"
STDLIB_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_stdlib_foundation.md"
STDLIB_README_PATH = ROOT / "stdlib" / "README.md"
STDLIB_PROGRAM_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_stdlib_program.md"
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


def forbid_token(text: str, token: str, *, path: Path, errors: list[str]) -> None:
    if token in text:
        errors.append(f"{path.relative_to(ROOT).as_posix()}: forbidden token present {token!r}")


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
        "## Capability-First Entry Points",
        "showcase/portfolio.json",
        "showcase/auroraBoard/main.objc3",
        "showcase/signalMesh/main.objc3",
        "showcase/patchKit/main.objc3",
        "actor-shaped messaging",
        "stdlib/README.md",
        "objc3.core",
        "objc3.concurrency",
        "objc3.keypath",
        "objc3.system",
        "scripts/check_showcase_surface.py",
        "tmp/artifacts/showcase/",
        "## Explicit Non-Goals",
    ):
        require_token(showcase_readme, token, path=SHOWCASE_README_PATH, errors=errors)
    forbid_token(
        showcase_readme,
        "target story: status bridging, actors, runtime messaging",
        path=SHOWCASE_README_PATH,
        errors=errors,
    )
    forbid_token(
        showcase_readme,
        "--capability actors",
        path=SHOWCASE_README_PATH,
        errors=errors,
    )

    tutorial_readme = TUTORIAL_README_PATH.read_text(encoding="utf-8")
    for token in (
        "# Tutorials And Migration Guides",
        "## Learning Paths",
        "## Start Here By Goal",
        "## Capability-Backed Routes",
        "## Canonical Inputs",
        "docs/tutorials/",
        "showcase/README.md",
        "showcase/portfolio.json",
        "actor-shaped messaging",
        "stdlib/README.md",
        "objc3.core",
        "objc3.concurrency",
        "objc3.keypath",
        "objc3.system",
        "## Exact Live Paths For Downstream Work",
        "scripts/check_documentation_surface.py",
        "## Explicit Non-Goals",
    ):
        require_token(tutorial_readme, token, path=TUTORIAL_README_PATH, errors=errors)
    forbid_token(
        tutorial_readme,
        "use it for actors, status bridging, and runtime messaging",
        path=TUTORIAL_README_PATH,
        errors=errors,
    )

    getting_started = GETTING_STARTED_PATH.read_text(encoding="utf-8")
    for token in (
        "# Getting Started With The Runnable Subset",
        "## What This Tutorial Proves",
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
        "actor-shaped messaging",
        "stdlib/README.md",
        "objc3.concurrency",
        "objc3.system",
        "## Canonical Inputs",
        "## Exact Live Paths For Downstream Work",
        "scripts/check_documentation_surface.py",
        "## Explicit Non-Goals",
    ):
        require_token(getting_started, token, path=GETTING_STARTED_PATH, errors=errors)
    forbid_token(
        getting_started,
        "`signalMesh` for actors, status bridging, and runtime messaging",
        path=GETTING_STARTED_PATH,
        errors=errors,
    )

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
        "## Start With The Capability That Matches The Question",
        "## Comparison Boundary",
        "## Current Truthful Comparison Shape",
        "## Canonical Inputs",
        "showcase/patchKit/main.objc3",
        "showcase/signalMesh/main.objc3",
        "actor-shaped workflow",
        "stdlib/README.md",
        "objc3.concurrency",
        "objc3.system",
        "objc3.keypath",
        "## Exact Live Paths For Downstream Work",
        "scripts/check_documentation_surface.py",
        "## Explicit Non-Goals",
    ):
        require_token(comparison_readme, token, path=COMPARISON_README_PATH, errors=errors)

    stdlib_runbook = STDLIB_RUNBOOK_PATH.read_text(encoding="utf-8")
    for token in (
        "# objc3c Standard Library Foundation",
        "## Working boundary",
        "stdlib/",
        "tmp/artifacts/stdlib/",
        "tmp/reports/stdlib/",
        "## Non-goals",
        "## Expected end state",
        "## Lowering And Import Artifact Surface",
        "## Exact Live Implementation Paths",
        "scripts/check_objc3c_stdlib_foundation_integration.py",
        "scripts/check_objc3c_runnable_stdlib_foundation_end_to_end.py",
        "## Exact Live Artifact And Output Paths",
        "tmp/reports/stdlib/runnable-end-to-end-summary.json",
        "## Exact Live Commands",
        "python scripts/objc3c_public_workflow_runner.py validate-runnable-stdlib-foundation",
        "npm run package:objc3c-native:runnable-toolchain",
        "## Public actions",
        "npm run test:stdlib:e2e",
        "stdlib/core_architecture.json",
        "stdlib/advanced_architecture.json",
        "stdlib/lowering_import_surface.json",
        "stdlib/advanced_helper_package_surface.json",
        "stdlib/program_surface.json",
        "module.runtime-registration-manifest.json",
        "identifier-safe implementation module declarations",
        "docs/runbooks/objc3c_stdlib_core.md",
        "docs/runbooks/objc3c_stdlib_advanced.md",
        "docs/runbooks/objc3c_stdlib_program.md",
    ):
        require_token(stdlib_runbook, token, path=STDLIB_RUNBOOK_PATH, errors=errors)

    stdlib_readme = STDLIB_README_PATH.read_text(encoding="utf-8")
    for token in (
        "# objc3c Standard Library",
        "## Boundary",
        "stdlib/workspace.json",
        "stdlib/module_inventory.json",
        "stdlib/stability_policy.json",
        "stdlib/package_surface.json",
        "stdlib/core_architecture.json",
        "stdlib/advanced_architecture.json",
        "stdlib/semantic_policy.json",
        "stdlib/lowering_import_surface.json",
        "stdlib/advanced_helper_package_surface.json",
        "stdlib/program_surface.json",
        "docs/runbooks/objc3c_stdlib_program.md",
        "docs/tutorials/",
        "showcase/",
        "site/src/index.body.md",
        "docs/runbooks/objc3c_stdlib_advanced.md",
        "machine-owned lowering roots",
        "## Working model",
    ):
        require_token(stdlib_readme, token, path=STDLIB_README_PATH, errors=errors)

    stdlib_core_runbook_path = ROOT / "docs" / "runbooks" / "objc3c_stdlib_core.md"
    stdlib_core_runbook = stdlib_core_runbook_path.read_text(encoding="utf-8")
    for token in (
        "# objc3c Core Stdlib Surface",
        "## Working boundary",
        "stdlib/core_architecture.json",
        "stdlib/modules/objc3.core/",
        "stdlib/modules/objc3.errors/",
        "stdlib/modules/objc3.keypath/",
        "stdlib/semantic_policy.json",
        "## Core family split",
        "`objc3.core` owns:",
        "`objc3.errors` owns:",
        "`objc3.keypath` remains in scope",
        "## Expected shipped API families",
        "profile-revision",
        "result-shape",
        "typed-keypath-text-compatibility",
        "## Exact checked-in source surface",
        "objc3_core_string_view_length",
        "objc3_core_string_view_prefix_units",
        "objc3_errors_text_data_compatibility_score",
        "objc3_errors_result_unwrap_or",
        "objc3_errors_result_bridge_diagnostic",
        "objc3_keypath_text_compatibility_diagnostic",
        "objc3_keypath_text_compatibility_score",
        "## Semantic guarantees",
        "presence or result tag says it is valid",
        "stable mismatch code `30601`",
        "stable mismatch codes `30602` and `30603`",
        "result_err_tag` stays `2`",
        "module semver metadata stays `1.0.0`",
        "## Explicit non-goals",
    ):
        require_token(stdlib_core_runbook, token, path=stdlib_core_runbook_path, errors=errors)

    stdlib_advanced_runbook_path = ROOT / "docs" / "runbooks" / "objc3c_stdlib_advanced.md"
    stdlib_advanced_runbook = stdlib_advanced_runbook_path.read_text(encoding="utf-8")
    for token in (
        "# objc3c Advanced Stdlib Helper Surface",
        "## Working boundary",
        "stdlib/advanced_architecture.json",
        "stdlib/advanced_helper_package_surface.json",
        "stdlib/modules/objc3.concurrency/",
        "stdlib/modules/objc3.keypath/",
        "stdlib/modules/objc3.system/",
        "## Advanced family split",
        "`objc3.concurrency` owns:",
        "`objc3.keypath` owns:",
        "`objc3.system` owns:",
        "## Expected shipped API families",
        "structured-child-spawn",
        "reflection-interop",
        "runtime-composition-hook",
        "## Exact checked-in source surface",
        "objc3_concurrency_spawn_token",
        "objc3_keypath_component_count",
        "objc3_system_resource_token",
        "## Layering rules",
        "## Explicit non-goals",
    ):
        require_token(stdlib_advanced_runbook, token, path=stdlib_advanced_runbook_path, errors=errors)

    stdlib_program_runbook = STDLIB_PROGRAM_RUNBOOK_PATH.read_text(encoding="utf-8")
    for token in (
        "# objc3c Stdlib Program Surface",
        "## Working Boundary",
        "stdlib/program_surface.json",
        "docs/tutorials/getting_started.md",
        "docs/tutorials/objc2_swift_cpp_comparison.md",
        "showcase/README.md",
        "showcase/portfolio.json",
        "## Onboarding And Capability-Story Policy",
        "runnable-now stories",
        "actor-shaped comparison or",
        "stdlib/README.md",
        "objc3.core",
        "objc3.concurrency",
        "objc3.keypath",
        "objc3.system",
        "showcase/tutorial_walkthrough.json",
        "site/src/index.body.md",
        "## Current Truthful Portfolio",
        "## Exact Live Implementation Paths",
        "scripts/check_stdlib_surface.py",
        "scripts/check_documentation_surface.py",
        "scripts/check_showcase_surface.py",
        "scripts/check_showcase_integration.py",
        "scripts/check_getting_started_integration.py",
        "## Exact Capability Demo Paths",
        "showcase/auroraBoard/main.objc3",
        "showcase/signalMesh/main.objc3",
        "showcase/patchKit/main.objc3",
        "## Exact Live Artifact And Output Paths",
        "tmp/artifacts/showcase/",
        "tmp/reports/showcase/",
        "## Publish And Package Surface",
        "publish_inputs",
        "staged_manifest_fields",
        "stdlib_program_surface",
        "stdlib_program_publish_inputs",
        "tmp/pkg/objc3c-native-runnable-toolchain/",
        "## Live Workflow Surface",
        "validate-stdlib-foundation",
        "validate-runnable-stdlib-foundation",
        "tmp/reports/tutorials/",
        "## Exact Live Commands",
        "python scripts/objc3c_public_workflow_runner.py validate-showcase",
        "python scripts/objc3c_public_workflow_runner.py validate-runnable-showcase",
        "python scripts/objc3c_public_workflow_runner.py inspect-capability-explorer",
        "npm run inspect:objc3c:capabilities",
        "## Working Rules For Downstream Issues",
        "## Explicit Non-Goals",
    ):
        require_token(stdlib_program_runbook, token, path=STDLIB_PROGRAM_RUNBOOK_PATH, errors=errors)
    forbid_token(
        stdlib_program_runbook,
        "for actors, status bridging, and runtime",
        path=STDLIB_PROGRAM_RUNBOOK_PATH,
        errors=errors,
    )

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
        "docs/runbooks/objc3c_performance.md",
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
        "capability_demo_compatibility",
        "stdlib/program_surface.json",
        "showcase/portfolio.json",
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
        "artifacts/package/objc3c-runnable-toolchain-package.json",
        "## Exact Live Commands",
        "python scripts/objc3c_public_workflow_runner.py compile-objc3c showcase/auroraBoard/main.objc3",
        "npm run compile:objc3c -- showcase/auroraBoard/main.objc3",
        "python scripts/objc3c_public_workflow_runner.py package-runnable-toolchain",
        "npm run package:objc3c-native:runnable-toolchain",
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

    performance_runbook = PERFORMANCE_RUNBOOK_PATH.read_text(encoding="utf-8")
    for token in (
        "# objc3c Performance Benchmark Boundary",
        "## Working Boundary",
        "compiler compile-latency benchmark flows",
        "comparative baseline workloads for ObjC2, Swift, and C++",
        "telemetry packets, normalization logic, and benchmark claim output",
        "## Benchmark Taxonomy And Claim Classes",
        "local-measurement",
        "toolchain-comparison",
        "availability-limited",
        "non-portable",
        "## Exact Live Implementation Paths",
        "scripts/objc3c_public_workflow_runner.py",
        "scripts/build_objc3c_native.ps1",
        "scripts/package_objc3c_runnable_toolchain.ps1",
        "showcase/portfolio.json",
        "docs/tutorials/objc2_swift_cpp_comparison.md",
        "tests/tooling/fixtures/performance/benchmark_portfolio.json",
        "## Exact Live Artifact And Output Paths",
        "tmp/artifacts/performance/",
        "tmp/reports/performance/",
        "tmp/pkg/objc3c-native-runnable-toolchain/",
        "## Exact Live Commands",
        "python scripts/objc3c_public_workflow_runner.py build-native-binaries",
        "python scripts/objc3c_public_workflow_runner.py benchmark-performance",
        "npm run inspect:objc3c:performance",
        "python scripts/objc3c_public_workflow_runner.py benchmark-comparative-baselines",
        "npm run inspect:objc3c:comparative-baselines",
        "python scripts/objc3c_public_workflow_runner.py validate-runnable-performance",
        "npm run test:objc3c:runnable-performance",
        "python scripts/objc3c_public_workflow_runner.py validate-performance-foundation",
        "npm run test:objc3c:performance",
        "python scripts/objc3c_public_workflow_runner.py package-runnable-toolchain",
        "## Exact Live Paths For Downstream Work",
        "docs/runbooks/objc3c_performance.md",
        "## Explicit Non-Goals",
    ):
        require_token(
            performance_runbook,
            token,
            path=PERFORMANCE_RUNBOOK_PATH,
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
