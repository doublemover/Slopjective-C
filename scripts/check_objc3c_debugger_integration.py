#!/usr/bin/env python3
"""Validate replayable Objective-C 3 debugger integration and stepping plans."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPT_ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

from scripts.objc3c_debugger_integration import (
    DEFAULT_FIXTURE_PATH,
    generate_stepping_plan_path,
    validate_replay_path,
)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--plan",
        action="store_true",
        help="emit the deterministic LLDB command and source-backed stepping plan",
    )
    parser.add_argument(
        "fixtures",
        nargs="*",
        type=Path,
        default=[DEFAULT_FIXTURE_PATH],
        help="debugger replay fixture paths",
    )
    args = parser.parse_args(argv)

    if args.plan:
        results = [generate_stepping_plan_path(path) for path in args.fixtures]
    else:
        results = [validate_replay_path(path).to_payload() for path in args.fixtures]
    ok = all(result["ok"] is True for result in results)
    payload = {
        "ok": ok,
        "contract_id": "objc3c.debugger-integration.plan-envelope.v1"
        if args.plan
        else "objc3c.debugger-integration.validation-envelope.v1",
        "fixture_count": len(results),
        "results": results,
    }
    print(json.dumps(payload, indent=2, sort_keys=True))
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
