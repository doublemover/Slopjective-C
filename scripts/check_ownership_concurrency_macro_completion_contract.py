#!/usr/bin/env python3
"""Validate the ownership/concurrency/macro completion contract."""

from __future__ import annotations

import json
from dataclasses import dataclass
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
CONTRACT_ID = "objc3c.ownership-concurrency-macro.completion.contract.v1"
SUMMARY_CONTRACT_ID = "objc3c.ownership-concurrency-macro.completion.summary.v1"
EXPECTED_ISSUES = {8166, 8167, 8168}
CONTRACT_PATH = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "ownership_concurrency_macro_completion"
    / "contract.json"
)
REPORT_PATH = (
    ROOT / "tmp" / "reports" / "ownership-concurrency-macro-completion.json"
)
REQUIRED_GROUPS = {
    "ownership_qualifiers_lifetimes_block_runtime_hooks",
    "public_concurrency_task_actor_usability",
    "macro_expansion_artifact_trust_sandbox_boundaries",
}
ALLOWED_UNSUPPORTED_STATUSES = {"reserved", "rejected", "deferred", "internal"}


@dataclass(frozen=True)
class CompletionContractResult:
    payload: dict[str, Any]
    failures: list[str]

    @property
    def passed(self) -> bool:
        return not self.failures


def _repo_rel(path: Path) -> str:
    try:
        return path.resolve().relative_to(ROOT.resolve()).as_posix()
    except ValueError:
        return path.as_posix()


def _load_json(path: Path) -> dict[str, Any]:
    with path.open(encoding="utf-8") as handle:
        payload = json.load(handle)
    if not isinstance(payload, dict):
        raise ValueError(f"{_repo_rel(path)} must contain a JSON object")
    return payload


def _as_list(value: object) -> list[object]:
    return value if isinstance(value, list) else []


def _as_dict(value: object) -> dict[str, Any]:
    return value if isinstance(value, dict) else {}


def _string_set(value: object) -> set[str]:
    return {str(item) for item in _as_list(value)}


def _normalized(path: str) -> str:
    return path.replace("\\", "/")


def _is_forbidden_path(path: str, forbidden_prefixes: list[str]) -> bool:
    normalized = _normalized(path)
    return any(normalized.startswith(prefix) for prefix in forbidden_prefixes)


def _source_text(
    relative_path: str,
    failures: list[str],
    forbidden_prefixes: list[str],
    *,
    label: str,
) -> str:
    if not relative_path:
        failures.append(f"{label}: missing source path")
        return ""
    if _is_forbidden_path(relative_path, forbidden_prefixes):
        failures.append(f"{label}: forbidden source-truth path {relative_path}")
        return ""
    path = ROOT / relative_path
    if not path.is_file():
        failures.append(f"{label}: source path missing {relative_path}")
        return ""
    return path.read_text(encoding="utf-8")


def _validate_source_record(
    record: dict[str, Any],
    failures: list[str],
    forbidden_prefixes: list[str],
    *,
    label: str,
) -> None:
    path = str(record.get("path", ""))
    text = _source_text(path, failures, forbidden_prefixes, label=label)
    tokens = [str(token) for token in _as_list(record.get("required_tokens"))]
    if not tokens:
        failures.append(f"{label}: source record has no required tokens")
    for token in tokens:
        if token not in text:
            failures.append(f"{label}: missing token in {path}: {token}")


def _validate_source_groups(
    contract: dict[str, Any],
    failures: list[str],
    forbidden_prefixes: list[str],
) -> dict[str, int]:
    group_counts: dict[str, int] = {}
    seen_groups: set[str] = set()
    for group in _as_list(contract.get("source_surface_groups")):
        if not isinstance(group, dict):
            failures.append("source_surface_groups entry is not an object")
            continue
        group_id = str(group.get("group_id", ""))
        seen_groups.add(group_id)
        if int(group.get("issue_ref", 0)) not in EXPECTED_ISSUES:
            failures.append(f"{group_id}: unexpected issue_ref {group.get('issue_ref')}")
        records = [record for record in _as_list(group.get("source_records")) if isinstance(record, dict)]
        group_counts[group_id] = len(records)
        minimum = int(group.get("minimum_source_records", 1))
        if len(records) < minimum:
            failures.append(f"{group_id}: expected at least {minimum} source records")
        for index, record in enumerate(records):
            _validate_source_record(
                record,
                failures,
                forbidden_prefixes,
                label=f"{group_id}.source_records[{index}]",
            )

    missing_groups = REQUIRED_GROUPS - seen_groups
    if missing_groups:
        failures.append(f"missing required source groups: {sorted(missing_groups)}")
    return group_counts


def _validate_issue_invariants(contract: dict[str, Any], failures: list[str]) -> dict[str, bool]:
    invariants = _as_dict(contract.get("issue_invariants"))
    checks: dict[str, bool] = {}

    ownership = _as_dict(invariants.get("8166"))
    checks["8166:qualifier_inventory"] = {
        "strong",
        "weak",
        "unowned",
        "borrowed",
        "consumed",
        "autoreleased",
    } <= _string_set(ownership.get("ownership_qualifiers"))
    checks["8166:lifetime_boundaries"] = {
        "block_capture_copy_dispose",
        "runtime_result_fail_closed",
        "weak_runtime_hooks",
    } <= _string_set(ownership.get("lifetime_boundaries"))
    checks["8166:runtime_hooks"] = bool(ownership.get("runtime_hooks_source_backed"))

    concurrency = _as_dict(invariants.get("8167"))
    checks["8167:api_families"] = {
        "task_spawn",
        "task_group_cancellation",
        "executor_hop",
        "actor_mailbox",
    } <= _string_set(concurrency.get("public_api_families"))
    checks["8167:runtime_backed"] = bool(concurrency.get("task_actor_runtime_backed"))
    checks["8167:unsupported_broad_async_reserved"] = bool(
        concurrency.get("unsupported_broad_async_claims_reserved")
    )

    macro = _as_dict(invariants.get("8168"))
    checks["8168:generated_artifact_not_authority"] = (
        macro.get("generated_artifact_support_claim_authority") is False
    )
    checks["8168:trust_required"] = bool(macro.get("package_trust_required"))
    checks["8168:sandbox_deny_default"] = bool(macro.get("sandbox_deny_by_default"))
    checks["8168:unsupported_host_io_rejected"] = bool(
        macro.get("unsupported_host_network_filesystem_rejected")
    )

    for name, passed in checks.items():
        if not passed:
            failures.append(f"issue invariant failed: {name}")
    return checks


