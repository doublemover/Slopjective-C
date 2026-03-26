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


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--list-suites", action="store_true")
    parser.add_argument("--show-suite")
    parser.add_argument("--check-roots", action="store_true")
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

    print(json.dumps({"ok": False, "error": "no mode selected"}, indent=2), file=sys.stderr)
    return 1


if __name__ == "__main__":
    raise SystemExit(main(sys.argv[1:]))
