from __future__ import annotations

import os
import platform
import shutil
from pathlib import Path
from typing import Iterable


DEFAULT_LLVM_VERSION = "22.1.6"
CORE_LLVM_TOOLS = ("clang", "clang++", "llc", "llvm-ar")


def _tool_executable_name(tool_name: str) -> str:
    if os.name == "nt" and not tool_name.endswith(".exe"):
        return f"{tool_name}.exe"
    return tool_name


def _configured_llvm_roots(explicit_root: str | os.PathLike[str] | None = None) -> Iterable[Path]:
    if explicit_root:
        yield Path(explicit_root)

    for env_name in ("OBJC3C_LLVM_ROOT", "LLVM_ROOT"):
        configured = os.environ.get(env_name)
        if configured:
            yield Path(configured)

    version = os.environ.get("OBJC3C_CI_LLVM_VERSION", DEFAULT_LLVM_VERSION)
    host_system = platform.system().lower()
    if os.name == "nt":
        user_profile = os.environ.get("USERPROFILE")
        if user_profile:
            yield Path(user_profile) / "Tools" / "LLVM" / f"llvm-{version}-msvc"
        yield Path("C:/Program Files/LLVM")
    elif host_system == "darwin":
        yield Path("/opt/homebrew/opt/llvm")
        yield Path("/usr/local/opt/llvm")
    else:
        for root in (
            "/usr/lib/llvm-22",
            "/usr/lib/llvm-21",
            "/usr/lib/llvm-20",
            "/usr/lib/llvm-19",
            "/usr/lib/llvm-18",
            "/usr/lib/llvm",
            "/usr/local/llvm",
        ):
            yield Path(root)


def llvm_root_candidates(explicit_root: str | os.PathLike[str] | None = None) -> list[Path]:
    seen: set[str] = set()
    candidates: list[Path] = []
    for root in _configured_llvm_roots(explicit_root):
        key = str(root).lower() if os.name == "nt" else str(root)
        if key in seen:
            continue
        seen.add(key)
        candidates.append(root)
    return candidates


def find_llvm_tool_path(
    tool_name: str,
    *,
    llvm_root: str | os.PathLike[str] | None = None,
    include_path: bool = True,
) -> Path | None:
    executable_name = _tool_executable_name(tool_name)
    for root in llvm_root_candidates(llvm_root):
        candidate = root / "bin" / executable_name
        if candidate.is_file():
            return candidate

    if include_path:
        resolved = shutil.which(tool_name) or shutil.which(executable_name)
        if resolved:
            return Path(resolved)
    return None


def is_complete_llvm_root(root: Path) -> bool:
    return all(
        (root / "bin" / _tool_executable_name(tool)).is_file()
        for tool in CORE_LLVM_TOOLS
    )


def default_llvm_tool_path(tool_name: str) -> Path:
    for root in llvm_root_candidates():
        if not is_complete_llvm_root(root):
            continue
        candidate = root / "bin" / _tool_executable_name(tool_name)
        if candidate.is_file():
            return candidate
    return Path(tool_name)
