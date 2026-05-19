from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PASS_MANAGER_CONTRACT = (
    ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager_contract.h"
)
PASS_MANAGER_HEADER = (
    ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.h"
)
PASS_MANAGER_SOURCE = (
    ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_sema_pass_manager.cpp"
)
PIPELINE_SEMA_STAGE_RUNNER = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "frontend_pipeline_sema_stage_runner.cpp"
)
SEMA_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "sema" / "CMakeLists.txt"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"


def read_expanded_source(path: Path, seen: set[Path] | None = None) -> str:
    if seen is None:
        seen = set()
    if path in seen:
        return ""
    seen.add(path)
    text = path.read_text(encoding="utf-8")
    expanded: list[str] = []
    for line in text.splitlines():
        expanded.append(line)
        stripped = line.strip()
        if not stripped.startswith('#include "'):
            continue
        include_path = stripped.split('"', 2)[1]
        target = ROOT / "native" / "objc3c" / "src" / include_path
        if target.exists():
            expanded.append(read_expanded_source(target, seen))
    return "\n".join(expanded)
