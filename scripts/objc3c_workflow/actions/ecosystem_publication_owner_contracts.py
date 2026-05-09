"""Owner contracts for ecosystem publication workflow actions."""

from __future__ import annotations

from dataclasses import dataclass


@dataclass(frozen=True)
class EcosystemPublicationOwnerContract:
    action_name: str
    feed_name: str
    owner_role: str
    source_contracts: tuple[str, ...]
    evidence_contracts: tuple[str, ...]
    claim_boundary: str
    forbidden_claims: tuple[str, ...]
    report_only_allowed: bool = False
    wrapper_only_allowed: bool = False
    retired_source_acceptance_claim_allowed: bool = False
    retired_surface_acceptance_claim_allowed: bool = False


PACKAGE_SOURCE_CONTRACTS = (
    "tests/tooling/fixtures/package_ecosystem/boundary_inventory.json",
    "tests/tooling/fixtures/package_ecosystem/artifact_contract.json",
    "tests/tooling/fixtures/package_ecosystem/dependency_lock_policy.json",
    "tests/tooling/fixtures/package_ecosystem/local_workspace_mirror_semantics.json",
    "tests/tooling/fixtures/package_ecosystem/registry_publication_semantics.json",
)
PACKAGE_FORBIDDEN_CLAIMS = (
    "hosted registry availability",
    "package-manager parity",
    "network-backed alternate install path",
    "generated artifact as source authority",
    "wrapper-only package publication",
)
ADOPTION_SOURCE_CONTRACTS = (
    "tests/tooling/fixtures/adoption_legibility/boundary_inventory.json",
    "tests/tooling/fixtures/adoption_legibility/artifact_contract.json",
    "tests/tooling/fixtures/adoption_legibility/public_claim_policy.json",
    "tests/tooling/fixtures/adoption_legibility/capability_comparison_semantics.json",
    "tests/tooling/fixtures/adoption_legibility/adoption_replay_semantics.json",
)
ADOPTION_FORBIDDEN_CLAIMS = (
    "drop-in Objective-C 2 retired source acceptance",
    "automatic conversion",
    "retired source acceptance path",
    "manual adoption metric",
    "publication wider than checked-in evidence",
)


def _package_contract(action_name: str, owner_role: str, claim_boundary: str) -> EcosystemPublicationOwnerContract:
    return EcosystemPublicationOwnerContract(
        action_name=action_name,
        feed_name="package-ecosystem",
        owner_role=owner_role,
        source_contracts=PACKAGE_SOURCE_CONTRACTS,
        evidence_contracts=(
            "objc3c.package_ecosystem.artifact_contract.v1",
            "objc3c.package_ecosystem.registry_publication_semantics.v1",
        ),
        claim_boundary=claim_boundary,
        forbidden_claims=PACKAGE_FORBIDDEN_CLAIMS,
    )


def _adoption_contract(action_name: str, claim_boundary: str) -> EcosystemPublicationOwnerContract:
    return EcosystemPublicationOwnerContract(
        action_name=action_name,
        feed_name="adoption-legibility",
        owner_role="adoption-legibility-owner",
        source_contracts=ADOPTION_SOURCE_CONTRACTS,
        evidence_contracts=(
            "objc3c.adoption_legibility.artifact_contract.v1",
            "objc3c.adoption_legibility.public_claim_policy.v1",
            "objc3c.adoption_legibility.adoption_replay_semantics.v1",
        ),
        claim_boundary=claim_boundary,
        forbidden_claims=ADOPTION_FORBIDDEN_CLAIMS,
    )


ECOSYSTEM_PUBLICATION_OWNER_CONTRACTS: dict[str, EcosystemPublicationOwnerContract] = {
    "build-package-lock": _package_contract(
        "build-package-lock",
        "package-ecosystem-lock-owner",
        "builds deterministic local lock evidence; it does not publish or imply hosted registry support",
    ),
    "validate-package-authoring": _package_contract(
        "validate-package-authoring",
        "package-ecosystem-authoring-owner",
        "validates checked-in package authoring contracts before any package publication claim",
    ),
    "validate-package-mirror": _package_contract(
        "validate-package-mirror",
        "package-ecosystem-mirror-owner",
        "proves local offline mirror reproducibility without network-dependent install claims",
    ),
    "validate-package-ecosystem": _package_contract(
        "validate-package-ecosystem",
        "package-ecosystem-registry-owner",
        "integrates lock, mirror, and registry metadata as local generated evidence only",
    ),
    "validate-runnable-package-ecosystem": _package_contract(
        "validate-runnable-package-ecosystem",
        "package-ecosystem-runnable-owner",
        "keeps package ecosystem claims attached to runnable local workflow evidence",
    ),
    "validate-adoption-legibility": _adoption_contract(
        "validate-adoption-legibility",
        "validates evaluator adoption evidence without widening into retired source acceptance promises",
    ),
    "publish-adoption-legibility": _adoption_contract(
        "publish-adoption-legibility",
        "publishes evaluator adoption metadata only when every public claim projects from checked-in source contracts",
    ),
}


def ecosystem_publication_owner_contract(action_name: str) -> EcosystemPublicationOwnerContract:
    return ECOSYSTEM_PUBLICATION_OWNER_CONTRACTS[action_name]


def ecosystem_publication_owner_contracts_by_feed(feed_name: str) -> tuple[EcosystemPublicationOwnerContract, ...]:
    return tuple(
        contract
        for contract in ECOSYSTEM_PUBLICATION_OWNER_CONTRACTS.values()
        if contract.feed_name == feed_name
    )


def require_ecosystem_publication_owner_contract(action_name: str) -> EcosystemPublicationOwnerContract:
    contract = ecosystem_publication_owner_contract(action_name)
    if not contract.source_contracts:
        raise RuntimeError(f"{action_name} has no source owner contract")
    if not contract.evidence_contracts:
        raise RuntimeError(f"{action_name} has no evidence contract")
    if contract.report_only_allowed:
        raise RuntimeError(f"{action_name} is report-only")
    if contract.wrapper_only_allowed:
        raise RuntimeError(f"{action_name} is wrapper-only")
    if contract.retired_source_acceptance_claim_allowed:
        raise RuntimeError(f"{action_name} allows retired source acceptance overclaims")
    if contract.retired_surface_acceptance_claim_allowed:
        raise RuntimeError(f"{action_name} allows retired source acceptance claims")
    return contract


__all__ = [
    "ADOPTION_FORBIDDEN_CLAIMS",
    "ADOPTION_SOURCE_CONTRACTS",
    "ECOSYSTEM_PUBLICATION_OWNER_CONTRACTS",
    "EcosystemPublicationOwnerContract",
    "PACKAGE_FORBIDDEN_CLAIMS",
    "PACKAGE_SOURCE_CONTRACTS",
    "ecosystem_publication_owner_contract",
    "ecosystem_publication_owner_contracts_by_feed",
    "require_ecosystem_publication_owner_contract",
]
