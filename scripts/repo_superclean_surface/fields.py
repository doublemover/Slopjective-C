"""Source-of-truth field declarations for the repo superclean surface checker."""

from __future__ import annotations

from .core_fields import CORE_SURFACE_FIELDS
from .experience_fields import EXPERIENCE_SURFACE_FIELDS
from .model import FrontendContractArtifact, RepoSupercleanSurfaceModel, SurfaceField
from .performance_fields import PERFORMANCE_SURFACE_FIELDS
from .release_fields import RELEASE_SURFACE_FIELDS
from .stdlib_fields import STDLIB_SURFACE_FIELDS


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
        *STDLIB_SURFACE_FIELDS,
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
    frontend_contract_artifacts=(
        FrontendContractArtifact(
            "frontend_source_graph",
            "source-derived",
            "tmp/artifacts/objc3c-native/frontend_source_graph.json",
        ),
        FrontendContractArtifact(
            "frontend_invocation_lock",
            "binary-derived",
            "tmp/artifacts/objc3c-native/frontend_invocation_lock.json",
        ),
        FrontendContractArtifact(
            "frontend_core_feature_expansion",
            "binary-derived",
            "tmp/artifacts/objc3c-native/frontend_core_feature_expansion.json",
        ),
        FrontendContractArtifact(
            "frontend_edge_compat",
            "closeout-derived",
            "tmp/artifacts/objc3c-native/frontend_edge_compat.json",
        ),
        FrontendContractArtifact(
            "frontend_edge_robustness",
            "closeout-derived",
            "tmp/artifacts/objc3c-native/frontend_edge_robustness.json",
        ),
        FrontendContractArtifact(
            "frontend_diagnostics_hardening",
            "closeout-derived",
            "tmp/artifacts/objc3c-native/frontend_diagnostics_hardening.json",
        ),
        FrontendContractArtifact(
            "frontend_recovery_determinism_hardening",
            "closeout-derived",
            "tmp/artifacts/objc3c-native/frontend_recovery_determinism_hardening.json",
        ),
        FrontendContractArtifact(
            "frontend_conformance_matrix",
            "closeout-derived",
            "tmp/artifacts/objc3c-native/frontend_conformance_matrix.json",
        ),
        FrontendContractArtifact(
            "frontend_conformance_corpus",
            "closeout-derived",
            "tmp/artifacts/objc3c-native/frontend_conformance_corpus.json",
        ),
        FrontendContractArtifact(
            "frontend_integration_closeout",
            "closeout-derived",
            "tmp/artifacts/objc3c-native/frontend_integration_closeout.json",
        ),
    ),
    explicit_non_goals=(
        "no milestone-coded command names",
        "no secondary source-of-truth directories",
        "no generated-output hand edits",
    ),
)
