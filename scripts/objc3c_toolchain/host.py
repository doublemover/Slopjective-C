"""Host admission and resource limits for reproducible native LLVM builds."""
from __future__ import annotations

import hashlib
import math
import os
import platform
import re
import shutil
import subprocess
import sys
from pathlib import Path
from typing import Any


def _output(command: list[str]) -> str:
    return subprocess.check_output(command, text=True, stderr=subprocess.STDOUT,
                                   timeout=30).strip()


def _digest(path: Path) -> str:
    with path.open("rb") as stream:
        return hashlib.file_digest(stream, "sha256").hexdigest()


def resource_limits(build: dict[str, Any]) -> dict[str, int]:
    cpus = getattr(os, "process_cpu_count", os.cpu_count)() or 1
    try:
        quota, period = Path("/sys/fs/cgroup/cpu.max").read_text().split()
        if quota != "max":
            cpus = min(cpus, max(1, math.ceil(int(quota) / int(period))))
    except (OSError, ValueError):
        pass
    memory = os.sysconf("SC_PHYS_PAGES") * os.sysconf("SC_PAGE_SIZE")
    try:
        maximum = Path("/sys/fs/cgroup/memory.max").read_text().strip()
        if maximum != "max":
            memory = min(memory, int(maximum))
    except (OSError, ValueError):
        pass
    gib = 1024 ** 3
    usable = max(gib, memory - 2 * gib)
    return {"cpu_count": cpus, "memory_bytes": memory,
            "compile_jobs": max(1, min(build["compile_jobs"], cpus, usable // (2 * gib))),
            "link_jobs": max(1, min(build["link_jobs"], usable // (6 * gib)))}


def linux_environment(manifest: dict[str, Any]) -> dict[str, Any]:
    if sys.platform != "linux" or platform.machine().lower() not in {"x86_64", "amd64"}:
        raise ValueError("Linux foundation requires a native Linux x86-64 host")
    expected = manifest["upstream"]["tag"].removeprefix("llvmorg-")
    root_value = os.environ.get("OBJC3C_LLVM_ROOT") or os.environ.get("LLVM_ROOT")
    root = Path(root_value).resolve() if root_value else None
    paths = {}
    for tool in ("clang", "clang++", "ld.lld", "llvm-config"):
        found = str(root / "bin" / tool) if root else shutil.which(tool)
        if not found or not Path(found).is_file():
            raise ValueError(f"Linux foundation requires {tool} from LLVM {expected}")
        paths[tool] = Path(found).absolute()
    roots = {path.resolve().parent.parent for path in paths.values()}
    if len(roots) != 1:
        raise ValueError("Linux bootstrap tools must resolve from one coherent LLVM root")
    if _output([str(paths["llvm-config"]), "--version"]) != expected:
        raise ValueError(f"Linux bootstrap LLVM must be exactly {expected}")
    tool_versions = {}
    for name in ("clang", "clang++", "ld.lld"):
        text = _output([str(paths[name]), "--version"])
        if not re.search(r"(?<![\d.])" + re.escape(expected) + r"(?![\d.])", text):
            raise ValueError(f"Linux bootstrap {name} must be exactly {expected}")
        tool_versions[name] = text
    tools = {}
    for name in ("cmake", "ninja"):
        found = shutil.which(name)
        if not found:
            raise ValueError(f"{name} must be available on PATH")
        tools[name] = Path(found).resolve()
    python = Path(sys.executable).resolve()
    python_version = _output([str(python), "--version"])
    required_python = manifest.get("host_tools", {}).get("python")
    if required_python and python_version != f"Python {required_python}":
        raise ValueError(f"Python {required_python} is required; found {python_version}")
    ccache = shutil.which("ccache")
    return {"host_toolset": "Clang/LLD", "target_triple": "x86_64-unknown-linux-gnu",
            "clang_path": str(paths["clang"]), "clangxx_path": str(paths["clang++"]),
            "link_path": str(paths["ld.lld"]), "bootstrap_root": str(next(iter(roots))),
            "clang_sha256": _digest(paths["clang"]),
            "link_sha256": _digest(paths["ld.lld"]),
            "llvm_version": expected, "python_executable": str(python),
            "bootstrap_tool_versions": tool_versions,
            "python_version": python_version, "python_sha256": _digest(python),
            "cmake_path": str(tools["cmake"]),
            "cmake_version": _output([str(tools["cmake"]), "--version"]).splitlines()[0],
            "ninja_path": str(tools["ninja"]),
            "ninja_version": _output([str(tools["ninja"]), "--version"]),
            "ccache_path": str(Path(ccache).resolve()) if ccache else None,
            "resource_limits": resource_limits(manifest["build"])}
