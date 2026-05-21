#!/usr/bin/env python3
"""Validate the #8174/#8175/#8159 reflection/optimization/performance contract."""

from __future__ import annotations

from dataclasses import dataclass
import argparse
import copy
import importlib.util
import json
from pathlib import Path
import sys
from typing import Any, Sequence


ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel
from objc3c_semantic_optimization_pipeline import validate_pipeline


CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "reflection_optimization_performance"
    / "completion_contract.json"
)
REPORT_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "reflection-optimization-performance"
    / "completion-contract.json"
)
SCALE_PROBE_PATH = ROOT / "scripts" / "probe_objc3c_runtime_scale_evidence.py"


@dataclass(frozen=True)
class CompletionContractResult:
    payload: dict[str, Any]
    failures: list[str]

    @property
    def passed(self) -> bool:
        return not self.failures


def _load_json(path: Path) -> dict[str, Any]:
    with path.open("r", encoding="utf-8") as handle:
        payload = json.load(handle)
    if not isinstance(payload, dict):
        raise ValueError(f"JSON root must be an object: {path}")
    return payload


def _write_variant(path: Path, payload: dict[str, Any]) -> Path:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(json.dumps(payload, indent=2) + "\n", encoding="utf-8")
    return path


def _as_dict(value: object) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _as_list(value: object) -> list[object]:
    return value if isinstance(value, list) else []


def _repo_path(path_text: str, failures: list[str], owner: str) -> Path:
    path = Path(path_text)
    if path.is_absolute():
        failures.append(f"{owner} path must be repo-relative: {path_text}")
        return ROOT / "__missing__"
    resolved = (ROOT / path).resolve()
    try:
        resolved.relative_to(ROOT.resolve())
    except ValueError:
        failures.append(f"{owner} path escapes repo: {path_text}")
        return ROOT / "__missing__"
    if not resolved.is_file():
        failures.append(f"{owner} path missing: {path_text}")
    return resolved


def _read(path: Path, failures: list[str], owner: str) -> str:
    if not path.is_file():
        failures.append(f"{owner} file missing: {repo_rel(path)}")
        return ""
    return path.read_text(encoding="utf-8")


def _require_contains(
    text: str,
    token: str,
    failures: list[str],
    owner: str,
) -> None:
    if token not in text:
        failures.append(f"{owner} missing token: {token}")


def _require_absent(
    text: str,
    token: str,
    failures: list[str],
    owner: str,
) -> None:
    if token in text:
        failures.append(f"{owner} exposes forbidden token: {token}")


