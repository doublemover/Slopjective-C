"""Checked paths and contract identifiers for public suite packaging."""

from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]

MANIFEST_PATH = ROOT / "tests" / "conformance" / "public_suite_manifest.json"
PACKAGE_REPLAY_EVIDENCE_PATH = (
    ROOT / "tests" / "conformance" / "public_suite_package_replay_evidence.json"
)
PACKAGE_CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "public_conformance_suite"
    / "package_contract.json"
)
MANIFEST_CHECKER_PATH = ROOT / "scripts" / "check_objc3c_public_conformance_suite_manifest.py"

DEFAULT_PACKAGE_ROOT = ROOT / "tmp" / "pkg" / "objc3-public-conformance-suite"
DEFAULT_REPORT_PATH = (
    ROOT / "tmp" / "reports" / "conformance" / "public-suite-package-replay-summary.json"
)

PUBLIC_COMMAND_PREFIX = "npm run objc3c -- "
PACKAGE_CONTRACT_ID = "objc3c.public_conformance_suite.package_contract.v1"
PACKAGE_MANIFEST_CONTRACT_ID = "objc3c.public_conformance_suite.package_manifest.v1"
PACKAGE_CASE_CONTRACT_ID = "objc3c.public_conformance_suite.case.v1"
PACKAGE_SUMMARY_CONTRACT_ID = "objc3c.public_conformance_suite.package_replay.summary.v1"
