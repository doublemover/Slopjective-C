#!/usr/bin/env python3
"""Build deterministic runtime stress/perf/fuzz scale evidence without broad benchmarks."""

from __future__ import annotations

import argparse
import hashlib
import json
import sys
from pathlib import Path
from typing import Any, Sequence

from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from scripts.objc3c_workflow.action_catalog_performance_benchmarks import (  # noqa: E402
    PERFORMANCE_BENCHMARK_ACTION_SPECS,
)
from scripts.objc3c_workflow.action_catalog_stress import STRESS_ACTION_SPECS  # noqa: E402
from scripts.objc3c_workflow.action_handlers import ACTION_HANDLERS  # noqa: E402

RUNTIME_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "runtime_performance"
STRESS_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "stress"
PERFORMANCE_GOVERNANCE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "performance_governance"

WORKLOAD_MANIFEST = RUNTIME_ROOT / "workload_manifest.json"
REPLAY_CONTRACT = RUNTIME_ROOT / "workload_replay_contract.json"
METADATA_RESILIENCE_CONTRACT = RUNTIME_ROOT / "metadata_resilience_contract.json"
STRESS_SANITIZER_CONTRACT = RUNTIME_ROOT / "stress_sanitizer_contract.json"
SCALE_SCENARIO_CONTRACT = RUNTIME_ROOT / "scale_scenario_contract.json"
BUDGET_MODEL = PERFORMANCE_GOVERNANCE_ROOT / "budget_model.json"
PARSER_SEMA_FUZZ_MANIFEST = STRESS_ROOT / "parser_sema_fuzz_manifest.json"
LOWERING_RUNTIME_STRESS_MANIFEST = STRESS_ROOT / "lowering_runtime_stress_manifest.json"
SUMMARY_OUT = ROOT / "tmp" / "reports" / "runtime-performance" / "scale-summary.json"


def _expect(condition: bool, message: str, failures: list[str]) -> None:
    if not condition:
        failures.append(message)


def _require_list(payload: dict[str, Any], key: str, owner: str, failures: list[str]) -> list[Any]:
    value = payload.get(key)
    if not isinstance(value, list):
        failures.append(f"{owner} missing array field {key}")
        return []
    return value


def _require_dict(payload: dict[str, Any], key: str, owner: str, failures: list[str]) -> dict[str, Any]:
    value = payload.get(key)
    if not isinstance(value, dict):
        failures.append(f"{owner} missing object field {key}")
        return {}
    return value


def _repo_file(path_text: str, *, failures: list[str], owner: str, root: Path = ROOT) -> Path | None:
    path = Path(path_text)
    if path.is_absolute():
        failures.append(f"{owner} path must be repo-relative: {path_text}")
        return None
    resolved = (root / path).resolve()
    try:
        resolved.relative_to(root.resolve())
    except ValueError:
        failures.append(f"{owner} path escapes repo: {path_text}")
        return None
    if not resolved.is_file():
        failures.append(f"{owner} path missing: {path_text}")
        return None
    return resolved


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(chunk)
    return "sha256:" + digest.hexdigest()


def _stable_id(payload: dict[str, Any]) -> str:
    text = json.dumps(payload, sort_keys=True, separators=(",", ":"))
    return hashlib.sha256(text.encode("utf-8")).hexdigest()


def _workload_rows(workload_manifest: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        str(row["workload_id"]): row
        for row in workload_manifest.get("workload_families", [])
        if isinstance(row, dict) and row.get("workload_id")
    }


def _replay_rows(replay_contract: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        str(row["workload_id"]): row
        for row in replay_contract.get("workload_replay_rows", [])
        if isinstance(row, dict) and row.get("workload_id")
    }


def _budget_links(budget_model: dict[str, Any]) -> dict[str, str]:
    linkage = budget_model.get("runtime_contract_linkage", {})
    if not isinstance(linkage, dict):
        return {}
    links: dict[str, str] = {}
    for row in linkage.get("workload_budget_links", []):
        if isinstance(row, dict) and row.get("workload_id") and row.get("metric_id"):
            links[str(row["workload_id"])] = str(row["metric_id"])
    return links


