#!/usr/bin/env python3
"""Verify objc3c package manifest digests and signature envelopes."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_package_manager.model import package_manifest_digest  # noqa: E402
from objc3c_package_manager.trust import (  # noqa: E402
    collect_manifest_trust_failures,
    load_trust_policy,
    resolve_package_trust_cli_path,
)
from objc3c_tooling.json_io import load_json_object as load_json  # noqa: E402


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description="Verify an objc3c package manifest")
    parser.add_argument("--manifest", required=True, help="package manifest JSON path")
    parser.add_argument(
        "--envelope",
        help="optional detached signature envelope path; defaults to manifest.trust",
    )
    parser.add_argument(
        "--trust-policy",
        help="optional package signing trust policy JSON path",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(list(argv or sys.argv[1:]))
    try:
        manifest = load_json(
            resolve_package_trust_cli_path(
                args.manifest,
                root=ROOT,
                purpose="package manifest input",
            )
        )
        if args.envelope:
            manifest = dict(manifest)
            manifest["trust"] = load_json(
                resolve_package_trust_cli_path(
                    args.envelope,
                    root=ROOT,
                    purpose="package signature envelope input",
                )
            )
        trust_policy = (
            load_trust_policy(
                resolve_package_trust_cli_path(
                    args.trust_policy,
                    root=ROOT,
                    purpose="package trust policy input",
                )
            )
            if args.trust_policy
            else None
        )
        recorded_digest = str(manifest.get("manifest_digest", ""))
        computed_digest = package_manifest_digest(manifest)
        failures: list[str] = []
        if recorded_digest != computed_digest:
            failures.append(
                f"manifest digest mismatch: recorded {recorded_digest} != computed {computed_digest}"
            )
        failures.extend(
            collect_manifest_trust_failures(
                manifest,
                manifest_digest=recorded_digest,
                trust_policy=trust_policy,
            )
        )
    except (RuntimeError, OSError, json.JSONDecodeError) as exc:
        print(str(exc), file=sys.stderr)
        return 1

    if failures:
        print("objc3c-package-verify: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-package-verify: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
