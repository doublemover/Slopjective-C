"""Typed security-hardening workflow target catalog."""

from __future__ import annotations

import sys
from dataclasses import dataclass
from pathlib import Path

from ..action_spec import ActionSpec
from .release_governance_security_paths import (
    SECURITY_HARDENING_END_TO_END_PY,
    SECURITY_HARDENING_POSTURE_PY,
    SECURITY_HARDENING_PUBLICATION_PY,
    SECURITY_HARDENING_RESPONSE_DRILL_PY,
    SECURITY_HARDENING_RUNTIME_HARDENING_PY,
    SECURITY_HARDENING_SOURCE_SURFACE_PY,
)
from .schema_surfaces_paths import SECURITY_HARDENING_SCHEMA_SURFACE_PY

CHECK_SECURITY_HARDENING_SURFACE = "check-security-hardening-surface"
CHECK_SECURITY_HARDENING_SCHEMA_SURFACE = "check-security-hardening-schema-surface"
CHECK_SECURITY_RESPONSE_DRILL = "check-security-response-drill"
CHECK_SECURITY_RUNTIME_HARDENING = "check-security-runtime-hardening"
BUILD_SECURITY_POSTURE = "build-security-posture"
PUBLISH_SECURITY_ADVISORIES = "publish-security-advisories"
VALIDATE_SECURITY_HARDENING = "validate-security-hardening"
VALIDATE_SECURITY_HARDENING_END_TO_END = "validate-security-hardening-end-to-end"

SECURITY_HARDENING_HARD_CUTOVER_GUARDRAILS: tuple[tuple[str, object], ...] = (
    ("evidence_log_security_proof_allowed", False),
    ("generated_report_capability_truth_allowed", False),
    ("local_tabletop_capability_truth_allowed", False),
    ("fallback_claims_allowed", False),
    ("trust_bypass_claims_allowed", False),
    ("wrapper_only_security_actions_allowed", False),
    ("security_positive_claim_requires_source_contract", True),
    ("security_positive_claim_requires_runtime_or_release_evidence", True),
)


@dataclass(frozen=True)
class SecurityHardeningOwnerContract:
    owner_id: str
    owner: str
    source_contract: str
    claim_authority: str
    required_actions: tuple[str, ...]
    forbidden_claims: tuple[str, ...]

    def as_fixture_contract(self) -> dict[str, object]:
        return {
            "source_contract": self.source_contract,
            "owner": self.owner,
            "claim_authority": self.claim_authority,
            "required_actions": list(self.required_actions),
            "forbidden_claims": list(self.forbidden_claims),
        }


@dataclass(frozen=True)
class SecurityHardeningTarget:
    action_name: str
    summary: str
    backend: str
    guarantee_owner: str
    script: Path | None
    validation_tier: str
    owner_contract_ids: tuple[str, ...] = ()
    hard_cutover_guardrails: tuple[tuple[str, object], ...] = (
        SECURITY_HARDENING_HARD_CUTOVER_GUARDRAILS
    )

    def to_action_spec(self) -> ActionSpec:
        guardrails = dict(self.hard_cutover_guardrails)
        disallowed_flags = (
            "evidence_log_security_proof_allowed",
            "generated_report_capability_truth_allowed",
            "local_tabletop_capability_truth_allowed",
            "fallback_claims_allowed",
            "trust_bypass_claims_allowed",
            "wrapper_only_security_actions_allowed",
        )
        if any(guardrails.get(flag) for flag in disallowed_flags):
            raise ValueError(f"{self.action_name} cannot publish bypass security claims")
        owner_contracts = ", ".join(self.owner_contract_ids)
        guarantee_owner = self.guarantee_owner
        if owner_contracts:
            guarantee_owner = f"{owner_contracts}: {guarantee_owner}"
        return ActionSpec(
            self.action_name,
            self.summary,
            self.backend,
            validation_tier=self.validation_tier,
            guarantee_owner=guarantee_owner,
        )


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
            "fallback macro package trust",
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
            "fallback release publication after drill failure",
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
            "runtime fallback acceptance",
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
            "fallback update trust",
            "trust bypass for unsigned payload",
        ),
    ),
}


