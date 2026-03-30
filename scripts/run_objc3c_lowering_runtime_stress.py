#!/usr/bin/env python3
"""Run bounded lowering and runtime stress validation over checked-in fixtures."""

from __future__ import annotations

import argparse
import json
import os
import shutil
import subprocess
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
PWSH = shutil.which("pwsh") or "pwsh"
BUILD_PS1 = ROOT / "scripts" / "build_objc3c_native.ps1"
COMPILE_PS1 = ROOT / "scripts" / "objc3c_native_compile.ps1"
EXECUTION_SMOKE_PS1 = ROOT / "scripts" / "check_objc3c_native_execution_smoke.ps1"
MANIFEST_PATH = ROOT / "tests" / "tooling" / "fixtures" / "stress" / "lowering_runtime_stress_manifest.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "stress" / "lowering-runtime-stress-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.stress.lowering-runtime.summary.v1"


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"expected JSON object at {repo_rel(path)}")
    return payload


def run_capture(command: Sequence[str], *, env: dict[str, str] | None = None) -> subprocess.CompletedProcess[str]:
    child_env = os.environ.copy()
    child_env["PYTHONDONTWRITEBYTECODE"] = "1"
    if env:
        child_env.update(env)
    result = subprocess.run(
        list(command),
        cwd=ROOT,
        check=False,
        text=True,
        capture_output=True,
        env=child_env,
    )
    if result.stdout:
        sys.stdout.write(result.stdout)
    if result.stderr:
        sys.stderr.write(result.stderr)
    return result


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--manifest", type=Path, default=MANIFEST_PATH)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_PATH)
    parser.add_argument("--contract-mode", action="store_true")
    return parser.parse_args(argv)


def load_manifest(path: Path) -> dict[str, list[str]]:
    payload = load_json(path)
    if payload.get("contract_id") != "objc3c.stress.lowering-runtime.manifest.v1":
        raise RuntimeError("lowering/runtime stress manifest contract_id drifted")
    if payload.get("schema_version") != 1:
        raise RuntimeError("lowering/runtime stress manifest schema_version drifted")
    compile_cases = payload.get("compile_cases")
    execution_cases = payload.get("execution_cases")
    if not isinstance(compile_cases, list) or not compile_cases:
        raise RuntimeError("lowering/runtime stress manifest missing compile_cases")
    if not isinstance(execution_cases, list) or not execution_cases:
        raise RuntimeError("lowering/runtime stress manifest missing execution_cases")
    for relative_path in [*compile_cases, *execution_cases]:
        if not isinstance(relative_path, str) or not relative_path:
            raise RuntimeError("lowering/runtime stress manifest contains a non-string case path")
        if not (ROOT / relative_path).is_file():
            raise RuntimeError(f"lowering/runtime stress manifest references missing case {relative_path}")
    return {
        "compile_cases": [str(item) for item in compile_cases],
        "execution_cases": [str(item) for item in execution_cases],
    }


def build_native_binaries() -> None:
    result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(BUILD_PS1),
            "-ExecutionMode",
            "binaries-only",
        ]
    )
    if result.returncode != 0:
        raise RuntimeError("native binary build failed for lowering/runtime stress")


def compile_cases(case_paths: list[str], run_root: Path) -> list[dict[str, Any]]:
    records: list[dict[str, Any]] = []
    compile_root = run_root / "compile"
    for relative_path in case_paths:
        case_name = Path(relative_path).stem
        out_dir = compile_root / case_name
        result = run_capture(
            [
                PWSH,
                "-NoProfile",
                "-ExecutionPolicy",
                "Bypass",
                "-File",
                str(COMPILE_PS1),
                relative_path,
                "--out-dir",
                out_dir.relative_to(ROOT).as_posix(),
                "--emit-prefix",
                "module",
            ]
        )
        if result.returncode != 0:
            raise RuntimeError(f"compile stress case failed: {relative_path}")
        manifest_path = out_dir / "module.manifest.json"
        llvm_ir_path = out_dir / "module.ll"
        if not manifest_path.is_file() or not llvm_ir_path.is_file():
            raise RuntimeError(f"compile stress case missing emitted artifacts: {relative_path}")
        records.append(
            {
                "source": relative_path,
                "out_dir": repo_rel(out_dir),
                "manifest_path": repo_rel(manifest_path),
                "llvm_ir_path": repo_rel(llvm_ir_path),
            }
        )
    return records


def run_execution_subset(case_paths: list[str], run_root: Path) -> dict[str, Any]:
    fixture_list_path = run_root / "execution-fixture-list.txt"
    fixture_list_path.parent.mkdir(parents=True, exist_ok=True)
    fixture_list_path.write_text("\n".join(case_paths) + "\n", encoding="utf-8")
    run_id = run_root.name
    result = run_capture(
        [
            PWSH,
            "-NoProfile",
            "-ExecutionPolicy",
            "Bypass",
            "-File",
            str(EXECUTION_SMOKE_PS1),
            "-FixtureList",
            fixture_list_path.relative_to(ROOT).as_posix(),
        ],
        env={"OBJC3C_NATIVE_EXECUTION_RUN_ID": run_id},
    )
    if result.returncode != 0:
        raise RuntimeError("execution smoke subset failed during lowering/runtime stress")
    smoke_summary_path = ROOT / "tmp" / "artifacts" / "objc3c-native" / "execution-smoke" / run_id / "summary.json"
    smoke_summary = load_json(smoke_summary_path)
    if smoke_summary.get("status") != "PASS":
        raise RuntimeError("execution smoke summary did not report PASS")
    return {
        "fixture_list": repo_rel(fixture_list_path),
        "summary_path": repo_rel(smoke_summary_path),
        "selected_positive": smoke_summary.get("selection", {}).get("selected_positive"),
        "selected_negative": smoke_summary.get("selection", {}).get("selected_negative"),
    }


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    manifest = load_manifest(args.manifest.resolve())
    build_native_binaries()

    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    run_root = ROOT / "tmp" / "artifacts" / "stress" / "lowering-runtime" / run_id
    run_root.mkdir(parents=True, exist_ok=True)

    compile_records = compile_cases(manifest["compile_cases"], run_root)
    execution_summary = run_execution_subset(manifest["execution_cases"], run_root)

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "manifest_path": repo_rel(args.manifest.resolve()),
        "run_root": repo_rel(run_root),
        "compile_case_count": len(compile_records),
        "execution_case_count": len(manifest["execution_cases"]),
        "compile_records": compile_records,
        "execution_summary": execution_summary,
    }
    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    if args.contract_mode:
        sys.stdout.write(json.dumps(payload, indent=2) + "\n")
    else:
        print(f"summary_path: {repo_rel(args.summary_out)}")
        print("objc3c-lowering-runtime-stress: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
