"""Path constants for release/runtime claim matrix publication."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.artifact_identity import current_host_artifact_identity

ROOT = Path(__file__).resolve().parents[2]
ARTIFACT_IDENTITY = current_host_artifact_identity()
REPORT_ROOT = ROOT / "tmp" / "reports" / "release_claims" / "publication_matrix"
JSON_OUT = REPORT_ROOT / "release_runtime_claim_matrix.json"
MD_OUT = REPORT_ROOT / "release_runtime_claim_matrix.md"

NATIVE_EXE = ROOT / ARTIFACT_IDENTITY.native_executable_relative_path
RUNNER_EXE = ROOT / ARTIFACT_IDENTITY.frontend_runner_relative_path
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
