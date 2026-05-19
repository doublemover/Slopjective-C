"""Path constants for the release manifest builder."""

from __future__ import annotations

import shutil
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PWSH = shutil.which("pwsh") or "pwsh"

PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
RELEASE_EVIDENCE_PY = ROOT / "scripts" / "check_release_evidence.py"
PACKAGE_STAGE_ROOT = ROOT / "tmp" / "pkg" / "objc3c-native-runnable-toolchain"
PACKAGE_MANIFEST_RELATIVE_PATH = "artifacts/package/objc3c-runnable-toolchain-package.json"

SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "source_surface.json"
PAYLOAD_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "release_payload_policy.json"
REPRO_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "reproducibility_policy.json"
PROVENANCE_POLICY = ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "provenance_policy.json"
WORKFLOW_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "workflow_surface.json"

MANIFEST_PATH = ROOT / "tmp" / "artifacts" / "release-foundation" / "manifest" / "objc3c-release-manifest.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-foundation" / "release-manifest-summary.json"
EVIDENCE_INDEX_PATH = ROOT / "tmp" / "reports" / "release_evidence" / "evidence-index.json"
SBOM_PATH = ROOT / "tmp" / "artifacts" / "release-foundation" / "sbom" / "objc3c-release-sbom.json"
ATTESTATION_PATH = (
    ROOT
    / "tmp"
    / "artifacts"
    / "release-foundation"
    / "attestation"
    / "objc3c-release-attestation.json"
)
PUBLICATION_SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-foundation" / "publication-summary.json"
