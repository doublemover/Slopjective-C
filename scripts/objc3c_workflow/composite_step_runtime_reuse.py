"""Runtime acceptance report reuse for composite workflow steps."""

from __future__ import annotations

import json
import os
from pathlib import Path
from collections.abc import Sequence

from .composite_step_payload import composite_step_payload
from .environment import ROOT
from scripts.objc3c_runtime_acceptance.cli_arguments import parse_args
from scripts.objc3c_runtime_acceptance.reuse_identity import (
    runtime_acceptance_options,
    validate_runtime_acceptance_reuse_identity,
)
from scripts.objc3c_runtime_acceptance.suite_catalog import RUNTIME_ACCEPTANCE_SUITE_CASES
from scripts.objc3c_workflow.actions.runtime_acceptance_route_catalog import RUNTIME_ACCEPTANCE_ROUTES


def _valid_runtime_report(
    report_path: Path,
    expected_labels: set[str],
    expected_options_digest: str | None = None,
) -> tuple[bool, str]:
    try:
        report = json.loads(report_path.read_text(encoding="utf-8"))
    except (OSError, json.JSONDecodeError):
        return False, "report missing or invalid"
    if not isinstance(report, dict) or report.get("status") != "PASS":
        return False, "report not passed"
    identity = report.get("runtime_reuse_identity")
    valid_identity, identity_reason = validate_runtime_acceptance_reuse_identity(identity, root=ROOT)
    if not valid_identity:
        return False, identity_reason
    if not expected_options_digest or report.get("options_digest") != expected_options_digest:
        return False, "options identity mismatch"
    if identity.get("options_digest") != expected_options_digest:
        return False, "producer options identity mismatch"
    case_labels = report.get("case_labels")
    if not isinstance(case_labels, list) or set(map(str, case_labels)) != expected_labels:
        return False, "case label inventory mismatch"
    if identity.get("case_labels") != case_labels:
        return False, "producer case label identity mismatch"
    pairs = identity.get("fixture_label_pairs")
    if not isinstance(pairs, list) or any(not isinstance(item, dict) for item in pairs):
        return False, "producer fixture mapping missing"
    labels_from_pairs = [str(item.get("label", "")) for item in pairs]
    fixtures_from_pairs = sorted(str(item.get("case_id", "")) for item in pairs)
    if len(set(labels_from_pairs)) != len(labels_from_pairs) or set(labels_from_pairs) != expected_labels:
        return False, "producer fixture mapping mismatch"
    fixture_ids = report.get("fixture_ids")
    if not isinstance(fixture_ids, list) or sorted(map(str, fixture_ids)) != fixtures_from_pairs:
        return False, "fixture inventory mismatch"
    if identity.get("fixture_ids") != fixtures_from_pairs:
        return False, "producer fixture identity mismatch"
    return True, "verified"


def _expected_run(action: str, command: Sequence[str], report_path: Path) -> tuple[set[str], str] | None:
    route = RUNTIME_ACCEPTANCE_ROUTES.get(action)
    if route is None:
        return None
    args: list[str] = ["--suite", route.suite]
    command_tokens = list(command)
    for index, token in enumerate(command_tokens[:-1]):
        if token == "--suite":
            args = ["--suite", command_tokens[index + 1]]
            break
    explicit_cases: list[str] = []
    for index, token in enumerate(command_tokens[:-1]):
        if token == "--case":
            explicit_cases.append(command_tokens[index + 1])
    for index, token in enumerate(command_tokens[:-1]):
        if token == "--suite" and command_tokens[index + 1] != route.suite:
            return None
    args.extend(value for case in explicit_cases for value in ("--case", case))
    try:
        parsed = parse_args(args)
    except SystemExit:
        return None
    if parsed.cases:
        expected_fixtures = set(parsed.cases)
    else:
        expected_fixtures = set(RUNTIME_ACCEPTANCE_SUITE_CASES.get(parsed.suite, ()))
        if not expected_fixtures:
            try:
                report = json.loads(report_path.read_text(encoding="utf-8"))
            except (OSError, json.JSONDecodeError):
                return None
            available = report.get("available_suites", {}) if isinstance(report, dict) else {}
            full_cases = available.get("full") if isinstance(available, dict) else None
            if not isinstance(full_cases, list) or not full_cases:
                return None
            expected_fixtures = {str(value) for value in full_cases}
    return expected_fixtures, __import__("hashlib").sha256(
        __import__("json").dumps(runtime_acceptance_options(parsed), sort_keys=True, separators=(",", ":"), ensure_ascii=False).encode("utf-8")
    ).hexdigest()


def runtime_acceptance_reuse_step(action: str, command: Sequence[str], started_at: float) -> dict[str, object] | None:
    if not (action.startswith("test-runtime-acceptance") and os.environ.get("OBJC3C_SKIP_RUNTIME_ACCEPTANCE_RERUN") == "1"):
        return None
    report_path = ROOT / "tmp/reports/runtime/acceptance/summary.json"
    expected_run = _expected_run(action, command, report_path)
    if expected_run is None:
        return None
    expected_fixtures, expected_options_digest = expected_run
    valid, reason = _valid_runtime_report(report_path, expected_fixtures, expected_options_digest)
    if not valid:
        return None
    return composite_step_payload(
        action=action,
        command=command,
        exit_code=0,
        report_paths=[report_path.relative_to(ROOT).as_posix()],
        started_at=started_at,
        extra={"report_reused": True, "report_reuse_source": "verified-runtime-acceptance-report", "report_reuse_validation": reason},
    )


__all__ = ["runtime_acceptance_reuse_step"]