def _parser_sema_case_paths(parser_sema_fuzz_manifest: dict[str, Any]) -> dict[str, str]:
    return {
        str(row["case_id"]): str(row["source_path"])
        for row in parser_sema_fuzz_manifest.get("cases", [])
        if isinstance(row, dict) and row.get("case_id") and row.get("source_path")
    }


def _stress_scale_contracts(stress_sanitizer_contract: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        str(row["stress_id"]): row
        for row in stress_sanitizer_contract.get("stress_scale_contracts", [])
        if isinstance(row, dict) and row.get("stress_id")
    }


def _sanitizer_contracts(stress_sanitizer_contract: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        str(row["sanitizer_id"]): row
        for row in stress_sanitizer_contract.get("sanitizer_contracts", [])
        if isinstance(row, dict) and row.get("sanitizer_id")
    }


def _metadata_fuzz_contracts(metadata_resilience_contract: dict[str, Any]) -> dict[str, dict[str, Any]]:
    return {
        str(row["fuzz_id"]): row
        for row in metadata_resilience_contract.get("metadata_fuzz_contracts", [])
        if isinstance(row, dict) and row.get("fuzz_id")
    }


def _available_action_specs() -> dict[str, Any]:
    return {
        **PERFORMANCE_BENCHMARK_ACTION_SPECS,
        **STRESS_ACTION_SPECS,
    }


def _python_backend_script(backend: str) -> str:
    prefix = "python:"
    if not backend.startswith(prefix):
        return ""
    return backend.removeprefix(prefix)


def _workload_artifacts(
    workload_id: str,
    *,
    workload_rows: dict[str, dict[str, Any]],
    replay_rows: dict[str, dict[str, Any]],
    budget_links: dict[str, str],
    failures: list[str],
) -> dict[str, Any]:
    workload = workload_rows.get(workload_id)
    replay = replay_rows.get(workload_id)
    metric_id = budget_links.get(workload_id)
    _expect(workload is not None, f"scale evidence references unknown workload {workload_id}", failures)
    _expect(replay is not None, f"scale evidence workload {workload_id} missing replay contract", failures)
    _expect(metric_id is not None, f"scale evidence workload {workload_id} missing budget metric link", failures)
    if workload is None or replay is None:
        return {"workload_id": workload_id, "budget_metric_id": metric_id}

    fixture = str(workload.get("fixture", ""))
    probe = str(workload.get("probe", ""))
    _expect(fixture == str(replay.get("fixture", "")), f"{workload_id} fixture drifted from replay contract", failures)
    _expect(probe == str(replay.get("probe", "")), f"{workload_id} probe drifted from replay contract", failures)
    fixture_path = _repo_file(fixture, failures=failures, owner=f"{workload_id} fixture")
    probe_path = _repo_file(probe, failures=failures, owner=f"{workload_id} probe")
    return {
        "workload_id": workload_id,
        "acceptance_case_id": str(workload.get("acceptance_case_id", "")),
        "hot_path_family": str(workload.get("hot_path_family", "")),
        "budget_metric_id": metric_id,
        "fixture": fixture,
        "fixture_sha256": _sha256(fixture_path) if fixture_path else None,
        "probe": probe,
        "probe_sha256": _sha256(probe_path) if probe_path else None,
    }


def _row_with_probe_id(row: dict[str, Any]) -> dict[str, Any]:
    stable_material = {
        key: value
        for key, value in row.items()
        if key not in {"deterministic_probe_id"}
    }
    row["deterministic_probe_id"] = _stable_id(stable_material)
    return row


