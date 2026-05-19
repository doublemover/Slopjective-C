from __future__ import annotations

import shutil
import subprocess
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_any as load_json
from objc3c_type_semantic_model_closure.reporting import SUMMARY_FIELDS


def _rel(root: Path, path: Path) -> str:
    return path.relative_to(root).as_posix()


def run_compiler(
    root: Path,
    compiler: Path,
    source: Path,
    out_dir: Path,
    extra_args: list[str] | None = None,
) -> dict[str, Any]:
    if not compiler.is_file():
        raise SystemExit(f"missing native compiler at {_rel(root, compiler)}; run scripts/build_objc3c_native.ps1 first")
    if out_dir.exists():
        shutil.rmtree(out_dir)
    out_dir.mkdir(parents=True, exist_ok=True)
    command = [str(compiler), str(source), "--out-dir", str(out_dir), "--emit-prefix", "module"]
    if extra_args:
        command.extend(extra_args)
    completed = subprocess.run(
        command,
        cwd=root,
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
        "source": _rel(root, source),
        "out_dir": _rel(root, out_dir),
        "runtime_import_surface_path": _rel(root, out_dir / "module.runtime-import-surface.json")
        if (out_dir / "module.runtime-import-surface.json").is_file()
        else None,
        "exit_code": completed.returncode,
        "stdout": completed.stdout.strip(),
        "stderr": completed.stderr.strip(),
        "diagnostics_path": _rel(root, diagnostics_path) if diagnostics_path.is_file() else None,
        "manifest_path": _rel(root, manifest_path) if manifest_path.is_file() else None,
        "llvm_ir_path": _rel(root, llvm_ir_path) if llvm_ir_path.is_file() else None,
        "diagnostics": diagnostics,
        "manifest": manifest,
    }


def find_type_semantic_model_manifest(node: Any) -> dict[str, Any] | None:
    if isinstance(node, dict):
        if set(SUMMARY_FIELDS).issubset(node.keys()):
            return node
        for value in node.values():
            found = find_type_semantic_model_manifest(value)
            if found is not None:
                return found
    elif isinstance(node, list):
        for value in node:
            found = find_type_semantic_model_manifest(value)
            if found is not None:
                return found
    return None


def nested_semantic_model(manifest: dict[str, Any] | None) -> dict[str, Any] | None:
    if not isinstance(manifest, dict):
        return None
    node: Any = manifest
    for key in ["frontend", "pipeline", "semantic_surface", "objc_type_system_type_semantic_model"]:
        if not isinstance(node, dict) or key not in node:
            return find_type_semantic_model_manifest(manifest)
        node = node[key]
    return node if isinstance(node, dict) else find_type_semantic_model_manifest(manifest)


def diagnostic_matches(diagnostics: list[dict[str, Any]], code: str, line: int, column: int) -> bool:
    return any(
        diag.get("code") == code
        and int(diag.get("line", -1)) == line
        and int(diag.get("column", -1)) == column
        for diag in diagnostics
    )
