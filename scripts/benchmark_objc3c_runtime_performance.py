#!/usr/bin/env python3
"""Benchmark live runtime startup and dispatch workloads through acceptance cases."""

from __future__ import annotations

import argparse
import importlib
import json
import statistics
import sys
import time
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.paths import repo_rel
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file as write_json
from objc3c_performance_benchmark.profile import machine_profile, tool_versions
from objc3c_performance_reproducibility import build_runtime_workload_reproducibility_evidence


WORKLOAD_MANIFEST = ROOT / "tests" / "tooling" / "fixtures" / "runtime_performance" / "workload_manifest.json"
ARTIFACT_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "runtime_performance" / "artifact_surface.json"
FIXTURE_MANIFEST = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "runtime_performance"
    / "executable_fixture_manifest.json"
)
PERFORMANCE_BUDGET_MODEL = ROOT / "tests" / "tooling" / "fixtures" / "performance_governance" / "budget_model.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "runtime-performance" / "benchmark-summary.json"
SUPPORTED_WORKLOAD_IDS = (
    "startup-installation",
    "dispatch-cache",
    "reflection-query",
    "ownership-helpers",
    "storage-ownership-reflection",
)
CASE_FUNCTION_NAMES = {
    "startup-installation": "check_installation_lifecycle_case",
    "dispatch-cache": "check_live_dispatch_fast_path_case",
    "reflection-query": "check_realization_lookup_reflection_runtime_case",
    "ownership-helpers": "check_arc_property_helper_case",
    "storage-ownership-reflection": "check_storage_ownership_reflection_case",
}


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    parser.add_argument("--fixture-manifest", type=Path, default=FIXTURE_MANIFEST)
    parser.add_argument("--warmup-runs", type=int, default=1)
    parser.add_argument("--measured-runs", type=int, default=3)
    parser.add_argument(
        "--workload-id",
        action="append",
        dest="workload_ids",
        help="limit execution to one or more supported workload IDs",
    )
    return parser.parse_args(argv)





def expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def _require_repo_file(path: Path, *, field_name: str, case_id: str) -> None:
    if path.is_absolute():
        raise RuntimeError(f"runtime performance fixture {case_id} {field_name} must be repo-relative")
    resolved = (ROOT / path).resolve()
    try:
        resolved.relative_to(ROOT.resolve())
    except ValueError as exc:
        raise RuntimeError(f"runtime performance fixture {case_id} {field_name} escapes the repo") from exc
    if not resolved.is_file():
        raise RuntimeError(f"runtime performance fixture {case_id} missing {field_name}: {path.as_posix()}")


