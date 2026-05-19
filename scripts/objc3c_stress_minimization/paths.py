"""Shared paths and contract identifiers for stress minimization."""

from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
COMPILER = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
MANIFEST_PATH = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "minimization_manifest.json"
ARTIFACT_SURFACE_PATH = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "artifact_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "stress" / "minimization-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stress.minimization.summary.v1"


__all__ = [
    "ARTIFACT_SURFACE_PATH",
    "COMPILER",
    "MANIFEST_PATH",
    "ROOT",
    "SUMMARY_CONTRACT_ID",
    "SUMMARY_PATH",
]
