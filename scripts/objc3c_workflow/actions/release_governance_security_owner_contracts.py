"""Security-hardening domain owner contracts."""

from __future__ import annotations

from .release_governance_security_names import (
    BUILD_SECURITY_POSTURE,
    CHECK_SECURITY_HARDENING_SCHEMA_SURFACE,
    CHECK_SECURITY_HARDENING_SURFACE,
    CHECK_SECURITY_RESPONSE_DRILL,
    CHECK_SECURITY_RUNTIME_HARDENING,
    PUBLISH_SECURITY_ADVISORIES,
    VALIDATE_SECURITY_HARDENING,
    VALIDATE_SECURITY_HARDENING_END_TO_END,
)
from .release_governance_security_target_models import SecurityHardeningOwnerContract

SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS = (
    "macro_provenance_owner",
    "response_drill_owner",
    "runtime_hardening_owner",
    "installer_update_key_owner",
)

SECURITY_HARDENING_DOMAIN_OWNER_CONTRACTS: dict[str, SecurityHardeningOwnerContract] = {
    "macro_provenance_owner": SecurityHardeningOwnerContract(
        owner_id="macro_provenance_owner",
        owner="security-hardening-macro-provenance",
        source_contract=(
            "tests/tooling/fixtures/security_hardening/"
            "macro_package_provenance_trust_policy.json"
        ),
        claim_authority=(
            "checked-in macro metadata, package identity, provenance, and runtime "
            "acceptance evidence"
        ),
        required_actions=(
            CHECK_SECURITY_HARDENING_SURFACE,
            CHECK_SECURITY_HARDENING_SCHEMA_SURFACE,
            BUILD_SECURITY_POSTURE,
        ),
        forbidden_claims=(
            "evidence-log macro trust proof",
            "retired route macro package trust",
            "trust bypass for missing provenance",
            "wrapper-only macro security action",
        ),
    ),
    "response_drill_owner": SecurityHardeningOwnerContract(
        owner_id="response_drill_owner",
        owner="security-hardening-response-drill",
        source_contract="tests/tooling/fixtures/security_hardening/response_drill_contract.json",
        claim_authority=(
            "release, distribution, platform, and posture evidence linked by the "
            "drill contract"
        ),
        required_actions=(
            CHECK_SECURITY_RESPONSE_DRILL,
            VALIDATE_SECURITY_HARDENING,
        ),
        forbidden_claims=(
            "local-only tabletop as capability truth",
            "evidence-log response readiness proof",
            "unowned disclosure override",
            "retired route release publication after drill failure",
        ),
    ),
    "runtime_hardening_owner": SecurityHardeningOwnerContract(
        owner_id="runtime_hardening_owner",
        owner="security-hardening-runtime",
        source_contract="tests/tooling/fixtures/security_hardening/runtime_hardening_contract.json",
        claim_authority="runtime acceptance and runnable release-candidate evidence",
        required_actions=(
            CHECK_SECURITY_RUNTIME_HARDENING,
            VALIDATE_SECURITY_HARDENING,
        ),
        forbidden_claims=(
            "runtime hardening without runnable evidence",
            "runtime retired route acceptance",
            "trust bypass for missing runtime case",
            "wrapper-only runtime security action",
        ),
    ),
    "installer_update_key_owner": SecurityHardeningOwnerContract(
        owner_id="installer_update_key_owner",
        owner="security-hardening-installer-update-key",
        source_contract=(
            "tests/tooling/fixtures/security_hardening/"
            "installer_update_release_key_hardening_policy.json"
        ),
        claim_authority=(
            "release manifest, provenance, update manifest, package channel, and "
            "distribution trust evidence"
        ),
        required_actions=(
            BUILD_SECURITY_POSTURE,
            PUBLISH_SECURITY_ADVISORIES,
            VALIDATE_SECURITY_HARDENING_END_TO_END,
        ),
        forbidden_claims=(
            "remote key custody",
            "automatic key rotation",
            "hosted revocation",
            "signed-installer trust",
            "retired route update trust",
            "trust bypass for unsigned payload",
        ),
    ),
}


def security_hardening_domain_owner_contracts() -> dict[str, dict[str, object]]:
    return {
        owner_id: contract.as_fixture_contract()
        for owner_id, contract in SECURITY_HARDENING_DOMAIN_OWNER_CONTRACTS.items()
    }


__all__ = [
    "SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS",
    "SECURITY_HARDENING_DOMAIN_OWNER_CONTRACTS",
    "security_hardening_domain_owner_contracts",
]