def _load_scale_probe_module():
    spec = importlib.util.spec_from_file_location(
        "reflection_optimization_performance_scale_probe",
        SCALE_PROBE_PATH,
    )
    if spec is None or spec.loader is None:
        raise RuntimeError("unable to load runtime scale evidence probe")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def _validate_public_reflection(
    contract: dict[str, Any],
    failures: list[str],
) -> dict[str, Any]:
    section = _as_dict(contract.get("public_reflection"))
    reflection_contract_path = _repo_path(
        str(section.get("source_contract", "")),
        failures,
        "public reflection source contract",
    )
    reflection_contract = _load_json(reflection_contract_path)

    if reflection_contract.get("issue") != 8174:
        failures.append("public reflection source contract is not bound to issue #8174")
    if reflection_contract.get("lifetime_model") != section.get("expected_lifetime_model"):
        failures.append("public reflection lifetime model drifted")
    if (
        reflection_contract.get("unsupported_metadata_policy")
        != section.get("expected_unsupported_metadata_policy")
    ):
        failures.append("public reflection unsupported metadata policy drifted")

    header_path = _repo_path(
        str(reflection_contract.get("public_header", "")),
        failures,
        "public reflection header",
    )
    implementation_path = _repo_path(
        str(reflection_contract.get("implementation", "")),
        failures,
        "public reflection implementation",
    )
    probe_path = _repo_path(
        str(reflection_contract.get("runtime_probe", "")),
        failures,
        "public reflection probe",
    )
    header = _read(header_path, failures, "public reflection header")
    implementation = _read(
        implementation_path,
        failures,
        "public reflection implementation",
    )
    probe = _read(probe_path, failures, "public reflection probe")

    for token in _as_list(section.get("header_tokens")):
        _require_contains(header, str(token), failures, "public reflection header")
    for token in _as_list(section.get("implementation_tokens")):
        _require_contains(
            implementation,
            str(token),
            failures,
            "public reflection implementation",
        )
    for token in _as_list(section.get("probe_tokens")):
        _require_contains(probe, str(token), failures, "public reflection probe")

    for symbol in _as_list(reflection_contract.get("entrypoints")):
        token = f"{symbol}("
        _require_contains(header, token, failures, "public reflection header")
        _require_contains(
            implementation,
            token,
            failures,
            "public reflection implementation",
        )
        _require_contains(probe, token, failures, "public reflection probe")

    for snapshot_type in _as_list(reflection_contract.get("snapshot_types")):
        _require_contains(
            header,
            f"typedef struct {snapshot_type}",
            failures,
            "public reflection header",
        )

    status_codes = {
        str(status) for status in _as_list(reflection_contract.get("status_codes"))
    }
    required_status_codes = {
        str(status) for status in _as_list(section.get("required_status_codes"))
    }
    missing_statuses = sorted(required_status_codes.difference(status_codes))
    if missing_statuses:
        failures.append(
            "public reflection source contract missing statuses: "
            + ", ".join(missing_statuses)
        )
    for status in required_status_codes:
        _require_contains(header, status, failures, "public reflection header")
        _require_contains(
            implementation,
            status,
            failures,
            "public reflection implementation",
        )

    for token in _as_list(section.get("forbidden_public_header_tokens")):
        _require_absent(header, str(token), failures, "public reflection header")
    for token in _as_list(section.get("forbidden_probe_tokens")):
        _require_absent(probe, str(token), failures, "public reflection probe")

    surface_fields = [str(field) for field in _as_list(section.get("required_surface_fields"))]
    for field in surface_fields:
        _require_contains(header, field, failures, "public reflection header")
        _require_contains(probe, field, failures, "public reflection probe")

    _require_contains(
        implementation,
        "PublicRuntimeReflectionSurfaceRecord",
        failures,
        "public reflection implementation",
    )
    _require_contains(
        implementation,
        "snapshot.fail_closed = 1",
        failures,
        "public reflection implementation",
    )

    return {
        "source_contract": repo_rel(reflection_contract_path),
        "entrypoint_count": len(_as_list(reflection_contract.get("entrypoints"))),
        "snapshot_type_count": len(_as_list(reflection_contract.get("snapshot_types"))),
        "status_count": len(status_codes),
        "lifetime_model": reflection_contract.get("lifetime_model"),
    }


