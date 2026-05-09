"""Structured model and report writer for the documentation surface checker."""

from __future__ import annotations

import sys
from dataclasses import dataclass
from pathlib import Path


CHECKER_NAME = "documentation-surface"
ROOT = Path(__file__).resolve().parents[1]
DOCUMENTATION_SURFACE_OWNER = "documentation-surface.owner"
DOCUMENTATION_SURFACE_OWNER_SURFACE = "scripts/check_documentation_surface_model.py"
DOCUMENTATION_SURFACE_BLOCKER_METADATA = {
    "blocker_contract": "hard-cutover-docs-surface-fail-closed",
    "blocker_scope": "reader-facing-docs-and-machine-appendix-boundary",
    "blocker_owner": DOCUMENTATION_SURFACE_OWNER,
    "blocker_owner_surface": DOCUMENTATION_SURFACE_OWNER_SURFACE,
}

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
RUNTIME_PERFORMANCE_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_runtime_performance.md"
COMPILER_THROUGHPUT_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_compiler_throughput.md"
STDLIB_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_stdlib_foundation.md"
STDLIB_CORE_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_stdlib_core.md"
STDLIB_ADVANCED_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_stdlib_advanced.md"
STDLIB_README_PATH = ROOT / "stdlib" / "README.md"
STDLIB_PROGRAM_RUNBOOK_PATH = ROOT / "docs" / "runbooks" / "objc3c_stdlib_program.md"
PUBLIC_COMMAND_SURFACE_PATH = ROOT / "docs" / "runbooks" / "objc3c_public_command_surface.md"
TUTORIAL_README_PATH = ROOT / "docs" / "tutorials" / "README.md"
GETTING_STARTED_PATH = ROOT / "docs" / "tutorials" / "getting_started.md"
BUILD_RUN_VERIFY_PATH = ROOT / "docs" / "tutorials" / "build_run_verify.md"
GUIDED_WALKTHROUGH_PATH = ROOT / "docs" / "tutorials" / "guided_walkthrough.md"
MIGRATION_GUIDE_PATH = ROOT / "docs" / "tutorials" / "objc2_to_objc3_migration.md"
COMPARISON_README_PATH = ROOT / "docs" / "tutorials" / "objc2_swift_cpp_comparison.md"


@dataclass(frozen=True)
class DocumentationSurfaceSource:
    path: Path
    required_tokens: tuple[str, ...] = ()
    forbidden_tokens: tuple[str, ...] = ()
    owner_id: str = DOCUMENTATION_SURFACE_OWNER
    owner_surface: str = DOCUMENTATION_SURFACE_OWNER_SURFACE

    @property
    def report_path(self) -> str:
        return self.path.relative_to(ROOT).as_posix()

    def validate(self) -> tuple[str, ...]:
        text = self.path.read_text(encoding="utf-8")
        errors: list[str] = []
        for token in self.required_tokens:
            if token not in text:
                errors.append(f"{self.report_path}: missing required token {token!r}")
        for token in self.forbidden_tokens:
            if token in text:
                errors.append(f"{self.report_path}: forbidden token present {token!r}")
        return tuple(errors)


@dataclass(frozen=True)
class DocumentationSurfaceReport:
    checker_name: str
    errors: tuple[str, ...]
    owner_contract: dict[str, object]

    @property
    def passed(self) -> bool:
        return not self.errors


class DocumentationSurfaceReportWriter:
    def write(self, report: DocumentationSurfaceReport) -> int:
        if report.passed:
            print(f"{report.checker_name}: OK")
            return 0

        print(f"{report.checker_name}: FAIL", file=sys.stderr)
        for error in report.errors:
            print(f"- {error}", file=sys.stderr)
        return 1