def _stress_scale_rows(
    stress_sanitizer_contract: dict[str, Any],
    *,
    workload_rows: dict[str, dict[str, Any]],
    replay_rows: dict[str, dict[str, Any]],
    budget_links: dict[str, str],
    failures: list[str],
) -> list[dict[str, Any]]:
    rows: list[dict[str, Any]] = []
    seen: set[str] = set()
    for contract in _require_list(stress_sanitizer_contract, "stress_scale_contracts", "stress/sanitizer", failures):
        if not isinstance(contract, dict):
            failures.append("stress scale contract contains a non-object row")
            continue
        stress_id = str(contract.get("stress_id", ""))
        workload_id = str(contract.get("workload_id", ""))
        _expect(bool(stress_id), "stress scale row missing stress_id", failures)
        _expect(stress_id not in seen, f"duplicate stress scale id {stress_id}", failures)
        seen.add(stress_id)
        min_measured_runs = int(contract.get("min_measured_runs", 0))
        scale_factor = int(contract.get("scale_factor", 0))
        _expect(min_measured_runs >= 3, f"{stress_id} must require at least three measured runs", failures)
        _expect(scale_factor >= min_measured_runs, f"{stress_id} scale_factor must cover measured runs", failures)
        _expect(contract.get("support_authority") is False, f"{stress_id} must be provenance-only", failures)
        invariants = [str(value) for value in contract.get("packet_invariants", []) if isinstance(value, str)]
        _expect("same-replay-key-across-samples" in invariants, f"{stress_id} must lock replay key stability", failures)
        rows.append(
            _row_with_probe_id(
                {
                    "evidence_kind": "stress-scale",
                    "stress_id": stress_id,
                    "scale_axis": str(contract.get("scale_axis", "")),
                    "scale_factor": scale_factor,
                    "min_measured_runs": min_measured_runs,
                    "packet_invariants": invariants,
                    "support_authority": False,
                    **_workload_artifacts(
                        workload_id,
                        workload_rows=workload_rows,
                        replay_rows=replay_rows,
                        budget_links=budget_links,
                        failures=failures,
                    ),
                }
            )
        )
    return rows


def _sanitizer_rows(
    stress_sanitizer_contract: dict[str, Any],
    *,
    workload_rows: dict[str, dict[str, Any]],
    replay_rows: dict[str, dict[str, Any]],
    budget_links: dict[str, str],
    failures: list[str],
) -> list[dict[str, Any]]:
    rows: list[dict[str, Any]] = []
    seen: set[str] = set()
    for contract in _require_list(stress_sanitizer_contract, "sanitizer_contracts", "stress/sanitizer", failures):
        if not isinstance(contract, dict):
            failures.append("sanitizer contract contains a non-object row")
            continue
        sanitizer_id = str(contract.get("sanitizer_id", ""))
        workload_id = str(contract.get("workload_id", ""))
        command = str(contract.get("targeted_command", ""))
        _expect(bool(sanitizer_id), "sanitizer row missing sanitizer_id", failures)
        _expect(sanitizer_id not in seen, f"duplicate sanitizer id {sanitizer_id}", failures)
        seen.add(sanitizer_id)
        _expect(contract.get("opt_in_only") is True, f"{sanitizer_id} must be opt-in", failures)
        _expect(contract.get("support_authority") is False, f"{sanitizer_id} must be provenance-only", failures)
        _expect(
            command.startswith(f"npm run objc3c -- benchmark-runtime-performance -- --workload-id {workload_id} "),
            f"{sanitizer_id} command must be workload-targeted",
            failures,
        )
        _expect("--measured-runs 3" in command, f"{sanitizer_id} command must require three measured runs", failures)
        modes = [str(value) for value in contract.get("sanitizer_modes", []) if isinstance(value, str)]
        _expect(bool(modes), f"{sanitizer_id} must declare sanitizer modes", failures)
        failure_modes = [str(value) for value in contract.get("required_failure_modes", []) if isinstance(value, str)]
        _expect(bool(failure_modes), f"{sanitizer_id} must declare fail-closed modes", failures)
        rows.append(
            _row_with_probe_id(
                {
                    "evidence_kind": "sanitizer-contract",
                    "sanitizer_id": sanitizer_id,
                    "sanitizer_modes": modes,
                    "targeted_command": command,
                    "required_failure_modes": failure_modes,
                    "support_authority": False,
                    **_workload_artifacts(
                        workload_id,
                        workload_rows=workload_rows,
                        replay_rows=replay_rows,
                        budget_links=budget_links,
                        failures=failures,
                    ),
                }
            )
        )
    return rows


