#!/usr/bin/env python3
"""Validate package network resolution and release-channel publication fixtures."""

from __future__ import annotations

import argparse
import sys
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
if str(ROOT) not in sys.path:
    sys.path.insert(0, str(ROOT))

from objc3c_package_manager.model import PACKAGE_MANAGER_TAMPER_CODE  # noqa: E402
from objc3c_package_manager.network_publication import (  # noqa: E402
    NETWORK_DEPENDENCY_RESOLUTION_CONTRACT_ID,
    PACKAGE_RELEASE_CHANNEL_PUBLICATION_CONTRACT_ID,
    collect_package_network_publication_failures,
)
from objc3c_tooling.json_io import load_json_object as load_json, write_json_file  # noqa: E402
from objc3c_tooling.paths import repo_rel  # noqa: E402

FIXTURE_ROOT = (
    ROOT
    / "tests"
    / "tooling"
    / "fixtures"
    / "package_ecosystem"
)
NETWORK_RESOLUTION_FIXTURE = (
    FIXTURE_ROOT
    / "network_resolution"
    / "network-dependency-resolution.json"
)
RELEASE_CHANNEL_PUBLICATION_FIXTURE = (
    FIXTURE_ROOT
    / "network_resolution"
    / "package-release-channel-publication.json"
)
HOSTED_REGISTRY_FIXTURE = (
    FIXTURE_ROOT
    / "hosted_registry"
    / "hosted-registry-index.json"
)
OFFLINE_MIRROR_FIXTURE = (
    FIXTURE_ROOT
    / "hosted_registry"
    / "offline-mirror-index.json"
)
SUMMARY_PATH = (
    ROOT
    / "tmp"
    / "reports"
    / "package-ecosystem"
    / "network-publication-summary.json"
)


def parse_args(argv: list[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--network-resolution", default=str(NETWORK_RESOLUTION_FIXTURE))
    parser.add_argument("--release-channel-publication", default=str(RELEASE_CHANNEL_PUBLICATION_FIXTURE))
    parser.add_argument("--hosted-registry-index", default=str(HOSTED_REGISTRY_FIXTURE))
    parser.add_argument("--offline-mirror-index", default=str(OFFLINE_MIRROR_FIXTURE))
    return parser.parse_args(argv)


def _summary_payload(
    *,
    network_resolution_path: Path,
    release_channel_publication_path: Path,
    hosted_registry_path: Path,
    offline_mirror_path: Path,
    network_resolution: dict[str, Any],
    publication: dict[str, Any],
    failures: list[str],
) -> dict[str, Any]:
    return {
        "contract_id": "objc3c.package_ecosystem.network_publication.summary.v1",
        "status": "PASS" if not failures else "FAIL",
        "network_resolution_contract": NETWORK_DEPENDENCY_RESOLUTION_CONTRACT_ID,
        "release_channel_publication_contract": PACKAGE_RELEASE_CHANNEL_PUBLICATION_CONTRACT_ID,
        "network_resolution": repo_rel(network_resolution_path),
        "release_channel_publication": repo_rel(release_channel_publication_path),
        "hosted_registry_index": repo_rel(hosted_registry_path),
        "offline_mirror_index": repo_rel(offline_mirror_path),
        "issue_refs": sorted(set(network_resolution.get("issue_refs", []) + publication.get("issue_refs", []))),
        "umbrella_issue_refs": sorted(set(network_resolution.get("umbrella_issue_refs", []) + publication.get("umbrella_issue_refs", []))),
        "network_policy": network_resolution.get("network_policy"),
        "publication_policy": publication.get("publication_policy"),
        "channel_freshness": publication.get("channel_freshness"),
        "failure_modes": sorted(set(network_resolution.get("failure_modes", []) + publication.get("failure_modes", []))),
        "tamper_diagnostic": PACKAGE_MANAGER_TAMPER_CODE,
        "failures": failures,
    }


def main(argv: list[str] | None = None) -> int:
    args = parse_args(argv or sys.argv[1:])
    network_resolution_path = Path(args.network_resolution)
    publication_path = Path(args.release_channel_publication)
    hosted_registry_path = Path(args.hosted_registry_index)
    offline_mirror_path = Path(args.offline_mirror_index)

    network_resolution = load_json(network_resolution_path)
    publication = load_json(publication_path)
    hosted_index = load_json(hosted_registry_path)
    mirror = load_json(offline_mirror_path)

    failures = collect_package_network_publication_failures(
        network_resolution,
        publication,
        hosted_index=hosted_index,
        mirror=mirror,
    )
    payload = _summary_payload(
        network_resolution_path=network_resolution_path,
        release_channel_publication_path=publication_path,
        hosted_registry_path=hosted_registry_path,
        offline_mirror_path=offline_mirror_path,
        network_resolution=network_resolution,
        publication=publication,
        failures=failures,
    )
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(SUMMARY_PATH, payload, sort_keys=True)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    if failures:
        print("objc3c-package-network-publication: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("objc3c-package-network-publication: PASS")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
