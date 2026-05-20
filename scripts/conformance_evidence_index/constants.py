from __future__ import annotations

from pathlib import Path

from objc3c_tooling.subprocesses import command_text, python_script_command

ROOT = Path(__file__).resolve().parents[2]
DEFAULT_INPUT_ROOT = Path("reports/conformance")
DEFAULT_GLOBS = ("**/*.json",)
SUPPORT_CLAIM_RUNNABLE_EVIDENCE_CATALOG = Path(
    "tests/conformance/support_claim_runnable_evidence_catalog.json"
)
SCHEMA_ID = "objc3-conformance-evidence-index/v1"
INDEX_VERSION = 1
ARTIFACT_AUTHENTICITY_SCHEMA_ID = "objc3c.artifact.authenticity.schema.v1"
EVIDENCE_INDEX_SURFACE_ID = "objc3c.public_conformance.evidence_index.v1"
EVIDENCE_INDEX_ARTIFACT_FAMILY_ID = (
    "objc3c.genuine_generated_output.conformance_evidence_index.v1"
)
EVIDENCE_INDEX_REPORT_FAMILY_ID = (
    "objc3c.genuine_generated_output.release_evidence_index_report.v1"
)
GENERATOR_SCRIPT = "scripts/generate_conformance_evidence_index.py"
GENERATOR_COMMAND = python_script_command(GENERATOR_SCRIPT)
GENERATOR_PATH = command_text(GENERATOR_COMMAND)
UNKNOWN_PROFILE = "unknown-profile"
UNKNOWN_RELEASE = "unknown-release"