def _validate_semantic_optimization(
    contract: dict[str, Any],
    failures: list[str],
) -> dict[str, Any]:
    section = _as_dict(contract.get("semantic_optimization"))
    pipeline_path = _repo_path(
        str(section.get("pipeline_contract", "")),
        failures,
        "semantic optimization pipeline contract",
    )
    pipeline = _load_json(pipeline_path)
    pipeline_result = validate_pipeline(pipeline_path)
    failures.extend(
        f"semantic optimization pipeline validation: {failure}"
        for failure in pipeline_result.failures
    )

    if pipeline.get("contract_id") != "objc3c.optimization.semantic.pipeline.v1":
        failures.append("semantic optimization pipeline contract_id drifted")
    issue_mapping = _as_dict(pipeline.get("issue_mapping"))
    issues = {int(issue) for issue in _as_list(issue_mapping.get("primary_issues"))}
    if not {8159, 8175}.issubset(issues):
        failures.append("semantic optimization pipeline must bind #8159 and #8175")

    pass_order = [
        str(pass_id)
        for pass_id in _as_list(_as_dict(pipeline.get("pass_order_contract")).get("ordered_pass_ids"))
    ]
    required_order = [str(pass_id) for pass_id in _as_list(section.get("required_pass_order"))]
    if pass_order != required_order:
        failures.append("semantic optimization pass order drifted from completion contract")

    pass_rows = {
        str(row.get("pass_id")): row
        for row in _as_list(pipeline.get("pass_registry"))
        if isinstance(row, dict) and row.get("pass_id")
    }
    preservation_rows = {
        str(row.get("pass_id")): row
        for row in _as_list(pipeline.get("semantic_preservation_contracts"))
        if isinstance(row, dict) and row.get("pass_id")
    }
    for pass_id in required_order:
        row = pass_rows.get(pass_id)
        if row is None:
            failures.append(f"semantic optimization pass missing: {pass_id}")
            continue
        if row.get("semantic_preserving") is not True:
            failures.append(f"semantic optimization pass is not semantic-preserving: {pass_id}")
        if row.get("mode") == "enabled" and row.get("rewrites_ir") is True:
            if not str(row.get("invalidation", "")):
                failures.append(f"mutating optimization pass lacks invalidation: {pass_id}")
        preservation = preservation_rows.get(pass_id)
        if preservation is None:
            failures.append(f"semantic optimization preservation row missing: {pass_id}")
        elif preservation.get("success_claim_on_skip") is not False:
            failures.append(f"semantic optimization pass can claim success on skip: {pass_id}")

    reserved_passes = [str(pass_id) for pass_id in _as_list(section.get("required_reserved_passes"))]
    for pass_id in reserved_passes:
        row = pass_rows.get(pass_id, {})
        if row.get("mode") != "reserved":
            failures.append(f"semantic optimization reserved pass is not reserved: {pass_id}")
        if row.get("rewrites_ir") is not False:
            failures.append(f"semantic optimization reserved pass rewrites IR: {pass_id}")
        for fixture in _as_list(row.get("fixtures")):
            fixture_payload = _load_json(
                _repo_path(
                    str(fixture),
                    failures,
                    f"semantic optimization reserved fixture {pass_id}",
                )
            )
            if fixture_payload.get("status") != "SKIPPED_FAIL_CLOSED":
                failures.append(f"semantic optimization reserved fixture does not skip: {pass_id}")
            if fixture_payload.get("success_claim") is not False:
                failures.append(f"semantic optimization reserved fixture claims success: {pass_id}")

    unsupported = _as_dict(pipeline.get("unsupported_policy"))
    for field in (
        "unsafe_flags_allowed",
        "whole_program_optimization_allowed",
        "compatibility_shims_allowed",
    ):
        if section.get(field) is not False:
            failures.append(f"completion contract must keep {field}=false")
        if unsupported.get(field) is not False:
            failures.append(f"semantic optimization unsupported policy drifted: {field}")
    if section.get("reserved_pass_success_claims_allowed") is not False:
        failures.append("completion contract permits reserved pass success claims")
    if unsupported.get("reserved_pass_success_claims_allowed") is not False:
        failures.append("semantic optimization policy permits reserved pass success claims")

    governance = _as_dict(pipeline.get("performance_governance"))
    if section.get("generated_report_input_allowed") is not False:
        failures.append("completion contract permits generated performance inputs")
    policy_text = str(governance.get("source_truth_policy", "")).lower()
    if "generated reports" not in policy_text or "not source truth" not in policy_text:
        failures.append("semantic optimization performance governance overclaims generated reports")
    for action in _as_list(section.get("required_performance_actions")):
        if str(action) not in {str(value) for value in _as_list(governance.get("required_public_actions"))}:
            failures.append(f"semantic optimization performance action missing: {action}")

    return {
        "pipeline_contract": repo_rel(pipeline_path),
        "pass_count": len(pass_rows),
        "reserved_passes": sorted(reserved_passes),
        "enabled_passes": sorted(
            pass_id for pass_id, row in pass_rows.items() if row.get("mode") == "enabled"
        ),
        "performance_governance_contract": governance.get("contract_id"),
        "pipeline_validation_status": pipeline_result.payload.get("status"),
    }


