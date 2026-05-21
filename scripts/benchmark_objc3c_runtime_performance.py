#!/usr/bin/env python3
"""Benchmark live runtime startup and dispatch workloads through acceptance cases."""

from __future__ import annotations

import argparse
import importlib
import json
import statistics
import sys
import time
from json import JSONDecodeError
from pathlib import Path
from typing import Any, Sequence

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_tooling.paths import repo_rel  # noqa: E402
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file as write_json  # noqa: E402
from objc3c_performance_benchmark.profile import machine_profile, tool_versions  # noqa: E402
from objc3c_performance_reproducibility import (  # noqa: E402
    build_runtime_workload_reproducibility_evidence,
    file_sha256,
)


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
REPLAY_CONTRACT_REL = Path("tests/tooling/fixtures/runtime_performance/workload_replay_contract.json")
METADATA_RESILIENCE_CONTRACT_REL = Path("tests/tooling/fixtures/runtime_performance/metadata_resilience_contract.json")
STRESS_SANITIZER_CONTRACT_REL = Path("tests/tooling/fixtures/runtime_performance/stress_sanitizer_contract.json")
REPLAY_CONTRACT = ROOT / REPLAY_CONTRACT_REL
METADATA_RESILIENCE_CONTRACT = ROOT / METADATA_RESILIENCE_CONTRACT_REL
STRESS_SANITIZER_CONTRACT = ROOT / STRESS_SANITIZER_CONTRACT_REL
RUNTIME_PERFORMANCE_CONTRACTS = (
    REPLAY_CONTRACT,
    METADATA_RESILIENCE_CONTRACT,
    STRESS_SANITIZER_CONTRACT,
)
SUMMARY_OUT = ROOT / "tmp" / "reports" / "runtime-performance" / "benchmark-summary.json"
SUPPORTED_WORKLOAD_IDS = (
    "startup-installation",
    "dispatch-cache",
    "reflection-query",
    "ownership-helpers",
    "storage-ownership-reflection",
    "stdlib-core-runtime",
    "stdlib-concurrency-runtime",
)
CASE_FUNCTION_NAMES = {
    "startup-installation": "check_installation_lifecycle_case",
    "dispatch-cache": "check_live_dispatch_fast_path_case",
    "reflection-query": "check_realization_lookup_reflection_runtime_case",
    "ownership-helpers": "check_arc_property_helper_case",
    "storage-ownership-reflection": "check_storage_ownership_reflection_case",
    "stdlib-core-runtime": "check_stdlib_core_runtime_probe_case",
    "stdlib-concurrency-runtime": "check_stdlib_concurrency_runtime_probe_case",
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


def _repo_file(path: Path, *, field_name: str, case_id: str, root: Path | None = None) -> Path:
    repo_root = ROOT if root is None else root
    if path.is_absolute():
        raise RuntimeError(f"runtime performance fixture {case_id} {field_name} must be repo-relative")
    resolved = (repo_root / path).resolve()
    try:
        resolved.relative_to(repo_root.resolve())
    except ValueError as exc:
        raise RuntimeError(f"runtime performance fixture {case_id} {field_name} escapes the repo") from exc
    if not resolved.is_file():
        raise RuntimeError(f"runtime performance fixture {case_id} missing {field_name}: {path.as_posix()}")
    return resolved


def _require_repo_file(path: Path, *, field_name: str, case_id: str, root: Path | None = None) -> None:
    _repo_file(path, field_name=field_name, case_id=case_id, root=root)


def _require_mapping(payload: dict[str, Any], key: str, owner: str) -> dict[str, Any]:
    value = payload.get(key)
    if not isinstance(value, dict):
        raise RuntimeError(f"{owner} missing object field {key}")
    return value


def _require_sequence(payload: dict[str, Any], key: str, owner: str) -> list[Any]:
    value = payload.get(key)
    if not isinstance(value, list):
        raise RuntimeError(f"{owner} missing array field {key}")
    return list(value)


def _require_string(value: Any, field_name: str, owner: str) -> str:
    if not isinstance(value, str) or not value:
        raise RuntimeError(f"{owner} missing string field {field_name}")
    return value


def validate_fixture_manifest(
    payload: dict[str, Any],
    *,
    selected_workload_ids: Sequence[str] | None = None,
    root: Path | None = None,
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
        _require_repo_file(Path(str(case.get("source_path", ""))), field_name="source_path", case_id=case_id, root=root)
        _require_repo_file(Path(str(case.get("probe_path", ""))), field_name="probe_path", case_id=case_id, root=root)
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
        _require_repo_file(Path(str(case.get("source_path", ""))), field_name="source_path", case_id=case_id, root=root)
        _require_repo_file(Path(str(case.get("metadata_path", ""))), field_name="metadata_path", case_id=case_id, root=root)
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


def validate_workload_replay_contract(
    payload: dict[str, Any],
    *,
    workload_rows: dict[str, dict[str, Any]],
) -> dict[str, Any]:
    if payload.get("contract_id") != "objc3c.runtime.performance.workload.replay.contract.v1":
        raise RuntimeError("runtime performance workload replay contract_id drifted")
    if payload.get("schema_version") != 1:
        raise RuntimeError("runtime performance workload replay contract schema_version drifted")
    replay_policy = _require_mapping(payload, "replay_policy", "runtime performance workload replay contract")
    replay_key_fields = _require_sequence(
        replay_policy,
        "replay_key_fields",
        "runtime performance workload replay policy",
    )
    required_replay_key_fields = {
        "workload_id",
        "acceptance_case_id",
        "hot_path_family",
        "fixture_sha256",
        "probe_sha256",
        "budget_metric_id",
    }
    if not required_replay_key_fields.issubset(set(replay_key_fields)):
        raise RuntimeError("runtime performance workload replay key field contract is incomplete")
    rows = _require_sequence(payload, "workload_replay_rows", "runtime performance workload replay contract")
    seen_ids: set[str] = set()
    for row in rows:
        if not isinstance(row, dict):
            raise RuntimeError("runtime performance workload replay contract contains a non-object row")
        workload_id = _require_string(row.get("workload_id"), "workload_id", "runtime performance replay row")
        if workload_id in seen_ids:
            raise RuntimeError(f"runtime performance replay row duplicated workload_id {workload_id}")
        seen_ids.add(workload_id)
        manifest_row = workload_rows.get(workload_id)
        if manifest_row is None:
            raise RuntimeError(f"runtime performance replay row references unknown workload_id {workload_id}")
        for field_name in ("acceptance_case_id", "fixture", "probe", "hot_path_family"):
            if str(row.get(field_name)) != str(manifest_row.get(field_name)):
                raise RuntimeError(f"runtime performance replay row {workload_id} drifted field {field_name}")
        if int(row.get("min_replay_samples", 0)) < 1:
            raise RuntimeError(f"runtime performance replay row {workload_id} must require at least one sample")
        nondeterministic_fields = set(_require_sequence(row, "allowed_nondeterministic_packet_fields", workload_id))
        if not {"duration_ms", "run_dir"}.issubset(nondeterministic_fields):
            raise RuntimeError(f"runtime performance replay row {workload_id} does not isolate timing/run-dir drift")
        required_summary_fields = _require_sequence(row, "required_summary_fields", workload_id)
        if not required_summary_fields:
            raise RuntimeError(f"runtime performance replay row {workload_id} has no required summary fields")
        measured_fields = set(_require_sequence(manifest_row, "measured_fields", workload_id))
        missing_measured_fields = sorted(set(str(field) for field in required_summary_fields) - measured_fields)
        if missing_measured_fields:
            raise RuntimeError(
                f"runtime performance replay row {workload_id} requires fields absent from workload manifest: "
                + ", ".join(missing_measured_fields)
            )
    expected_ids = set(workload_rows)
    if seen_ids != expected_ids:
        missing = sorted(expected_ids - seen_ids)
        extra = sorted(seen_ids - expected_ids)
        raise RuntimeError(f"runtime performance replay rows drifted from workload manifest; missing={missing} extra={extra}")
    return {
        "contract_id": str(payload["contract_id"]),
        "row_count": len(rows),
        "replay_key_fields": list(replay_key_fields),
    }


def required_replay_summary_fields(
    replay_contract: dict[str, Any],
    workload_id: str,
) -> list[str]:
    rows = _require_sequence(replay_contract, "workload_replay_rows", "runtime performance workload replay contract")
    for row in rows:
        if isinstance(row, dict) and row.get("workload_id") == workload_id:
            return [str(field) for field in _require_sequence(row, "required_summary_fields", workload_id)]
    raise RuntimeError(f"runtime performance replay contract missing workload_id {workload_id}")


def validate_replay_sample_summary(
    *,
    replay_contract: dict[str, Any],
    workload_id: str,
    sample_summary: dict[str, Any],
) -> None:
    required_fields = required_replay_summary_fields(replay_contract, workload_id)
    missing_fields = [field for field in required_fields if field not in sample_summary]
    if missing_fields:
        raise RuntimeError(
            f"runtime performance sample {workload_id} missing replay summary fields: "
            + ", ".join(missing_fields)
        )


def validate_metadata_resilience_contract(
    payload: dict[str, Any],
    *,
    root: Path | None = None,
) -> dict[str, Any]:
    if payload.get("contract_id") != "objc3c.runtime.performance.metadata.resilience.contract.v1":
        raise RuntimeError("runtime performance metadata resilience contract_id drifted")
    if payload.get("schema_version") != 1:
        raise RuntimeError("runtime performance metadata resilience contract schema_version drifted")
    malformed_cases = _require_sequence(
        payload,
        "malformed_metadata_cases",
        "runtime performance metadata resilience contract",
    )
    fuzz_contracts = _require_sequence(
        payload,
        "metadata_fuzz_contracts",
        "runtime performance metadata resilience contract",
    )
    invalid_json_count = 0
    for case in malformed_cases:
        if not isinstance(case, dict):
            raise RuntimeError("runtime performance metadata resilience contract contains a non-object case")
        case_id = _require_string(case.get("case_id"), "case_id", "runtime performance metadata resilience case")
        workload_id = _require_string(case.get("workload_id"), "workload_id", case_id)
        if workload_id not in SUPPORTED_WORKLOAD_IDS:
            raise RuntimeError(f"runtime performance metadata resilience case {case_id} has unsupported workload_id")
        _require_repo_file(Path(str(case.get("source_path", ""))), field_name="source_path", case_id=case_id, root=root)
        metadata_path = _repo_file(
            Path(str(case.get("metadata_path", ""))),
            field_name="metadata_path",
            case_id=case_id,
            root=root,
        )
        diagnostic = _require_mapping(case, "stable_diagnostic", case_id)
        _require_string(diagnostic.get("code"), "code", case_id)
        _require_mapping(diagnostic, "source_range", case_id)
        handling = _require_mapping(case, "expected_handling", case_id)
        if handling.get("fail_closed") is not True:
            raise RuntimeError(f"runtime performance metadata resilience case {case_id} must fail closed")
        if handling.get("packet_written") is not False:
            raise RuntimeError(f"runtime performance metadata resilience case {case_id} must not write a packet")
        malformed_kind = _require_string(case.get("malformed_kind"), "malformed_kind", case_id)
        if malformed_kind == "invalid-json":
            try:
                json.loads(metadata_path.read_text(encoding="utf-8"))
            except JSONDecodeError:
                invalid_json_count += 1
            else:
                raise RuntimeError(f"runtime performance metadata resilience case {case_id} expected invalid JSON")
        else:
            raise RuntimeError(f"runtime performance metadata resilience case {case_id} has unknown malformed_kind")

    for contract in fuzz_contracts:
        if not isinstance(contract, dict):
            raise RuntimeError("runtime performance metadata fuzz contract contains a non-object row")
        fuzz_id = _require_string(contract.get("fuzz_id"), "fuzz_id", "runtime performance metadata fuzz contract")
        corpus_paths = _require_sequence(contract, "corpus_paths", fuzz_id)
        if not corpus_paths:
            raise RuntimeError(f"runtime performance metadata fuzz contract {fuzz_id} has no corpus paths")
        for corpus_path in corpus_paths:
            _require_repo_file(Path(str(corpus_path)), field_name="corpus_path", case_id=fuzz_id, root=root)
        if int(contract.get("max_payload_bytes", 0)) < 1:
            raise RuntimeError(f"runtime performance metadata fuzz contract {fuzz_id} has no max payload cap")
        if contract.get("support_authority") is not False:
            raise RuntimeError(f"runtime performance metadata fuzz contract {fuzz_id} must be provenance-only")
    return {
        "contract_id": str(payload["contract_id"]),
        "malformed_case_count": len(malformed_cases),
        "invalid_json_case_count": invalid_json_count,
        "fuzz_contract_count": len(fuzz_contracts),
    }


def validate_stress_sanitizer_contract(
    payload: dict[str, Any],
    *,
    workload_rows: dict[str, dict[str, Any]],
) -> dict[str, Any]:
    if payload.get("contract_id") != "objc3c.runtime.performance.stress.sanitizer.contract.v1":
        raise RuntimeError("runtime performance stress/sanitizer contract_id drifted")
    if payload.get("schema_version") != 1:
        raise RuntimeError("runtime performance stress/sanitizer contract schema_version drifted")
    sanitizer_contracts = _require_sequence(
        payload,
        "sanitizer_contracts",
        "runtime performance stress/sanitizer contract",
    )
    stress_scale_contracts = _require_sequence(
        payload,
        "stress_scale_contracts",
        "runtime performance stress/sanitizer contract",
    )
    for row in sanitizer_contracts:
        if not isinstance(row, dict):
            raise RuntimeError("runtime performance sanitizer contract contains a non-object row")
        sanitizer_id = _require_string(row.get("sanitizer_id"), "sanitizer_id", "runtime performance sanitizer row")
        workload_id = _require_string(row.get("workload_id"), "workload_id", sanitizer_id)
        if workload_id not in workload_rows:
            raise RuntimeError(f"runtime performance sanitizer row {sanitizer_id} references unknown workload")
        command = _require_string(row.get("targeted_command"), "targeted_command", sanitizer_id)
        if "benchmark-runtime-performance" not in command or workload_id not in command:
            raise RuntimeError(f"runtime performance sanitizer row {sanitizer_id} is not tied to its workload command")
        if row.get("opt_in_only") is not True:
            raise RuntimeError(f"runtime performance sanitizer row {sanitizer_id} must be opt-in")
        if row.get("support_authority") is not False:
            raise RuntimeError(f"runtime performance sanitizer row {sanitizer_id} must be provenance-only")
        if not _require_sequence(row, "required_failure_modes", sanitizer_id):
            raise RuntimeError(f"runtime performance sanitizer row {sanitizer_id} has no failure modes")
    for row in stress_scale_contracts:
        if not isinstance(row, dict):
            raise RuntimeError("runtime performance stress contract contains a non-object row")
        stress_id = _require_string(row.get("stress_id"), "stress_id", "runtime performance stress row")
        workload_id = _require_string(row.get("workload_id"), "workload_id", stress_id)
        if workload_id not in workload_rows:
            raise RuntimeError(f"runtime performance stress row {stress_id} references unknown workload")
        if int(row.get("min_measured_runs", 0)) < 2:
            raise RuntimeError(f"runtime performance stress row {stress_id} must require repeated samples")
        if int(row.get("scale_factor", 0)) < 1:
            raise RuntimeError(f"runtime performance stress row {stress_id} must declare a positive scale factor")
        if not _require_sequence(row, "packet_invariants", stress_id):
            raise RuntimeError(f"runtime performance stress row {stress_id} has no packet invariants")
        if row.get("support_authority") is not False:
            raise RuntimeError(f"runtime performance stress row {stress_id} must be provenance-only")
    return {
        "contract_id": str(payload["contract_id"]),
        "sanitizer_contract_count": len(sanitizer_contracts),
        "stress_scale_contract_count": len(stress_scale_contracts),
    }


def validate_runtime_performance_contracts(
    *,
    replay_contract: dict[str, Any],
    metadata_resilience_contract: dict[str, Any],
    stress_sanitizer_contract: dict[str, Any],
    workload_rows: dict[str, dict[str, Any]],
    root: Path | None = None,
) -> dict[str, Any]:
    repo_root = ROOT if root is None else root
    contract_paths = (
        repo_root / REPLAY_CONTRACT_REL,
        repo_root / METADATA_RESILIENCE_CONTRACT_REL,
        repo_root / STRESS_SANITIZER_CONTRACT_REL,
    )
    return {
        "replay": validate_workload_replay_contract(replay_contract, workload_rows=workload_rows),
        "metadata_resilience": validate_metadata_resilience_contract(
            metadata_resilience_contract,
            root=repo_root,
        ),
        "stress_sanitizer": validate_stress_sanitizer_contract(
            stress_sanitizer_contract,
            workload_rows=workload_rows,
        ),
        "contract_files": [
            {
                "path": repo_rel(path),
                "sha256": file_sha256(path),
            }
            for path in contract_paths
        ],
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
    runtime_contract_paths = (
        ROOT / REPLAY_CONTRACT_REL,
        ROOT / METADATA_RESILIENCE_CONTRACT_REL,
        ROOT / STRESS_SANITIZER_CONTRACT_REL,
    )
    workload_manifest = load_json(WORKLOAD_MANIFEST)
    artifact_surface = load_json(ARTIFACT_SURFACE)
    fixture_manifest = load_json(args.fixture_manifest)
    budget_model = load_json(PERFORMANCE_BUDGET_MODEL)
    replay_contract = load_json(runtime_contract_paths[0])
    metadata_resilience_contract = load_json(runtime_contract_paths[1])
    stress_sanitizer_contract = load_json(runtime_contract_paths[2])
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
    runtime_contract_summary = validate_runtime_performance_contracts(
        replay_contract=replay_contract,
        metadata_resilience_contract=metadata_resilience_contract,
        stress_sanitizer_contract=stress_sanitizer_contract,
        workload_rows=workload_rows,
    )

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
        reproducibility_evidence = build_runtime_workload_reproducibility_evidence(
            root=ROOT,
            workload=workload,
            workload_manifest_path=WORKLOAD_MANIFEST,
            artifact_surface_path=ARTIFACT_SURFACE,
            budget_model=budget_model,
            profile=profile,
            versions=versions,
            contract_paths=runtime_contract_paths,
        )
        for sample_index in range(args.measured_runs):
            run_dir = ROOT / "tmp" / "artifacts" / "runtime-performance" / workload_id / f"sample-{sample_index + 1}"
            run_dir.mkdir(parents=True, exist_ok=True)
            started = time.perf_counter()
            case_result = case_fn(clangxx, run_dir)
            duration_ms = round((time.perf_counter() - started) * 1000.0, 3)
            expect(case_result.passed, f"{workload_id} sample {sample_index + 1} failed", failures)
            try:
                validate_replay_sample_summary(
                    replay_contract=replay_contract,
                    workload_id=workload_id,
                    sample_summary=case_result.summary,
                )
            except RuntimeError as exc:
                failures.append(str(exc))
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
                "replay_key": reproducibility_evidence["replay_key"],
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
                "reproducibility_evidence": reproducibility_evidence,
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
        "runtime_contract_summary": runtime_contract_summary,
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