def validate_fixture_manifest(
    payload: dict[str, Any],
    *,
    selected_workload_ids: Sequence[str] | None = None,
) -> dict[str, Any]:
    if payload.get("contract_id") != "objc3c.runtime.performance.executable.fixture.manifest.v1":
        raise RuntimeError("runtime performance executable fixture manifest contract_id drifted")
    if payload.get("schema_version") != 1:
        raise RuntimeError("runtime performance executable fixture manifest schema_version drifted")

    allowed_workload_ids = set(SUPPORTED_WORKLOAD_IDS)
    if selected_workload_ids is not None:
        allowed_workload_ids &= set(selected_workload_ids)

    positive_cases = payload.get("positive_cases")
    negative_cases = payload.get("negative_cases")
    if not isinstance(positive_cases, list) or not positive_cases:
        raise RuntimeError("runtime performance executable fixture manifest missing positive_cases")
    if not isinstance(negative_cases, list) or not negative_cases:
        raise RuntimeError("runtime performance executable fixture manifest missing negative_cases")

    positive_count = 0
    negative_count = 0
    for case in positive_cases:
        if not isinstance(case, dict):
            raise RuntimeError("runtime performance executable fixture manifest has non-object positive case")
        case_id = str(case.get("case_id", ""))
        workload_id = str(case.get("workload_id", ""))
        if not case_id:
            raise RuntimeError("runtime performance executable fixture positive case missing case_id")
        if workload_id not in SUPPORTED_WORKLOAD_IDS:
            raise RuntimeError(f"runtime performance executable fixture {case_id} has unsupported workload_id")
        _require_repo_file(Path(str(case.get("source_path", ""))), field_name="source_path", case_id=case_id)
        _require_repo_file(Path(str(case.get("probe_path", ""))), field_name="probe_path", case_id=case_id)
        if workload_id in allowed_workload_ids:
            positive_count += 1

    for case in negative_cases:
        if not isinstance(case, dict):
            raise RuntimeError("runtime performance executable fixture manifest has non-object negative case")
        case_id = str(case.get("case_id", ""))
        workload_id = str(case.get("workload_id", ""))
        if not case_id:
            raise RuntimeError("runtime performance executable fixture negative case missing case_id")
        if workload_id not in SUPPORTED_WORKLOAD_IDS:
            raise RuntimeError(f"runtime performance executable fixture {case_id} has unsupported workload_id")
        _require_repo_file(Path(str(case.get("source_path", ""))), field_name="source_path", case_id=case_id)
        _require_repo_file(Path(str(case.get("metadata_path", ""))), field_name="metadata_path", case_id=case_id)
        diagnostic = case.get("stable_diagnostic")
        if not isinstance(diagnostic, dict):
            raise RuntimeError(f"runtime performance executable fixture {case_id} missing stable_diagnostic")
        if not isinstance(diagnostic.get("code"), str) or not diagnostic.get("code"):
            raise RuntimeError(f"runtime performance executable fixture {case_id} missing stable diagnostic code")
        source_range = diagnostic.get("source_range")
        if not isinstance(source_range, dict):
            raise RuntimeError(f"runtime performance executable fixture {case_id} missing source_range")
        if workload_id in allowed_workload_ids:
            negative_count += 1

    return {
        "contract_id": payload["contract_id"],
        "positive_case_count": positive_count if selected_workload_ids is not None else len(positive_cases),
        "negative_case_count": negative_count if selected_workload_ids is not None else len(negative_cases),
    }


def load_runtime_acceptance_module():
    scripts_root = str(ROOT / "scripts")
    if scripts_root not in sys.path:
        sys.path.insert(0, scripts_root)
    return importlib.import_module("objc3c_runtime_acceptance.case_exports")


def summarize_durations(durations: list[float]) -> dict[str, float]:
    return {
        "sample_count": len(durations),
        "min_duration_ms": min(durations),
        "median_duration_ms": statistics.median(durations),
        "max_duration_ms": max(durations),
    }


