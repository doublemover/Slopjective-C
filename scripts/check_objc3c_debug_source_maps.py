#!/usr/bin/env python3
"""Validate Objective-C 3 source maps, debug maps, and native line tables."""

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

from scripts.objc3c_debug_maps.model import (
    DEFAULT_FIXTURE_PATH,
    INSPECTION_CONTRACT_ID,
    VALIDATION_CONTRACT_ID,
    inspect_bundle_path,
    validate_bundle_path,
)


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--inspect",
        action="store_true",
        help="emit a debug-map inspection summary instead of the validation envelope",
    )
    parser.add_argument(
        "fixtures",
        nargs="*",
        type=Path,
        default=[DEFAULT_FIXTURE_PATH],
        help="debug/source-map fixture bundle paths",
    )
    args = parser.parse_args(argv)

    if args.inspect:
        results = [inspect_bundle_path(path) for path in args.fixtures]
    else:
        results = [validate_bundle_path(path).to_payload() for path in args.fixtures]
    ok = all(result["ok"] is True for result in results)
    payload = {
        "ok": ok,
        "contract_id": INSPECTION_CONTRACT_ID if args.inspect else VALIDATION_CONTRACT_ID,
        "fixture_count": len(results),
        "results": results,
    }
    print(json.dumps(payload, indent=2, sort_keys=True))
    return 0 if ok else 1


if __name__ == "__main__":
    raise SystemExit(main())
