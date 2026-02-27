#!/usr/bin/env python3
"""Fail-closed contract validator for v0.15 M28 lane-B activation tooling."""

from __future__ import annotations

import argparse
import json
import subprocess
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "activation-execution-hardening-w2-contract-v1"
ACTIVATION_SEED_CONTRACT_ID = "activation-seed-contract/v0.15"
DEFAULT_ACTIONABLE_STATUSES = ("open", "open-blocked", "blocked")

DEFAULT_CHECKER_SCRIPT_PATH = ROOT / "scripts" / "check_activation_triggers.py"
DEFAULT_FIXTURE_ROOT = ROOT / "tests" / "tooling" / "fixtures" / "activation_triggers"

TRIGGER_ORDER = ("T1-ISSUES", "T2-MILESTONES", "T3-ACTIONABLE-ROWS", "T5-OPEN-BLOCKERS")
TRIGGER_CONDITIONS = {
    "T1-ISSUES": "open issues > 0",
    "T2-MILESTONES": "open milestones > 0",
    "T3-ACTIONABLE-ROWS": "actionable catalog rows > 0",
    "T5-OPEN-BLOCKERS": "open blockers > 0",
}

EXPECTED_PAYLOAD_KEY_ORDER = (
    "mode",
    "contract_id",
    "fail_closed",
    "inputs",
    "actionable_statuses",
    "trigger_order",
    "freshness",
    "triggers",
    "active_trigger_ids",
    "activation_required",
    "open_blockers",
    "t4_governance_overlay",
    "gate_open",
    "queue_state",
    "exit_code",
)
EXPECTED_INPUT_KEY_ORDER = (
    "issues_json",
    "milestones_json",
    "catalog_json",
    "open_blockers_json",
    "t4_governance_overlay_json",
)
EXPECTED_FRESHNESS_KEY_ORDER = ("issues", "milestones")
EXPECTED_FRESHNESS_ENTRY_KEY_ORDER = (
    "requested",
    "max_age_seconds",
    "generated_at_utc",
    "age_seconds",
    "fresh",
)
EXPECTED_TRIGGER_ENTRY_KEY_ORDER = ("id", "condition", "count", "fired")
EXPECTED_OPEN_BLOCKERS_KEY_ORDER = ("count", "trigger_id", "trigger_fired")
EXPECTED_T4_OVERLAY_KEY_ORDER = ("new_scope_publish", "source")


@dataclass(frozen=True)
class DriftFinding:
    check_id: str
    detail: str


@dataclass(frozen=True)
class CheckerRun:
    exit_code: int
    stdout: str
    stderr: str
    command: list[str]


@dataclass(frozen=True)
class HappyScenario:
    check_id: str
    scenario: str
    output_format: str
    expected_output_name: str
    expected_exit_code: int


@dataclass(frozen=True)
class FailClosedScenario:
    check_id: str
    scenario: str


BASE_HAPPY_SCENARIOS: tuple[HappyScenario, ...] = (
    HappyScenario(
        check_id="W2C-01",
        scenario="zero_open",
        output_format="json",
        expected_output_name="expected.json",
        expected_exit_code=0,
    ),
    HappyScenario(
        check_id="W2C-02",
        scenario="zero_open",
        output_format="markdown",
        expected_output_name="expected.md",
        expected_exit_code=0,
    ),
    HappyScenario(
        check_id="W2C-03",
        scenario="activation_triggered",
        output_format="json",
        expected_output_name="expected.json",
        expected_exit_code=1,
    ),
    HappyScenario(
        check_id="W2C-04",
        scenario="activation_triggered",
        output_format="markdown",
        expected_output_name="expected.md",
        expected_exit_code=1,
    ),
)
SCHEMA_CHECK_IDS: tuple[str, ...] = (
    "W2C-05",
    "W2C-06",
    "W2C-07",
    "W2C-08",
    "W2C-09",
    "W2C-17",
    "W2C-18",
    "W2C-19",
    "W2C-20",
)