def main() -> int:
    args = parse_args(sys.argv[1:])
    workload_manifest = load_json(WORKLOAD_MANIFEST)
    artifact_surface = load_json(ARTIFACT_SURFACE)
    fixture_manifest = load_json(args.fixture_manifest)
    budget_model = load_json(PERFORMANCE_BUDGET_MODEL)
    profile = machine_profile()
    versions = tool_versions()
    acceptance = load_runtime_acceptance_module()
    acceptance.ensure_native_binaries()
    clangxx = acceptance.find_clangxx()

    requested_ids = args.workload_ids or list(SUPPORTED_WORKLOAD_IDS)
    selected_ids = [workload_id for workload_id in requested_ids if workload_id in SUPPORTED_WORKLOAD_IDS]
    if not selected_ids:
        raise RuntimeError("no supported runtime-performance workload IDs were selected")
    fixture_manifest_summary = validate_fixture_manifest(
        fixture_manifest,
        selected_workload_ids=selected_ids,
    )

    workload_rows = {
        str(row["workload_id"]): row
        for row in workload_manifest.get("workload_families", [])
        if isinstance(row, dict) and str(row.get("workload_id", "")) in SUPPORTED_WORKLOAD_IDS
    }

    packet_paths: list[str] = []
    workload_summaries: list[dict[str, Any]] = []
    failures: list[str] = []

    for workload_id in selected_ids:
        workload = workload_rows.get(workload_id)
        if workload is None:
            failures.append(f"workload manifest did not publish {workload_id}")
            continue
        case_function_name = CASE_FUNCTION_NAMES[workload_id]
        case_fn = getattr(acceptance, case_function_name, None)
        if case_fn is None:
            failures.append(f"runtime acceptance module missing {case_function_name}")
            continue

        for warmup_index in range(args.warmup_runs):
            warmup_run_dir = ROOT / "tmp" / "artifacts" / "runtime-performance" / workload_id / f"warmup-{warmup_index + 1}"
            warmup_run_dir.mkdir(parents=True, exist_ok=True)
            warmup_result = case_fn(clangxx, warmup_run_dir)
            expect(warmup_result.passed, f"{workload_id} warmup failed", failures)

        durations: list[float] = []
        summary_rows: list[dict[str, Any]] = []
        for sample_index in range(args.measured_runs):
            run_dir = ROOT / "tmp" / "artifacts" / "runtime-performance" / workload_id / f"sample-{sample_index + 1}"
            run_dir.mkdir(parents=True, exist_ok=True)
            started = time.perf_counter()
            case_result = case_fn(clangxx, run_dir)
            duration_ms = round((time.perf_counter() - started) * 1000.0, 3)
            expect(case_result.passed, f"{workload_id} sample {sample_index + 1} failed", failures)
            durations.append(duration_ms)
            summary_rows.append(
                {
                    "sample_id": f"{workload_id}-{sample_index + 1}",
                    "duration_ms": duration_ms,
                    "case_summary": case_result.summary,
                    "probe": case_result.probe,
                    "fixture": case_result.fixture,
                }
            )

            packet = {
                "contract_id": "objc3c.runtime.performance.telemetry.v1",
                "schema_version": 1,
                "benchmark_kind": str(workload["hot_path_family"]),
                "workload_id": workload_id,
                "acceptance_case_id": str(workload["acceptance_case_id"]),
                "duration_ms": duration_ms,
                "counter_snapshot": case_result.summary,
                "summary": case_result.summary,
                "ok": bool(case_result.passed),
                "probe": case_result.probe,
                "fixture": case_result.fixture,
                "run_dir": repo_rel(run_dir),
            }
            packet_path = ROOT / "tmp" / "reports" / "runtime-performance" / workload_id / f"sample-{sample_index + 1}.json"
            write_json(packet_path, packet)
            packet_paths.append(repo_rel(packet_path))

        workload_summaries.append(
            {
                "workload_id": workload_id,
                "acceptance_case_id": str(workload["acceptance_case_id"]),
                "summary": summarize_durations(durations),
                "measured_fields": workload.get("measured_fields", []),
                "reproducibility_evidence": build_runtime_workload_reproducibility_evidence(
                    root=ROOT,
                    workload=workload,
                    workload_manifest_path=WORKLOAD_MANIFEST,
                    artifact_surface_path=ARTIFACT_SURFACE,
                    budget_model=budget_model,
                    profile=profile,
                    versions=versions,
                ),
                "samples": summary_rows,
            }
        )

    payload = {
        "contract_id": "objc3c.runtime.performance.summary.v1",
        "schema_version": 1,
        "ok": not failures,
        "artifact_surface_contract_id": artifact_surface["contract_id"],
        "workload_manifest_contract_id": workload_manifest["contract_id"],
        "fixture_manifest_path": repo_rel(args.fixture_manifest.resolve()),
        "fixture_manifest_contract_id": fixture_manifest_summary["contract_id"],
        "fixture_manifest_summary": fixture_manifest_summary,
        "budget_model_contract_id": budget_model["contract_id"],
        "budget_model_path": repo_rel(PERFORMANCE_BUDGET_MODEL),
        "machine_profile": profile,
        "tool_versions": versions,
        "selected_workload_ids": selected_ids,
        "packet_paths": packet_paths,
        "workloads": workload_summaries,
        "failures": failures,
    }
    write_json(args.summary_out, payload)
    print(f"summary_path: {repo_rel(args.summary_out)}")
    if failures:
        print("objc3c-runtime-performance: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-runtime-performance: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
