#!/usr/bin/env python3
"""Shared compiler/runtime acceptance harness for M313."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import asdict, dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
REGISTRY_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_b002_shared_compiler_runtime_integration_harness_core_feature_implementation_registry.json"
SCHEMA_JSON = ROOT / "spec" / "planning" / "compiler" / "m313" / "m313_c001_acceptance_artifact_schema_and_replay_contract_contract_and_architecture_freeze_schema.json"


@dataclass(frozen=True)
class SuiteEntry:
    suite_id: str
    roots: list[str]
    migration_owner_issue: str
    ci_owner_issue: str


def load_registry() -> tuple[dict[str, object], list[SuiteEntry]]:
    payload = json.loads(REGISTRY_JSON.read_text(encoding="utf-8"))
    suites = [SuiteEntry(**entry) for entry in payload["suite_registry"]]
    return payload, suites


def load_schema() -> dict[str, object]:
    return json.loads(SCHEMA_JSON.read_text(encoding="utf-8"))


def validate_roots(entries: Sequence[SuiteEntry]) -> dict[str, object]:
    suite_results = []
    ok = True
    for entry in entries:
        root_results = []
        for root in entry.roots:
            exists = (ROOT / root).exists()
            root_results.append({"root": root, "exists": exists})
            ok = ok and exists
        suite_results.append({
            "suite_id": entry.suite_id,
            "roots": root_results,
            "migration_owner_issue": entry.migration_owner_issue,
            "ci_owner_issue": entry.ci_owner_issue,
        })
    return {"ok": ok, "suite_results": suite_results}


def _classify_roots(roots: Sequence[str]) -> tuple[list[str], list[str]]:
    fixture_roots: list[str] = []
    probe_roots: list[str] = []
    for root in roots:
        if root.startswith("tests/tooling/runtime"):
            probe_roots.append(root)
        else:
            fixture_roots.append(root)
    return fixture_roots, probe_roots


def _count_matches(root: Path, pattern: str) -> int:
    if not root.exists():
        return 0
    return sum(1 for path in root.rglob(pattern) if path.is_file())


def build_suite_execution_summary(entry: SuiteEntry, summary_out: Path | None) -> dict[str, object]:
    schema = load_schema()
    fixture_roots, probe_roots = _classify_roots(entry.roots)
    root_results = [{"root": root, "exists": (ROOT / root).exists()} for root in entry.roots]
    existing_root_count = sum(1 for item in root_results if item["exists"])
    objc3_fixture_count = sum(_count_matches(ROOT / root, "*.objc3") for root in fixture_roots)
    runtime_probe_count = sum(_count_matches(ROOT / root, "*.cpp") for root in probe_roots)
    total_files_count = sum(_count_matches(ROOT / root, "*") for root in entry.roots)

    if summary_out is None:
        summary_out = ROOT / "tmp" / "reports" / "m313" / "acceptance" / entry.suite_id / "summary.json"

    summary_rel = str(summary_out.relative_to(ROOT)).replace("\\", "/")
    return {
        "schema_version": schema["schema_version"],
        "contract_id": schema["contract_id"],
        "suite_id": entry.suite_id,
        "artifact_class": "suite_execution_summary",
        "producer": {
            "tool": "scripts/shared_compiler_runtime_acceptance_harness.py",
            "issue": "M313-C002",
            "validation_posture": "shared_acceptance_harness",
        },
        "ok": existing_root_count == len(entry.roots),
        "inputs": {
            "roots": entry.roots,
        },
        "outputs": {
            "summary_path": summary_rel,
        },
        "replay": {
            "commands": [
                f"python scripts/shared_compiler_runtime_acceptance_harness.py --run-suite {entry.suite_id} --summary-out {summary_rel}"
            ],
            "cwd": ".",
        },
        "measurements": {
            "case_counts": {
                "root_count": len(entry.roots),
                "existing_root_count": existing_root_count,
                "objc3_fixture_count": objc3_fixture_count,
                "runtime_probe_count": runtime_probe_count,
                "total_files_count": total_files_count,
            },
            "fixture_roots": fixture_roots,
            "probe_roots": probe_roots,
            "root_results": root_results,
        },
    }


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--list-suites", action="store_true")
    parser.add_argument("--show-suite")
    parser.add_argument("--check-roots", action="store_true")
    parser.add_argument("--run-suite")
    parser.add_argument("--summary-out", type=Path)
    return parser.parse_args(argv)


def main(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    payload, suites = load_registry()
    selected = suites
    if args.show_suite:
        selected = [entry for entry in suites if entry.suite_id == args.show_suite]
        if not selected:
            print(json.dumps({"ok": False, "error": f"unknown suite: {args.show_suite}"}, indent=2), file=sys.stderr)
            return 1

    if args.list_suites:
        print(json.dumps({"contract_id": payload["contract_id"], "suite_registry": [asdict(entry) for entry in suites]}, indent=2))
        return 0

    if args.show_suite:
        print(json.dumps({"contract_id": payload["contract_id"], "suite": asdict(selected[0])}, indent=2))
        return 0

    if args.check_roots:
        summary = {
            "mode": payload["mode"],
            "contract_id": payload["contract_id"],
            **validate_roots(selected),
        }
        rendered = json.dumps(summary, indent=2)
        if args.summary_out is not None:
            args.summary_out.parent.mkdir(parents=True, exist_ok=True)
            args.summary_out.write_text(rendered + "\n", encoding="utf-8")
        print(rendered)
        return 0 if summary["ok"] else 1

    if args.run_suite:
        selected = [entry for entry in suites if entry.suite_id == args.run_suite]
        if not selected:
            print(json.dumps({"ok": False, "error": f"unknown suite: {args.run_suite}"}, indent=2), file=sys.stderr)
            return 1
        summary = build_suite_execution_summary(selected[0], args.summary_out)
        rendered = json.dumps(summary, indent=2)
        target = args.summary_out or (ROOT / summary["outputs"]["summary_path"])
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_text(rendered + "\n", encoding="utf-8")
        print(rendered)
        return 0 if summary["ok"] else 1

    print(json.dumps({"ok": False, "error": "no mode selected"}, indent=2), file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
