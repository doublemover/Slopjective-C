from __future__ import annotations

import subprocess
from pathlib import Path

from .compiler_runs import find_llvm_tool, run_compiler
from .manifest_loading import artifact_hashes, read, read_json, sha256
from .paths import (
    DETERMINISTIC_ARTIFACTS,
    NEGATIVE_FIXTURE,
    POSITIVE_FIXTURE,
    REQUIRED_ARTIFACTS,
    REQUIRED_IR_TOKENS,
    REQUIRED_MANIFEST_KEYS,
    REQUIRED_OBJECT_SECTIONS,
    REQUIRED_OBJECT_SYMBOLS,
    ROOT,
    SCRATCH,
    rel,
)


def inspect_object(obj_path: Path) -> dict:
    readobj = find_llvm_tool("llvm-readobj")
    objdump = find_llvm_tool("llvm-objdump")
    if not readobj or not objdump:
        return {
            "tools_available": False,
            "readobj": str(readobj) if readobj else "",
            "objdump": str(objdump) if objdump else "",
            "sections": {section: False for section in REQUIRED_OBJECT_SECTIONS},
            "symbols": {symbol: False for symbol in REQUIRED_OBJECT_SYMBOLS},
        }
    section_result = subprocess.run(
        [str(readobj), "--sections", str(obj_path)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    symbol_result = subprocess.run(
        [str(objdump), "--syms", str(obj_path)],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )
    return {
        "tools_available": section_result.returncode == 0 and symbol_result.returncode == 0,
        "readobj": rel(readobj) if readobj.is_relative_to(ROOT) else str(readobj),
        "objdump": rel(objdump) if objdump.is_relative_to(ROOT) else str(objdump),
        "sections": {
            section: section in section_result.stdout
            for section in REQUIRED_OBJECT_SECTIONS
        },
        "symbols": {
            symbol: symbol in symbol_result.stdout
            for symbol in REQUIRED_OBJECT_SYMBOLS
        },
    }


def build_positive_runs() -> dict:
    runs = {}
    for run_name in ("run1", "run2"):
        out_dir = SCRATCH / "positive" / run_name
        result = run_compiler(POSITIVE_FIXTURE, out_dir)
        artifacts = {name: (out_dir / name).is_file() for name in REQUIRED_ARTIFACTS}
        hashes = artifact_hashes(out_dir, DETERMINISTIC_ARTIFACTS)
        manifest = read_json(out_dir / "module.manifest.json") if artifacts["module.manifest.json"] else {}
        conformance = (
            read_json(out_dir / "module.objc3-conformance-report.json")
            if artifacts["module.objc3-conformance-report.json"]
            else {}
        )
        ir_text = read(out_dir / "module.ll") if artifacts["module.ll"] else ""
        runs[run_name] = {
            "out_dir": rel(out_dir),
            "exit_code": result.returncode,
            "compiled": result.returncode == 0,
            "artifacts": artifacts,
            "hashes": hashes,
            "ir_tokens": {token: token in ir_text for token in REQUIRED_IR_TOKENS},
            "manifest_keys": {key: key in manifest for key in REQUIRED_MANIFEST_KEYS},
            "object_backend": read(out_dir / "module.object-backend.txt").strip()
            if (out_dir / "module.object-backend.txt").is_file()
            else "",
            "conformance_report": {
                "schema_id": conformance.get("schema_id", ""),
                "ready": conformance.get("ready") is True,
                "runtime_capability_ready": conformance.get("runtime_capability_report", {}).get("ready") is True,
                "public_schema_id": conformance.get("public_conformance_report", {}).get("schema_id", ""),
            },
        }
    return runs


def build_negative_runs() -> dict:
    runs = {}
    for run_name in ("run1", "run2"):
        out_dir = SCRATCH / "negative" / run_name
        result = run_compiler(NEGATIVE_FIXTURE, out_dir)
        diagnostics_path = out_dir / "module.diagnostics.json"
        diagnostics = read_json(diagnostics_path).get("diagnostics", []) if diagnostics_path.is_file() else []
        observed_codes = sorted({item.get("code", "") for item in diagnostics})
        runs[run_name] = {
            "out_dir": rel(out_dir),
            "exit_code": result.returncode,
            "rejected": result.returncode != 0,
            "observed_codes": observed_codes,
            "expected_codes_present": "O3P150" in observed_codes and "O3S260" in observed_codes,
            "emitted_artifacts_absent": {
                name: not (out_dir / name).is_file()
                for name in ["module.manifest.json", "module.ll", "module.obj"]
            },
            "diagnostics_hash": sha256(diagnostics_path) if diagnostics_path.is_file() else "",
        }
    return runs