def _validate_performance_evidence(
    contract: dict[str, Any],
    failures: list[str],
) -> dict[str, Any]:
    section = _as_dict(contract.get("performance_evidence"))
    if section.get("support_authority_allowed") is not False:
        failures.append("completion contract permits performance support authority")
    if section.get("generated_report_authority_allowed") is not False:
        failures.append("completion contract permits generated report authority")

    workload_manifest = _load_json(
        _repo_path(str(section.get("workload_manifest", "")), failures, "workload manifest")
    )
    workload_rows = [
        row for row in _as_list(workload_manifest.get("workload_families")) if isinstance(row, dict)
    ]
    workload_ids = {str(row.get("workload_id")) for row in workload_rows}
    for workload_id in _as_list(section.get("required_workload_ids")):
        if str(workload_id) not in workload_ids:
            failures.append(f"runtime performance workload missing: {workload_id}")
    for row in workload_rows:
        if not str(row.get("support_boundary", "")):
            failures.append(f"runtime performance workload missing boundary text: {row.get('workload_id')}")
        for path_key in ("probe", "fixture"):
            _repo_path(
                str(row.get(path_key, "")),
                failures,
                f"runtime performance {row.get('workload_id')} {path_key}",
            )

    metadata_contract = _load_json(
        _repo_path(
            str(section.get("metadata_resilience_contract", "")),
            failures,
            "metadata resilience contract",
        )
    )
    for case in _as_list(metadata_contract.get("malformed_metadata_cases")):
        if not isinstance(case, dict):
            failures.append("metadata malformed case is not an object")
            continue
        expected = _as_dict(case.get("expected_handling"))
        if expected.get("fail_closed") is not True:
            failures.append(f"metadata case does not fail closed: {case.get('case_id')}")
        if expected.get("packet_written") is not False:
            failures.append(f"metadata case writes packets on failure: {case.get('case_id')}")
        if expected.get("generated_report_allowed") is not False:
            failures.append(f"metadata case allows generated report authority: {case.get('case_id')}")
        metadata_path = _repo_path(
            str(case.get("metadata_path", "")),
            failures,
            f"metadata malformed case {case.get('case_id')}",
        )
        try:
            json.loads(metadata_path.read_text(encoding="utf-8"))
        except json.JSONDecodeError:
            pass
        else:
            failures.append(f"metadata malformed fixture is valid JSON: {case.get('case_id')}")

    stress_contract = _load_json(
        _repo_path(
            str(section.get("stress_sanitizer_contract", "")),
            failures,
            "stress sanitizer contract",
        )
    )
    for row in _as_list(stress_contract.get("sanitizer_contracts")):
        if not isinstance(row, dict):
            failures.append("sanitizer row is not an object")
            continue
        if row.get("opt_in_only") is not True:
            failures.append(f"sanitizer is not opt-in: {row.get('sanitizer_id')}")
        if row.get("support_authority") is not False:
            failures.append(f"sanitizer overclaims support authority: {row.get('sanitizer_id')}")
    for row in _as_list(stress_contract.get("stress_scale_contracts")):
        if not isinstance(row, dict):
            failures.append("stress scale row is not an object")
            continue
        if row.get("support_authority") is not False:
            failures.append(f"stress scale overclaims support authority: {row.get('stress_id')}")
        if int(row.get("min_measured_runs", 0)) < 3:
            failures.append(f"stress scale run floor is too low: {row.get('stress_id')}")

    scale_contract = _load_json(
        _repo_path(
            str(section.get("scale_scenario_contract", "")),
            failures,
            "scale scenario contract",
        )
    )
    scenario_ids = set()
    for row in _as_list(scale_contract.get("scale_scenarios")):
        if not isinstance(row, dict):
            failures.append("scale scenario row is not an object")
            continue
        scenario_id = str(row.get("scenario_id", ""))
        scenario_ids.add(scenario_id)
        if row.get("support_authority") is not False:
            failures.append(f"scale scenario overclaims support authority: {scenario_id}")
        if row.get("generated_report_allowed") is not False:
            failures.append(f"scale scenario allows generated report authority: {scenario_id}")
        command = str(row.get("exact_replay_command", ""))
        workload_id = str(row.get("workload_id", ""))
        if not command.startswith(
            f"npm run objc3c -- benchmark-runtime-performance -- --workload-id {workload_id} "
        ):
            failures.append(f"scale scenario replay command is not workload-targeted: {scenario_id}")
    for scenario_id in _as_list(section.get("required_scale_scenarios")):
        if str(scenario_id) not in scenario_ids:
            failures.append(f"scale scenario missing: {scenario_id}")

    parser_manifest = _load_json(
        _repo_path(
            str(section.get("parser_sema_fuzz_manifest", "")),
            failures,
            "parser/sema fuzz manifest",
        )
    )
    subsystem_counts: dict[str, int] = {}
    for row in _as_list(parser_manifest.get("cases")):
        if not isinstance(row, dict):
            continue
        subsystem = str(row.get("subsystem", ""))
        subsystem_counts[subsystem] = subsystem_counts.get(subsystem, 0) + 1
        _repo_path(
            str(row.get("source_path", "")),
            failures,
            f"parser/sema fuzz case {row.get('case_id')}",
        )
    for subsystem in _as_list(section.get("required_fuzz_subsystems")):
        if subsystem_counts.get(str(subsystem), 0) == 0:
            failures.append(f"parser/sema fuzz subsystem missing: {subsystem}")

    scale_probe = _load_scale_probe_module()
    summary = scale_probe.build_scale_evidence_summary(
        workload_manifest=workload_manifest,
        replay_contract=_load_json(
            _repo_path(
                str(section.get("workload_replay_contract", "")),
                failures,
                "workload replay contract",
            )
        ),
        metadata_resilience_contract=metadata_contract,
        stress_sanitizer_contract=stress_contract,
        scale_scenario_contract=scale_contract,
        budget_model=_load_json(
            _repo_path(str(section.get("budget_model", "")), failures, "budget model")
        ),
        parser_sema_fuzz_manifest=parser_manifest,
        lowering_runtime_stress_manifest=_load_json(
            _repo_path(
                str(section.get("lowering_runtime_stress_manifest", "")),
                failures,
                "lowering/runtime stress manifest",
            )
        ),
    )
    if summary.get("status") != "PASS":
        failures.extend(
            f"runtime scale evidence validation: {failure}"
            for failure in _as_list(summary.get("failures"))
        )
    if summary.get("support_authority") is not False:
        failures.append("runtime scale evidence summary overclaims support authority")
    counts = _as_dict(summary.get("summary_counts"))
    for key, floor in _as_dict(section.get("minimum_counts")).items():
        if int(counts.get(str(key), 0)) < int(floor):
            failures.append(f"runtime scale evidence count below floor for {key}")
    for row in _as_list(summary.get("evidence_rows")):
        if isinstance(row, dict) and row.get("support_authority") is not False:
            failures.append(f"runtime scale evidence row overclaims support: {row}")

    return {
        "workload_count": len(workload_rows),
        "required_workload_count": len(_as_list(section.get("required_workload_ids"))),
        "scale_evidence_counts": counts,
        "scale_scenario_count": len(scenario_ids),
        "fuzz_subsystem_counts": subsystem_counts,
    }


