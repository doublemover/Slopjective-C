"""Source-of-truth field declarations for the repo superclean surface checker."""

from __future__ import annotations

from .core_fields import CORE_SURFACE_FIELDS
from .experience_fields import EXPERIENCE_SURFACE_FIELDS
from .model import RepoSupercleanSurfaceModel, SurfaceField
from .performance_fields import PERFORMANCE_SURFACE_FIELDS
from .release_fields import RELEASE_SURFACE_FIELDS


REPO_SUPERCLEAN_SURFACE_MODEL = RepoSupercleanSurfaceModel(
    fields=(
        *CORE_SURFACE_FIELDS,
        *EXPERIENCE_SURFACE_FIELDS,
        *PERFORMANCE_SURFACE_FIELDS,
        *RELEASE_SURFACE_FIELDS,
        SurfaceField(
            "stress_validation_surface",
            {
                "source_surface_contract": "tests/tooling/fixtures/stress/source_surface.json",
                "artifact_surface_contract": "tests/tooling/fixtures/stress/artifact_surface.json",
                "source_readme": "tests/tooling/fixtures/stress/README.md",
                "safety_policy": "tests/tooling/fixtures/stress/safety_policy.json",
                "runbook": "docs/runbooks/objc3c_stress_validation.md",
                "source_check_script": "scripts/check_stress_source_surface.py",
                "checked_in_roots": [
                    "tests/tooling/fixtures/stress",
                    "tests/tooling/fixtures/native",
                    "tests/tooling/fixtures/objc3c",
                    "tests/tooling/fixtures/parser_conformance_corpus",
                    "tests/conformance",
                ],
                "source_family_ids": [
                    "parser-sema-fuzz",
                    "lowering-runtime-stress",
                    "mixed-module-differential",
                    "replay-backed-contracts",
                ],
            },
            "stress_validation_surface drifted",
        ),
        SurfaceField(
            "stdlib_foundation_surface",
            {
                "workspace_contract": "stdlib/workspace.json",
                "module_inventory": "stdlib/module_inventory.json",
                "stability_policy": "stdlib/stability_policy.json",
                "package_surface": "stdlib/package_surface.json",
                "core_architecture": "stdlib/core_architecture.json",
                "advanced_architecture": "stdlib/advanced_architecture.json",
                "semantic_policy": "stdlib/semantic_policy.json",
                "lowering_import_surface": "stdlib/lowering_import_surface.json",
                "advanced_helper_package_surface": "stdlib/advanced_helper_package_surface.json",
                "source_roots": [
                    "stdlib/README.md",
                    "stdlib/advanced_helper_package_surface.json",
                    "stdlib/modules/objc3.core/module.objc3",
                    "stdlib/modules/objc3.errors/module.objc3",
                    "stdlib/modules/objc3.concurrency/module.objc3",
                    "stdlib/modules/objc3.keypath/module.objc3",
                    "stdlib/modules/objc3.system/module.objc3",
                ],
                "report_roots": [
                    "tmp/artifacts/stdlib",
                    "tmp/reports/stdlib",
                    "tmp/pkg/objc3c-native-runnable-toolchain",
                ],
                "public_actions": [
                    "check-stdlib-surface",
                    "materialize-stdlib-workspace",
                    "validate-stdlib-foundation",
                    "validate-stdlib-advanced",
                    "validate-runnable-stdlib-advanced",
                    "validate-runnable-stdlib-foundation",
                    "package-runnable-toolchain",
                ],
            },
            "stdlib_foundation_surface drifted",
        ),
        SurfaceField(
            "stdlib_program_surface",
            {
                "program_contract": "stdlib/program_surface.json",
                "stdlib_readme": "stdlib/README.md",
                "runbook": "docs/runbooks/objc3c_stdlib_program.md",
                "site_entry": "site/src/index.body.md",
                "publish_inputs": [
                    "stdlib/README.md",
                    "docs/runbooks/objc3c_stdlib_program.md",
                    "docs/tutorials/README.md",
                    "docs/tutorials/getting_started.md",
                    "docs/tutorials/objc2_swift_cpp_comparison.md",
                    "showcase/README.md",
                    "showcase/portfolio.json",
                    "showcase/tutorial_walkthrough.json",
                    "site/src/index.body.md",
                ],
                "report_roots": [
                    "tmp/artifacts/stdlib",
                    "tmp/reports/stdlib",
                    "tmp/pkg/objc3c-native-runnable-toolchain",
                ],
                "workflow_surface": {
                    "report_root": "tmp/reports/stdlib",
                    "showcase_report_root": "tmp/reports/showcase",
                    "tutorial_report_root": "tmp/reports/tutorials",
                    "integration_entrypoint": "validate-stdlib-program",
                    "packaged_validation_entrypoint": "validate-runnable-stdlib-program",
                    "integration_actions": [
                        "check-documentation-surface",
                        "validate-getting-started",
                        "validate-showcase",
                        "validate-stdlib-foundation",
                        "inspect-capability-explorer",
                    ],
                    "release_actions": [
                        "validate-runnable-showcase",
                        "validate-runnable-stdlib-foundation",
                        "package-runnable-toolchain",
                    ],
                },
                "public_actions": [
                    "check-documentation-surface",
                    "check-showcase-surface",
                    "validate-getting-started",
                    "validate-showcase",
                    "validate-runnable-showcase",
                    "validate-stdlib-foundation",
                    "validate-runnable-stdlib-foundation",
                    "validate-stdlib-program",
                    "validate-runnable-stdlib-program",
                    "inspect-capability-explorer",
                    "package-runnable-toolchain",
                ],
            },
            "stdlib_program_surface drifted",
        ),
        SurfaceField(
            "conformance_corpus_surface",
            {
                "corpus_contract": "tests/conformance/corpus_surface.json",
                "suite_readme": "tests/conformance/README.md",
                "coverage_map": "tests/conformance/COVERAGE_MAP.md",
                "runbook": "docs/runbooks/objc3c_conformance_corpus.md",
                "longitudinal_manifest": "tests/conformance/longitudinal_suites.json",
                "report_roots": [
                    "tmp/artifacts/conformance",
                    "tmp/reports/conformance",
                    "tmp/pkg/objc3c-native-runnable-toolchain",
                ],
                "workflow_surface": {
                    "report_root": "tmp/reports/conformance",
                    "artifact_root": "tmp/artifacts/conformance",
                    "package_stage_root": "tmp/pkg/objc3c-native-runnable-toolchain",
                    "surface_check_script": "scripts/check_conformance_corpus_surface.py",
                    "coverage_index_script": "scripts/generate_conformance_corpus_index.py",
                    "legacy_suite_gate_script": "scripts/check_conformance_suite.ps1",
                    "coverage_map": "tests/conformance/COVERAGE_MAP.md",
                    "longitudinal_suite_manifest": "tests/conformance/longitudinal_suites.json",
                },
            },
            "conformance_corpus_surface drifted",
        ),
    ),
    frontend_contract_artifact_names=(
        "frontend_source_graph",
        "frontend_invocation_lock",
        "frontend_core_feature_expansion",
        "frontend_edge_compat",
        "frontend_edge_robustness",
        "frontend_diagnostics_hardening",
        "frontend_recovery_determinism_hardening",
        "frontend_conformance_matrix",
        "frontend_conformance_corpus",
        "frontend_integration_closeout",
    ),
    explicit_non_goals=(
        "no milestone-coded command names",
        "no secondary source-of-truth directories",
        "no generated-output hand edits",
    ),
)