SECURITY_HARDENING_PUBLIC_TARGETS: dict[str, SecurityHardeningTarget] = {
    CHECK_SECURITY_HARDENING_SURFACE: SecurityHardeningTarget(
        CHECK_SECURITY_HARDENING_SURFACE,
        "validate the checked-in security-hardening source surface",
        "python:scripts/check_security_hardening_source_surface.py",
        (
            "security hardening only publishes from the checked-in trust boundary, "
            "policy, and workflow contracts"
        ),
        SECURITY_HARDENING_SOURCE_SURFACE_PY,
        "repo",
        SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    ),
    CHECK_SECURITY_HARDENING_SCHEMA_SURFACE: SecurityHardeningTarget(
        CHECK_SECURITY_HARDENING_SCHEMA_SURFACE,
        "validate the checked-in security-hardening schema surface",
        "python:scripts/check_security_hardening_schema_surface.py",
        "security posture and advisory artifacts stay on checked-in schema contracts",
        SECURITY_HARDENING_SCHEMA_SURFACE_PY,
        "repo",
        SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    ),
    BUILD_SECURITY_POSTURE: SecurityHardeningTarget(
        BUILD_SECURITY_POSTURE,
        "derive the machine-owned security posture from live hardening evidence",
        "python:scripts/build_objc3c_security_posture.py",
        (
            "security posture stays derived from the live trust boundary, release, "
            "and runtime evidence"
        ),
        SECURITY_HARDENING_POSTURE_PY,
        "repo",
        SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    ),
    PUBLISH_SECURITY_ADVISORIES: SecurityHardeningTarget(
        PUBLISH_SECURITY_ADVISORIES,
        "publish the machine-owned security advisory artifacts",
        "python:scripts/publish_objc3c_security_advisories.py",
        (
            "security advisory publication stays traceable to the live posture and "
            "checked-in hardening policies"
        ),
        SECURITY_HARDENING_PUBLICATION_PY,
        "repo",
        SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    ),
    VALIDATE_SECURITY_HARDENING: SecurityHardeningTarget(
        VALIDATE_SECURITY_HARDENING,
        "run the integrated security-hardening publication workflow",
        "runner-internal security-hardening child actions",
        (
            "security posture and advisory publication stay executable on the live "
            "release, trust, and hardening surfaces"
        ),
        None,
        "nightly",
        SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    ),
    VALIDATE_SECURITY_HARDENING_END_TO_END: SecurityHardeningTarget(
        VALIDATE_SECURITY_HARDENING_END_TO_END,
        (
            "validate security-hardening entrypoints, command-surface sync, and "
            "publication artifacts end to end"
        ),
        "python:scripts/check_objc3c_security_hardening_end_to_end.py",
        (
            "security-hardening entrypoints and publication artifacts stay coherent "
            "with the live command and evidence surfaces"
        ),
        SECURITY_HARDENING_END_TO_END_PY,
        "full",
        SECURITY_HARDENING_ALL_DOMAIN_OWNER_IDS,
    ),
}

SECURITY_HARDENING_INTERNAL_TARGETS: dict[str, SecurityHardeningTarget] = {
    CHECK_SECURITY_RESPONSE_DRILL: SecurityHardeningTarget(
        CHECK_SECURITY_RESPONSE_DRILL,
        "validate response-drill evidence before publishing hardening claims",
        "python:scripts/check_security_hardening_response_drill.py",
        (
            "response drill status stays tied to release, distribution, platform, "
            "and posture evidence"
        ),
        SECURITY_HARDENING_RESPONSE_DRILL_PY,
        "nightly",
        ("response_drill_owner",),
    ),
    CHECK_SECURITY_RUNTIME_HARDENING: SecurityHardeningTarget(
        CHECK_SECURITY_RUNTIME_HARDENING,
        "validate runtime hardening evidence before publishing hardening claims",
        "python:scripts/check_security_hardening_runtime_hardening.py",
        (
            "runtime hardening claims stay gated by runtime acceptance and runnable "
            "release-candidate evidence"
        ),
        SECURITY_HARDENING_RUNTIME_HARDENING_PY,
        "nightly",
        ("runtime_hardening_owner",),
    ),
}

SECURITY_HARDENING_ACTION_SPECS: dict[str, ActionSpec] = {
    action_name: target.to_action_spec()
    for action_name, target in SECURITY_HARDENING_PUBLIC_TARGETS.items()
}

SECURITY_HARDENING_VALIDATION_CHILD_ACTIONS = (
    CHECK_SECURITY_RESPONSE_DRILL,
    CHECK_SECURITY_RUNTIME_HARDENING,
    CHECK_SECURITY_HARDENING_SURFACE,
    CHECK_SECURITY_HARDENING_SCHEMA_SURFACE,
    BUILD_SECURITY_POSTURE,
    PUBLISH_SECURITY_ADVISORIES,
)

SECURITY_HARDENING_ACTION_OWNER_CONTRACT_IDS: dict[str, tuple[str, ...]] = {
    action_name: target.owner_contract_ids
    for action_name, target in (
        SECURITY_HARDENING_PUBLIC_TARGETS | SECURITY_HARDENING_INTERNAL_TARGETS
    ).items()
}


def security_hardening_hard_cutover_guardrails() -> dict[str, object]:
    return dict(SECURITY_HARDENING_HARD_CUTOVER_GUARDRAILS)


def security_hardening_domain_owner_contracts() -> dict[str, dict[str, object]]:
    return {
        owner_id: contract.as_fixture_contract()
        for owner_id, contract in SECURITY_HARDENING_DOMAIN_OWNER_CONTRACTS.items()
    }


def security_hardening_target(action_name: str) -> SecurityHardeningTarget:
    if action_name in SECURITY_HARDENING_PUBLIC_TARGETS:
        return SECURITY_HARDENING_PUBLIC_TARGETS[action_name]
    return SECURITY_HARDENING_INTERNAL_TARGETS[action_name]


def security_hardening_command(action_name: str) -> list[str]:
    target = security_hardening_target(action_name)
    if target.script is None:
        raise ValueError(f"{action_name} is a composite security-hardening action")
    return [sys.executable, str(target.script)]
