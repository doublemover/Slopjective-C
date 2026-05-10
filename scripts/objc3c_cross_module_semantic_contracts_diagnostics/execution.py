from __future__ import annotations

import shutil
import subprocess
from pathlib import Path
from typing import Any

from objc3c_cross_module_semantic_contracts_diagnostics.paths import COMPILER
from objc3c_cross_module_semantic_contracts_diagnostics.paths import ROOT
from objc3c_cross_module_semantic_contracts_diagnostics.paths import rel
from objc3c_tooling.json_io import load_json_any as load_json


def run_compiler(source: Path, out_dir: Path) -> dict[str, Any]:
    if not COMPILER.is_file():
        raise SystemExit(f"missing native compiler at {rel(COMPILER)}; run scripts/build_objc3c_native.ps1 first")
    if out_dir.exists():
        shutil.rmtree(out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    completed = subprocess.run(
        [str(COMPILER), str(source), "--out-dir", str(out_dir), "--emit-prefix", "module"],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    diagnostics_path = out_dir / "module.diagnostics.json"
    manifest_path = out_dir / "module.manifest.json"
    llvm_ir_path = out_dir / "module.ll"
    diagnostics = []
    if diagnostics_path.is_file():
        diagnostics = load_json(diagnostics_path).get("diagnostics", [])
    manifest = load_json(manifest_path) if manifest_path.is_file() else None
    return {
        "source": rel(source),
        "out_dir": rel(out_dir),
        "exit_code": completed.returncode,
        "stdout": completed.stdout.strip(),
        "stderr": completed.stderr.strip(),
        "diagnostics_path": rel(diagnostics_path) if diagnostics_path.is_file() else None,
        "manifest_path": rel(manifest_path) if manifest_path.is_file() else None,
        "llvm_ir_path": rel(llvm_ir_path) if llvm_ir_path.is_file() else None,
        "diagnostics": diagnostics,
        "manifest": manifest,
    }
