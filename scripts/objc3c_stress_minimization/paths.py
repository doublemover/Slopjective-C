"""Shared paths and contract identifiers for stress minimization."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.artifact_identity import current_host_artifact_identity

ROOT = Path(__file__).resolve().parents[2]
ARTIFACT_IDENTITY = current_host_artifact_identity()
COMPILER = ROOT / ARTIFACT_IDENTITY.native_executable_relative_path
MANIFEST_PATH = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "minimization_manifest.json"
ARTIFACT_SURFACE_PATH = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "artifact_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "stress" / "minimization-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stress.minimization.summary.v1"


__all__ = [
    "ARTIFACT_IDENTITY",
    "ARTIFACT_SURFACE_PATH",
    "COMPILER",
    "MANIFEST_PATH",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "SUMMARY_PATH",
]
