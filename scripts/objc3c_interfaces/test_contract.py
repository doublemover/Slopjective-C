#!/usr/bin/env python3
"""Run the H5 native contract checks and source-only Clang baseline fixture."""
import os
import pathlib
import shlex
import shutil
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
configured_compiler = os.environ.get("CXX")
compiler_path = shutil.which("clang++") or shutil.which("g++")
if not configured_compiler and not compiler_path:
    raise SystemExit("No C++ compiler available for the interface contract test")
command = (shlex.split(configured_compiler) if configured_compiler else [compiler_path]) + [
    "-std=c++20",
    "-Wall",
    "-Wextra",
    "-pedantic",
    "-I" + str(ROOT / "native" / "objc3c" / "src"),
    str(ROOT / "tests" / "clang_foundation" / "interface_contract_test.cpp"),
    str(ROOT / "native" / "objc3c" / "src" / "clang_foundation" / "interface_contract.cpp"),
    str(ROOT / "native" / "objc3c" / "src" / "clang_foundation" / "interface_validation.cpp"),
]
with tempfile.TemporaryDirectory(prefix="objc3-interface-contract-") as directory:
    binary = pathlib.Path(directory) / ("contract.exe" if os.name == "nt" else "contract")
    subprocess.run(command + ["-o", str(binary)], cwd=ROOT, check=True)
    subprocess.run([str(binary)], cwd=ROOT, check=True)
subprocess.run([os.environ.get("PYTHON", "python"),
                str(ROOT / "scripts" / "objc3c_interfaces" / "test_source_only_baseline.py")],
               cwd=ROOT, check=True)
print("PASS: semantic interface contract and dependency graph checks")
