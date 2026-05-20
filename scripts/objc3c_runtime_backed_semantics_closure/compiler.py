from __future__ import annotations

import json
import shutil
import subprocess
from pathlib import Path

from objc3c_runtime_backed_semantics_closure.inputs import NEGATIVE_FIXTURES
from objc3c_runtime_backed_semantics_closure.inputs import POSITIVE_FIXTURE_EXTRA_ARGS
from objc3c_runtime_backed_semantics_closure.inputs import POSITIVE_FIXTURE_IMPORT_SURFACE_EXPECTATIONS
from objc3c_runtime_backed_semantics_closure.inputs import POSITIVE_FIXTURE_IR_CALL_TOKENS
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


def _check_ir_call_tokens(name: str, ir_text: str) -> dict[str, bool]:
    return {
        token: token in ir_text
        for token in POSITIVE_FIXTURE_IR_CALL_TOKENS.get(name, [])
    }


def _parse_replay_key_counts(replay_key: object) -> dict[str, int]:
    counts: dict[str, int] = {}
    if not isinstance(replay_key, str):
        return counts
    for segment in replay_key.split(";"):
        if "=" not in segment:
            continue
        key, value = segment.split("=", 1)
        if value.isdigit():
            counts[key] = int(value)
    return counts


def _check_runtime_import_surface(
    name: str,
    out_dir: Path,
) -> dict[str, bool]:
    expectations = POSITIVE_FIXTURE_IMPORT_SURFACE_EXPECTATIONS.get(name, {})
    if not expectations:
        return {}

    import_surface_path = out_dir / "module.runtime-import-surface.json"
    result = {
        "module.runtime-import-surface.json exists": import_surface_path.is_file(),
    }
    if not import_surface_path.is_file():
        return result

    import_surface = json.loads(read(import_surface_path))
    result["module_metadata_import_surface_landed"] = (
        import_surface.get("module_metadata_import_surface_landed") is True
    )
    result["public_frontend_api_module_surface_landed"] = (
        import_surface.get("public_frontend_api_module_surface_landed") is True
    )

    for surface_name, surface_expectations in expectations.items():
        surface = import_surface.get(surface_name)
        result[f"{surface_name} exists"] = isinstance(surface, dict)
        if not isinstance(surface, dict):
            continue

        for field, expected in surface_expectations.get("booleans", {}).items():
            result[f"{surface_name}.{field}"] = surface.get(field) is expected
        for field, minimum in surface_expectations.get("minimums", {}).items():
            value = surface.get(field)
            result[f"{surface_name}.{field} >= {minimum}"] = (
                isinstance(value, int) and value >= minimum
            )
        for replay_key_field, minimums in surface_expectations.get(
            "replay_key_minimums", {}
        ).items():
            counts = _parse_replay_key_counts(surface.get(replay_key_field))
            for field, minimum in minimums.items():
                result[
                    f"{surface_name}.{replay_key_field}.{field} >= {minimum}"
                ] = counts.get(field, 0) >= minimum

    return result


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
            "ir_call_tokens": _check_ir_call_tokens(name, ir_text),
            "runtime_import_surface": _check_runtime_import_surface(name, out_dir),
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
