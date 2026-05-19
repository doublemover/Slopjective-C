"""Repository paths for the external validation source-surface checker."""

from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "external_validation" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "external-validation" / "source-surface-summary.json"

EXPECTED_REQUIRED_PATHS = {
    "runbook": "docs/runbooks/objc3c_external_validation.md",
    "source_root": "tests/tooling/fixtures/external_validation",
    "source_readme": "tests/tooling/fixtures/external_validation/README.md",
    "source_check_script": "scripts/check_external_validation_source_surface.py",
    "trust_policy": "tests/tooling/fixtures/external_validation/trust_policy.json",
    "intake_manifest": "tests/tooling/fixtures/external_validation/intake_manifest.json",
    "quarantine_manifest": "tests/tooling/fixtures/external_validation/quarantine_manifest.json",
    "artifact_surface": "tests/tooling/fixtures/external_validation/artifact_surface.json",
    "workflow_surface": "tests/tooling/fixtures/external_validation/workflow_surface.json",
}

EXPECTED_ROOTS = (
    "tests/tooling/fixtures/external_validation",
    "tests/tooling/fixtures/objc3c",
    "tests/conformance",
    "docs/runbooks",
)

EXPECTED_ARTIFACT_ROOT = "tmp/artifacts/external-validation"
EXPECTED_REPORT_ROOT = "tmp/reports/external-validation"
