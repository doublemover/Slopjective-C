from __future__ import annotations

import shutil
import subprocess
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_any as load_json

from objc3c_effects_ownership_semantic_model.contracts import CONTRACT_ID
from objc3c_effects_ownership_semantic_model.paths import COMPILER
from objc3c_effects_ownership_semantic_model.paths import ROOT
from objc3c_effects_ownership_semantic_model.paths import rel


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
    manifest = None
    if manifest_path.is_file():
        manifest = load_json(manifest_path)
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


def find_effects_model(node: Any) -> dict[str, Any] | None:
    if isinstance(node, dict):
        if node.get("contract_id") == CONTRACT_ID:
            return node
        for value in node.values():
            found = find_effects_model(value)
            if found is not None:
                return found
    elif isinstance(node, list):
        for value in node:
            found = find_effects_model(value)
            if found is not None:
                return found
    return None