def _metadata_fuzz_rows(
    metadata_resilience_contract: dict[str, Any],
    *,
    failures: list[str],
) -> list[dict[str, Any]]:
    rows: list[dict[str, Any]] = []
    for contract in _require_list(
        metadata_resilience_contract,
        "metadata_fuzz_contracts",
        "metadata resilience",
        failures,
    ):
        if not isinstance(contract, dict):
            failures.append("metadata fuzz contract contains a non-object row")
            continue
        fuzz_id = str(contract.get("fuzz_id", ""))
        _expect(contract.get("support_authority") is False, f"{fuzz_id} must be provenance-only", failures)
        _expect(int(contract.get("max_payload_bytes", 0)) <= 4096, f"{fuzz_id} payload cap is too broad", failures)
        corpus_rows: list[dict[str, str]] = []
        for corpus_path_text in contract.get("corpus_paths", []):
            corpus_path = _repo_file(str(corpus_path_text), failures=failures, owner=f"{fuzz_id} corpus")
            if corpus_path is None:
                continue
            try:
                json.loads(corpus_path.read_text(encoding="utf-8"))
            except json.JSONDecodeError:
                pass
            else:
                failures.append(f"{fuzz_id} corpus path must remain malformed JSON: {corpus_path_text}")
            corpus_rows.append(
                {
                    "path": str(corpus_path_text),
                    "sha256": _sha256(corpus_path),
                }
            )
        rows.append(
            _row_with_probe_id(
                {
                    "evidence_kind": "metadata-fuzz",
                    "fuzz_id": fuzz_id,
                    "workload_id": str(contract.get("workload_id", "")),
                    "max_payload_bytes": int(contract.get("max_payload_bytes", 0)),
                    "corpus": corpus_rows,
                    "support_authority": False,
                }
            )
        )
    return rows


def _parser_sema_fuzz_summary(parser_sema_fuzz_manifest: dict[str, Any], *, failures: list[str]) -> dict[str, Any]:
    subsystem_counts: dict[str, int] = {}
    seen: set[str] = set()
    case_rows: list[dict[str, str]] = []
    for case in _require_list(parser_sema_fuzz_manifest, "cases", "parser/sema fuzz manifest", failures):
        if not isinstance(case, dict):
            failures.append("parser/sema fuzz manifest contains a non-object case")
            continue
        case_id = str(case.get("case_id", ""))
        subsystem = str(case.get("subsystem", ""))
        source_path_text = str(case.get("source_path", ""))
        _expect(bool(case_id), "parser/sema fuzz case missing case_id", failures)
        _expect(case_id not in seen, f"duplicate parser/sema fuzz case {case_id}", failures)
        seen.add(case_id)
        _expect(subsystem in {"parser", "semantic"}, f"{case_id} has unsupported fuzz subsystem {subsystem}", failures)
        source_path = _repo_file(source_path_text, failures=failures, owner=f"{case_id} source")
        subsystem_counts[subsystem] = subsystem_counts.get(subsystem, 0) + 1
        case_rows.append(
            {
                "case_id": case_id,
                "subsystem": subsystem,
                "source_path": source_path_text,
                "source_sha256": _sha256(source_path) if source_path else "",
            }
        )
    return {
        "manifest_path": repo_rel(PARSER_SEMA_FUZZ_MANIFEST),
        "case_count": len(case_rows),
        "subsystem_counts": subsystem_counts,
        "cases": case_rows,
    }


def _lowering_runtime_stress_summary(
    lowering_runtime_stress_manifest: dict[str, Any],
    *,
    failures: list[str],
) -> dict[str, Any]:
    summary: dict[str, Any] = {"manifest_path": repo_rel(LOWERING_RUNTIME_STRESS_MANIFEST)}
    for key in ("compile_cases", "execution_cases", "semantic_provenance_cases"):
        paths = _require_list(lowering_runtime_stress_manifest, key, "lowering/runtime stress manifest", failures)
        digests: list[dict[str, str]] = []
        for path_text in paths:
            source_path = _repo_file(str(path_text), failures=failures, owner=f"{key} source")
            if source_path is not None:
                digests.append({"path": str(path_text), "sha256": _sha256(source_path)})
        summary[f"{key}_count"] = len(paths)
        summary[f"{key}_digests"] = digests
    _expect(summary.get("compile_cases_count", 0) >= 10, "lowering/runtime stress compile coverage is too shallow", failures)
    _expect(summary.get("execution_cases_count", 0) >= 3, "lowering/runtime stress execution coverage is too shallow", failures)
    _expect(
        summary.get("semantic_provenance_cases_count", 0) >= 2,
        "lowering/runtime stress semantic provenance coverage is too shallow",
        failures,
    )
    return summary


