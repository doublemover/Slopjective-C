"""Stable contracts for the conformance corpus surface checker."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any


CHECKER_NAME = "conformance-corpus-surface"
SURFACE_CONTRACT_ID = "objc3c.conformance.corpus.surface.v1"
LONGITUDINAL_CONTRACT_ID = "objc3c.conformance.longitudinal_suites.v1"
SUMMARY_CONTRACT_ID = "objc3c.conformance.corpus.surface.summary.v1"
EXPECTED_PRIMARY_BUCKETS = [
    "parser",
    "semantic",
    "lowering_abi",
    "module_roundtrip",
    "diagnostics",
]
EXPECTED_SUPPLEMENTAL_BUCKETS = [
    "examples",
    "spec_open_issues",
    "workpacks",
]
EXPECTED_WORKFLOW_SURFACE = {
    "report_root": "tmp/reports/conformance",
    "artifact_root": "tmp/artifacts/conformance",
    "package_stage_root": "tmp/pkg/objc3c-native-runnable-toolchain",
    "package_bridge": "objc3c",
    "surface_check_action": "validate-conformance-corpus",
    "surface_check_command": "npm run objc3c -- validate-conformance-corpus",
    "runnable_surface_check_action": "validate-runnable-conformance-corpus",
    "runnable_surface_check_command": (
        "npm run objc3c -- validate-runnable-conformance-corpus"
    ),
    "implementation_anchors": [
        "scripts/check_conformance_corpus_surface.py",
        "scripts/generate_conformance_corpus_index.py",
        "scripts/check_conformance_suite.ps1",
    ],
    "coverage_map": "tests/conformance/COVERAGE_MAP.md",
    "longitudinal_suite_manifest": "tests/conformance/longitudinal_suites.json",
}


class SurfaceValidationError(RuntimeError):
    def __init__(self, message: str) -> None:
        super().__init__(message)
        self.message = message


@dataclass(frozen=True)
class SurfaceField:
    name: str
    expected: Any
    drift_message: str


SURFACE_FIELDS = (
    SurfaceField("contract_id", SURFACE_CONTRACT_ID, "corpus surface contract_id drifted"),
    SurfaceField("schema_version", 1, "corpus surface schema_version drifted"),
    SurfaceField("corpus_root", "tests/conformance", "corpus_root drifted"),
    SurfaceField("suite_readme", "tests/conformance/README.md", "suite_readme drifted"),
    SurfaceField("coverage_map", "tests/conformance/COVERAGE_MAP.md", "coverage_map drifted"),
    SurfaceField("runbook", "docs/runbooks/objc3c_conformance_corpus.md", "runbook drifted"),
    SurfaceField("primary_buckets", EXPECTED_PRIMARY_BUCKETS, "primary_buckets drifted"),
    SurfaceField(
        "supplemental_buckets",
        EXPECTED_SUPPLEMENTAL_BUCKETS,
        "supplemental_buckets drifted",
    ),
)


__all__ = (
    "CHECKER_NAME",
    "EXPECTED_PRIMARY_BUCKETS",
    "EXPECTED_SUPPLEMENTAL_BUCKETS",
    "EXPECTED_WORKFLOW_SURFACE",
    "LONGITUDINAL_CONTRACT_ID",
    "SUMMARY_CONTRACT_ID",
    "SURFACE_CONTRACT_ID",
    "SURFACE_FIELDS",
    "SurfaceField",
    "SurfaceValidationError",
)
