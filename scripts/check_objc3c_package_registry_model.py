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
from objc3c_package_manager.hosted_service import (  # noqa: E402
    HOSTED_REGISTRY_SERVICE_DEFAULT_SUBJECT_ID,
    HOSTED_REGISTRY_SERVICE_DEFAULT_TOKEN_ID,
    HOSTED_REGISTRY_SERVICE_ID,
    HostedRegistryServiceError,
    HostedRegistryServiceRequest,
    resolve_hosted_registry_service_request,
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
    parser.add_argument("--service-id", default=HOSTED_REGISTRY_SERVICE_ID)
    parser.add_argument(
        "--auth-subject-id",
        default=HOSTED_REGISTRY_SERVICE_DEFAULT_SUBJECT_ID,
    )
    parser.add_argument(
        "--auth-token-id",
        default=HOSTED_REGISTRY_SERVICE_DEFAULT_TOKEN_ID,
    )
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

    failures = collect_hosted_registry_model_failures(index, mirror, root=ROOT)
    resolution: dict[str, Any] | None = None
    service_decision: dict[str, Any] | None = None
    request = HostedRegistryResolutionRequest(
        package_id=str(args.package_id),
        package_version=str(args.package_version),
        language_version=str(args.language_version),
        abi_identity=str(args.abi_identity),
        service_id=str(args.service_id),
        auth_subject_id=str(args.auth_subject_id),
        auth_token_id=str(args.auth_token_id),
        allow_network=bool(args.allow_network),
        registry_url=args.registry_url,
    )
    service_request = HostedRegistryServiceRequest(
        package_id=request.package_id,
        package_version=request.package_version,
        service_id=request.service_id,
        endpoint_id=request.endpoint_id,
        channel_id=request.channel_id,
        auth_subject_id=request.auth_subject_id,
        auth_token_id=request.auth_token_id,
        allow_network=request.allow_network or request.registry_url is not None,
    )
    service_ref = index.get("hosted_service")
    if isinstance(service_ref, dict):
        service_fixture_path = service_ref.get("service_fixture_path")
        if isinstance(service_fixture_path, str):
            try:
                service = load_json(ROOT / service_fixture_path)
                decision = resolve_hosted_registry_service_request(
                    service,
                    service_request,
                    index=index,
                    root=ROOT,
                )
                service_decision = {
                    "service_id": decision.service_id,
                    "package_id": decision.package_id,
                    "package_version": decision.package_version,
                    "operation": decision.operation,
                    "registry_index_path": decision.registry_index_path,
                    "offline_mirror_path": decision.offline_mirror_path,
                    "auth_subject_id": decision.auth_subject_id,
                }
            except HostedRegistryServiceError as exc:
                failures.extend(
                    failure for failure in exc.failures if failure not in failures
                )
            except RuntimeError as exc:
                failure = f"{PACKAGE_MANAGER_TAMPER_CODE}: hosted registry service fixture load failed: {exc}"
                if failure not in failures:
                    failures.append(failure)
    try:
        resolved = resolve_hosted_registry_package(index, mirror, request)
        resolution = {
            "package_id": resolved.package_id,
            "package_version": resolved.package_version,
            "source_digest": resolved.source_digest,
            "manifest_digest": resolved.manifest_digest,
            "cache_path": resolved.cache_path,
            "cache_digest": resolved.cache_digest,
            "snapshot_id": resolved.snapshot_id,
            "cache_key": resolved.cache_key,
            "offline_mirror_path": resolved.offline_mirror_path,
            "registry_record_digest": resolved.registry_record_digest,
            "registry_signature_id": resolved.registry_signature_id,
            "trust_result_id": resolved.trust_result_id,
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
        "service_request": {
            "service_id": args.service_id,
            "auth_subject_id": args.auth_subject_id,
            "auth_token_id": args.auth_token_id,
            "operation": service_request.operation,
        },
        "service_decision": service_decision,
        "service_boundary": index.get("service_boundary"),
        "hosted_service": index.get("hosted_service"),
        "provider_model": index.get("provider_model"),
        "snapshot": index.get("snapshot"),
        "service_availability": index.get("service_availability"),
        "endpoint_identity": index.get("endpoint_identity"),
        "lock_materialization": index.get("lock_materialization"),
        "lock_trust_material": index.get("lock_trust_material"),
        "failure_modes": index.get("failure_modes"),
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
