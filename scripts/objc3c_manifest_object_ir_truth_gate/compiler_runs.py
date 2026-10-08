from __future__ import annotations

import subprocess
from pathlib import Path

from objc3c_tooling.llvm_discovery import find_llvm_tool_path

from .paths import COMPILER, ROOT


def find_llvm_tool(name: str) -> Path | None:
    return find_llvm_tool_path(name)


def run_compiler(source: Path, out_dir: Path) -> subprocess.CompletedProcess[str]:
    if out_dir.exists():
        shutil.rmtree(out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    return subprocess.run(
        [
            str(COMPILER),
            str(source),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
