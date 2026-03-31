#!/usr/bin/env python3
"""Historical checker compatibility bridge generator."""

from __future__ import annotations

import argparse
import json
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
PLAN_JSON = ROOT / "spec" / "planning" / "compiler" / "historical_checker" / "m313_c003_historical_checker_compatibility_bridge_and_deprecation_surface_core_feature_expansion_plan.json"
SCHEMA_JSON = ROOT / "spec" / "planning" / "compiler" / "historical_checker" / "m313_c001_acceptance_artifact_schema_and_replay_contract_contract_and_architecture_freeze_schema.json"


def load_plan() -> dict[str, object]:
    return json.loads(PLAN_JSON.read_text(encoding="utf-8"))


def load_schema() -> dict[str, object]:
    return json.loads(SCHEMA_JSON.read_text(encoding="utf-8"))


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--list-bridges", action="store_true")
    parser.add_argument("--run-bridge")
    parser.add_argument("--summary-out", type=Path)
    return parser.parse_args(argv)


def match_paths(globs: list[str]) -> list[str]:
    matched: set[str] = set()
    for glob in globs:
        for path in ROOT.glob(glob):
            matched.add(str(path.relative_to(ROOT)).replace("\\", "/"))
    return sorted(matched)


def build_bridge_summary(bridge: dict[str, object], summary_out: Path | None) -> dict[str, object]:
    schema = load_schema()
    if summary_out is None:
        summary_out = ROOT / "tmp" / "reports" / "historical_checker" / "compatibility" / str(bridge["bridge_id"]) / "summary.json"
    summary_rel = str(summary_out.relative_to(ROOT)).replace("\\", "/")
    matched = match_paths(list(bridge["wrapper_globs"]))
    return {
        "schema_version": schema["schema_version"],
        "contract_id": schema["contract_id"],
        "suite_id": bridge["suite_id"],
        "artifact_class": "compatibility_bridge_summary",
        "producer": {
            "tool": "scripts/historical_checker_compatibility_bridge.py",
            "surface_id": "objc3c.validation.acceptance.compatibilitybridge.v1",
            "validation_posture": "migration_bridge",
        },
        "ok": True,
        "inputs": {
            "roots": bridge["wrapper_globs"],
        },
        "outputs": {
            "summary_path": summary_rel,
        },
        "replay": {
            "commands": [
                f"python scripts/historical_checker_compatibility_bridge.py --run-bridge {bridge['bridge_id']} --summary-out {summary_rel}"
            ],
            "cwd": ".",
        },
        "measurements": {
            "legacy_wrappers_observed": len(matched),
            "legacy_wrappers_remaining": len(matched),
            "deprecation_owner_issue": bridge["deprecation_owner_issue"],
            "matched_paths_sample": matched[:20],
        },
    }


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    plan = load_plan()
    bridges = list(plan["bridge_families"])

    if args.list_bridges:
        print(json.dumps({"contract_id": plan["contract_id"], "bridge_families": bridges}, indent=2))
        return 0

    if args.run_bridge:
        selected = [entry for entry in bridges if entry["bridge_id"] == args.run_bridge]
        if not selected:
            print(json.dumps({"ok": False, "error": f"unknown bridge: {args.run_bridge}"}, indent=2))
            return 1
        summary = build_bridge_summary(selected[0], args.summary_out)
        rendered = json.dumps(summary, indent=2)
        target = args.summary_out or (ROOT / summary["outputs"]["summary_path"])
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(rendered + "\n", encoding="utf-8")
        print(rendered)
        return 0

    print(json.dumps({"ok": False, "error": "no mode selected"}, indent=2))
    return 1


if __name__ == "__main__":
    raise SystemExit(main(__import__("sys").argv[1:]))
