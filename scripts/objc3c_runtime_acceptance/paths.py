"""Canonical paths for runtime acceptance infrastructure."""

from __future__ import annotations

from pathlib import Path

from objc3c_tooling.artifact_identity import current_host_artifact_identity


ROOT = Path(__file__).resolve().parents[2]
ARTIFACT_IDENTITY = current_host_artifact_identity()
BUILD_PS1 = ROOT / "scripts" / "build_objc3c_native.ps1"
COMPILE_PS1 = ROOT / "scripts" / "objc3c_native_compile.ps1"
NATIVE_EXE_RELATIVE_PATH = ARTIFACT_IDENTITY.native_executable_relative_path
FRONTEND_RUNNER_RELATIVE_PATH = ARTIFACT_IDENTITY.frontend_runner_relative_path
RUNTIME_LIB_RELATIVE_PATH = ARTIFACT_IDENTITY.runtime_library_relative_path
TAMPERED_RUNTIME_LIB_RELATIVE_PATH = (
    ARTIFACT_IDENTITY.tampered_runtime_library_relative_path
)
RUNTIME_LIB = ROOT / RUNTIME_LIB_RELATIVE_PATH
NATIVE_EXE = ROOT / NATIVE_EXE_RELATIVE_PATH
FRONTEND_RUNNER = ROOT / FRONTEND_RUNNER_RELATIVE_PATH


__all__ = [
    "ARTIFACT_IDENTITY",
    "BUILD_PS1",
    "COMPILE_PS1",
    "FRONTEND_RUNNER",
    "FRONTEND_RUNNER_RELATIVE_PATH",
    "NATIVE_EXE",
    "NATIVE_EXE_RELATIVE_PATH",
    "ROOT",
    "RUNTIME_LIB",
    "RUNTIME_LIB_RELATIVE_PATH",
    "TAMPERED_RUNTIME_LIB_RELATIVE_PATH",
]