def _validate_fail_closed_boundaries(
    contract: dict[str, Any],
    failures: list[str],
) -> dict[str, int]:
    boundary_rows = [
        row
        for row in _as_list(contract.get("fail_closed_boundaries"))
        if isinstance(row, dict)
    ]
    if len(boundary_rows) < 4:
        failures.append("completion contract must enumerate all fail-closed boundaries")
    boundary_ids = {str(row.get("boundary_id")) for row in boundary_rows}
    for required in (
        "public-reflection-unsupported-private-metadata",
        "semantic-optimization-reserved-pass-skip",
        "runtime-performance-malformed-metadata",
        "runtime-performance-generated-report-provenance",
    ):
        if required not in boundary_ids:
            failures.append(f"fail-closed boundary missing: {required}")

    for row in boundary_rows:
        boundary_id = str(row.get("boundary_id", ""))
        if boundary_id == "public-reflection-unsupported-private-metadata":
            if row.get("must_not_expose_private_snapshot") is not True:
                failures.append("public reflection boundary must block private snapshots")
        elif boundary_id == "semantic-optimization-reserved-pass-skip":
            if row.get("must_not_claim_success") is not True:
                failures.append("semantic optimization reserved boundary must block success claims")
        elif boundary_id == "runtime-performance-malformed-metadata":
            if row.get("must_fail_closed") is not True:
                failures.append("runtime performance metadata boundary must fail closed")
        elif boundary_id == "runtime-performance-generated-report-provenance":
            if row.get("support_authority") is not False:
                failures.append("runtime performance report boundary must not be support authority")

    return {"boundary_count": len(boundary_rows)}


