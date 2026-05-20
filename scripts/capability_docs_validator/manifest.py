from __future__ import annotations

from typing import Any

from objc3c_tooling.paths import resolve_repo_path

from capability_docs_validator.errors import CapabilityDocsError

PUBLIC_OBJC3C_COMMAND_PREFIX = "npm run objc3c -- "


def _manifest_support_claims(manifest: dict[str, Any]) -> dict[str, dict[str, str]]:
    raw_fixtures = manifest.get("fixtures")
    if not isinstance(raw_fixtures, list):
        raise CapabilityDocsError("canonical manifest fixtures must be a list")
    fixture_paths: set[str] = set()
    for index, fixture in enumerate(raw_fixtures):
        if not isinstance(fixture, dict):
            raise CapabilityDocsError(f"canonical manifest fixtures[{index}] must be an object")
        raw_path = fixture.get("path")
        if not isinstance(raw_path, str) or not raw_path:
            raise CapabilityDocsError(f"canonical manifest fixtures[{index}].path must be non-empty")
        fixture_paths.add(raw_path)

    raw_claims = manifest.get("support_claims")
    if not isinstance(raw_claims, list) or not raw_claims:
        raise CapabilityDocsError("canonical manifest support_claims must be a non-empty list")

    claims: dict[str, dict[str, str]] = {}
    for index, claim in enumerate(raw_claims):
        if not isinstance(claim, dict):
            raise CapabilityDocsError(f"canonical manifest support_claims[{index}] must be an object")
        claim_id = claim.get("claim_id")
        owner_phase = claim.get("owner_phase")
        behavior_fixture = claim.get("behavior_fixture")
        executable_command = claim.get("executable_command")
        if not isinstance(claim_id, str) or not claim_id:
            raise CapabilityDocsError(f"canonical manifest support_claims[{index}].claim_id must be non-empty")
        if claim_id in claims:
            raise CapabilityDocsError(f"duplicate canonical manifest support claim: {claim_id}")
        if not isinstance(owner_phase, str) or not owner_phase:
            raise CapabilityDocsError(f"{claim_id} owner_phase must be non-empty")
        if not isinstance(behavior_fixture, str) or not behavior_fixture:
            raise CapabilityDocsError(f"{claim_id} behavior_fixture must be non-empty")
        if behavior_fixture not in fixture_paths:
            raise CapabilityDocsError(
                f"{claim_id} behavior fixture is not listed in canonical manifest fixtures: {behavior_fixture}"
            )
        if not resolve_repo_path(behavior_fixture).is_file():
            raise CapabilityDocsError(f"{claim_id} behavior fixture is missing: {behavior_fixture}")
        if (
            not isinstance(executable_command, str)
            or not executable_command.startswith(PUBLIC_OBJC3C_COMMAND_PREFIX)
            or executable_command == PUBLIC_OBJC3C_COMMAND_PREFIX
        ):
            raise CapabilityDocsError(
                f"{claim_id} must use executable command through "
                f"{PUBLIC_OBJC3C_COMMAND_PREFIX}<action>"
            )
        claims[claim_id] = {
            "claim_id": claim_id,
            "owner_phase": owner_phase,
            "behavior_fixture": behavior_fixture,
            "executable_command": executable_command,
        }
    return claims
