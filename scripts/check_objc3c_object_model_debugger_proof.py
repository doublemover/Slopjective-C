#!/usr/bin/env python3
"""Validate object-model debugger source, line-table, and value-inspection proof."""

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

from scripts.objc3c_object_model_debugger_proof import (  # noqa: E402
    DEFAULT_CONTRACT_PATH,
    VALIDATION_CONTRACT_ID,
    validate_contract_path,
)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "fixtures",
        nargs="*",
        type=Path,
        default=[DEFAULT_CONTRACT_PATH],
        help="object-model debugger proof contract paths",
    )
    args = parser.parse_args(argv)

    results = [
        validate_contract_path(path, run_production_probe=True).to_payload()
        for path in args.fixtures
    ]
    ok = all(result["ok"] is True for result in results)
    payload = {
        "ok": ok,
        "contract_id": VALIDATION_CONTRACT_ID,
        "fixture_count": len(results),
        "results": results,
    }
    print(json.dumps(payload, indent=2, sort_keys=True))
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
