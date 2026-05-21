#!/usr/bin/env python3
"""Create deterministic local package signature envelopes for fixture replay."""

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
    LOCAL_PACKAGE_SIGNING_BACKEND,
    PackageTrustError,
    load_trust_policy,
    production_signing_reserved_diagnostic,
    sign_manifest_trust_envelope,
)
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file  # noqa: E402


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(
        description=(
            "Sign an objc3c package manifest. Production signing is reserved; "
            "deterministic-test-replay requires --fixture-replay."
        )
    )
    parser.add_argument("--manifest", required=True, help="package manifest JSON path")
    parser.add_argument("--out", help="signature envelope output path")
    parser.add_argument(
        "--trust-policy",
        help="optional package signing trust policy JSON path",
    )
    parser.add_argument(
        "--backend",
        default="production",
        choices=("production", LOCAL_PACKAGE_SIGNING_BACKEND),
        help="signing backend; production is reserved-fail-closed",
    )
    parser.add_argument(
        "--fixture-replay",
        action="store_true",
        help="allow the deterministic local fixture/replay signer",
    )
    return parser.parse_args(argv)


def main(argv: list[str] | None = None) -> int:
    args = parse_args(list(argv or sys.argv[1:]))
    if args.backend != LOCAL_PACKAGE_SIGNING_BACKEND or not args.fixture_replay:
        print(production_signing_reserved_diagnostic(), file=sys.stderr)
        return 1

    try:
        manifest = load_json(ROOT / args.manifest if not Path(args.manifest).is_absolute() else Path(args.manifest))
        trust_policy = load_trust_policy(args.trust_policy) if args.trust_policy else None
        envelope = sign_manifest_trust_envelope(
            manifest,
            manifest_digest=package_manifest_digest(manifest),
            trust_policy=trust_policy,
            backend=LOCAL_PACKAGE_SIGNING_BACKEND,
            fixture_replay=True,
        )
    except (PackageTrustError, RuntimeError, OSError, json.JSONDecodeError) as exc:
        print(str(exc), file=sys.stderr)
        return 1

    if args.out:
        output_path = ROOT / args.out if not Path(args.out).is_absolute() else Path(args.out)
        write_json_file(output_path, envelope)
        print(f"signature_envelope: {output_path.relative_to(ROOT).as_posix() if output_path.is_relative_to(ROOT) else output_path}")
    else:
        sys.stdout.write(json.dumps(envelope, indent=2, sort_keys=True) + "\n")
    print("objc3c-package-sign: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
