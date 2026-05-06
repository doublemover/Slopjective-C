#!/usr/bin/env python3
"""Run the behavior-first native fixture matrix from tests/native."""

from __future__ import annotations

import argparse
import shutil
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Sequence

from objc3c_tooling.behavior_fixtures import (
    BehaviorFixture,
    REQUIRED_TREE,
    load_behavior_fixtures,
)
from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import ROOT, repo_rel
from objc3c_tooling.subprocesses import CommandExecution, bounded_text, run_timed

NATIVE_EXE = ROOT / "artifacts" / "bin" / "objc3c-native.exe"
RUNTIME_LIB = ROOT / "artifacts" / "lib" / "objc3_runtime.lib"
BUILD_SCRIPT = ROOT / "scripts" / "build_objc3c_native.ps1"
REPORT_PATH = ROOT / "tmp" / "reports" / "objc3c-behavior-matrix" / "summary.json"
CONTRACT_ID = "objc3c.behavior.fixture.matrix.v1"


class BehaviorMatrixFailure(RuntimeError):
    pass


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--phase", choices=sorted(REQUIRED_TREE), action="append", default=[])
    parser.add_argument("--limit", type=int, default=0)
    parser.add_argument("--report-out", type=Path, default=REPORT_PATH)
    return parser.parse_args(argv)


def ensure_native_binaries() -> None:
    if NATIVE_EXE.is_file() and RUNTIME_LIB.is_file():
        return
    pwsh = shutil.which("pwsh") or "pwsh"
    result = run_timed(
        [pwsh, "-NoProfile", "-ExecutionPolicy", "Bypass", "-File", BUILD_SCRIPT, "-ExecutionMode", "binaries-only"],
        metadata={"stage": "build-native-binaries"},
    )
    if result.returncode != 0:
        raise BehaviorMatrixFailure(
            "native binary build failed before behavior matrix:\n"
            f"{bounded_text(result.stdout + result.stderr)}"
        )
    if not NATIVE_EXE.is_file():
        raise BehaviorMatrixFailure(f"native compiler missing after build: {repo_rel(NATIVE_EXE)}")
    if not RUNTIME_LIB.is_file():
        raise BehaviorMatrixFailure(f"runtime library missing after build: {repo_rel(RUNTIME_LIB)}")


def select_fixtures(
    fixtures: list[BehaviorFixture],
    *,
    phases: Sequence[str],
    limit: int,
) -> list[BehaviorFixture]:
    selected = [fixture for fixture in fixtures if not phases or fixture.owner_phase in phases]
    if limit < 0:
        raise BehaviorMatrixFailure("--limit must be non-negative")
    if limit:
        selected = selected[:limit]
    if not selected:
        raise BehaviorMatrixFailure("no behavior fixtures matched the requested selection")
    return selected


def missing_tokens(text: str, tokens: Sequence[str]) -> list[str]:
    return [token for token in tokens if token not in text]


def compile_fixture(fixture: BehaviorFixture, case_dir: Path) -> CommandExecution:
    out_dir = case_dir / "compile"
    out_dir.mkdir(parents=True, exist_ok=True)
    return run_timed(
        [
            NATIVE_EXE,
            fixture.source_path,
            "--out-dir",
            out_dir,
            "--emit-prefix",
            "module",
            *fixture.native_compile_args,
        ],
        metadata={
            "stage": "compile",
            "fixture": fixture.relative_source,
        },
    )


def compile_output_text(result: CommandExecution, compile_dir: Path) -> str:
    diagnostics_path = compile_dir / "module.diagnostics.txt"
    diagnostics = diagnostics_path.read_text(encoding="utf-8") if diagnostics_path.is_file() else ""
    return "\n".join(part for part in (diagnostics, result.stdout, result.stderr) if part)


def object_path(compile_dir: Path) -> Path:
    obj_path = compile_dir / "module.obj"
    if not obj_path.is_file():
        raise BehaviorMatrixFailure(f"compile did not publish object artifact: {repo_rel(obj_path)}")
    return obj_path