def discover_happy_scenarios(*, fixture_root: Path) -> list[HappyScenario]:
    scenarios = list(BASE_HAPPY_SCENARIOS)
    baseline_scenarios = {scenario.scenario for scenario in BASE_HAPPY_SCENARIOS}

    extra_roots = sorted(
        (
            path
            for path in fixture_root.iterdir()
            if path.is_dir()
            and path.name not in baseline_scenarios
            and not (path / "expected_exit_code.txt").exists()
            and (path / "expected.json").exists()
            and (path / "expected.md").exists()
        ),
        key=lambda path: path.name.casefold(),
    )

    for index, scenario_root in enumerate(extra_roots, start=1):
        expected_json_path = scenario_root / "expected.json"
        raw_expected_json = load_text(
            expected_json_path,
            label=f"happy-scenario expected JSON for {scenario_root.name}",
        )
        try:
            payload = json.loads(raw_expected_json)
        except json.JSONDecodeError as exc:
            raise ValueError(
                f"happy-scenario expected JSON must be valid object: {display_path(expected_json_path)}"
            ) from exc
        if not isinstance(payload, dict):
            raise ValueError(
                f"happy-scenario expected JSON root must be object: {display_path(expected_json_path)}"
            )
        expected_exit_code = payload.get("exit_code")
        if (
            isinstance(expected_exit_code, bool)
            or not isinstance(expected_exit_code, int)
            or expected_exit_code not in (0, 1)
        ):
            raise ValueError(
                "happy-scenario expected JSON must include integer field "
                f"'exit_code' in {{0,1}}: {display_path(expected_json_path)}"
            )

        scenario_name = scenario_root.name
        scenarios.append(
            HappyScenario(
                check_id=f"W2C-H{index:02d}J",
                scenario=scenario_name,
                output_format="json",
                expected_output_name="expected.json",
                expected_exit_code=expected_exit_code,
            )
        )
        scenarios.append(
            HappyScenario(
                check_id=f"W2C-H{index:02d}M",
                scenario=scenario_name,
                output_format="markdown",
                expected_output_name="expected.md",
                expected_exit_code=expected_exit_code,
            )
        )

    return scenarios


def discover_fail_closed_scenarios(*, fixture_root: Path) -> list[FailClosedScenario]:
    scenario_roots = sorted(
        (
            path
            for path in fixture_root.iterdir()
            if path.is_dir()
            and (path / "expected_exit_code.txt").exists()
            and (path / "expected_stderr.txt").exists()
        ),
        key=lambda path: path.name.casefold(),
    )

    scenarios: list[FailClosedScenario] = []
    for index, scenario_root in enumerate(scenario_roots, start=1):
        scenario_name = scenario_root.name
        expected_exit_code_path = scenario_root / "expected_exit_code.txt"
        expected_stderr_path = scenario_root / "expected_stderr.txt"
        if not expected_exit_code_path.exists() or not expected_stderr_path.exists():
            raise ValueError(
                "missing required fail-closed fixture scenario files for "
                f"{scenario_name!r} under {display_path(scenario_root)}"
            )
        for required_input_name in ("issues.json", "milestones.json", "catalog.json"):
            required_input_path = scenario_root / required_input_name
            if not required_input_path.exists():
                raise ValueError(
                    "missing required fail-closed fixture input "
                    f"{required_input_name!r} for {scenario_name!r} under "
                    f"{display_path(scenario_root)}"
                )
        scenarios.append(
            FailClosedScenario(
                check_id=f"W2C-F{index:02d}",
                scenario=scenario_name,
            )
        )
    return scenarios


