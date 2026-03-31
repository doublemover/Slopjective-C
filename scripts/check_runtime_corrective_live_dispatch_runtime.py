#!/usr/bin/env python3
from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
ACCEPTANCE_SCRIPT = ROOT / "scripts/check_objc3c_runtime_acceptance.py"
OUT_DIR = ROOT / "tmp/reports/m316/M316-D002"
SUMMARY_PATH = OUT_DIR / "live_dispatch_runtime_summary.json"
RUNBOOK_PATH = ROOT / "docs/runbooks/objc3c_runtime_corrective.md"


def load_acceptance_module():
    spec = importlib.util.spec_from_file_location("objc3c_runtime_acceptance", ACCEPTANCE_SCRIPT)
    if spec is None or spec.loader is None:
        raise RuntimeError(f"failed to load acceptance module from {ACCEPTANCE_SCRIPT}")
    module = importlib.util.module_from_spec(spec)
    sys.modules[spec.name] = module
    spec.loader.exec_module(module)
    return module


def case_to_dict(case) -> dict[str, object]:
    return {
        "case_id": case.case_id,
        "probe": case.probe,
        "fixture": case.fixture,
        "claim_class": case.claim_class,
        "passed": case.passed,
        "summary": case.summary,
    }


def main() -> int:
    acceptance = load_acceptance_module()
    clangxx = acceptance.find_clangxx()
    acceptance.ensure_native_binaries()
    runbook_text = RUNBOOK_PATH.read_text(encoding="utf-8")

    run_dir = OUT_DIR / "run"
    run_dir.mkdir(parents=True, exist_ok=True)

    cases = [
        acceptance.check_live_dispatch_fast_path_case(clangxx, run_dir),
        acceptance.check_realization_lookup_reflection_runtime_case(clangxx, run_dir),
    ]

    summary = {
        "issue": "M316-D002",
        "acceptance_script": str(ACCEPTANCE_SCRIPT.relative_to(ROOT)).replace("\\", "/"),
        "runbook_mentions_script": "check_runtime_corrective_live_dispatch_runtime.py" in runbook_text,
        "case_count": len(cases),
        "cases": [case_to_dict(case) for case in cases],
    }
    summary["ok"] = summary["runbook_mentions_script"] and all(case.passed for case in cases)

    OUT_DIR.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(summary, indent=2))
    return 0 if summary["ok"] else 1


if __name__ == "__main__":
    raise SystemExit(main())
