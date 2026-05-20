"""Native build, package, and proof workflow paths."""

from __future__ import annotations

from ..environment import ROOT

BUILD_PS1 = ROOT / "scripts" / "build_objc3c_native.ps1"
CLEAN_ROOM_REBUILD_PY = ROOT / "scripts" / "check_objc3c_native_clean_room_rebuild.py"
COMPILE_PS1 = ROOT / "scripts" / "objc3c_native_compile.ps1"
PACKAGE_PS1 = ROOT / "scripts" / "package_objc3c_runnable_toolchain.ps1"
PROOF_PS1 = ROOT / "scripts" / "run_objc3c_native_compile_proof.ps1"
