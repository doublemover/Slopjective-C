from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

SCRIPT_PATH = Path(__file__).resolve().parents[2] / "scripts" / "run_activation_preflight.py"
SPEC = importlib.util.spec_from_file_location("run_activation_preflight", SCRIPT_PATH)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError("Unable to load scripts/run_activation_preflight.py for tests.")
run_activation_preflight = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = run_activation_preflight
SPEC.loader.exec_module(run_activation_preflight)

REPO_ROOT = Path(__file__).resolve().parents[2]
FIXTURE_ROOT = Path(__file__).resolve().parent / "fixtures" / "activation_triggers" / "preflight"


def run_scenario(tmp_path: Path, name: str) -> tuple[int, Path]:
    scenario_root = FIXTURE_ROOT / name
    output_dir = tmp_path / f"{name}_artifacts"
    open_blockers_path = scenario_root / "open_blockers.json"
    spec_glob = (scenario_root / "spec.md").resolve().relative_to(REPO_ROOT).as_posix()
    args = [
        "--issues-json",
        str(scenario_root / "issues.json"),
        "--milestones-json",
        str(scenario_root / "milestones.json"),
        "--catalog-json",
        str(scenario_root / "catalog.json"),
        "--spec-glob",
        spec_glob,
        "--output-dir",
        str(output_dir),
    ]
    if open_blockers_path.exists():
        args.extend(["--open-blockers-json", str(open_blockers_path)])
    code = run_activation_preflight.main(args)
    return code, output_dir


def read_json(path: Path) -> dict[str, object]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    assert isinstance(payload, dict)
    return payload


def bool_text(value: bool) -> str:
    return "true" if value else "false"


def markdown_freshness_cell(raw_value: object) -> str:
    if raw_value is None:
        return "_none_"
    if isinstance(raw_value, bool):
        return f"`{bool_text(raw_value)}`"
    return f"`{raw_value}`"


def render_checker_markdown(payload: dict[str, object]) -> str:
    inputs = payload["inputs"]
    assert isinstance(inputs, dict)
    actionable_statuses = payload["actionable_statuses"]
    assert isinstance(actionable_statuses, list)
    freshness_payload = payload["freshness"]
    assert isinstance(freshness_payload, dict)
    issues_freshness = freshness_payload["issues"]
    milestones_freshness = freshness_payload["milestones"]
    assert isinstance(issues_freshness, dict)
    assert isinstance(milestones_freshness, dict)
    t4_overlay = payload["t4_governance_overlay"]
    assert isinstance(t4_overlay, dict)
    open_blockers = payload["open_blockers"]
    assert isinstance(open_blockers, dict)
    triggers = payload["triggers"]
    assert isinstance(triggers, list)
    active_trigger_ids = payload["active_trigger_ids"]
    assert isinstance(active_trigger_ids, list)
    open_blockers_input = inputs["open_blockers_json"]
    open_blockers_input_text = (
        f"`{open_blockers_input}`" if open_blockers_input is not None else "_none_"
    )
    t4_overlay_input = inputs["t4_governance_overlay_json"]
    t4_overlay_input_text = (
        f"`{t4_overlay_input}`" if t4_overlay_input is not None else "_none_"
    )

    lines = [
        "# Activation Trigger Check",
        "",
        f"- Mode: `{payload['mode']}`",
        f"- Issues snapshot: `{inputs['issues_json']}`",
        f"- Milestones snapshot: `{inputs['milestones_json']}`",
        f"- Catalog JSON: `{inputs['catalog_json']}`",
        f"- Open blockers JSON: {open_blockers_input_text}",
        f"- T4 governance overlay JSON: {t4_overlay_input_text}",
        (
            "- Actionable statuses: "
            + ", ".join(f"`{status}`" for status in actionable_statuses)
        ),
        f"- Activation required: `{bool_text(bool(payload['activation_required']))}`",
        f"- T4 new scope publish: `{bool_text(bool(t4_overlay['new_scope_publish']))}`",
        f"- T4 source: `{t4_overlay['source']}`",
        f"- Gate open: `{bool_text(bool(payload['gate_open']))}`",
        f"- Queue state: `{payload['queue_state']}`",
        f"- Exit code: `{payload['exit_code']}`",
        f"- Open blockers count: `{open_blockers['count']}`",
        f"- Open blockers trigger fired: `{bool_text(bool(open_blockers['trigger_fired']))}`",
        "",
        "## Snapshot Freshness",
        "",
        "| Snapshot | Requested | Max age (s) | Generated at UTC | Age (s) | Fresh |",
        "| --- | --- | --- | --- | --- | --- |",
        (
            f"| Issues | {markdown_freshness_cell(issues_freshness.get('requested'))} | "
            f"{markdown_freshness_cell(issues_freshness.get('max_age_seconds'))} | "
            f"{markdown_freshness_cell(issues_freshness.get('generated_at_utc'))} | "
            f"{markdown_freshness_cell(issues_freshness.get('age_seconds'))} | "
            f"{markdown_freshness_cell(issues_freshness.get('fresh'))} |"
        ),
        (
            f"| Milestones | {markdown_freshness_cell(milestones_freshness.get('requested'))} | "
            f"{markdown_freshness_cell(milestones_freshness.get('max_age_seconds'))} | "
            f"{markdown_freshness_cell(milestones_freshness.get('generated_at_utc'))} | "
            f"{markdown_freshness_cell(milestones_freshness.get('age_seconds'))} | "
            f"{markdown_freshness_cell(milestones_freshness.get('fresh'))} |"
        ),
        "",
        "## Trigger Results",
        "",
        "| Trigger ID | Fired | Count | Condition |",
        "| --- | --- | --- | --- |",
    ]
    for entry in triggers:
        assert isinstance(entry, dict)
        lines.append(
            "| "
            f"`{entry['id']}` | "
            f"`{bool_text(bool(entry['fired']))}` | "
            f"{entry['count']} | "
            f"{entry['condition']} |"
        )

    lines.append("")
    if active_trigger_ids:
        lines.append(
            "- Active triggers: "
            + ", ".join(f"`{trigger_id}`" for trigger_id in active_trigger_ids)
        )
    else:
        lines.append("- Active triggers: _none_")
    return "\n".join(lines) + "\n"


__all__ = [
    "FIXTURE_ROOT",
    "read_json",
    "render_checker_markdown",
    "run_activation_preflight",
    "run_scenario",
]
