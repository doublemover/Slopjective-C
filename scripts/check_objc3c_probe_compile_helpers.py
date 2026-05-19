#!/usr/bin/env python3
"""Fast checks for shared compiler discovery and probe compile command helpers."""

from __future__ import annotations

import os
import tempfile
from pathlib import Path
from typing import Sequence

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
        with tempfile.TemporaryDirectory() as raw_tmp:
            tmp = Path(raw_tmp)
            llvm_bin = tmp / "llvm" / "bin"
            llvm_bin.mkdir(parents=True)
            fake_clang = llvm_bin / "clang++.exe"
            fake_clang.write_text("", encoding="utf-8")
            os.environ["LLVM_ROOT"] = str(tmp / "llvm")
            expect(find_clangxx() == str(fake_clang), "LLVM_ROOT clang++.exe should win discovery")

            os.environ.pop("LLVM_ROOT", None)
            path_bin = tmp / "path-bin"
            path_bin.mkdir()
            path_clang = path_bin / "clang++.exe"
            path_clang.write_text("", encoding="utf-8")
            os.environ["PATH"] = str(path_bin)
            expect(Path(find_clangxx()).name.lower() == "clang++.exe", "PATH clang++ should be discovered")

        command = probe_compile_command(
            "clang++",
            Path("tests/tooling/runtime/probe.cpp"),
            Path("tmp/probe.exe"),
            runtime_library=Path("artifacts/lib/objc3_runtime.lib"),
            object_inputs=(Path("tmp/module.obj"),),
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
        expect("tmp/module.obj" in joined or "tmp\\module.obj" in joined, "compile command should include extra objects")
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