def display_path(path: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(ROOT).as_posix()
    except ValueError:
        return resolved.as_posix()


def resolve_input_path(raw_path: Path) -> Path:
    if raw_path.is_absolute():
        return raw_path
    return ROOT / raw_path


def load_text(path: Path, *, label: str) -> str:
    if not path.exists():
        raise ValueError(f"{label} does not exist: {display_path(path)}")
    if not path.is_file():
        raise ValueError(f"{label} is not a file: {display_path(path)}")
    try:
        return path.read_text(encoding="utf-8")
    except UnicodeDecodeError as exc:
        raise ValueError(f"{label} is not valid UTF-8: {display_path(path)}") from exc
    except OSError as exc:
        raise ValueError(f"unable to read {label} {display_path(path)}: {exc}") from exc


def load_expected_exit_code(path: Path) -> int:
    value = load_text(path, label="expected exit-code file").strip()
    try:
        return int(value)
    except ValueError as exc:
        raise ValueError(
            f"expected exit-code file must contain an integer: {display_path(path)}"
        ) from exc


def shell_quote(token: str) -> str:
    if any(character.isspace() for character in token):
        return f'"{token}"'
    return token


def render_command(tokens: list[str]) -> str:
    return " ".join(shell_quote(token) for token in tokens)


def run_checker(
    *,
    checker_script: Path,
    fixture_root: Path,
    scenario: str,
    output_format: str,
    include_open_blockers: bool = False,
    include_t4_overlay: bool = False,
) -> CheckerRun:
    scenario_root = fixture_root / scenario
    command = [
        sys.executable,
        str(checker_script),
        "--issues-json",
        str(scenario_root / "issues.json"),
        "--milestones-json",
        str(scenario_root / "milestones.json"),
        "--catalog-json",
        str(scenario_root / "catalog.json"),
    ]
    if include_open_blockers:
        command.extend(
            [
                "--open-blockers-json",
                str(scenario_root / "open_blockers.json"),
            ]
        )
    if include_t4_overlay:
        command.extend(
            [
                "--t4-governance-overlay-json",
                str(scenario_root / "t4_overlay.json"),
            ]
        )
    command.extend(["--format", output_format])
    completed = subprocess.run(
        command,
        check=False,
        capture_output=True,
        text=True,
        encoding="utf-8",
    )
    return CheckerRun(
        exit_code=completed.returncode,
        stdout=completed.stdout,
        stderr=completed.stderr,
        command=command,
    )


def validate_zero_open_json_schema(
    payload: dict[str, object],
) -> list[DriftFinding]:
    findings: list[DriftFinding] = []

    if tuple(payload.keys()) != EXPECTED_PAYLOAD_KEY_ORDER:
        findings.append(
            DriftFinding(
                check_id="W2C-05",
                detail=(
                    "top-level payload key order drift: "
                    f"expected={list(EXPECTED_PAYLOAD_KEY_ORDER)!r} "
                    f"observed={list(payload.keys())!r}"
                ),
            )
        )

    inputs = payload.get("inputs")
    if not isinstance(inputs, dict):
        findings.append(DriftFinding(check_id="W2C-06", detail="payload field 'inputs' must be an object"))
    elif tuple(inputs.keys()) != EXPECTED_INPUT_KEY_ORDER:
        findings.append(
            DriftFinding(
                check_id="W2C-06",
                detail=(
                    "inputs key order drift: "
                    f"expected={list(EXPECTED_INPUT_KEY_ORDER)!r} "
                    f"observed={list(inputs.keys())!r}"
                ),
            )
        )

    freshness = payload.get("freshness")
    if not isinstance(freshness, dict):
        findings.append(
            DriftFinding(check_id="W2C-07", detail="payload field 'freshness' must be an object")
        )
    else:
        if tuple(freshness.keys()) != EXPECTED_FRESHNESS_KEY_ORDER:
            findings.append(
                DriftFinding(
                    check_id="W2C-07",
                    detail=(
                        "freshness key order drift: "
                        f"expected={list(EXPECTED_FRESHNESS_KEY_ORDER)!r} "
                        f"observed={list(freshness.keys())!r}"
                    ),
                )
            )
        for label in EXPECTED_FRESHNESS_KEY_ORDER:
            entry = freshness.get(label)
            if not isinstance(entry, dict):
                findings.append(
                    DriftFinding(
                        check_id="W2C-07",
                        detail=f"freshness.{label} must be an object",
                    )
                )
                continue
            if tuple(entry.keys()) != EXPECTED_FRESHNESS_ENTRY_KEY_ORDER:
                findings.append(
                    DriftFinding(
                        check_id="W2C-07",
                        detail=(
                            f"freshness.{label} key order drift: "
                            f"expected={list(EXPECTED_FRESHNESS_ENTRY_KEY_ORDER)!r} "
                            f"observed={list(entry.keys())!r}"
                        ),
                    )
                )

    trigger_order = payload.get("trigger_order")
    triggers = payload.get("triggers")
    if trigger_order != list(TRIGGER_ORDER):
        findings.append(
            DriftFinding(
                check_id="W2C-08",
                detail=(
                    "trigger_order drift: "
                    f"expected={list(TRIGGER_ORDER)!r} observed={trigger_order!r}"
                ),
            )
        )
    if not isinstance(triggers, list):
        findings.append(
            DriftFinding(check_id="W2C-08", detail="payload field 'triggers' must be an array")
        )
    else:
        observed_trigger_ids: list[str] = []
        for index, entry in enumerate(triggers):
            if not isinstance(entry, dict):
                findings.append(
                    DriftFinding(
                        check_id="W2C-08",
                        detail=f"trigger row {index} must be an object",
                    )
                )
                continue
            if tuple(entry.keys()) != EXPECTED_TRIGGER_ENTRY_KEY_ORDER:
                findings.append(
                    DriftFinding(
                        check_id="W2C-08",
                        detail=(
                            f"trigger row {index} key order drift: "
                            f"expected={list(EXPECTED_TRIGGER_ENTRY_KEY_ORDER)!r} "
                            f"observed={list(entry.keys())!r}"
                        ),
                    )
                )
            trigger_id = entry.get("id")
            if isinstance(trigger_id, str):
                observed_trigger_ids.append(trigger_id)
        if observed_trigger_ids != list(TRIGGER_ORDER):
            findings.append(
                DriftFinding(
                    check_id="W2C-08",
                    detail=(
                        "trigger row id order drift: "
                        f"expected={list(TRIGGER_ORDER)!r} observed={observed_trigger_ids!r}"
                    ),
                )
            )

    open_blockers = payload.get("open_blockers")
    if not isinstance(open_blockers, dict):
        findings.append(
            DriftFinding(check_id="W2C-09", detail="payload field 'open_blockers' must be an object")
        )
    elif tuple(open_blockers.keys()) != EXPECTED_OPEN_BLOCKERS_KEY_ORDER:
        findings.append(
            DriftFinding(
                check_id="W2C-09",
                detail=(
                    "open_blockers key order drift: "
                    f"expected={list(EXPECTED_OPEN_BLOCKERS_KEY_ORDER)!r} "
                    f"observed={list(open_blockers.keys())!r}"
                ),
            )
        )

    t4_overlay = payload.get("t4_governance_overlay")
    if not isinstance(t4_overlay, dict):
        findings.append(
            DriftFinding(
                check_id="W2C-09",
                detail="payload field 't4_governance_overlay' must be an object",
            )
        )
    elif tuple(t4_overlay.keys()) != EXPECTED_T4_OVERLAY_KEY_ORDER:
        findings.append(
            DriftFinding(
                check_id="W2C-09",
                detail=(
                    "t4_governance_overlay key order drift: "
                    f"expected={list(EXPECTED_T4_OVERLAY_KEY_ORDER)!r} "
                    f"observed={list(t4_overlay.keys())!r}"
                ),
            )
        )

    return findings


def validate_zero_open_json_semantics(
    payload: dict[str, object],
    *,
    fixture_root: Path,
) -> list[DriftFinding]:
    findings: list[DriftFinding] = []

    if payload.get("mode") != "offline-deterministic":
        findings.append(
            DriftFinding(
                check_id="W2C-17",
                detail=(
                    "payload field 'mode' drift: "
                    f"expected='offline-deterministic' observed={payload.get('mode')!r}"
                ),
            )
        )

    if payload.get("contract_id") != ACTIVATION_SEED_CONTRACT_ID:
        findings.append(
            DriftFinding(
                check_id="W2C-17",
                detail=(
                    "payload field 'contract_id' drift: "
                    f"expected={ACTIVATION_SEED_CONTRACT_ID!r} "
                    f"observed={payload.get('contract_id')!r}"
                ),
            )
        )

    if payload.get("fail_closed") is not True:
        findings.append(
            DriftFinding(
                check_id="W2C-17",
                detail=f"payload field 'fail_closed' must be true; observed={payload.get('fail_closed')!r}",
            )
        )

    actionable_statuses = payload.get("actionable_statuses")
    if not isinstance(actionable_statuses, list) or not all(
        isinstance(status, str) and status for status in actionable_statuses
    ):
        findings.append(
            DriftFinding(
                check_id="W2C-17",
                detail="payload field 'actionable_statuses' must be a non-empty string array",
            )
        )
    else:
        if actionable_statuses != list(DEFAULT_ACTIONABLE_STATUSES):
            findings.append(
                DriftFinding(
                    check_id="W2C-17",
                    detail=(
                        "payload field 'actionable_statuses' drift: "
                        f"expected={list(DEFAULT_ACTIONABLE_STATUSES)!r} "
                        f"observed={actionable_statuses!r}"
                    ),
                )
            )
        if len(set(actionable_statuses)) != len(actionable_statuses):
            findings.append(
                DriftFinding(
                    check_id="W2C-17",
                    detail="payload field 'actionable_statuses' must not contain duplicates",
                )
            )

    if payload.get("active_trigger_ids") != []:
        findings.append(
            DriftFinding(
                check_id="W2C-17",
                detail=(
                    "payload field 'active_trigger_ids' drift for zero-open baseline: "
                    f"observed={payload.get('active_trigger_ids')!r}"
                ),
            )
        )

    if payload.get("activation_required") is not False:
        findings.append(
            DriftFinding(
                check_id="W2C-17",
                detail=(
                    "payload field 'activation_required' must be false for zero-open baseline; "
                    f"observed={payload.get('activation_required')!r}"
                ),
            )
        )

    if payload.get("gate_open") is not False:
        findings.append(
            DriftFinding(
                check_id="W2C-17",
                detail=(
                    "payload field 'gate_open' must be false for zero-open baseline; "
                    f"observed={payload.get('gate_open')!r}"
                ),
            )
        )

    if payload.get("queue_state") != "idle":
        findings.append(
            DriftFinding(
                check_id="W2C-17",
                detail=(
                    "payload field 'queue_state' drift for zero-open baseline: "
                    f"expected='idle' observed={payload.get('queue_state')!r}"
                ),
            )
        )

    if payload.get("exit_code") != 0:
        findings.append(
            DriftFinding(
                check_id="W2C-17",
                detail=(
                    "payload field 'exit_code' drift for zero-open baseline: "
                    f"expected=0 observed={payload.get('exit_code')!r}"
                ),
            )
        )

    open_blockers = payload.get("open_blockers")
    if not isinstance(open_blockers, dict):
        findings.append(
            DriftFinding(
                check_id="W2C-17",
                detail="payload field 'open_blockers' must be an object for zero-open baseline",
            )
        )
    else:
        if open_blockers.get("count") != 0:
            findings.append(
                DriftFinding(
                    check_id="W2C-17",
                    detail=(
                        "open_blockers.count drift for zero-open baseline: "
                        f"expected=0 observed={open_blockers.get('count')!r}"
                    ),
                )
            )
        if open_blockers.get("trigger_id") != "T5-OPEN-BLOCKERS":
            findings.append(
                DriftFinding(
                    check_id="W2C-17",
                    detail=(
                        "open_blockers.trigger_id drift: "
                        f"expected='T5-OPEN-BLOCKERS' observed={open_blockers.get('trigger_id')!r}"
                    ),
                )
            )
        if open_blockers.get("trigger_fired") is not False:
            findings.append(
                DriftFinding(
                    check_id="W2C-17",
                    detail=(
                        "open_blockers.trigger_fired must be false for zero-open baseline; "
                        f"observed={open_blockers.get('trigger_fired')!r}"
                    ),
                )
            )

    t4_overlay = payload.get("t4_governance_overlay")
    if not isinstance(t4_overlay, dict):
        findings.append(
            DriftFinding(
                check_id="W2C-17",
                detail="payload field 't4_governance_overlay' must be an object",
            )
        )
    else:
        if t4_overlay.get("new_scope_publish") is not False:
            findings.append(
                DriftFinding(
                    check_id="W2C-17",
                    detail=(
                        "t4_governance_overlay.new_scope_publish must be false for zero-open baseline; "
                        f"observed={t4_overlay.get('new_scope_publish')!r}"
                    ),
                )
            )
        if t4_overlay.get("source") != "default-false":
            findings.append(
                DriftFinding(
                    check_id="W2C-17",
                    detail=(
                        "t4_governance_overlay.source drift for zero-open baseline: "
                        f"expected='default-false' observed={t4_overlay.get('source')!r}"
                    ),
                )
            )

    triggers = payload.get("triggers")
    if not isinstance(triggers, list):
        findings.append(
            DriftFinding(
                check_id="W2C-18",
                detail="payload field 'triggers' must be an array",
            )
        )
    else:
        observed_trigger_ids: list[str] = []
        for index, entry in enumerate(triggers):
            if not isinstance(entry, dict):
                findings.append(
                    DriftFinding(
                        check_id="W2C-18",
                        detail=f"trigger row {index} must be an object",
                    )
                )
                continue

            trigger_id = entry.get("id")
            if isinstance(trigger_id, str):
                observed_trigger_ids.append(trigger_id)
                expected_condition = TRIGGER_CONDITIONS.get(trigger_id)
                if expected_condition is not None and entry.get("condition") != expected_condition:
                    findings.append(
                        DriftFinding(
                            check_id="W2C-18",
                            detail=(
                                f"trigger row {index} condition drift for {trigger_id!r}: "
                                f"expected={expected_condition!r} observed={entry.get('condition')!r}"
                            ),
                        )
                    )

            if entry.get("count") != 0:
                findings.append(
                    DriftFinding(
                        check_id="W2C-18",
                        detail=(
                            f"trigger row {index} count drift for zero-open baseline: "
                            f"expected=0 observed={entry.get('count')!r}"
                        ),
                    )
                )
            if entry.get("fired") is not False:
                findings.append(
                    DriftFinding(
                        check_id="W2C-18",
                        detail=(
                            f"trigger row {index} fired drift for zero-open baseline: "
                            f"expected=False observed={entry.get('fired')!r}"
                        ),
                    )
                )

        if observed_trigger_ids != list(TRIGGER_ORDER):
            findings.append(
                DriftFinding(
                    check_id="W2C-18",
                    detail=(
                        "trigger row id order drift for zero-open baseline: "
                        f"expected={list(TRIGGER_ORDER)!r} observed={observed_trigger_ids!r}"
                    ),
                )
            )

    freshness = payload.get("freshness")
    if not isinstance(freshness, dict):
        findings.append(
            DriftFinding(
                check_id="W2C-19",
                detail="payload field 'freshness' must be an object",
            )
        )
    else:
        for label in EXPECTED_FRESHNESS_KEY_ORDER:
            entry = freshness.get(label)
            if not isinstance(entry, dict):
                findings.append(
                    DriftFinding(
                        check_id="W2C-19",
                        detail=f"freshness.{label} must be an object",
                    )
                )
                continue

            expected_values = {
                "requested": False,
                "max_age_seconds": None,
                "generated_at_utc": None,
                "age_seconds": None,
                "fresh": None,
            }
            for key, expected_value in expected_values.items():
                observed_value = entry.get(key)
                if observed_value != expected_value:
                    findings.append(
                        DriftFinding(
                            check_id="W2C-19",
                            detail=(
                                f"freshness.{label}.{key} drift: "
                                f"expected={expected_value!r} observed={observed_value!r}"
                            ),
                        )
                    )

    inputs = payload.get("inputs")
    if not isinstance(inputs, dict):
        findings.append(
            DriftFinding(
                check_id="W2C-20",
                detail="payload field 'inputs' must be an object",
            )
        )
    else:
        scenario_root = fixture_root / "zero_open"
        expected_inputs = {
            "issues_json": display_path(scenario_root / "issues.json"),
            "milestones_json": display_path(scenario_root / "milestones.json"),
            "catalog_json": display_path(scenario_root / "catalog.json"),
            "open_blockers_json": None,
            "t4_governance_overlay_json": None,
        }
        for key, expected_value in expected_inputs.items():
            observed_value = inputs.get(key)
            if observed_value != expected_value:
                findings.append(
                    DriftFinding(
                        check_id="W2C-20",
                        detail=(
                            f"inputs.{key} drift for zero-open baseline: "
                            f"expected={expected_value!r} observed={observed_value!r}"
                        ),
                    )
                )

    return findings


def collect_findings(
    *,
    checker_script: Path,
    fixture_root: Path,
    happy_scenarios: Sequence[HappyScenario],
    fail_closed_scenarios: Sequence[FailClosedScenario],
) -> list[DriftFinding]:
    findings: list[DriftFinding] = []

    for scenario in happy_scenarios:
        expected_output_path = fixture_root / scenario.scenario / scenario.expected_output_name
        expected_output = load_text(expected_output_path, label=f"{scenario.check_id} expected output")
        first_run = run_checker(
            checker_script=checker_script,
            fixture_root=fixture_root,
            scenario=scenario.scenario,
            output_format=scenario.output_format,
        )
        second_run = run_checker(
            checker_script=checker_script,
            fixture_root=fixture_root,
            scenario=scenario.scenario,
            output_format=scenario.output_format,
        )

        if first_run.exit_code != scenario.expected_exit_code:
            findings.append(
                DriftFinding(
                    check_id=scenario.check_id,
                    detail=(
                        f"{scenario.scenario}/{scenario.output_format} exit drift: "
                        f"expected={scenario.expected_exit_code} observed={first_run.exit_code}"
                    ),
                )
            )
        if first_run.stderr != "":
            findings.append(
                DriftFinding(
                    check_id=scenario.check_id,
                    detail=(
                        f"{scenario.scenario}/{scenario.output_format} emitted stderr: "
                        f"{first_run.stderr.strip()!r}"
                    ),
                )
            )
        if first_run.stdout != expected_output:
            findings.append(
                DriftFinding(
                    check_id=scenario.check_id,
                    detail=(
                        f"{scenario.scenario}/{scenario.output_format} output drift against "
                        f"{display_path(expected_output_path)}"
                    ),
                )
            )
        if (
            first_run.exit_code != second_run.exit_code
            or first_run.stdout != second_run.stdout
            or first_run.stderr != second_run.stderr
        ):
            findings.append(
                DriftFinding(
                    check_id=scenario.check_id,
                    detail=(
                        f"{scenario.scenario}/{scenario.output_format} replay drift detected "
                        "(exit/stdout/stderr must be stable across reruns)"
                    ),
                )
            )

        if scenario.scenario == "zero_open" and scenario.output_format == "json":
            try:
                payload = json.loads(first_run.stdout)
            except json.JSONDecodeError as exc:
                findings.append(
                    DriftFinding(
                        check_id="W2C-05",
                        detail=f"zero_open/json output must be valid JSON object: {exc}",
                    )
                )
            else:
                if not isinstance(payload, dict):
                    findings.append(
                        DriftFinding(
                            check_id="W2C-05",
                            detail="zero_open/json output root must be an object",
                        )
                    )
                else:
                    findings.extend(validate_zero_open_json_schema(payload))
                    findings.extend(
                        validate_zero_open_json_semantics(
                            payload,
                            fixture_root=fixture_root,
                        )
                    )

    for scenario in fail_closed_scenarios:
        check_id = scenario.check_id
        scenario_name = scenario.scenario
        scenario_root = fixture_root / scenario_name
        expected_exit_code = load_expected_exit_code(scenario_root / "expected_exit_code.txt")
        expected_stderr = load_text(scenario_root / "expected_stderr.txt", label=f"{check_id} expected stderr")
        run = run_checker(
            checker_script=checker_script,
            fixture_root=fixture_root,
            scenario=scenario_name,
            output_format="json",
            include_open_blockers=(scenario_root / "open_blockers.json").exists(),
            include_t4_overlay=(scenario_root / "t4_overlay.json").exists(),
        )
        if run.exit_code != expected_exit_code:
            findings.append(
                DriftFinding(
                    check_id=check_id,
                    detail=(
                        f"{scenario_name} exit drift: expected={expected_exit_code} "
                        f"observed={run.exit_code}"
                    ),
                )
            )
        if run.stdout != "":
            findings.append(
                DriftFinding(
                    check_id=check_id,
                    detail=f"{scenario_name} must not emit stdout on fail-closed path",
                )
            )
        if run.stderr != expected_stderr:
            findings.append(
                DriftFinding(
                    check_id=check_id,
                    detail=(
                        f"{scenario_name} stderr drift against "
                        f"{display_path(scenario_root / 'expected_stderr.txt')}"
                    ),
                )
            )

    return findings


def render_drift_report(*, findings: list[DriftFinding], checker_script: Path, fixture_root: Path) -> str:
    rerun_command = render_command(
        [
            "python",
            "scripts/check_activation_execution_hardening_w2_contract.py",
            "--checker-script",
            display_path(checker_script),
            "--fixture-root",
            display_path(fixture_root),
        ]
    )
    lines = [
        "activation-execution-hardening-w2-contract: contract drift detected "
        f"({len(findings)} failed check(s)).",
        "drift findings:",
    ]
    for finding in findings:
        lines.append(f"- {finding.check_id}")
        lines.append(f"  {finding.detail}")
    lines.extend(
        [
            "remediation:",
            "1. Restore deterministic checker schema/output ordering and fixture parity for the failed checks.",
            "2. Re-run validator:",
            rerun_command,
        ]
    )
    return "\n".join(lines)


def check_contract(*, checker_script: Path, fixture_root: Path) -> int:
    if not checker_script.exists():
        raise ValueError(f"checker script does not exist: {display_path(checker_script)}")
    if not checker_script.is_file():
        raise ValueError(f"checker script is not a file: {display_path(checker_script)}")
    if not fixture_root.exists():
        raise ValueError(f"fixture root does not exist: {display_path(fixture_root)}")
    if not fixture_root.is_dir():
        raise ValueError(f"fixture root is not a directory: {display_path(fixture_root)}")

    happy_scenarios = discover_happy_scenarios(fixture_root=fixture_root)
    fail_closed_scenarios = discover_fail_closed_scenarios(fixture_root=fixture_root)
    findings = collect_findings(
        checker_script=checker_script,
        fixture_root=fixture_root,
        happy_scenarios=happy_scenarios,
        fail_closed_scenarios=fail_closed_scenarios,
    )
    if findings:
        print(
            render_drift_report(
                findings=findings,
                checker_script=checker_script,
                fixture_root=fixture_root,
            ),
            file=sys.stderr,
        )
        return 1

    checks_passed = len(happy_scenarios) + len(SCHEMA_CHECK_IDS) + len(fail_closed_scenarios)
    print("activation-execution-hardening-w2-contract: OK")
    print(f"- mode={MODE}")
    print(f"- checker_script={display_path(checker_script)}")
    print(f"- fixture_root={display_path(fixture_root)}")
    print(f"- checks_passed={checks_passed}")
    print("- fail_closed=true")
    return 0


def build_parser() -> argparse.ArgumentParser:
    parser = argparse.ArgumentParser(
        prog="check_activation_execution_hardening_w2_contract.py",
        description=(
            "Fail-closed validator for v0.15 M28 lane-B activation-trigger schema/order "
            "and fixture parity contracts for activation execution hardening W2."
        ),
    )
    parser.add_argument(
        "--checker-script",
        type=Path,
        default=DEFAULT_CHECKER_SCRIPT_PATH,
        help="Path to scripts/check_activation_triggers.py.",
    )
    parser.add_argument(
        "--fixture-root",
        type=Path,
        default=DEFAULT_FIXTURE_ROOT,
        help="Path to tests/tooling/fixtures/activation_triggers.",
    )
    return parser


def main(argv: Sequence[str] | None = None) -> int:
    args = build_parser().parse_args(argv)
    checker_script = resolve_input_path(args.checker_script)
    fixture_root = resolve_input_path(args.fixture_root)

    try:
        return check_contract(checker_script=checker_script, fixture_root=fixture_root)
    except ValueError as exc:
        print(f"activation-execution-hardening-w2-contract: error: {exc}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    raise SystemExit(main())