def _validate_unsupported_behaviors(
    contract: dict[str, Any],
    failures: list[str],
    forbidden_prefixes: list[str],
) -> int:
    count = 0
    for behavior in _as_list(contract.get("unsupported_behaviors")):
        if not isinstance(behavior, dict):
            failures.append("unsupported_behaviors entry is not an object")
            continue
        count += 1
        behavior_id = str(behavior.get("id", ""))
        status = str(behavior.get("status", ""))
        if status not in ALLOWED_UNSUPPORTED_STATUSES:
            failures.append(f"{behavior_id}: unsupported behavior status widened to {status}")
        if int(behavior.get("issue_ref", 0)) not in EXPECTED_ISSUES:
            failures.append(f"{behavior_id}: unexpected issue_ref {behavior.get('issue_ref')}")
        anchor = _as_dict(behavior.get("anchor"))
        _validate_source_record(
            {
                "path": anchor.get("path", ""),
                "required_tokens": anchor.get("required_tokens", []),
            },
            failures,
            forbidden_prefixes,
            label=f"{behavior_id}.anchor",
        )
    return count


def _validate_fail_closed_fixture_cases(
    contract: dict[str, Any],
    failures: list[str],
    forbidden_prefixes: list[str],
) -> int:
    count = 0
    for case in _as_list(contract.get("fail_closed_fixture_cases")):
        if not isinstance(case, dict):
            failures.append("fail_closed_fixture_cases entry is not an object")
            continue
        count += 1
        fixture_path = str(case.get("path", ""))
        expected_fragment = str(case.get("expected_failure_fragment", ""))
        if _is_forbidden_path(fixture_path, forbidden_prefixes):
            failures.append(f"fail-closed fixture uses forbidden path: {fixture_path}")
            continue
        path = ROOT / fixture_path
        if not path.is_file():
            failures.append(f"fail-closed fixture missing: {fixture_path}")
            continue
        fixture_result = _validate_contract_path(path, validate_fail_closed_cases=False)
        if fixture_result.passed:
            failures.append(f"fail-closed fixture unexpectedly passed: {fixture_path}")
        elif expected_fragment and not any(
            expected_fragment in failure for failure in fixture_result.failures
        ):
            failures.append(
                f"fail-closed fixture {fixture_path} did not fail with {expected_fragment}"
            )
    return count


def _validate_contract_path(
    contract_path: Path,
    *,
    validate_fail_closed_cases: bool,
) -> CompletionContractResult:
    contract = _load_json(contract_path)
    failures: list[str] = []
    forbidden_prefixes = [
        str(prefix) for prefix in _as_list(contract.get("forbidden_source_truth_prefixes"))
    ]

    if contract.get("contract_id") != CONTRACT_ID:
        failures.append("contract_id drifted")
    if {int(issue) for issue in _as_list(contract.get("issue_refs"))} != EXPECTED_ISSUES:
        failures.append("issue_refs must exactly cover #8166, #8167, and #8168")
    if contract.get("source_truth") != "checked-in source files and checked-in contract fixtures only":
        failures.append("source_truth must stay checked-in and non-generated")

    group_counts = _validate_source_groups(contract, failures, forbidden_prefixes)
    invariant_checks = _validate_issue_invariants(contract, failures)
    unsupported_count = _validate_unsupported_behaviors(
        contract, failures, forbidden_prefixes
    )
    fail_closed_case_count = (
        _validate_fail_closed_fixture_cases(contract, failures, forbidden_prefixes)
        if validate_fail_closed_cases
        else len(_as_list(contract.get("fail_closed_fixture_cases")))
    )

    payload = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "source_contract_id": contract.get("contract_id"),
        "status": "PASS" if not failures else "FAIL",
        "contract_path": _repo_rel(contract_path),
        "issue_refs": contract.get("issue_refs", []),
        "source_group_counts": group_counts,
        "invariant_checks": invariant_checks,
        "unsupported_behavior_count": unsupported_count,
        "fail_closed_fixture_case_count": fail_closed_case_count,
        "failures": failures,
    }
    return CompletionContractResult(payload=payload, failures=failures)


def validate_ownership_concurrency_macro_completion_contract(
    contract_path: Path = CONTRACT_PATH,
) -> CompletionContractResult:
    return _validate_contract_path(contract_path, validate_fail_closed_cases=True)


def main() -> int:
    result = validate_ownership_concurrency_macro_completion_contract()
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    REPORT_PATH.write_text(
        json.dumps(result.payload, indent=2, sort_keys=True) + "\n",
        encoding="utf-8",
    )
    print(f"summary_path: {_repo_rel(REPORT_PATH)}")
    return 0 if result.passed else 1


if __name__ == "__main__":
    raise SystemExit(main())
