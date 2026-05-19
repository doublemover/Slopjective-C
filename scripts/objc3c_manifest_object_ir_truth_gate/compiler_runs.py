from __future__ import annotations

import os
import shutil
import subprocess
from pathlib import Path

from .paths import COMPILER, ROOT


def find_llvm_tool(name: str) -> Path | None:
    found = shutil.which(name)
    if found:
        return Path(found)
    llvm_root = os.environ.get("LLVM_ROOT")
    candidates = []
    if llvm_root:
        candidates.append(Path(llvm_root) / "bin" / f"{name}.exe")
    candidates.append(Path("C:/Program Files/LLVM/bin") / f"{name}.exe")
    for candidate in candidates:
        if candidate.is_file():
            return candidate
    return None


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