@dataclass(frozen=True)
class DocumentationSurfaceModel:
    sources: tuple[DocumentationSurfaceSource, ...]

    def owner_contract(self) -> dict[str, object]:
        return {
            "owner_id": DOCUMENTATION_SURFACE_OWNER,
            "owner_surface": DOCUMENTATION_SURFACE_OWNER_SURFACE,
            "checked_source_count": len(self.sources),
            "checked_sources": [source.report_path for source in self.sources],
            "blocker_metadata": dict(DOCUMENTATION_SURFACE_BLOCKER_METADATA),
        }

    def validate(self) -> DocumentationSurfaceReport:
        errors: list[str] = []
        for source in self.sources:
            errors.extend(source.validate())
        return DocumentationSurfaceReport(
            CHECKER_NAME,
            tuple(errors),
            self.owner_contract(),
        )


def _source(
    path: Path,
    *,
    required_tokens: tuple[str, ...] = (),
    forbidden_tokens: tuple[str, ...] = (),
) -> DocumentationSurfaceSource:
    return DocumentationSurfaceSource(
        path=path,
        required_tokens=required_tokens,
        forbidden_tokens=forbidden_tokens,
        owner_id=DOCUMENTATION_SURFACE_OWNER,
        owner_surface=DOCUMENTATION_SURFACE_OWNER_SURFACE,
    )


