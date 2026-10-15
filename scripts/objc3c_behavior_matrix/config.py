"""Filesystem constants for the behavior matrix checker."""

from __future__ import annotations

from objc3c_tooling.artifact_identity import current_host_artifact_identity
from objc3c_tooling.paths import ROOT

ARTIFACT_IDENTITY = current_host_artifact_identity()
NATIVE_EXE = ROOT / ARTIFACT_IDENTITY.native_executable_relative_path
RUNTIME_LIB = ROOT / ARTIFACT_IDENTITY.runtime_library_relative_path
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
REPORT_PATH = ROOT / "tmp" / "reports" / "objc3c-behavior-matrix" / "summary.json"
CONTRACT_ID = "objc3c.behavior.fixture.matrix.v1"
