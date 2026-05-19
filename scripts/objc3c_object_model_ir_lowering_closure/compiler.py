from __future__ import annotations

import re
import shutil
import subprocess
from pathlib import Path

from objc3c_object_model_ir_lowering_closure.paths import COMPILER
from objc3c_object_model_ir_lowering_closure.paths import ROOT


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


def parse_ivar_offsets(ir_text: str) -> dict[str, int]:
    name_by_index: dict[str, str] = {}
    offset_by_index: dict[str, int] = {}
    for match in re.finditer(
        r"@__objc3_meta_ivar_property_name_(\d+)\s*=\s*private constant \[\d+ x i8\] c\"([^\"]*)\\00\"",
        ir_text,
    ):
        name_by_index[match.group(1)] = match.group(2)
    for match in re.finditer(
        r"@__objc3_meta_ivar_offset_(\d+)\s*=\s*private global i64 (\d+)",
        ir_text,
    ):
        offset_by_index[match.group(1)] = int(match.group(2))
    return {
        name: offset_by_index[index]
        for index, name in name_by_index.items()
        if index in offset_by_index
    }