DOCUMENTATION_SURFACE_MODEL = DocumentationSurfaceModel(
    sources=(
        _source(
            README_PATH,
            required_tokens=(
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
            ),
        ),
        _source(
            CONTRIBUTING_PATH,
            required_tokens=(
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
                "npm run objc3c -- check-repo-superclean-surface",
            ),
        ),
        _source(
            SHOWCASE_README_PATH,
            required_tokens=(
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
            ),
            forbidden_tokens=(
                "target story: status bridging, actors, runtime messaging",
                "--capability actors",
            ),
        ),
        _source(
            TUTORIAL_README_PATH,
            required_tokens=(
                "# Tutorials And Canonicalization Guides",
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
            ),
            forbidden_tokens=(
                "use it for actors, status bridging, and runtime messaging",
            ),
        ),
        _source(
            GETTING_STARTED_PATH,
            required_tokens=(
                "# Getting Started With The Runnable Subset",
                "## What This Tutorial Proves",
                "## Teaching Model",
                "## Step 1 Verify The Toolchain",
                "npm run objc3c -- build-native-binaries",
                "npm run objc3c -- test-smoke",
                "## Step 2 Compile One Runnable Example",
                "showcase/auroraBoard/main.objc3",
                "## Step 3 Use The Showcase Surface As The Tutorial Backbone",
                "npm run objc3c -- check-showcase-surface",
                "npm run objc3c -- validate-showcase",
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
            ),
            forbidden_tokens=(
                "`signalMesh` for actors, status bridging, and runtime messaging",
            ),
        ),
        _source(
            BUILD_RUN_VERIFY_PATH,
            required_tokens=(
                "# Tutorial Build Run And Verify Surface",
                "## Workflow Boundary",
                "## Build",
                "npm run objc3c -- build-native-binaries",
                "## Run The First Compile",
                "npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3",
                "## Verify The Portfolio Surface",
                "npm run objc3c -- check-showcase-surface",
                "npm run objc3c -- validate-showcase",
                "npm run objc3c -- validate-runnable-showcase",
                "## Validation Surface",
                "scripts/check_getting_started_surface.py",
                "scripts/check_getting_started_integration.py",
                "npm run objc3c -- validate-getting-started",
                "## Artifact And Report Expectations",
                "tmp/artifacts/showcase/<example-id>/",
                "tmp/reports/showcase/",
                "## Canonical Inputs",
                "## Exact Live Paths For Downstream Work",
                "scripts.objc3c_workflow",
                "## Explicit Non-Goals",
            ),
        ),
        _source(
            GUIDED_WALKTHROUGH_PATH,
            required_tokens=(
                "# Guided Showcase Walkthrough",
                "## Walkthrough Boundary",
                "## Walkthrough Steps",
                "npm run objc3c -- build-native-binaries",
                "npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3",
                "npm run objc3c -- compile-objc3c showcase/signalMesh/main.objc3",
                "npm run objc3c -- compile-objc3c showcase/patchKit/main.objc3",
                "npm run objc3c -- check-showcase-surface",
                "npm run objc3c -- validate-showcase",
                "## Walkthrough Assets",
                "showcase/tutorial_walkthrough.json",
                "scripts/check_getting_started_surface.py",
                "scripts/check_getting_started_integration.py",
                "npm run objc3c -- validate-getting-started",
                "## Canonical Inputs",
                "## Exact Live Paths For Downstream Work",
                "scripts/check_showcase_surface.py",
                "## Explicit Non-Goals",
            ),
        ),
        _source(
            MIGRATION_GUIDE_PATH,
            required_tokens=(
                "# ObjC2 To ObjC3 Canonicalization Guide",
                "## Canonicalization Boundary",
                "## Step 1 Keep The Familiar ObjC Shape, Drop The Implicit Assumptions",
                "showcase/auroraBoard/main.objc3",
                "## Step 3 Treat Swift-Facing Async And Imported Hooks As Explicit Interop Contracts",
                "showcase/signalMesh/main.objc3",
                "showcase/patchKit/main.objc3",
                "npm run objc3c -- check-showcase-surface",
                "npm run objc3c -- validate-showcase",
                "## Recommended Reading Order",
                "## Canonical Inputs",
                "## Exact Live Paths For Downstream Work",
                "scripts/check_documentation_surface.py",
                "## Explicit Non-Goals",
            ),
        ),
        _source(
            COMPARISON_README_PATH,
            required_tokens=(
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
            ),
        ),
        _source(
            STDLIB_RUNBOOK_PATH,
            required_tokens=(
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
                "npm run objc3c -- validate-runnable-stdlib-foundation",
                "npm run objc3c -- package-runnable-toolchain",
                "## Public actions",
                "npm run objc3c -- validate-runnable-stdlib-foundation",
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
            ),
        ),
        _source(
            STDLIB_README_PATH,
            required_tokens=(
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
            ),
        ),
        _source(
            STDLIB_CORE_RUNBOOK_PATH,
            required_tokens=(
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
            ),
        ),
        _source(
            STDLIB_ADVANCED_RUNBOOK_PATH,
            required_tokens=(
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
            ),
        ),
        _source(
            STDLIB_PROGRAM_RUNBOOK_PATH,
            required_tokens=(
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
                "validate-stdlib-program",
                "validate-runnable-stdlib-program",
                "npm run objc3c -- validate-stdlib-program",
                "npm run objc3c -- validate-runnable-stdlib-program",
                "tmp/reports/tutorials/",
                "## Exact Live Commands",
                "npm run objc3c -- validate-showcase",
                "npm run objc3c -- validate-runnable-showcase",
                "npm run objc3c -- inspect-capability-explorer",
                "npm run objc3c -- inspect-capability-explorer",
                "## Working Rules For Downstream Issues",
                "## Explicit Non-Goals",
            ),
            forbidden_tokens=(
                "for actors, status bridging, and runtime",
            ),
        ),
        _source(
            SITE_BODY_PATH,
            required_tokens=(
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
                "[capability matrix](../docs/support/capability_matrix.md)",
                "[evidence map](../docs/support/evidence_map.md)",
            ),
            forbidden_tokens=(
                "[archived spec index](../docs/reference/legacy_spec_anchor_index.md#legacy-files)",
            ),
        ),
        _source(
            SITE_POLICY_PATH,
            required_tokens=(
                "## Tone and Accessibility Rules",
            ),
        ),
        _source(
            NATIVE_OWNERSHIP_PATH,
            required_tokens=(
                "## Public Doc Style And Accessibility Rules",
            ),
        ),
        _source(
            NATIVE_FRAGMENT_README_PATH,
            required_tokens=(
                "## Canonical Naming And Path Rules",
                "user-facing package entrypoints come from `package.json`",
                "transient outputs stay under `tmp/`",
                "published binaries and libraries stay under `artifacts/`",
            ),
        ),
        _source(
            MAINTAINER_WORKFLOW_PATH,
            required_tokens=(
                "## Superclean Working Boundary",
                "implementation roots:",
                "generated checked-in outputs:",
                "docs/tutorials/",
                "CONTRIBUTING.md",
                "showcase/",
                "docs/runbooks/objc3c_developer_tooling.md",
                "docs/runbooks/objc3c_bonus_experiences.md",
                "docs/runbooks/objc3c_performance.md",
                "docs/runbooks/objc3c_runtime_performance.md",
                "docs/runbooks/objc3c_compiler_throughput.md",
                "Do not add milestone-specific wrappers, sidecar compatibility files, or",
            ),
        ),
        _source(
            DEVELOPER_TOOLING_RUNBOOK_PATH,
            required_tokens=(
                "# objc3c Developer Tooling Boundary",
                "## Working Boundary",
                "## Exact Live Implementation Paths",
                "native/objc3c/src/tools/objc3c_frontend_c_api_runner.cpp",
                "native/objc3c/src/io/objc3_cli_reporting_output_contract_scaffold.h",
                "scripts/build_objc3c_native.ps1",
                "scripts.objc3c_workflow",
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
                "npm run objc3c -- build-native-contracts",
                "artifacts/bin/objc3c-frontend-c-api-runner.exe tests/tooling/fixtures/native/hello.objc3",
                "npm run objc3c -- inspect-compile-observability",
                "npm run objc3c -- inspect-compile-observability",
                "npm run objc3c -- inspect-runtime-inspector",
                "npm run objc3c -- inspect-runtime-inspector",
                "npm run objc3c -- inspect-capability-explorer",
                "npm run objc3c -- inspect-capability-explorer",
                "npm run objc3c -- trace-compile-stages",
                "npm run objc3c -- trace-compile-stages",
                "npm run objc3c -- validate-developer-tooling",
                "npm run objc3c -- validate-developer-tooling",
                "## Runtime Introspection Primitives",
                "objc3_runtime_copy_arc_debug_state_for_testing",
                "runtime_metadata_object_inspection_uses_llvm_objdump",
                "## Explicit Non-Goals",
            ),
        ),
        _source(
            BONUS_EXPERIENCES_RUNBOOK_PATH,
            required_tokens=(
                "# objc3c Bonus Experiences Boundary",
                "## Working Boundary",
                "interactive playground flows",
                "visual runtime inspection and capability-explorer flows",
                "starter-template and project-generator flows",
                "## Current Truthful Portfolio",
                "scripts.objc3c_workflow",
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
                "npm run objc3c -- inspect-capability-explorer",
                "npm run objc3c -- inspect-capability-explorer",
                "tmp/reports/tutorials/",
                "artifacts/package/objc3c-runnable-toolchain-package.json",
                "## Exact Live Commands",
                "npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3",
                "npm run objc3c -- compile-objc3c showcase/auroraBoard/main.objc3",
                "npm run objc3c -- package-runnable-toolchain",
                "npm run objc3c -- package-runnable-toolchain",
                "npm run objc3c -- validate-showcase",
                "npm run objc3c -- validate-runnable-showcase",
                "npm run objc3c -- validate-getting-started",
                "## Feasibility And Working Model",
                "## Working Rules For Downstream Issues",
                "## Explicit Non-Goals",
            ),
        ),
        _source(
            PERFORMANCE_RUNBOOK_PATH,
            required_tokens=(
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
                "scripts.objc3c_workflow",
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
                "npm run objc3c -- build-native-binaries",
                "npm run objc3c -- benchmark-performance",
                "npm run objc3c -- benchmark-performance",
                "npm run objc3c -- benchmark-comparative-baselines",
                "npm run objc3c -- benchmark-comparative-baselines",
                "npm run objc3c -- validate-runnable-performance",
                "npm run objc3c -- validate-runnable-performance",
                "npm run objc3c -- validate-performance-foundation",
                "npm run objc3c -- validate-performance-foundation",
                "npm run objc3c -- package-runnable-toolchain",
                "## Exact Live Paths For Downstream Work",
                "docs/runbooks/objc3c_performance.md",
                "## Explicit Non-Goals",
            ),
        ),
        _source(
            RUNTIME_PERFORMANCE_RUNBOOK_PATH,
            required_tokens=(
                "# objc3c Runtime Performance Boundary",
                "## Working Boundary",
                "startup registration and replay costs",
                "selector lookup and dispatch cache behavior",
                "property/reflection query hot paths",
                "runtime performance summaries, regression artifacts, and packaged validation",
                "## Runtime Hot-Path Taxonomy",
                "startup-installation",
                "dispatch-cache",
                "reflection-query",
                "ownership-helpers",
                "runtime-counter-snapshot",
                "## Exact Live Implementation Paths",
                "native/objc3c/src/runtime/objc3_runtime.cpp",
                "native/objc3c/src/runtime/objc3_runtime_bootstrap_internal.h",
                "scripts/check_objc3c_runtime_acceptance.py",
                "scripts/benchmark_objc3c_runtime_performance.py",
                "scripts/check_objc3c_runtime_performance_integration.py",
                "scripts/check_objc3c_runnable_runtime_performance_end_to_end.py",
                "tests/tooling/fixtures/runtime_performance/source_surface.json",
                "## Exact Live Artifact And Output Paths",
                "tmp/artifacts/runtime-performance/",
                "tmp/reports/runtime-performance/",
                "## Exact Live Commands",
                "npm run objc3c -- benchmark-runtime-performance",
                "npm run objc3c -- benchmark-runtime-performance",
                "npm run objc3c -- validate-runtime-performance",
                "npm run objc3c -- validate-runtime-performance",
                "npm run objc3c -- validate-runnable-runtime-performance",
                "npm run objc3c -- validate-runnable-runtime-performance",
                "## Explicit Non-Goals",
            ),
        ),
        _source(
            COMPILER_THROUGHPUT_RUNBOOK_PATH,
            required_tokens=(
                "# objc3c Compiler Throughput Boundary",
                "## Working Boundary",
                "incremental build cache reuse and invalidation behavior",
                "macro-host cache publication and compile-coupled docs generation cost",
                "heavyweight validation-tier ownership and duplicate compile removal",
                "## Throughput Taxonomy",
                "compile-cold",
                "compile-cache-hit",
                "incremental-invalidation",
                "macro-host-runtime-boundary",
                "docs-generation",
                "validation-tier-overlap",
                "## Exact Live Implementation Paths",
                "scripts/check_objc3c_native_perf_budget.ps1",
                "scripts/check_objc3c_compiler_throughput_integration.py",
                "tests/tooling/fixtures/compiler_throughput/source_surface.json",
                "tests/tooling/fixtures/compiler_throughput/workload_manifest.json",
                "tests/tooling/fixtures/compiler_throughput/validation_tier_map.json",
                "tests/tooling/fixtures/compiler_throughput/optimization_policy.json",
                "tests/tooling/fixtures/compiler_throughput/artifact_surface.json",
                "schemas/objc3c-compiler-throughput-summary-v1.schema.json",
                "## Exact Live Artifact And Output Paths",
                "tmp/artifacts/objc3c-native/perf-budget/",
                "tmp/reports/compiler-throughput/",
                "## Exact Live Commands",
                "npm run objc3c -- benchmark-compiler-throughput",
                "## Explicit Non-Goals",
            ),
        ),
        _source(
            PUBLIC_COMMAND_SURFACE_PATH,
            required_tokens=(
                "operator-facing appendix",
                "## Operator Notes",
                "Treat this file as a generated machine-facing appendix",
                "Canonical user-facing command names come from `package.json`",
                "`native/objc3c/`, `scripts/`, and `tests/` are the live implementation roots",
            ),
        ),
    ),
)
