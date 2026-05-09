from __future__ import annotations

from typing import Any

from capability_docs_validator.errors import CapabilityDocsError
from capability_docs_validator.manifest import _manifest_support_claims


def _row_support_claims(row: dict[str, Any]) -> list[str]:
    capability_id = str(row["id"])
    raw_claims = row.get("support_claims", [])
    if not isinstance(raw_claims, list):
        raise CapabilityDocsError(f"{capability_id} support_claims must be a list")
    claims: list[str] = []
    seen: set[str] = set()
    for index, raw_claim in enumerate(raw_claims):
        if not isinstance(raw_claim, str) or not raw_claim:
            raise CapabilityDocsError(f"{capability_id} support_claims[{index}] must be non-empty")
        if raw_claim in seen:
            raise CapabilityDocsError(f"{capability_id} duplicates support claim {raw_claim}")
        seen.add(raw_claim)
        claims.append(raw_claim)
    return claims


def _validate_support_claim_links(rows: list[dict[str, Any]], manifest: dict[str, Any]) -> None:
    manifest_claims = _manifest_support_claims(manifest)
    documented_by_claim: dict[str, str] = {}

    for row in rows:
        capability_id = str(row["id"])
        state = str(row["state"])
        support_claims = _row_support_claims(row)
        if state == "implemented" and not support_claims:
            raise CapabilityDocsError(f"{capability_id} implemented rows must link canonical support_claims")
        if state != "implemented" and support_claims:
            raise CapabilityDocsError(f"{capability_id} support_claims are only allowed on implemented rows")

        evidence = row["evidence"]
        test_evidence = {
            (str(item.get("path")), str(item.get("command")))
            for item in evidence
            if isinstance(item, dict) and item.get("kind") == "test"
        }
        for claim_id in support_claims:
            if claim_id not in manifest_claims:
                raise CapabilityDocsError(f"{capability_id} references unknown support claim: {claim_id}")
            previous_capability = documented_by_claim.get(claim_id)
            if previous_capability is not None:
                raise CapabilityDocsError(
                    f"support claim {claim_id} is linked by both {previous_capability} and {capability_id}"
                )
            claim = manifest_claims[claim_id]
            expected_evidence = (claim["behavior_fixture"], claim["executable_command"])
            if expected_evidence not in test_evidence:
                raise CapabilityDocsError(
                    f"{capability_id} support claim {claim_id} must include executable evidence "
                    f"{claim['behavior_fixture']} via {claim['executable_command']}"
                )
            documented_by_claim[claim_id] = capability_id

    missing_claims = sorted(set(manifest_claims) - set(documented_by_claim))
    if missing_claims:
        raise CapabilityDocsError(
            "canonical manifest support claims missing from capability matrix: "
            + ", ".join(missing_claims)
        )
