#!/usr/bin/env python3
"""Compile and run the LLVM Support JSON sidecar codec tests."""
import os
import pathlib
import shlex
import shutil
import subprocess
import tempfile

ROOT = pathlib.Path(__file__).resolve().parents[2]
llvm_config = os.environ.get("LLVM_CONFIG") or shutil.which("llvm-config")
if not llvm_config:
    raise SystemExit("LLVM_CONFIG or llvm-config is required for interface codec tests")

def query(*args):
    return subprocess.check_output([llvm_config, *args], text=True).strip()

include_dir = query("--includedir")
lib_dir = query("--libdir")
compiler = shutil.which("clang-cl")
with tempfile.TemporaryDirectory(prefix="objc3-interface-codec-") as directory:
    binary = pathlib.Path(directory) / "interface-codec-test.exe"
    sources = [
        str(ROOT / "tests" / "clang_foundation" / "interface_codec_test.cpp"),
        str(ROOT / "native" / "objc3c" / "src" / "clang_foundation" / "interface_codec.cpp"),
    ]
    if os.name == "nt":
        if not compiler:
            raise SystemExit("clang-cl is required to link against the installed MSVC LLVM SDK")
        libraries = query("--link-static", "--libs", "support").split()
        system_libraries = [lib for lib in query("--system-libs").split()
                            if lib.lower() != "xml2s.lib"]
        command = [compiler, "/nologo", "/std:c++20", "/EHsc", "/MT",
                   "/I" + str(ROOT / "native" / "objc3c" / "src"),
                   "/I" + include_dir, *sources, *libraries,
                   "/link", "/LIBPATH:" + lib_dir,
                   "/OUT:" + str(binary), *system_libraries]
    else:
        compiler = os.environ.get("CXX") or shutil.which("clang++") or shutil.which("g++")
        if not compiler:
            raise SystemExit("CXX, clang++, or g++ is required for interface codec tests")
        command = shlex.split(compiler) + [
            "-std=c++20", "-I" + str(ROOT / "native" / "objc3c" / "src"),
            "-I" + include_dir, *sources, "-L" + lib_dir,
            *query("--link-static", "--libs", "support").split(),
            *query("--system-libs").split(), "-o", str(binary)]
    subprocess.run(command, cwd=ROOT, check=True)
    subprocess.run([str(binary)], cwd=ROOT, check=True)
print("PASS: LLVM JSON semantic interface codec roundtrip and rejection cases")
