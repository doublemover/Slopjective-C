"""Canonical paths for runtime acceptance infrastructure."""

from __future__ import annotations

from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
BUILD_PS1 = ROOT / "scripts" / "build_objc3c_native.ps1"
COMPILE_PS1 = ROOT / "scripts" / "objc3c_native_compile.ps1"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"


__all__ = [
    "BUILD_PS1",
    "COMPILE_PS1",
    "NATIVE_EXE",
    "ROOT",
    "RUNTIME_LIB",
]
