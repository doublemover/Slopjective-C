"""Filesystem constants for the behavior matrix checker."""

from __future__ import annotations

from objc3c_tooling.paths import ROOT

NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
REPORT_PATH = ROOT / "tmp" / "reports" / "objc3c-behavior-matrix" / "summary.json"
CONTRACT_ID = "objc3c.behavior.fixture.matrix.v1"
