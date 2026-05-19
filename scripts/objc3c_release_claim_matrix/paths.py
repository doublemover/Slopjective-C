"""Path constants for release/runtime claim matrix publication."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
REPORT_ROOT = ROOT / "tmp" / "reports" / "release_claims" / "publication_matrix"
JSON_OUT = REPORT_ROOT / "release_runtime_claim_matrix.json"
MD_OUT = REPORT_ROOT / "release_runtime_claim_matrix.md"

NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNNER_EXE = ROOT / "artifacts" / "bin" / "objc3c-frontend-c-api-runner.exe"
HELLO_FIXTURE = ROOT / "tests" / "tooling" / "fixtures" / "native" / "hello.objc3"
METADATA_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "native"
    / "runtime_metadata_source_records_class_protocol_property_ivar.objc3"
)
RELEASE_CLAIMS_ROOT = ROOT / "tmp" / "reports" / "release_claims"

PUBLISHED_MATRIX_ARTIFACT_ROOT = (
    ROOT / "tmp" / "artifacts" / "compilation" / "objc3c-native" / "release_claims" / "published_matrix"
)
