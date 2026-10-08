from __future__ import annotations

import os
import subprocess
from pathlib import Path
from typing import Callable, Sequence

from objc3c_tooling.llvm_discovery import find_llvm_tool_path
from objc3c_tooling.paths import ROOT, repo_rel
from objc3c_tooling.subprocesses import failure_snippet, run_capture


def normal_user_manifest_link_args() -> list[str]:
    if os.name != "nt":
        return []
    return [
        "-fuse-ld=lld",
        "-Xlinker",
        "/MANIFEST:EMBED",
        "-Xlinker",
        "/MANIFESTUAC:level='asInvoker' uiAccess='false'",
    ]


def find_clangxx(*, llvm_root: str | None = None) -> str:
    configured_root = llvm_root if llvm_root is not None else os.environ.get("LLVM_ROOT")
    candidate = find_llvm_tool_path("clang++", llvm_root=configured_root)
    if candidate:
        return str(candidate)
    raise RuntimeError("clang++ not found; set LLVM_ROOT or ensure clang++ is on PATH")


def probe_compile_command(
    clangxx: str,
    probe_source: Path,
    output_exe: Path,
    *,
    cwd: Path = ROOT,
    runtime_library: Path,
    object_inputs: Sequence[Path] = (),
    extra_args: Sequence[str] = (),
    native_include_root: Path | None = None,
    runtime_support_include_root: Path | None = None,
) -> list[str]:
    native_include = native_include_root or (cwd / "native" / "objc3c" / "src")
    support_include = runtime_support_include_root or (cwd / "tests" / "tooling" / "runtime")
    return [
        clangxx,
        "-std=c++20",
        "-fms-runtime-lib=dll",
        *normal_user_manifest_link_args(),
        "-I",
        str(native_include.resolve()),
        "-I",
        str(support_include.resolve()),
        *extra_args,
        str(probe_source),
        *[str(path) for path in object_inputs],
        str(runtime_library),
        "-o",
        str(output_exe),
    ]


def compile_probe(
    clangxx: str,
    probe_source: Path,
    output_exe: Path,
    *,
    cwd: Path = ROOT,
    runtime_library: Path,
    object_inputs: Sequence[Path] = (),
    extra_args: Sequence[str] = (),
    failure_context: str | None = None,
    runner: Callable[[list[str]], subprocess.CompletedProcess[str]] | None = None,
) -> subprocess.CompletedProcess[str]:
    output_exe.parent.mkdir(parents=True, exist_ok=True)
    command = probe_compile_command(
        clangxx,
        probe_source,
        output_exe,
        cwd=cwd,
        runtime_library=runtime_library,
        object_inputs=object_inputs,
        extra_args=extra_args,
    )
    result = runner(command) if runner else run_capture(command, cwd=cwd)
    if result.returncode != 0:
        context = failure_context or f"probe compile failed for {repo_rel(probe_source)}"
        raise RuntimeError(f"{context}\n{failure_snippet(result)}")
    return result
