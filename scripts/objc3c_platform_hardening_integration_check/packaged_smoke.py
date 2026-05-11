"""Packaged-bundle platform-hardening smoke validation."""

from __future__ import annotations

import sys

from objc3c_tooling.json_io import load_json_object as load_json, write_json_file
from objc3c_tooling.paths import repo_rel
from platform_hardening_contracts import (
    PACKAGE_MANIFEST_PATH,
    PACKAGED_SMOKE_INTEGRATION_CONTRACT_PATH,
    PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH,
    platform_hardening_owner_payload,
)

from .assertions import expect


def run_packaged_bundle_smoke() -> int:
    contract = load_json(PACKAGED_SMOKE_INTEGRATION_CONTRACT_PATH)
    manifest = load_json(PACKAGE_MANIFEST_PATH)
    failures: list[str] = []

    for field in contract["manifest_fields"]:
        expect(manifest.get(field) not in (None, "", []), f"package manifest missing {field}", failures)

    command_surfaces = manifest.get("command_surfaces", {})
    for command_name in contract["required_command_surfaces"]:
        expect(command_name in command_surfaces, f"package manifest missing command surface {command_name}", failures)

    public_actions = manifest.get("platform_hardening_public_actions", [])
    package_bridge = str(contract["package_bridge"])
    manifest_package_bridge = manifest.get("package_bridge")
    for action in contract["public_actions"]:
        expect(action in public_actions, f"package manifest missing public action {action}", failures)
    expect(manifest_package_bridge == package_bridge, f"package manifest missing package bridge {package_bridge}", failures)

    payload = {
        "contract_id": "objc3c.platform.hardening.integration.summary.v1",
        "ok": not failures,
        "owner_policy": platform_hardening_owner_payload(),
        "blocker_metadata": {
            "blocker_owner": "platform-hardening-blockers",
            "blocking_conditions": [
                "packaged platform hardening manifest missing source contract",
                "packaged command surface absent from objc3c bridge",
                "packaged platform hardening validation emitted evidence-log status",
            ],
        },
        "failures": failures,
        "mode": "packaged-bundle-smoke",
        "reports": {
            "package_manifest": repo_rel(PACKAGE_MANIFEST_PATH),
        },
    }
    PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH, payload)
    print(f"summary_path: {repo_rel(PLATFORM_HARDENING_INTEGRATION_SUMMARY_PATH)}")
    if failures:
        print("platform-hardening-integration: FAIL", file=sys.stderr)
        for failure in failures:
            print(f"- {failure}", file=sys.stderr)
        return 1
    print("platform-hardening-integration: PASS")
    return 0