def _validate_scale_scenario_actions(
    scale_scenario_contract: dict[str, Any],
    *,
    failures: list[str],
) -> dict[str, dict[str, Any]]:
    action_specs = _available_action_specs()
    validated: dict[str, dict[str, Any]] = {}
    seen: set[str] = set()
    for row in _require_list(
        scale_scenario_contract,
        "validation_action_contracts",
        "scale scenario contract",
        failures,
    ):
        if not isinstance(row, dict):
            failures.append("scale scenario validation action contains a non-object row")
            continue
        action_id = str(row.get("action_id", ""))
        backend_kind = str(row.get("backend_kind", ""))
        _expect(bool(action_id), "scale scenario validation action missing action_id", failures)
        _expect(action_id not in seen, f"duplicate scale scenario validation action {action_id}", failures)
        seen.add(action_id)
        spec = action_specs.get(action_id)
        _expect(spec is not None, f"scale scenario action {action_id} missing action spec", failures)
        if row.get("handler_required") is True:
            _expect(action_id in ACTION_HANDLERS, f"scale scenario action {action_id} missing workflow handler", failures)
        if spec is not None:
            expected_pass_through = row.get("pass_through_args_required")
            if isinstance(expected_pass_through, bool):
                _expect(
                    spec.pass_through_args is expected_pass_through,
                    f"scale scenario action {action_id} pass-through contract drifted",
                    failures,
                )
            if backend_kind == "python":
                script_path = str(row.get("script_path", ""))
                _expect(bool(script_path), f"scale scenario action {action_id} missing script_path", failures)
                _expect(
                    _python_backend_script(spec.backend) == script_path,
                    f"scale scenario action {action_id} backend script drifted",
                    failures,
                )
                _repo_file(script_path, failures=failures, owner=f"{action_id} script")
            elif backend_kind == "runner-internal":
                _expect(
                    spec.backend.startswith("runner-internal"),
                    f"scale scenario action {action_id} must stay runner-internal",
                    failures,
                )
                for child_action in row.get("child_actions", []):
                    child = str(child_action)
                    _expect(child in action_specs, f"{action_id} child action {child} missing action spec", failures)
                    _expect(child in ACTION_HANDLERS, f"{action_id} child action {child} missing workflow handler", failures)
            else:
                failures.append(f"scale scenario action {action_id} has unsupported backend_kind {backend_kind}")
        validated[action_id] = row
    return validated