def validate_completion_contract(
    contract_path: Path = CONTRACT_PATH,
) -> CompletionContractResult:
    failures: list[str] = []
    contract = _load_json(contract_path)

    if contract.get("contract_id") != "objc3c.reflection.optimization.performance.completion.v1":
        failures.append("completion contract_id drifted")
    issue_mapping = _as_dict(contract.get("issue_mapping"))
    issues = {int(issue) for issue in _as_list(issue_mapping.get("issues"))}
    if not {8174, 8175, 8159}.issubset(issues):
        failures.append("completion contract must bind issues #8174, #8175, and #8159")

    public_reflection = _validate_public_reflection(contract, failures)
    semantic_optimization = _validate_semantic_optimization(contract, failures)
    performance_evidence = _validate_performance_evidence(contract, failures)
    fail_closed_boundaries = _validate_fail_closed_boundaries(contract, failures)

    payload = {
        "contract_id": "objc3c.reflection.optimization.performance.completion.validation.v1",
        "status": "PASS" if not failures else "FAIL",
        "contract_path": repo_rel(contract_path) if contract_path.is_relative_to(ROOT) else str(contract_path),
        "issues": sorted(issues),
        "public_reflection": public_reflection,
        "semantic_optimization": semantic_optimization,
        "performance_evidence": performance_evidence,
        "fail_closed_boundaries": fail_closed_boundaries,
        "failures": failures,
    }
    return CompletionContractResult(payload=payload, failures=failures)


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--contract", type=Path, default=CONTRACT_PATH)
    parser.add_argument("--summary-out", type=Path, default=REPORT_PATH)
    return parser.parse_args(argv)


def main(argv: Sequence[str] | None = None) -> int:
    args = parse_args(sys.argv[1:] if argv is None else argv)
    result = validate_completion_contract(args.contract)
    write_json_file(args.summary_out, result.payload)
    print(f"summary_path: {repo_rel(args.summary_out)}")
    print(f"reflection-optimization-performance-completion: {result.payload['status']}")
    for failure in result.failures:
        print(f"failure: {failure}")
    return 0 if result.passed else 1


__all__ = [
    "CONTRACT_PATH",
    "REPORT_PATH",
    "CompletionContractResult",
    "copy",
    "main",
    "validate_completion_contract",
    "_load_json",
    "_write_variant",
]


if __name__ == "__main__":
    raise SystemExit(main())
