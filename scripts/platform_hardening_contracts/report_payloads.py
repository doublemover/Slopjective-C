"""Report payload construction for platform-hardening contracts."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel, resolve_repo_path

from .constants import PLATFORM_HARDENING_OWNER_POLICY, PUBLICATION_SURFACE
from .contract_predicates import expect, synthetic_unsupported_host_check
from .models import HostSnapshot
from .orchestration import current_host, required_tool_probes, utc_now
from .source_surface_catalog import (
    BOUNDARY_INVENTORY_PATH,
    SUPPORT_TIER_POLICY_PATH,
    SUPPORTED_PLATFORMS_PATH,
    UNSUPPORTED_HOST_POLICY_PATH,
)
from .support_evidence import (
    build_support_evidence_matrix_sections,
    load_platform_toolchain_support_evidence,
    validate_platform_toolchain_support_evidence,
)


def load_json_object(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"{repo_rel(path)} did not contain a JSON object")
    return payload


def build_unsupported_host_fail_closed_payload(
    unsupported_host_policy: dict[str, Any],
    *,
    default_platform_id: str,
    supported_platform_ids: list[str],
) -> dict[str, Any]:
    synthetic_checks = unsupported_host_policy.get("synthetic_unsupported_host_checks")
    expect(
        isinstance(synthetic_checks, list) and synthetic_checks,
        "unsupported host policy missing synthetic_unsupported_host_checks",
    )

    checks: list[dict[str, Any]] = []
    for check in synthetic_checks:
        expect(isinstance(check, dict), "unsupported host policy synthetic check must be an object")
        expect(
            check.get("expected_behavior") == "fail-closed",
            "unsupported host policy synthetic check must expect fail-closed behavior",
        )
        host_id = str(check["host_id"])
        failure_id = str(check["failure_id"])
        host = HostSnapshot(
            os=str(check["host_os"]),
            arch=str(check["host_arch"]),
            system=str(check["host_system"]).lower(),
            machine=str(check["host_machine"]).lower(),
        )
        checks.append(
            synthetic_unsupported_host_check(
                host_id=host_id,
                host=host,
                failure_id=failure_id,
                default_platform_id=default_platform_id,
                supported_platform_ids=supported_platform_ids,
            )
        )

    failed_closed_count = sum(1 for check in checks if check["failed_closed"] is True)
    failure_ids = [str(check["failure_id"]) for check in checks]
    support_claim_widened = any(check["claimed_as_supported"] is True for check in checks)
    all_fail_closed = failed_closed_count == len(checks)
    return {
        "status": "PASS" if all_fail_closed and not support_claim_widened else "FAIL",
        "failure_class_id": "unsupported-host-os-or-arch",
        "summary": {
            "checked_host_count": len(checks),
            "fail_closed_host_count": failed_closed_count,
            "support_claim_widened": support_claim_widened,
            "supported_platform_ids": supported_platform_ids,
            "failure_ids": failure_ids,
        },
        "checks": checks,
    }


def build_support_matrix_payload() -> dict[str, Any]:
    boundary = load_json_object(BOUNDARY_INVENTORY_PATH)
    tier_policy = load_json_object(SUPPORT_TIER_POLICY_PATH)
    supported_platforms = load_json_object(SUPPORTED_PLATFORMS_PATH)
    unsupported_host_policy = load_json_object(UNSUPPORTED_HOST_POLICY_PATH)
    support_evidence = load_platform_toolchain_support_evidence()
    validate_platform_toolchain_support_evidence(
        support_evidence,
        boundary=boundary,
        supported_platforms=supported_platforms,
        tier_policy=tier_policy,
        unsupported_host_policy=unsupported_host_policy,
    )
    host = current_host()
    supported_platform_ids = list(boundary["supported_platform_ids"])
    payload = {
        "contract_id": "objc3c.platform.hardening.support.matrix.v1",
        "schema_version": 1,
        "generated_at_utc": utc_now(),
        "default_platform_id": supported_platforms["default_platform_id"],
        "matrix_dimensions": supported_platforms["matrix_dimensions"],
        "platform_count": len(supported_platforms["supported_platforms"]),
        "platforms": supported_platforms["supported_platforms"],
        "packaged_runtime_acceptance": supported_platforms["packaged_runtime_acceptance"],
        "channels": boundary["supported_channels"],
        "tiers": tier_policy["tiers"],
        "current_host": host.as_json(),
        "required_tool_probes": required_tool_probes(),
        "claim_boundary": {
            "supported_platform_ids": supported_platform_ids,
            "gap_claims": boundary["gap_claims"],
            "forbidden_claims": tier_policy["forbidden_claims"],
        },
        "unsupported_host_fail_closed": build_unsupported_host_fail_closed_payload(
            unsupported_host_policy,
            default_platform_id=str(supported_platforms["default_platform_id"]),
            supported_platform_ids=supported_platform_ids,
        ),
        "publication_surface": PUBLICATION_SURFACE,
    }
    payload.update(build_support_evidence_matrix_sections(support_evidence))
    return payload


def platform_hardening_owner_payload() -> dict[str, object]:
    return dict(PLATFORM_HARDENING_OWNER_POLICY)


def policy_path_entries() -> list[Path]:
    boundary = load_json_object(BOUNDARY_INVENTORY_PATH)
    return [resolve_repo_path(str(path)) for path in boundary["policy_contract_paths"]]


__all__ = [
    "build_unsupported_host_fail_closed_payload",
    "build_support_matrix_payload",
    "load_json_object",
    "platform_hardening_owner_payload",
    "policy_path_entries",
]
