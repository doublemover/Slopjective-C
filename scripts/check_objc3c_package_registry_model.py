#!/usr/bin/env python3
"""Validate and resolve the hosted package registry fixture model."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_package_manager.hosted_registry import (  # noqa: E402
    HostedRegistryResolutionError,
    HostedRegistryResolutionRequest,
    collect_hosted_registry_model_failures,
    resolve_hosted_registry_package,
)
from objc3c_package_manager.model import PACKAGE_MANAGER_TAMPER_CODE  # noqa: E402
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file  # noqa: E402
from objc3c_tooling.paths import repo_rel  # noqa: E402

HOSTED_REGISTRY_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
    / "hosted_registry"
    / "hosted-registry-index.json"
)
OFFLINE_MIRROR_FIXTURE = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
    / "hosted_registry"
    / "offline-mirror-index.json"
)
SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "package-ecosystem"
    / "hosted-registry-resolution-summary.json"
)


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--registry-index", default=str(HOSTED_REGISTRY_FIXTURE))
    parser.add_argument("--offline-mirror-index", default=str(OFFLINE_MIRROR_FIXTURE))
    parser.add_argument("--package-id", default="fixture:network.core")
    parser.add_argument("--package-version", default="1.0.0")
    parser.add_argument("--language-version", default="3.0")
    parser.add_argument("--abi-identity", default="objc3-abi-2025Q4")
    parser.add_argument("--registry-url")
    parser.add_argument("--allow-network", action="store_true")
    parser.add_argument("positional", nargs="*")
    args = parser.parse_args(argv)
    if args.positional:
        args.package_id = args.positional[0]
    if len(args.positional) > 1:
        args.package_version = args.positional[1]
    if len(args.positional) > 2:
        parser.error("expected at most package_id and package_version positional arguments")
    return args


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    registry_path = Path(args.registry_index)
    mirror_path = Path(args.offline_mirror_index)
    index = load_json(registry_path)
    mirror = load_json(mirror_path)

    failures = collect_hosted_registry_model_failures(index, mirror)
    resolution: dict[str, Any] | None = None
    request = HostedRegistryResolutionRequest(
        package_id=str(args.package_id),
        package_version=str(args.package_version),
        language_version=str(args.language_version),
        abi_identity=str(args.abi_identity),
        allow_network=bool(args.allow_network),
        registry_url=args.registry_url,
    )
    try:
        resolved = resolve_hosted_registry_package(index, mirror, request)
        resolution = {
            "package_id": resolved.package_id,
            "package_version": resolved.package_version,
            "source_digest": resolved.source_digest,
            "manifest_digest": resolved.manifest_digest,
            "cache_path": resolved.cache_path,
            "cache_digest": resolved.cache_digest,
            "registry_record_digest": resolved.registry_record_digest,
            "registry_signature_id": resolved.registry_signature_id,
        }
    except HostedRegistryResolutionError as exc:
        failures.extend(
            failure for failure in exc.failures if failure not in failures
        )

    payload = {
        "contract_id": "objc3c.package_ecosystem.hosted_registry_resolution.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "registry_index": repo_rel(registry_path),
        "offline_mirror_index": repo_rel(mirror_path),
        "package_id": args.package_id,
        "package_version": args.package_version,
        "network_request": {
            "allow_network": bool(args.allow_network),
            "registry_url": args.registry_url,
        },
        "resolution": resolution,
        "tamper_diagnostic": PACKAGE_MANAGER_TAMPER_CODE,
        "failures": failures,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload, sort_keys=True)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if failures:
        print("objc3c-package-registry-model: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-package-registry-model: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