def _scale_scenario_rows(
    scale_scenario_contract: dict[str, Any],
    *,
    workload_rows: dict[str, dict[str, Any]],
    replay_rows: dict[str, dict[str, Any]],
    budget_links: dict[str, str],
    stress_sanitizer_contract: dict[str, Any],
    metadata_resilience_contract: dict[str, Any],
    parser_sema_fuzz_manifest: dict[str, Any],
    lowering_runtime_stress_manifest: dict[str, Any],
    failures: list[str],
) -> tuple[list[dict[str, Any]], dict[str, dict[str, Any]]]:
    if scale_scenario_contract.get("contract_id") != "objc3c.runtime.performance.scale.scenario.contract.v1":
        failures.append("scale scenario contract_id drifted")
    if scale_scenario_contract.get("schema_version") != 1:
        failures.append("scale scenario contract schema_version drifted")

    for source_contract in _require_list(
        scale_scenario_contract,
        "source_contracts",
        "scale scenario contract",
        failures,
    ):
        _repo_file(str(source_contract), failures=failures, owner="scale scenario source contract")

    actions = _validate_scale_scenario_actions(scale_scenario_contract, failures=failures)
    defaults = _require_dict(scale_scenario_contract, "scenario_defaults", "scale scenario contract", failures)
    required_metadata_fields = {
        "evidence_kind",
        "scenario_id",
        "workload_id",
        "fixture_sha256",
        "probe_sha256",
        "budget_metric_id",
        "scale_axis",
        "action_ids",
    }
    default_metadata_fields = {str(value) for value in defaults.get("deterministic_metadata_fields", [])}
    _expect(
        required_metadata_fields.issubset(default_metadata_fields),
        "scale scenario defaults do not lock deterministic metadata fields",
        failures,
    )
    _expect(defaults.get("support_authority") is False, "scale scenario defaults must be provenance-only", failures)
    _expect(
        defaults.get("generated_report_allowed") is False,
        "scale scenario defaults must reject generated report authority",
        failures,
    )
    output_report_root = str(defaults.get("output_report_root", ""))
    _expect(
        output_report_root.startswith("tmp/reports/runtime-performance"),
        "scale scenario default report root must stay under tmp runtime-performance reports",
        failures,
    )

    stress_contracts = _stress_scale_contracts(stress_sanitizer_contract)
    sanitizer_contracts = _sanitizer_contracts(stress_sanitizer_contract)
    metadata_fuzz_contracts = _metadata_fuzz_contracts(metadata_resilience_contract)
    parser_case_paths = _parser_sema_case_paths(parser_sema_fuzz_manifest)
    lowering_paths = {
        str(path)
        for key in ("compile_cases", "execution_cases", "semantic_provenance_cases")
        for path in lowering_runtime_stress_manifest.get(key, [])
    }

    rows: list[dict[str, Any]] = []
    seen: set[str] = set()
    for scenario in _require_list(scale_scenario_contract, "scale_scenarios", "scale scenario contract", failures):
        if not isinstance(scenario, dict):
            failures.append("scale scenario contract contains a non-object scenario")
            continue
        scenario_id = str(scenario.get("scenario_id", ""))
        workload_id = str(scenario.get("workload_id", ""))
        stress_id = str(scenario.get("stress_id", ""))
        _expect(bool(scenario_id), "scale scenario missing scenario_id", failures)
        _expect(scenario_id not in seen, f"duplicate scale scenario id {scenario_id}", failures)
        seen.add(scenario_id)
        _expect(scenario.get("support_authority") is False, f"{scenario_id} must be provenance-only", failures)
        _expect(
            scenario.get("generated_report_allowed") is False,
            f"{scenario_id} must not promote generated reports",
            failures,
        )
        _expect(
            str(scenario.get("output_report_root", "")).startswith("tmp/reports/runtime-performance"),
            f"{scenario_id} report root must stay under tmp runtime-performance reports",
            failures,
        )

        stress = stress_contracts.get(stress_id)
        _expect(stress is not None, f"{scenario_id} references unknown stress_id {stress_id}", failures)
        if stress is not None:
            _expect(
                str(stress.get("workload_id", "")) == workload_id,
                f"{scenario_id} stress workload drifted from scenario workload",
                failures,
            )
        sanitizer_id = scenario.get("sanitizer_id")
        sanitizer_modes: list[str] = []
        if isinstance(sanitizer_id, str) and sanitizer_id:
            sanitizer = sanitizer_contracts.get(sanitizer_id)
            _expect(sanitizer is not None, f"{scenario_id} references unknown sanitizer_id {sanitizer_id}", failures)
            if sanitizer is not None:
                _expect(
                    str(sanitizer.get("workload_id", "")) == workload_id,
                    f"{scenario_id} sanitizer workload drifted from scenario workload",
                    failures,
                )
                sanitizer_modes = [str(value) for value in sanitizer.get("sanitizer_modes", [])]

        fuzz_id = scenario.get("metadata_fuzz_id")
        corpus_paths: list[str] = []
        if isinstance(fuzz_id, str) and fuzz_id:
            fuzz = metadata_fuzz_contracts.get(fuzz_id)
            _expect(fuzz is not None, f"{scenario_id} references unknown metadata_fuzz_id {fuzz_id}", failures)
            if fuzz is not None:
                _expect(
                    str(fuzz.get("workload_id", "")) == workload_id,
                    f"{scenario_id} metadata fuzz workload drifted from scenario workload",
                    failures,
                )
                corpus_paths = [str(path) for path in fuzz.get("corpus_paths", [])]

        parser_case_ids = [str(case_id) for case_id in scenario.get("parser_sema_case_ids", [])]
        _expect(bool(parser_case_ids), f"{scenario_id} must cite parser/sema fuzz cases", failures)
        for case_id in parser_case_ids:
            source_path = parser_case_paths.get(case_id)
            _expect(source_path is not None, f"{scenario_id} references unknown parser/sema fuzz case {case_id}", failures)
            if source_path is not None:
                _repo_file(source_path, failures=failures, owner=f"{scenario_id} parser/sema source")

        scenario_lowering_paths = [str(path) for path in scenario.get("lowering_runtime_case_paths", [])]
        _expect(bool(scenario_lowering_paths), f"{scenario_id} must cite lowering/runtime stress cases", failures)
        for path in scenario_lowering_paths:
            _expect(path in lowering_paths, f"{scenario_id} lowering/runtime case is not in stress manifest: {path}", failures)
            _repo_file(path, failures=failures, owner=f"{scenario_id} lowering/runtime source")

        required_action_ids = [str(action_id) for action_id in scenario.get("required_action_ids", [])]
        _expect(bool(required_action_ids), f"{scenario_id} must cite validation actions", failures)
        for action_id in required_action_ids:
            _expect(action_id in actions, f"{scenario_id} references undeclared validation action {action_id}", failures)

        exact_replay_command = str(scenario.get("exact_replay_command", ""))
        expected_prefix = f"npm run objc3c -- benchmark-runtime-performance -- --workload-id {workload_id} "
        _expect(
            exact_replay_command.startswith(expected_prefix),
            f"{scenario_id} exact replay command is not workload-targeted",
            failures,
        )
        _expect(
            "--warmup-runs 0" in exact_replay_command and "--measured-runs 3" in exact_replay_command,
            f"{scenario_id} exact replay command must stay bounded and deterministic",
            failures,
        )

        rows.append(
            _row_with_probe_id(
                {
                    "evidence_kind": "scale-scenario",
                    "scenario_id": scenario_id,
                    "stress_id": stress_id,
                    "sanitizer_id": sanitizer_id or "",
                    "sanitizer_modes": sanitizer_modes,
                    "metadata_fuzz_id": fuzz_id or "",
                    "metadata_fuzz_corpus": corpus_paths,
                    "parser_sema_case_ids": parser_case_ids,
                    "lowering_runtime_case_paths": scenario_lowering_paths,
                    "action_ids": required_action_ids,
                    "exact_replay_command": exact_replay_command,
                    "scale_axis": str(stress.get("scale_axis", "")) if stress is not None else "",
                    "support_authority": False,
                    **_workload_artifacts(
                        workload_id,
                        workload_rows=workload_rows,
                        replay_rows=replay_rows,
                        budget_links=budget_links,
                        failures=failures,
                    ),
                }
            )
        )
    return rows, actions