def clang_command() -> str:
    return shutil.which("clang") or "clang"


def link_fixture(
    fixture: BehaviorFixture,
    case_dir: Path,
    compile_dir: Path,
    *,
    include_runtime: bool,
) -> CommandExecution:
    exe_path = case_dir / "module.exe"
    command: list[object] = [clang_command(), object_path(compile_dir)]
    if include_runtime:
        command.append(RUNTIME_LIB)
    command.extend(["-o", exe_path, "-fno-color-diagnostics"])
    return run_timed(
        command,
        metadata={
            "stage": "link",
            "fixture": fixture.relative_source,
            "include_runtime": include_runtime,
        },
    )


def canonical_link_text(fixture: BehaviorFixture, link_result: CommandExecution, compile_dir: Path) -> str:
    text = "\n".join(part for part in (link_result.stdout, link_result.stderr) if part)
    symbol = fixture.runtime_dispatch_symbol
    canonical_lines: list[str] = []
    if symbol and symbol in text:
        canonical_lines.append(f"link.unresolved_symbol:{symbol}")
    if object_path(compile_dir).name.startswith("module."):
        canonical_lines.append("link.input_object_basename:module.")
    return "\n".join([text, *canonical_lines])


def run_executable(fixture: BehaviorFixture, case_dir: Path) -> CommandExecution:
    return run_timed(
        [case_dir / "module.exe"],
        metadata={
            "stage": "run",
            "fixture": fixture.relative_source,
        },
    )


def assert_tokens(fixture: BehaviorFixture, text: str) -> None:
    missing = missing_tokens(text, fixture.required_tokens)
    if missing:
        raise BehaviorMatrixFailure(
            f"missing expected token(s) for {fixture.relative_source}: {', '.join(missing)}\n"
            f"{bounded_text(text)}"
        )


