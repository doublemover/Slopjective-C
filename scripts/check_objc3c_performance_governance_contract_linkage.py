#!/usr/bin/env python3
"""Validate runtime-performance contract linkage into performance governance."""

from __future__ import annotations

import sys
from datetime import datetime, timezone
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
SCRIPT_ROOT = ROOT / "scripts"
for import_root in (ROOT, SCRIPT_ROOT):
    import_root_text = str(import_root)
    if import_root_text not in sys.path:
        sys.path.insert(0, import_root_text)

from objc3c_shared.json_io import write_report_json  # noqa: E402
from objc3c_tooling.paths import repo_rel  # noqa: E402
from scripts.objc3c_performance_governance_contract_linkage import (  # noqa: E402
    validate_runtime_contract_linkage,
)


SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "performance-governance"
    / "runtime-contract-linkage-summary.json"
)


def main() -> int:
    try:
        summary = validate_runtime_contract_linkage()
    except RuntimeError as exc:
        print(f"objc3c-performance-governance-runtime-contract-linkage: FAIL\n- {exc}", file=sys.stderr)
        return 1

    payload = {
        **summary,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
    }
    write_report_json(SUMMARY_PATH, payload, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("objc3c-performance-governance-runtime-contract-linkage: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