def build_scale_evidence_summary(
    *,
    workload_manifest: dict[str, Any],
    replay_contract: dict[str, Any],
    metadata_resilience_contract: dict[str, Any],
    stress_sanitizer_contract: dict[str, Any],
    scale_scenario_contract: dict[str, Any],
    budget_model: dict[str, Any],
    parser_sema_fuzz_manifest: dict[str, Any],
    lowering_runtime_stress_manifest: dict[str, Any],
) -> dict[str, Any]:
    failures: list[str] = []
    workloads = _workload_rows(workload_manifest)
    replay = _replay_rows(replay_contract)
    budget = _budget_links(budget_model)

    stress_scale_rows = _stress_scale_rows(
        stress_sanitizer_contract,
        workload_rows=workloads,
        replay_rows=replay,
        budget_links=budget,
        failures=failures,
    )
    sanitizer_rows = _sanitizer_rows(
        stress_sanitizer_contract,
        workload_rows=workloads,
        replay_rows=replay,
        budget_links=budget,
        failures=failures,
    )
    metadata_fuzz_rows = _metadata_fuzz_rows(metadata_resilience_contract, failures=failures)
    parser_sema_fuzz = _parser_sema_fuzz_summary(parser_sema_fuzz_manifest, failures=failures)
    lowering_runtime_stress = _lowering_runtime_stress_summary(lowering_runtime_stress_manifest, failures=failures)
    scale_scenario_rows, validation_actions = _scale_scenario_rows(
        scale_scenario_contract,
        workload_rows=workloads,
        replay_rows=replay,
        budget_links=budget,
        stress_sanitizer_contract=stress_sanitizer_contract,
        metadata_resilience_contract=metadata_resilience_contract,
        parser_sema_fuzz_manifest=parser_sema_fuzz_manifest,
        lowering_runtime_stress_manifest=lowering_runtime_stress_manifest,
        failures=failures,
    )
    evidence_rows = stress_scale_rows + sanitizer_rows + metadata_fuzz_rows + scale_scenario_rows

    return {
        "contract_id": "objc3c.runtime.performance.scale.evidence.summary.v1",
        "schema_version": 1,
        "status": "PASS" if not failures else "FAIL",
        "support_authority": False,
        "source_contracts": [
            repo_rel(WORKLOAD_MANIFEST),
            repo_rel(REPLAY_CONTRACT),
            repo_rel(METADATA_RESILIENCE_CONTRACT),
            repo_rel(STRESS_SANITIZER_CONTRACT),
            repo_rel(SCALE_SCENARIO_CONTRACT),
            repo_rel(BUDGET_MODEL),
            repo_rel(PARSER_SEMA_FUZZ_MANIFEST),
            repo_rel(LOWERING_RUNTIME_STRESS_MANIFEST),
        ],
        "summary_counts": {
            "stress_scale": len(stress_scale_rows),
            "sanitizer": len(sanitizer_rows),
            "metadata_fuzz": len(metadata_fuzz_rows),
            "scale_scenarios": len(scale_scenario_rows),
            "validation_actions": len(validation_actions),
            "parser_sema_fuzz_cases": parser_sema_fuzz["case_count"],
            "lowering_runtime_compile_cases": lowering_runtime_stress["compile_cases_count"],
            "lowering_runtime_execution_cases": lowering_runtime_stress["execution_cases_count"],
            "lowering_runtime_semantic_provenance_cases": lowering_runtime_stress[
                "semantic_provenance_cases_count"
            ],
        },
        "scale_scenario_contract": {
            "path": repo_rel(SCALE_SCENARIO_CONTRACT),
            "contract_id": scale_scenario_contract.get("contract_id"),
            "validation_action_ids": sorted(validation_actions),
        },
        "parser_sema_fuzz": parser_sema_fuzz,
        "lowering_runtime_stress": lowering_runtime_stress,
        "evidence_rows": evidence_rows,
        "failures": failures,
    }


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--summary-out", type=Path, default=SUMMARY_OUT)
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)
    summary = build_scale_evidence_summary(
        workload_manifest=load_json(WORKLOAD_MANIFEST),
        replay_contract=load_json(REPLAY_CONTRACT),
        metadata_resilience_contract=load_json(METADATA_RESILIENCE_CONTRACT),
        stress_sanitizer_contract=load_json(STRESS_SANITIZER_CONTRACT),
        scale_scenario_contract=load_json(SCALE_SCENARIO_CONTRACT),
        budget_model=load_json(BUDGET_MODEL),
        parser_sema_fuzz_manifest=load_json(PARSER_SEMA_FUZZ_MANIFEST),
        lowering_runtime_stress_manifest=load_json(LOWERING_RUNTIME_STRESS_MANIFEST),
    )
    write_json_file(args.summary_out, summary)
    print(f"summary_path: {repo_rel(args.summary_out)}")
    if summary["status"] != "PASS":
        print("objc3c-runtime-scale-evidence: FAIL", file=sys.stderr)
        for failure in summary["failures"]:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-runtime-scale-evidence: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
