from __future__ import annotations

import json
import shutil
import subprocess
from pathlib import Path

from objc3c_runtime_backed_semantics_closure.inputs import NEGATIVE_FIXTURES
from objc3c_runtime_backed_semantics_closure.inputs import POSITIVE_FIXTURE_EXTRA_ARGS
from objc3c_runtime_backed_semantics_closure.inputs import POSITIVE_FIXTURES
from objc3c_runtime_backed_semantics_closure.paths import COMPILER
from objc3c_runtime_backed_semantics_closure.paths import ROOT
from objc3c_runtime_backed_semantics_closure.paths import SCRATCH
from objc3c_runtime_backed_semantics_closure.paths import read
from objc3c_runtime_backed_semantics_closure.paths import rel


def run_compiler(
    source: Path,
    out_dir: Path,
    extra_args: list[str] | None = None,
) -> subprocess.CompletedProcess[str]:
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
            *(extra_args or []),
        ],
        cwd=ROOT,
        text=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        check=False,
    )


def read_diagnostics(path: Path) -> list[dict]:
    if not path.is_file():
        return []
    return json.loads(read(path)).get("diagnostics", [])


def compile_positive_fixtures() -> tuple[dict[str, dict], str]:
    compile_results: dict[str, dict] = {}
    ir_chunks: list[str] = []
    for name, fixture in POSITIVE_FIXTURES.items():
        out_dir = SCRATCH / "positive" / name
        result = run_compiler(
            fixture,
            out_dir,
            POSITIVE_FIXTURE_EXTRA_ARGS.get(name),
        )
        ir_path = out_dir / "module.ll"
        manifest_path = out_dir / "module.manifest.json"
        ir_text = read(ir_path) if ir_path.is_file() else ""
        if ir_text:
            ir_chunks.append(ir_text)
        compile_results[name] = {
            "fixture": rel(fixture),
            "exit_code": result.returncode,
            "compiled": result.returncode == 0,
            "ir_exists": ir_path.is_file(),
            "manifest_exists": manifest_path.is_file(),
        }
    return compile_results, "\n".join(ir_chunks)


def compile_negative_fixtures() -> dict[str, dict]:
    compile_results: dict[str, dict] = {}
    for name, spec in NEGATIVE_FIXTURES.items():
        fixture = spec["path"]
        expected_codes = spec["codes"]
        out_dir = SCRATCH / "negative" / name
        result = run_compiler(fixture, out_dir)
        diagnostics = read_diagnostics(out_dir / "module.diagnostics.json")
        observed_codes = sorted({diagnostic.get("code", "") for diagnostic in diagnostics})
        compile_results[name] = {
            "fixture": rel(fixture),
            "exit_code": result.returncode,
            "rejected": result.returncode != 0,
            "expected_codes": expected_codes,
            "observed_codes": observed_codes,
            "expected_codes_present": all(code in observed_codes for code in expected_codes),
        }
    return compile_results
