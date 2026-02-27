import importlib.util
import io
import json
import re
import sys
from contextlib import redirect_stderr, redirect_stdout
from datetime import datetime, timezone
from pathlib import Path

import pytest

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "check_activation_triggers.py"
SPEC = importlib.util.spec_from_file_location("check_activation_triggers", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/check_activation_triggers.py for tests.")
check_activation_triggers = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = check_activation_triggers
SPEC.loader.exec_module(check_activation_triggers)

FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "activation_triggers"
T4_TRIGGER_ID = "T4-NEW-SCOPE-PUBLISH"
OPEN_BLOCKERS_TRIGGER_ID = "T5-OPEN-BLOCKERS"
TOOLING_TRIGGER_IDS = (
    "T1-ISSUES",
    "T2-MILESTONES",
    "T3-ACTIONABLE-ROWS",
    OPEN_BLOCKERS_TRIGGER_ID,
)
EXPECTED_TRIGGER_ORDER = list(TOOLING_TRIGGER_IDS)
EXPECTED_PAYLOAD_KEY_ORDER = [
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
]
EXPECTED_INPUT_KEY_ORDER = [
    "issues_json",
    "milestones_json",
    "catalog_json",
    "open_blockers_json",
    "t4_governance_overlay_json",
]
EXPECTED_FRESHNESS_KEY_ORDER = ["issues", "milestones"]
EXPECTED_FRESHNESS_ENTRY_KEY_ORDER = [
    "requested",
    "max_age_seconds",
    "generated_at_utc",
    "age_seconds",
    "fresh",
]
EXPECTED_TRIGGER_ENTRY_KEY_ORDER = ["id", "condition", "count", "fired"]
EXPECTED_OPEN_BLOCKERS_KEY_ORDER = ["count", "trigger_id", "trigger_fired"]
EXPECTED_T4_OVERLAY_KEY_ORDER = ["new_scope_publish", "source"]
INLINE_T4_OPTION_CANDIDATES = (
    "--t4-new-scope-publish",
    "--new-scope-publish",
    "--t4",
)
JSON_T4_OPTION_CANDIDATES = (
    "--t4-governance-overlay-json",
    "--t4-overlay-json",
    "--governance-overlay-json",
    "--activation-overlay-json",
    "--overlay-json",
    "--t4-json",
)
GATE_OPEN_FIELD_CANDIDATES = ("gate_open", "activation_gate_open", "queue_gate_open")
T4_FIELD_CANDIDATES = (
    "t4_new_scope_publish",
    "new_scope_publish",
    "t4_overlay",
    "governance_overlay_t4",
)


def run_main(args: list[str]) -> tuple[int, str, str]:
    stdout = io.StringIO()
    stderr = io.StringIO()
    with redirect_stdout(stdout), redirect_stderr(stderr):
        code = check_activation_triggers.main(args)
    return code, stdout.getvalue(), stderr.getvalue()


def write_json(path: Path, payload: object) -> None:
    path.write_text(json.dumps(payload), encoding="utf-8")


def parser_option_actions() -> dict[str, object]:
    parser = check_activation_triggers.build_parser()
    return parser._option_string_actions


def supports_t4_contract() -> bool:
    option_actions = parser_option_actions()
    if any(option in option_actions for option in INLINE_T4_OPTION_CANDIDATES):
        return True
    return any(option in option_actions for option in JSON_T4_OPTION_CANDIDATES)


def bool_text(value: bool) -> str:
    return "true" if value else "false"


def trigger_map(payload: dict[str, object]) -> dict[str, dict[str, object]]:
    entries = payload["triggers"]
    assert isinstance(entries, list)
    rows: dict[str, dict[str, object]] = {}
    for entry in entries:
        assert isinstance(entry, dict)
        trigger_id = entry.get("id")
        assert isinstance(trigger_id, str)
        rows[trigger_id] = entry
    return rows


def read_bool_field(
    payload: dict[str, object], *, candidates: tuple[str, ...], pattern: str
) -> bool:
    for key in candidates:
        if key not in payload:
            continue
        value = payload[key]
        assert isinstance(value, bool), f"expected boolean payload field '{key}'"
        return value

    regex = re.compile(pattern)
    for key, value in payload.items():
        if not isinstance(key, str):
            continue
        if regex.search(key):
            assert isinstance(value, bool), f"expected boolean payload field '{key}'"
            return value

    raise AssertionError(
        f"expected payload field matching candidates={candidates} or pattern={pattern!r}"
    )


def build_t4_overlay_args(scenario_root: Path, t4_new_scope_publish: bool) -> list[str]:
    option_actions = parser_option_actions()
    for option in INLINE_T4_OPTION_CANDIDATES:
        if option not in option_actions:
            continue
        action = option_actions[option]
        nargs = getattr(action, "nargs", None)
        if nargs == 0:
            return [option] if t4_new_scope_publish else []
        return [option, bool_text(t4_new_scope_publish)]

    for option in JSON_T4_OPTION_CANDIDATES:
        if option not in option_actions:
            continue
        overlay_path = scenario_root / (
            "overlay_true.json" if t4_new_scope_publish else "overlay_false.json"
        )
        return [option, str(overlay_path)]

    raise AssertionError(
        "Lane A T4 overlay contract not detected; expected inline or JSON overlay option."
    )


def run_scenario(
    name: str,
    output_format: str,
    *,
    t4_new_scope_publish: bool | None = None,
    open_blockers_fixture: str | None = None,
) -> tuple[int, str, str]:
    scenario_root = FIXTURE_ROOT / name
    args = [
        "--issues-json",
        str(scenario_root / "issues.json"),
        "--milestones-json",
        str(scenario_root / "milestones.json"),
        "--catalog-json",
        str(scenario_root / "catalog.json"),
    ]
    if open_blockers_fixture is not None:
        args.extend(
            [
                "--open-blockers-json",
                str(scenario_root / open_blockers_fixture),
            ]
        )
    if t4_new_scope_publish is not None:
        args.extend(build_t4_overlay_args(scenario_root, t4_new_scope_publish))
    args.extend(["--format", output_format])
    return run_main(args)


def run_failure_fixture_scenario(name: str, output_format: str = "json") -> tuple[int, str, str]:
    scenario_root = FIXTURE_ROOT / name
    args = [
        "--issues-json",
        str(scenario_root / "issues.json"),
        "--milestones-json",
        str(scenario_root / "milestones.json"),
        "--catalog-json",
        str(scenario_root / "catalog.json"),
    ]
    open_blockers_path = scenario_root / "open_blockers.json"
    if open_blockers_path.exists():
        args.extend(["--open-blockers-json", str(open_blockers_path)])
    t4_overlay_path = scenario_root / "t4_overlay.json"
    if t4_overlay_path.exists():
        args.extend(["--t4-governance-overlay-json", str(t4_overlay_path)])
    args.extend(["--format", output_format])
    return run_main(args)


def assert_tooling_trigger_state(
    payload: dict[str, object],
    *,
    expected_counts: dict[str, int],
    expected_fired: dict[str, bool],
) -> None:
    rows = trigger_map(payload)
    for trigger_id in TOOLING_TRIGGER_IDS:
        assert trigger_id in rows

    normalized_counts = {trigger_id: 0 for trigger_id in TOOLING_TRIGGER_IDS}
    normalized_counts.update(expected_counts)
    normalized_fired = {trigger_id: False for trigger_id in TOOLING_TRIGGER_IDS}
    normalized_fired.update(expected_fired)

    for trigger_id in TOOLING_TRIGGER_IDS:
        assert rows[trigger_id]["count"] == normalized_counts[trigger_id]
        assert rows[trigger_id]["fired"] is normalized_fired[trigger_id]


def extract_t4_new_scope_publish(payload: dict[str, object]) -> tuple[bool, str | None]:
    try:
        return (
            read_bool_field(
                payload,
                candidates=T4_FIELD_CANDIDATES,
                pattern=r"(?:^t4|scope.*publish|publish.*scope|overlay.*t4)",
            ),
            None,
        )
    except AssertionError:
        pass

    overlay_payload = payload.get("t4_governance_overlay")
    if isinstance(overlay_payload, dict):
        raw_value = overlay_payload.get("new_scope_publish")
        if isinstance(raw_value, bool):
            source = overlay_payload.get("source")
            assert source is None or isinstance(source, str)
            return raw_value, source

    rows = trigger_map(payload)
    if T4_TRIGGER_ID in rows:
        return bool(rows[T4_TRIGGER_ID]["fired"]), None

    raise AssertionError("missing T4 state in payload (field, nested overlay, or trigger row)")


def assert_freshness_entry(
    entry: object,
    *,
    requested: bool,
    max_age_seconds: int | None,
    generated_at_utc: str | None,
    age_seconds: int | None,
    fresh: bool | None,
) -> None:
    assert isinstance(entry, dict)
    assert entry["requested"] is requested
    assert entry["max_age_seconds"] == max_age_seconds
    assert entry["generated_at_utc"] == generated_at_utc
    assert entry["age_seconds"] == age_seconds
    if fresh is None:
        assert entry["fresh"] is None
    else:
        assert entry["fresh"] is fresh


def assert_t4_gate_reduction_json(
    payload: dict[str, object], *, expectations: dict[str, object]
) -> None:
    rows = trigger_map(payload)

    expected_activation_required = expectations["activation_required"]
    expected_t4 = expectations["t4_new_scope_publish"]
    expected_gate_open = expectations["gate_open"]
    expected_queue_state = expectations["queue_state"]
    expected_exit_code = expectations["exit_code"]
    expected_t4_source = expectations.get("t4_source")
    expected_counts = expectations["tooling_trigger_counts"]
    expected_fired = expectations["tooling_trigger_fired"]

    assert isinstance(expected_activation_required, bool)
    assert isinstance(expected_t4, bool)
    assert isinstance(expected_gate_open, bool)
    assert isinstance(expected_queue_state, str)
    assert isinstance(expected_exit_code, int)
    assert isinstance(expected_counts, dict)
    assert isinstance(expected_fired, dict)
    assert expected_t4_source is None or isinstance(expected_t4_source, str)

    assert_tooling_trigger_state(
        payload,
        expected_counts={key: int(value) for key, value in expected_counts.items()},
        expected_fired={key: bool(value) for key, value in expected_fired.items()},
    )

    assert payload["activation_required"] is expected_activation_required
    assert payload["queue_state"] == expected_queue_state
    assert payload["exit_code"] == expected_exit_code

    tooling_activation = any(
        bool(rows[trigger_id]["fired"]) for trigger_id in TOOLING_TRIGGER_IDS
    )
    assert payload["activation_required"] is tooling_activation

    t4_from_payload, t4_source = extract_t4_new_scope_publish(payload)
    assert t4_from_payload is expected_t4
    if expected_t4_source is not None:
        assert t4_source == expected_t4_source

    if T4_TRIGGER_ID in rows:
        assert bool(rows[T4_TRIGGER_ID]["fired"]) is expected_t4
        t4_count = rows[T4_TRIGGER_ID]["count"]
        assert isinstance(t4_count, int)
        assert t4_count >= 0
        if expected_t4:
            assert t4_count > 0
        else:
            assert t4_count == 0

    gate_open = read_bool_field(
        payload,
        candidates=GATE_OPEN_FIELD_CANDIDATES,
        pattern=r"(?:gate.*open|open.*gate)",
    )
    assert gate_open is expected_gate_open
    assert gate_open is (tooling_activation or t4_from_payload)

    expected_queue_from_gate = "dispatch-open" if gate_open else "idle"
    expected_exit_from_gate = 1 if gate_open else 0
    assert payload["queue_state"] == expected_queue_from_gate
    assert payload["exit_code"] == expected_exit_from_gate


def assert_t4_gate_reduction_markdown(
    markdown: str, *, expectations: dict[str, object]
) -> None:
    expected_activation_required = bool(expectations["activation_required"])
    expected_t4 = bool(expectations["t4_new_scope_publish"])
    expected_gate_open = bool(expectations["gate_open"])
    expected_queue_state = str(expectations["queue_state"])
    expected_t4_source = expectations.get("t4_source")
    expected_fired = expectations["tooling_trigger_fired"]
    assert isinstance(expected_fired, dict)
    assert expected_t4_source is None or isinstance(expected_t4_source, str)

    normalized_expected_fired = {trigger_id: False for trigger_id in TOOLING_TRIGGER_IDS}
    normalized_expected_fired.update(
        {str(key): bool(value) for key, value in expected_fired.items()}
    )

    assert f"- Activation required: `{bool_text(expected_activation_required)}`" in markdown
    assert f"- T4 new scope publish: `{bool_text(expected_t4)}`" in markdown
    if expected_t4_source is not None:
        assert f"- T4 source: `{expected_t4_source}`" in markdown
    assert f"- Queue state: `{expected_queue_state}`" in markdown

    for trigger_id, expected_flag in normalized_expected_fired.items():
        row_pattern = re.compile(
            rf"(?m)^\|\s*`{re.escape(trigger_id)}`\s*\|\s*`{bool_text(bool(expected_flag))}`\s*\|"
        )
        assert row_pattern.search(markdown)

    gate_open_line = re.compile(
        rf"(?im)^- .*gate.*open.*`{bool_text(expected_gate_open)}`"
    )
    assert gate_open_line.search(markdown)


def load_expectations(name: str) -> dict[str, object]:
    raw = (FIXTURE_ROOT / name / "expectations.json").read_text(encoding="utf-8")
    payload = json.loads(raw)
    assert isinstance(payload, dict)
    return payload


def test_zero_open_baseline_json_matches_expected_fixture() -> None:
    code, stdout, stderr = run_scenario("zero_open", "json")

    assert code == 0
    assert stderr == ""
    expected = (FIXTURE_ROOT / "zero_open" / "expected.json").read_text(
        encoding="utf-8"
    )
    assert stdout == expected

    payload = json.loads(stdout)
    assert isinstance(payload, dict)
    assert payload["contract_id"] == check_activation_triggers.ACTIVATION_SEED_CONTRACT_ID
    assert payload["fail_closed"] is True
    assert payload["trigger_order"] == EXPECTED_TRIGGER_ORDER
    assert payload["activation_required"] is False
    assert payload["queue_state"] == "idle"
    assert payload["active_trigger_ids"] == []
    open_blockers = payload["open_blockers"]
    assert isinstance(open_blockers, dict)
    assert open_blockers["count"] == 0
    assert open_blockers["trigger_id"] == OPEN_BLOCKERS_TRIGGER_ID
    assert open_blockers["trigger_fired"] is False
    freshness = payload["freshness"]
    assert isinstance(freshness, dict)
    assert_freshness_entry(
        freshness["issues"],
        requested=False,
        max_age_seconds=None,
        generated_at_utc=None,
        age_seconds=None,
        fresh=None,
    )
    assert_freshness_entry(
        freshness["milestones"],
        requested=False,
        max_age_seconds=None,
        generated_at_utc=None,
        age_seconds=None,
        fresh=None,
    )
    assert_tooling_trigger_state(
        payload,
        expected_counts={
            "T1-ISSUES": 0,
            "T2-MILESTONES": 0,
            "T3-ACTIONABLE-ROWS": 0,
            OPEN_BLOCKERS_TRIGGER_ID: 0,
        },
        expected_fired={
            "T1-ISSUES": False,
            "T2-MILESTONES": False,
            "T3-ACTIONABLE-ROWS": False,
            OPEN_BLOCKERS_TRIGGER_ID: False,
        },
    )


def test_zero_open_json_output_shape_is_deterministic() -> None:
    code, stdout, stderr = run_scenario("zero_open", "json")

    assert code == 0
    assert stderr == ""
    payload = json.loads(stdout)
    assert isinstance(payload, dict)
    assert list(payload.keys()) == EXPECTED_PAYLOAD_KEY_ORDER
    inputs = payload["inputs"]
    assert isinstance(inputs, dict)
    assert list(inputs.keys()) == EXPECTED_INPUT_KEY_ORDER
    freshness = payload["freshness"]
    assert isinstance(freshness, dict)
    assert list(freshness.keys()) == EXPECTED_FRESHNESS_KEY_ORDER
    for label in EXPECTED_FRESHNESS_KEY_ORDER:
        entry = freshness[label]
        assert isinstance(entry, dict)
        assert list(entry.keys()) == EXPECTED_FRESHNESS_ENTRY_KEY_ORDER
    triggers = payload["triggers"]
    assert isinstance(triggers, list)
    for row in triggers:
        assert isinstance(row, dict)
        assert list(row.keys()) == EXPECTED_TRIGGER_ENTRY_KEY_ORDER
    open_blockers = payload["open_blockers"]
    assert isinstance(open_blockers, dict)
    assert list(open_blockers.keys()) == EXPECTED_OPEN_BLOCKERS_KEY_ORDER
    t4_overlay = payload["t4_governance_overlay"]
    assert isinstance(t4_overlay, dict)
    assert list(t4_overlay.keys()) == EXPECTED_T4_OVERLAY_KEY_ORDER
    rows = trigger_map(payload)
    assert list(rows.keys()) == [
        "T1-ISSUES",
        "T2-MILESTONES",
        "T3-ACTIONABLE-ROWS",
        OPEN_BLOCKERS_TRIGGER_ID,
    ]
    assert payload["trigger_order"] == EXPECTED_TRIGGER_ORDER


def test_zero_open_replay_is_deterministic_for_json_and_markdown() -> None:
    first_json = run_scenario("zero_open", "json")
    second_json = run_scenario("zero_open", "json")
    assert first_json == second_json

    first_markdown = run_scenario("zero_open", "markdown")
    second_markdown = run_scenario("zero_open", "markdown")
    assert first_markdown == second_markdown


def test_zero_open_baseline_markdown_matches_expected_fixture() -> None:
    code, stdout, stderr = run_scenario("zero_open", "markdown")

    assert code == 0
    assert stderr == ""
    expected = (FIXTURE_ROOT / "zero_open" / "expected.md").read_text(
        encoding="utf-8"
    )
    assert stdout == expected
    assert (
        f"- Contract ID: `{check_activation_triggers.ACTIVATION_SEED_CONTRACT_ID}`"
        in stdout
    )
    assert "- Fail closed: `true`" in stdout
    assert (
        "- Trigger order: `T1-ISSUES`, `T2-MILESTONES`, `T3-ACTIONABLE-ROWS`, "
        "`T5-OPEN-BLOCKERS`"
        in stdout
    )
    assert "- Activation required: `false`" in stdout
    assert "- Queue state: `idle`" in stdout
    assert "- Open blockers count: `0`" in stdout
    assert "- Open blockers trigger fired: `false`" in stdout
    assert "## Snapshot Freshness" in stdout
    assert "| Issues | `false` | _none_ | _none_ | _none_ | _none_ |" in stdout
    assert "| Milestones | `false` | _none_ | _none_ | _none_ | _none_ |" in stdout
    assert "| `T5-OPEN-BLOCKERS` | `false` | 0 | open blockers > 0 |" in stdout
    assert "- Active triggers: _none_" in stdout


def test_activation_triggered_json_matches_expected_fixture() -> None:
    code, stdout, stderr = run_scenario("activation_triggered", "json")

    assert code == 1
    assert stderr == ""
    expected = (FIXTURE_ROOT / "activation_triggered" / "expected.json").read_text(
        encoding="utf-8"
    )
    assert stdout == expected

    payload = json.loads(stdout)
    assert isinstance(payload, dict)
    assert payload["contract_id"] == check_activation_triggers.ACTIVATION_SEED_CONTRACT_ID
    assert payload["fail_closed"] is True
    assert payload["trigger_order"] == EXPECTED_TRIGGER_ORDER
    assert payload["activation_required"] is True
    assert payload["queue_state"] == "dispatch-open"
    assert payload["active_trigger_ids"] == [
        "T1-ISSUES",
        "T2-MILESTONES",
        "T3-ACTIONABLE-ROWS",
    ]
    open_blockers = payload["open_blockers"]
    assert isinstance(open_blockers, dict)
    assert open_blockers["count"] == 0
    assert open_blockers["trigger_id"] == OPEN_BLOCKERS_TRIGGER_ID
    assert open_blockers["trigger_fired"] is False
    assert_tooling_trigger_state(
        payload,
        expected_counts={
            "T1-ISSUES": 2,
            "T2-MILESTONES": 1,
            "T3-ACTIONABLE-ROWS": 2,
            OPEN_BLOCKERS_TRIGGER_ID: 0,
        },
        expected_fired={
            "T1-ISSUES": True,
            "T2-MILESTONES": True,
            "T3-ACTIONABLE-ROWS": True,
            OPEN_BLOCKERS_TRIGGER_ID: False,
        },
    )


def test_activation_triggered_markdown_matches_expected_fixture() -> None:
    code, stdout, stderr = run_scenario("activation_triggered", "markdown")

    assert code == 1
    assert stderr == ""
    expected = (FIXTURE_ROOT / "activation_triggered" / "expected.md").read_text(
        encoding="utf-8"
    )
    assert stdout == expected
    assert (
        f"- Contract ID: `{check_activation_triggers.ACTIVATION_SEED_CONTRACT_ID}`"
        in stdout
    )
    assert "- Fail closed: `true`" in stdout
    assert "- Activation required: `true`" in stdout
    assert "- Queue state: `dispatch-open`" in stdout
    assert "- Open blockers count: `0`" in stdout
    assert "- Open blockers trigger fired: `false`" in stdout
    assert (
        "- Active triggers: `T1-ISSUES`, `T2-MILESTONES`, `T3-ACTIONABLE-ROWS`"
        in stdout
    )
    assert "| `T5-OPEN-BLOCKERS` | `false` | 0 | open blockers > 0 |" in stdout


@pytest.mark.parametrize(
    "scenario_name",
    (
        "catalog_metadata_happy",
        "catalog_execution_metadata_happy",
        "dispatch_evidence_w2_happy",
        "dispatch_evidence_w3_happy",
        "dispatch_evidence_w4_happy",
        "dispatch_evidence_w5_happy",
        "release_scale_foundation_w1_happy",
        "release_scale_foundation_w2_happy",
        "release_scale_foundation_w3_happy",
    ),
)
@pytest.mark.parametrize(
    ("output_format", "expected_file"),
    (
        ("json", "expected.json"),
        ("markdown", "expected.md"),
    ),
)
def test_catalog_contract_happy_fixture_matches_expected(
    scenario_name: str,
    output_format: str,
    expected_file: str,
) -> None:
    code, stdout, stderr = run_scenario(scenario_name, output_format)

    assert code == 1
    assert stderr == ""
    expected = (FIXTURE_ROOT / scenario_name / expected_file).read_text(
        encoding="utf-8"
    )
    assert stdout == expected

    if output_format == "json":
        payload = json.loads(stdout)
        assert isinstance(payload, dict)
        assert payload["active_trigger_ids"] == ["T3-ACTIONABLE-ROWS"]
        assert payload["activation_required"] is True
        assert payload["exit_code"] == 1
    else:
        assert "- Activation required: `true`" in stdout
        assert "- Queue state: `dispatch-open`" in stdout
        assert "| `T3-ACTIONABLE-ROWS` | `true` | 1 | actionable catalog rows > 0 |" in stdout


def test_open_blockers_trigger_forces_activation_open_json() -> None:
    code, stdout, stderr = run_scenario(
        "zero_open",
        "json",
        open_blockers_fixture="open_blockers_nonzero.json",
    )

    assert code == 1
    assert stderr == ""

    payload = json.loads(stdout)
    assert isinstance(payload, dict)
    assert payload["activation_required"] is True
    assert payload["gate_open"] is True
    assert payload["queue_state"] == "dispatch-open"
    assert payload["active_trigger_ids"] == [OPEN_BLOCKERS_TRIGGER_ID]
    open_blockers = payload["open_blockers"]
    assert isinstance(open_blockers, dict)
    assert open_blockers["count"] == 2
    assert open_blockers["trigger_id"] == OPEN_BLOCKERS_TRIGGER_ID
    assert open_blockers["trigger_fired"] is True
    assert_tooling_trigger_state(
        payload,
        expected_counts={OPEN_BLOCKERS_TRIGGER_ID: 2},
        expected_fired={OPEN_BLOCKERS_TRIGGER_ID: True},
    )


def test_open_blockers_trigger_forces_activation_open_markdown() -> None:
    code, stdout, stderr = run_scenario(
        "zero_open",
        "markdown",
        open_blockers_fixture="open_blockers_nonzero.json",
    )

    assert code == 1
    assert stderr == ""
    assert "- Activation required: `true`" in stdout
    assert "- Gate open: `true`" in stdout
    assert "- Queue state: `dispatch-open`" in stdout
    assert "- Open blockers count: `2`" in stdout
    assert "- Open blockers trigger fired: `true`" in stdout
    assert "| `T5-OPEN-BLOCKERS` | `true` | 2 | open blockers > 0 |" in stdout
    assert "- Active triggers: `T5-OPEN-BLOCKERS`" in stdout


def test_open_blockers_schema_validation_rejects_malformed_payload(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    blockers_path = tmp_path / "blockers.json"
    write_json(issues_path, [])
    write_json(milestones_path, {"count": 0})
    write_json(catalog_path, {"tasks": []})
    write_json(blockers_path, {"open_blockers": []})

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--open-blockers-json",
            str(blockers_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "open blockers snapshot object must include non-negative integer" in stderr
    assert "open_blocker_count" in stderr


@pytest.mark.parametrize(
    "open_blockers_payload",
    (
        {
            "generated_at_utc": "2026-02-24T12:00:00Z",
            "open_blocker_count": 0,
            "open_blockers": [],
        },
        {
            "source": "fixtures/open_blockers",
            "open_blocker_count": 0,
            "open_blockers": [],
        },
    ),
)
def test_open_blockers_snapshot_requires_generated_at_source_pair(
    tmp_path: Path,
    open_blockers_payload: dict[str, object],
) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    blockers_path = tmp_path / "open_blockers.json"
    write_json(issues_path, [])
    write_json(milestones_path, {"count": 0})
    write_json(catalog_path, {"tasks": []})
    write_json(blockers_path, open_blockers_payload)

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--open-blockers-json",
            str(blockers_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "must include both 'generated_at_utc' and 'source' when either field is present" in stderr
    assert "open_blockers.json" in stderr


def test_open_blockers_source_must_be_canonical_string(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    blockers_path = tmp_path / "open_blockers.json"
    write_json(issues_path, [])
    write_json(milestones_path, {"count": 0})
    write_json(catalog_path, {"tasks": []})
    write_json(
        blockers_path,
        {
            "generated_at_utc": "2026-02-24T12:00:00Z",
            "source": " fixtures/open_blockers ",
            "open_blocker_count": 0,
            "open_blockers": [],
        },
    )

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--open-blockers-json",
            str(blockers_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "open blockers snapshot field 'source'" in stderr
    assert "must not contain leading/trailing whitespace" in stderr
    assert "open_blockers.json" in stderr


def test_open_blockers_line_alias_mismatch_exits_fail_closed(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    blockers_path = tmp_path / "open_blockers.json"
    write_json(issues_path, [])
    write_json(milestones_path, {"count": 0})
    write_json(catalog_path, {"tasks": []})
    write_json(
        blockers_path,
        {
            "open_blocker_count": 1,
            "open_blockers": [
                {
                    "blocker_id": "BLK-100-01",
                    "source_path": "spec/planning/example.md",
                    "line_number": 14,
                    "line": 12,
                }
            ],
        },
    )

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--open-blockers-json",
            str(blockers_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "line alias mismatch" in stderr
    assert "line_number=14 line=12" in stderr


def test_open_blockers_array_payload_from_extractor_rows_is_accepted(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    blockers_path = tmp_path / "open_blockers.json"
    write_json(issues_path, [])
    write_json(milestones_path, {"count": 0})
    write_json(catalog_path, {"tasks": []})
    write_json(
        blockers_path,
        [
            {
                "blocker_id": "BLK-900-01",
                "source_path": "spec/planning/example_alpha.md",
                "line": 5,
                "owner": "Lane D",
                "due_date_utc": "2026-03-01",
                "summary": "Alpha blocker.",
                "status": "OPEN",
            },
            {
                "blocker_id": "BLK-900-02",
                "source_path": "spec/planning/example_alpha.md",
                "line": 6,
                "owner": "Lane D",
                "due_date_utc": "2026-03-02",
                "summary": "Beta blocker.",
                "status": "OPEN",
            },
        ],
    )

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--open-blockers-json",
            str(blockers_path),
            "--format",
            "json",
        ]
    )

    assert code == 1
    assert stderr == ""
    payload = json.loads(stdout)
    assert isinstance(payload, dict)
    assert payload["active_trigger_ids"] == [OPEN_BLOCKERS_TRIGGER_ID]
    open_blockers = payload["open_blockers"]
    assert isinstance(open_blockers, dict)
    assert open_blockers["count"] == 2
    assert open_blockers["trigger_fired"] is True


def test_open_blockers_array_payload_unsorted_exits_fail_closed(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    blockers_path = tmp_path / "open_blockers.json"
    write_json(issues_path, [])
    write_json(milestones_path, {"count": 0})
    write_json(catalog_path, {"tasks": []})
    write_json(
        blockers_path,
        [
            {
                "blocker_id": "BLK-900-02",
                "source_path": "spec/planning/example_alpha.md",
                "line": 6,
            },
            {
                "blocker_id": "BLK-900-01",
                "source_path": "spec/planning/example_alpha.md",
                "line": 5,
            },
        ],
    )

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--open-blockers-json",
            str(blockers_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "open blockers snapshot array must be sorted by" in stderr
    assert "open_blockers.json" in stderr


def test_open_blockers_array_payload_duplicate_rows_exits_fail_closed(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    blockers_path = tmp_path / "open_blockers.json"
    write_json(issues_path, [])
    write_json(milestones_path, {"count": 0})
    write_json(catalog_path, {"tasks": []})
    write_json(
        blockers_path,
        [
            {
                "blocker_id": "BLK-900-01",
                "source_path": "spec/planning/example_alpha.md",
                "line": 5,
            },
            {
                "blocker_id": "BLK-900-01",
                "source_path": "spec/planning/example_alpha.md",
                "line": 5,
            },
        ],
    )

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--open-blockers-json",
            str(blockers_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "open blockers snapshot array contains duplicate blocker row" in stderr
    assert "open_blockers.json" in stderr


def test_t4_overlay_conflicting_aliases_exits_fail_closed(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    overlay_path = tmp_path / "overlay.json"
    write_json(issues_path, [])
    write_json(milestones_path, {"count": 0})
    write_json(catalog_path, {"tasks": []})
    write_json(
        overlay_path,
        {
            "t4_new_scope_publish": False,
            "T4_NEW_SCOPE_PUBLISH": True,
        },
    )

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--t4-governance-overlay-json",
            str(overlay_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "must match when both are provided" in stderr


def test_milestones_snapshot_row_unknown_field_exits_fail_closed(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    write_json(issues_path, [])
    write_json(
        milestones_path,
        {
            "count": 1,
            "items": [
                {
                    "number": 26,
                    "title": "v0.15 Activation Seed Hardening W6",
                    "unexpected": True,
                }
            ],
        },
    )
    write_json(catalog_path, {"tasks": []})

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "open milestones snapshot field 'items'[0] object" in stderr
    assert "unexpected field(s)" in stderr


def test_milestones_snapshot_rows_must_be_sorted_by_number(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    write_json(issues_path, [])
    write_json(
        milestones_path,
        [
            {"number": 41, "title": "Later milestone"},
            {"number": 5, "title": "Earlier milestone"},
        ],
    )
    write_json(catalog_path, {"tasks": []})

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "open milestones snapshot rows must be sorted by 'number'" in stderr


def test_milestones_snapshot_rows_reject_duplicate_number(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    write_json(issues_path, [])
    write_json(
        milestones_path,
        [
            {"number": 26, "title": "Milestone A"},
            {"number": 26, "title": "Milestone B"},
        ],
    )
    write_json(catalog_path, {"tasks": []})

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "duplicate milestone number 26" in stderr


def test_milestones_snapshot_row_rejects_whitespace_title(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    write_json(issues_path, [])
    write_json(
        milestones_path,
        [{"number": 26, "title": "  v0.15 Activation Seed Hardening W6  "}],
    )
    write_json(catalog_path, {"tasks": []})

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "open milestones snapshot entries[0].title" in stderr
    assert "must not contain leading/trailing whitespace" in stderr


def test_dispatch_evidence_ref_contract_uses_explicit_error_class() -> None:
    with pytest.raises(check_activation_triggers.DispatchEvidenceContractError):
        check_activation_triggers.parse_dispatch_evidence_reference(
            "spec/planning/evidence/lane_b/v015_dispatch_evidence_reliability_w4_tooling_validation_20260225.md",
            context="catalog task row 0 field 'execution_status_evidence_refs[0]'",
        )


@pytest.mark.parametrize(
    "scenario_name",
    (
        "drift_issues_root_key_order",
        "drift_issues_unknown_field",
        "drift_milestones_metadata_pair_mismatch",
        "drift_milestones_root_key_order",
        "drift_milestones_unknown_field",
        "drift_milestones_row_key_order",
        "drift_milestones_row_unknown_field",
        "drift_milestones_unsorted_numbers",
        "drift_milestones_duplicate_number",
        "hard_fail_milestones_title_whitespace",
        "drift_open_blockers_root_key_order",
        "drift_open_blockers_row_key_order",
        "drift_t4_overlay_unknown_field",
        "drift_open_blockers_unsorted",
        "drift_open_blockers_line_alias_mismatch",
        "hard_fail_open_blockers_invalid_generated_at_utc",
        "hard_fail_open_blockers_metadata_pair_mismatch",
        "hard_fail_open_blockers_source_whitespace",
        "hard_fail_open_blockers_source_path_windows_separator",
        "hard_fail_open_blockers_source_path_not_relative",
        "hard_fail_invalid_issues_json",
        "drift_t4_overlay_conflicting_alias",
        "drift_catalog_root_key_order",
        "drift_catalog_unknown_field",
        "drift_catalog_task_count_mismatch",
        "drift_catalog_unsorted_task_ids",
        "drift_catalog_status_alias_mismatch",
        "drift_catalog_lane_label_alias_mismatch",
        "drift_catalog_execution_metadata_pair_mismatch",
        "hard_fail_catalog_missing_status",
        "hard_fail_catalog_milestone_title_missing_validation_commands",
        "hard_fail_catalog_blocker_metadata_pair_mismatch",
        "hard_fail_catalog_path_line_pair_mismatch",
        "hard_fail_catalog_path_windows_separator",
        "hard_fail_catalog_path_not_relative",
        "hard_fail_catalog_dispatch_validation_commands_unsorted",
        "drift_catalog_dispatch_evidence_refs_unsorted",
        "drift_catalog_dispatch_evidence_refs_line_numeric_unsorted",
        "hard_fail_catalog_dispatch_evidence_ref_http_url",
        "hard_fail_catalog_dispatch_evidence_ref_non_md_path",
        "hard_fail_catalog_dispatch_override_source_mismatch",
        "hard_fail_catalog_dispatch_validation_command_control_character",
        "hard_fail_catalog_dispatch_validation_command_forbidden_token",
        "hard_fail_catalog_dispatch_evidence_ref_missing_line",
        "hard_fail_catalog_dispatch_evidence_ref_outside_lane_b",
        "drift_catalog_dispatch_w5_missing_required_command",
        "hard_fail_catalog_dispatch_w5_dependencies_mismatch",
        "hard_fail_catalog_dispatch_evidence_ref_missing_file",
        "hard_fail_catalog_dispatch_w5_override_source_not_w5_artifact",
        "drift_catalog_release_scale_w1_missing_required_command",
        "hard_fail_catalog_release_scale_w1_dependencies_mismatch",
        "hard_fail_catalog_release_scale_w1_override_source_not_w1_artifact",
        "drift_catalog_release_scale_w2_missing_required_command",
        "hard_fail_catalog_release_scale_w2_dependencies_mismatch",
        "hard_fail_catalog_release_scale_w2_override_source_not_w2_artifact",
        "drift_catalog_release_scale_w3_missing_required_command",
        "hard_fail_catalog_release_scale_w3_dependencies_mismatch",
        "hard_fail_catalog_release_scale_w3_override_source_not_w3_artifact",
        "hard_fail_open_blockers_duplicate_rows",
    ),
)
def test_fixture_backed_fail_closed_paths(scenario_name: str) -> None:
    scenario_root = FIXTURE_ROOT / scenario_name
    expected_exit_code = int(
        (scenario_root / "expected_exit_code.txt").read_text(encoding="utf-8").strip()
    )
    expected_stderr = (scenario_root / "expected_stderr.txt").read_text(
        encoding="utf-8"
    )

    code, stdout, stderr = run_failure_fixture_scenario(scenario_name)

    assert code == expected_exit_code
    assert stdout == ""
    assert stderr == expected_stderr


def test_freshness_requested_json_reports_recent_snapshot_state(
    monkeypatch: pytest.MonkeyPatch, tmp_path: Path
) -> None:
    fixed_now = datetime(2026, 2, 23, 12, 0, 0, tzinfo=timezone.utc)
    monkeypatch.setattr(check_activation_triggers, "utc_now", lambda: fixed_now)

    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    write_json(
        issues_path,
        {
            "generated_at_utc": "2026-02-23T11:59:45Z",
            "source": "fixture/issues",
            "count": 0,
        },
    )
    write_json(
        milestones_path,
        {
            "generated_at_utc": "2026-02-23T11:59:30Z",
            "source": "fixture/milestones",
            "count": 0,
        },
    )
    write_json(catalog_path, {"tasks": []})

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--issues-max-age-seconds",
            "30",
            "--milestones-max-age-seconds",
            "40",
            "--format",
            "json",
        ]
    )

    assert code == 0
    assert stderr == ""
    payload = json.loads(stdout)
    assert isinstance(payload, dict)
    freshness = payload["freshness"]
    assert isinstance(freshness, dict)
    assert_freshness_entry(
        freshness["issues"],
        requested=True,
        max_age_seconds=30,
        generated_at_utc="2026-02-23T11:59:45Z",
        age_seconds=15,
        fresh=True,
    )
    assert_freshness_entry(
        freshness["milestones"],
        requested=True,
        max_age_seconds=40,
        generated_at_utc="2026-02-23T11:59:30Z",
        age_seconds=30,
        fresh=True,
    )


def test_freshness_requested_missing_generated_at_utc_exits_with_error(
    monkeypatch: pytest.MonkeyPatch, tmp_path: Path
) -> None:
    fixed_now = datetime(2026, 2, 23, 12, 0, 0, tzinfo=timezone.utc)
    monkeypatch.setattr(check_activation_triggers, "utc_now", lambda: fixed_now)

    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    write_json(issues_path, {"count": 0})
    write_json(
        milestones_path,
        {
            "generated_at_utc": "2026-02-23T11:59:30Z",
            "source": "fixture/milestones",
            "count": 0,
        },
    )
    write_json(catalog_path, {"tasks": []})

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--issues-max-age-seconds",
            "60",
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "generated_at_utc" in stderr
    assert "--issues-max-age-seconds" in stderr
    assert "Add the field or omit" in stderr


def test_freshness_requested_stale_snapshot_exits_with_error(
    monkeypatch: pytest.MonkeyPatch, tmp_path: Path
) -> None:
    fixed_now = datetime(2026, 2, 23, 12, 0, 0, tzinfo=timezone.utc)
    monkeypatch.setattr(check_activation_triggers, "utc_now", lambda: fixed_now)

    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    write_json(
        issues_path,
        {
            "generated_at_utc": "2026-02-23T11:58:00Z",
            "source": "fixture/issues",
            "count": 0,
        },
    )
    write_json(
        milestones_path,
        {
            "generated_at_utc": "2026-02-23T11:59:50Z",
            "source": "fixture/milestones",
            "count": 0,
        },
    )
    write_json(catalog_path, {"tasks": []})

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--issues-max-age-seconds",
            "60",
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "freshness check failed" in stderr
    assert "age_seconds=120" in stderr
    assert "max_age_seconds=60" in stderr
    assert "--issues-max-age-seconds" in stderr


def test_freshness_requested_missing_source_exits_with_provenance_error(
    monkeypatch: pytest.MonkeyPatch, tmp_path: Path
) -> None:
    fixed_now = datetime(2026, 2, 23, 12, 0, 0, tzinfo=timezone.utc)
    monkeypatch.setattr(check_activation_triggers, "utc_now", lambda: fixed_now)

    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    write_json(
        issues_path,
        {"generated_at_utc": "2026-02-23T11:59:45Z", "count": 0},
    )
    write_json(
        milestones_path,
        {
            "generated_at_utc": "2026-02-23T11:59:30Z",
            "source": "fixture/milestones",
            "count": 0,
        },
    )
    write_json(catalog_path, {"tasks": []})

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--issues-max-age-seconds",
            "60",
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "must include both 'generated_at_utc' and 'source'" in stderr
    assert "issues.json" in stderr


def test_snapshot_count_items_parity_drift_exits_with_error(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    write_json(
        issues_path,
        {
            "generated_at_utc": "2026-02-23T11:59:45Z",
            "source": "fixture/issues",
            "count": 1,
            "items": [],
        },
    )
    write_json(
        milestones_path,
        {
            "generated_at_utc": "2026-02-23T11:59:45Z",
            "source": "fixture/milestones",
            "count": 0,
        },
    )
    write_json(catalog_path, {"tasks": []})

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "snapshot parity mismatch" in stderr
    assert "declared count 1 != discovered count 0" in stderr


def test_catalog_contract_fields_accept_canonical_milestone_row(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    write_json(issues_path, [])
    write_json(milestones_path, [])
    write_json(
        catalog_path,
        {
            "tasks": [
                {
                    "task_id": "SPT-4001",
                    "path": "spec/planning/v015_activation_execution_hardening_w1_dispatch_review_20260224.md",
                    "line": 24,
                    "lane": "B",
                    "lane_name": "Tooling Validation",
                    "milestone_title": "v0.15 Activation Execution Hardening W1",
                    "labels": [
                        "lane:B",
                        "type:tooling",
                    ],
                    "dependencies": [
                        "#979",
                        "#980",
                    ],
                    "validation_commands": [
                        "python -m pytest tests/tooling -q",
                    ],
                    "execution_status": "closed",
                    "execution_status_evidence_refs": [
                        "spec/planning/evidence/lane_b/v015_activation_execution_hardening_w1_tooling_validation_20260224.md:1",
                    ],
                }
            ]
        },
    )

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--format",
            "json",
        ]
    )

    assert code == 0
    assert stderr == ""
    payload = json.loads(stdout)
    assert isinstance(payload, dict)
    assert payload["exit_code"] == 0
    assert payload["activation_required"] is False


def test_catalog_contract_lane_label_alias_mismatch_exits_fail_closed(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    write_json(issues_path, [])
    write_json(milestones_path, [])
    write_json(
        catalog_path,
        {
            "tasks": [
                {
                    "task_id": "SPT-4001",
                    "lane": "B",
                    "labels": [
                        "lane:C",
                        "type:tooling",
                    ],
                    "validation_commands": [
                        "python -m pytest tests/tooling -q",
                    ],
                    "execution_status": "closed",
                    "execution_status_evidence_refs": [
                        "spec/planning/evidence/lane_b/v015_activation_execution_hardening_w1_tooling_validation_20260224.md:1",
                    ],
                }
            ]
        },
    )

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "catalog task lane label alias mismatch" in stderr
    assert "expected label 'lane:B'" in stderr


def test_catalog_contract_duplicate_validation_commands_exits_fail_closed(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    write_json(issues_path, [])
    write_json(milestones_path, [])
    write_json(
        catalog_path,
        {
            "tasks": [
                {
                    "task_id": "SPT-4001",
                    "milestone_title": "v0.15 Activation Execution Hardening W1",
                    "validation_commands": [
                        "python -m pytest tests/tooling -q",
                        "python -m pytest tests/tooling -q",
                    ],
                    "execution_status": "closed",
                    "execution_status_evidence_refs": [
                        "spec/planning/evidence/lane_b/v015_activation_execution_hardening_w1_tooling_validation_20260224.md:1",
                    ],
                }
            ]
        },
    )

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "catalog task row 0 field 'validation_commands' contains duplicate entry" in stderr


def test_catalog_contract_milestone_title_requires_validation_refs(tmp_path: Path) -> None:
    issues_path = tmp_path / "issues.json"
    milestones_path = tmp_path / "milestones.json"
    catalog_path = tmp_path / "catalog.json"
    write_json(issues_path, [])
    write_json(milestones_path, [])
    write_json(
        catalog_path,
        {
            "tasks": [
                {
                    "task_id": "SPT-4001",
                    "milestone_title": "v0.15 Activation Execution Hardening W1",
                    "execution_status": "closed",
                }
            ]
        },
    )

    code, stdout, stderr = run_main(
        [
            "--issues-json",
            str(issues_path),
            "--milestones-json",
            str(milestones_path),
            "--catalog-json",
            str(catalog_path),
            "--format",
            "json",
        ]
    )

    assert code == 2
    assert stdout == ""
    assert "field 'milestone_title' requires non-empty field 'validation_commands'" in stderr


@pytest.mark.parametrize(
    "scenario_name",
    ("all_false_t4_false", "t4_only", "mixed_t1_t4"),
)
def test_t4_gate_reduction_json_semantics(scenario_name: str) -> None:
    if not supports_t4_contract():
        pytest.skip("Lane A T4 contract is not available yet.")

    expectations = load_expectations(scenario_name)
    t4_override = expectations["t4_override"]
    assert isinstance(t4_override, bool)

    code, stdout, stderr = run_scenario(
        scenario_name, "json", t4_new_scope_publish=t4_override
    )
    assert stderr == ""

    payload = json.loads(stdout)
    assert isinstance(payload, dict)
    assert code == expectations["exit_code"]
    assert_t4_gate_reduction_json(payload, expectations=expectations)


@pytest.mark.parametrize(
    "scenario_name",
    ("all_false_t4_false", "t4_only", "mixed_t1_t4"),
)
def test_t4_gate_reduction_markdown_semantics(scenario_name: str) -> None:
    if not supports_t4_contract():
        pytest.skip("Lane A T4 contract is not available yet.")

    expectations = load_expectations(scenario_name)
    t4_override = expectations["t4_override"]
    assert isinstance(t4_override, bool)

    code, stdout, stderr = run_scenario(
        scenario_name, "markdown", t4_new_scope_publish=t4_override
    )
    assert stderr == ""
    assert code == expectations["exit_code"]
    assert_t4_gate_reduction_markdown(stdout, expectations=expectations)