def execute_fixture(fixture: BehaviorFixture, case_dir: Path) -> dict[str, object]:
    compile_result = compile_fixture(fixture, case_dir)
    compile_dir = case_dir / "compile"
    compile_text = compile_output_text(compile_result, compile_dir)
    expected_stage = fixture.expected_stage

    if expected_stage in {"parse", "compile"}:
        if fixture.fixture_kind in {"negative", "strict-error", "rejection"}:
            if compile_result.returncode == 0:
                raise BehaviorMatrixFailure(f"expected compile failure for {fixture.relative_source}")
            assert_tokens(fixture, compile_text)
        elif compile_result.returncode != 0:
            raise BehaviorMatrixFailure(
                f"compile failed for {fixture.relative_source}:\n{bounded_text(compile_text)}"
            )
        return {
            "fixture": fixture.relative_source,
            "metadata": fixture.relative_metadata,
            "owner_phase": fixture.owner_phase,
            "behavior_family": fixture.behavior_family,
            "fixture_kind": fixture.fixture_kind,
            "expected_stage": expected_stage,
            "compile": compile_result.to_dict(include_output=False),
            "status": "PASS",
        }

    if compile_result.returncode != 0:
        raise BehaviorMatrixFailure(
            f"compile failed before {expected_stage} check for {fixture.relative_source}:\n"
            f"{bounded_text(compile_text)}"
        )

    if expected_stage == "link":
        link_result = link_fixture(fixture, case_dir, compile_dir, include_runtime=False)
        link_text = canonical_link_text(fixture, link_result, compile_dir)
        if fixture.fixture_kind in {"negative", "strict-error", "rejection"}:
            if link_result.returncode == 0:
                raise BehaviorMatrixFailure(f"expected link failure for {fixture.relative_source}")
            assert_tokens(fixture, link_text)
        elif link_result.returncode != 0:
            raise BehaviorMatrixFailure(
                f"link failed for {fixture.relative_source}:\n{bounded_text(link_text)}"
            )
        return {
            "fixture": fixture.relative_source,
            "metadata": fixture.relative_metadata,
            "owner_phase": fixture.owner_phase,
            "behavior_family": fixture.behavior_family,
            "fixture_kind": fixture.fixture_kind,
            "expected_stage": expected_stage,
            "compile": compile_result.to_dict(include_output=False),
            "link": link_result.to_dict(include_output=False),
            "status": "PASS",
        }

    if expected_stage == "run":
        link_result = link_fixture(fixture, case_dir, compile_dir, include_runtime=True)
        link_text = "\n".join(part for part in (link_result.stdout, link_result.stderr) if part)
        if link_result.returncode != 0:
            raise BehaviorMatrixFailure(
                f"link failed before run for {fixture.relative_source}:\n{bounded_text(link_text)}"
            )
        run_result = run_executable(fixture, case_dir)
        run_text = "\n".join(part for part in (run_result.stdout, run_result.stderr) if part)
        if fixture.fixture_kind in {"negative", "strict-error", "rejection"}:
            if run_result.returncode == 0:
                raise BehaviorMatrixFailure(f"expected run failure for {fixture.relative_source}")
            assert_tokens(fixture, run_text)
        elif run_result.returncode != fixture.expected_exit_code:
            raise BehaviorMatrixFailure(
                f"unexpected run exit for {fixture.relative_source}: "
                f"expected={fixture.expected_exit_code} actual={run_result.returncode}\n"
                f"{bounded_text(run_text)}"
            )
        return {
            "fixture": fixture.relative_source,
            "metadata": fixture.relative_metadata,
            "owner_phase": fixture.owner_phase,
            "behavior_family": fixture.behavior_family,
            "fixture_kind": fixture.fixture_kind,
            "expected_stage": expected_stage,
            "compile": compile_result.to_dict(include_output=False),
            "link": link_result.to_dict(include_output=False),
            "run": run_result.to_dict(include_output=False),
            "expected_exit_code": fixture.expected_exit_code,
            "status": "PASS",
        }

    raise BehaviorMatrixFailure(f"unsupported expected stage {expected_stage} for {fixture.relative_source}")


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)
    report_path = args.report_out if args.report_out.is_absolute() else ROOT / args.report_out
    case_root = report_path.parent / "cases"

    results: list[dict[str, object]] = []
    failures: list[dict[str, object]] = []
    selected: list[BehaviorFixture] = []
    try:
        ensure_native_binaries()
        selected = select_fixtures(
            load_behavior_fixtures(),
            phases=args.phase,
            limit=args.limit,
        )
        for index, fixture in enumerate(selected, start=1):
            print(
                f"behavior-matrix-progress: [{index}/{len(selected)}] "
                f"START fixture={fixture.relative_source}",
                flush=True,
            )
            case_dir = case_root / f"{index:03d}-{fixture.owner_phase}-{fixture.behavior_family}-{fixture.source_path.stem}"
            try:
                result = execute_fixture(fixture, case_dir)
                results.append(result)
                print(
                    f"behavior-matrix-progress: [{index}/{len(selected)}] "
                    f"DONE fixture={fixture.relative_source} status=PASS",
                    flush=True,
                )
            except BehaviorMatrixFailure as exc:
                failure = {
                    "fixture": fixture.relative_source,
                    "metadata": fixture.relative_metadata,
                    "message": str(exc),
                }
                failures.append(failure)
                print(
                    f"behavior-matrix-progress: [{index}/{len(selected)}] "
                    f"DONE fixture={fixture.relative_source} status=FAIL",
                    flush=True,
                )
                break
    except BehaviorMatrixFailure as exc:
        failures.append({"fixture": "", "metadata": "", "message": str(exc)})

    payload = {
        "contract_id": CONTRACT_ID,
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "status": "PASS" if not failures else "FAIL",
        "fixture_count": len(selected),
        "passed": len(results),
        "failed": len(failures),
        "phase_coverage": sorted({result["owner_phase"] for result in results}),
        "required_phase_coverage": sorted(REQUIRED_TREE),
        "results": results,
        "failures": failures,
    }
    write_json_file(report_path, payload)
    print(f"summary_path: {repo_rel(report_path)}")
    print(f"status: {payload['status']}")
    return 0 if not failures else 1


if __name__ == "__main__":
    raise SystemExit(main())
