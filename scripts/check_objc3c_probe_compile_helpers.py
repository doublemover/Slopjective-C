#!/usr/bin/env python3
"""Fast checks for shared compiler discovery and probe compile command helpers."""

from __future__ import annotations

import os
import tempfile
from pathlib import Path
from typing import Sequence

from objc3c_tooling.artifact_identity import current_host_artifact_identity
from objc3c_tooling.probe_compile import (
    find_clangxx,
    normal_user_manifest_link_args,
    probe_compile_command,
)


def expect(condition: bool, message: str) -> None:
    if not condition:
        raise RuntimeError(message)


def main(argv: Sequence[str] | None = None) -> int:
    _ = argv
    old_llvm_root = os.environ.get("LLVM_ROOT")
    old_path = os.environ.get("PATH", "")
    try:
        artifact_identity = current_host_artifact_identity()
        with tempfile.TemporaryDirectory() as raw_tmp:
            tmp = Path(raw_tmp)
            llvm_bin = tmp / "llvm" / "bin"
            llvm_bin.mkdir(parents=True)
            clangxx_name = "clang++.exe" if os.name == "nt" else "clang++"
            fake_clang = llvm_bin / clangxx_name
            fake_clang.write_text("", encoding="utf-8")
            os.environ["LLVM_ROOT"] = str(tmp / "llvm")
            expect(find_clangxx() == str(fake_clang), "LLVM_ROOT clang++ should win discovery")

            os.environ.pop("LLVM_ROOT", None)
            path_bin = tmp / "path-bin"
            path_bin.mkdir()
            path_clang = path_bin / clangxx_name
            path_clang.write_text("", encoding="utf-8")
            os.environ["PATH"] = str(path_bin)
            expect(Path(find_clangxx()).name.lower() == clangxx_name.lower(), "PATH clang++ should be discovered")

        probe_executable_name = (
            "probe.exe"
            if artifact_identity.native_executable_name.endswith(".exe")
            else "probe"
        )
        command = probe_compile_command(
            "clang++",
            Path("tests/tooling/runtime/probe.cpp"),
            Path("tmp") / probe_executable_name,
            runtime_library=Path(artifact_identity.runtime_library_relative_path),
            object_inputs=(Path("tmp") / artifact_identity.module_object_artifact_name,),
            extra_args=("-DTEST=1",),
        )
        joined = " ".join(command)
        expect("-std=c++20" in command, "compile command should set C++ standard")
        expect("-fms-runtime-lib=dll" in command, "compile command should preserve MSVC runtime flag")
        manifest_args = normal_user_manifest_link_args()
        if os.name == "nt":
            expect("-fuse-ld=lld" in command, "Windows probe links should use lld for embedded manifests")
            expect("/MANIFEST:EMBED" in command, "Windows probe links should embed a manifest")
            expect(
                "/MANIFESTUAC:level='asInvoker' uiAccess='false'" in command,
                "Windows probe links should force normal-user launch semantics",
            )
        else:
            expect(not manifest_args, "non-Windows probe links should not request Windows manifests")
        expect("native\\objc3c\\src" in joined or "native/objc3c/src" in joined, "compile command should include native source root")
        expect("tests\\tooling\\runtime" in joined or "tests/tooling/runtime" in joined, "compile command should include runtime support root")
        expect("-DTEST=1" in command, "compile command should preserve extra args")
        expected_object = f"tmp/{artifact_identity.module_object_artifact_name}"
        expect(
            expected_object in joined or expected_object.replace("/", "\\") in joined,
            "compile command should include extra objects",
        )
    finally:
        if old_llvm_root is None:
            os.environ.pop("LLVM_ROOT", None)
        else:
            os.environ["LLVM_ROOT"] = old_llvm_root
        os.environ["PATH"] = old_path

    print("objc3c-probe-compile-helpers: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
