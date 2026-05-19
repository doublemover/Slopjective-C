"""Contract payload helpers for platform hardening."""

from __future__ import annotations

import json
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel, resolve_repo_path

from .constants import (
    BOUNDARY_INVENTORY_PATH,
    PLATFORM_HARDENING_OWNER_POLICY,
    PUBLICATION_SURFACE,
    SUPPORTED_PLATFORMS_PATH,
    SUPPORT_TIER_POLICY_PATH,
)
from .probes import current_host, required_tool_probes
from .reporting import utc_now
from .validation import expect


def load_json_object(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    expect(isinstance(payload, dict), f"{repo_rel(path)} did not contain a JSON object")
    return payload


def build_support_matrix_payload() -> dict[str, Any]:
    boundary = load_json_object(BOUNDARY_INVENTORY_PATH)
    tier_policy = load_json_object(SUPPORT_TIER_POLICY_PATH)
    supported_platforms = load_json_object(SUPPORTED_PLATFORMS_PATH)
    host = current_host()
    payload = {
        "contract_id": "objc3c.platform.hardening.support.matrix.v1",
        "schema_version": 1,
        "generated_at_utc": utc_now(),
        "default_platform_id": supported_platforms["default_platform_id"],
        "platform_count": len(supported_platforms["supported_platforms"]),
        "platforms": supported_platforms["supported_platforms"],
        "channels": boundary["supported_channels"],
        "tiers": tier_policy["tiers"],
        "current_host": host.as_json(),
        "required_tool_probes": required_tool_probes(),
        "claim_boundary": {
            "supported_platform_ids": boundary["supported_platform_ids"],
            "gap_claims": boundary["gap_claims"],
            "forbidden_claims": tier_policy["forbidden_claims"],
        },
        "publication_surface": PUBLICATION_SURFACE,
    }
    return payload


def platform_hardening_owner_payload() -> dict[str, object]:
    return dict(PLATFORM_HARDENING_OWNER_POLICY)


def policy_path_entries() -> list[Path]:
    boundary = load_json_object(BOUNDARY_INVENTORY_PATH)
    return [resolve_repo_path(str(path)) for path in boundary["policy_contract_paths"]]


__all__ = [
    "build_support_matrix_payload",
    "load_json_object",
    "platform_hardening_owner_payload",
    "policy_path_entries",
]
