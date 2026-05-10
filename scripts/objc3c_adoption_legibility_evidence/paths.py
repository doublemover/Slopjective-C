from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from objc3c_tooling.paths import ROOT
from objc3c_tooling.subprocesses import python_script_command


CONTRACT_ID = "objc3c.adoption_legibility.evidence.v1"
SUMMARY_CONTRACT_ID = "objc3c.adoption_legibility.evidence.summary.v1"
SUPPORT_STATE = "evaluator-ready-with-same-major-adoption-replay-and-evidence-linked-comparison"
PACKAGE_BRIDGE = "objc3c"
PUBLIC_ACTIONS = [
    "validate-adoption-legibility",
    "publish-adoption-legibility",
]
OWNER_SPLIT = {
    "boundary_inventory": "tests/tooling/fixtures/adoption_legibility/boundary_inventory.json",
    "artifact_contract": "tests/tooling/fixtures/adoption_legibility/artifact_contract.json",
    "capability_comparison": "tests/tooling/fixtures/adoption_legibility/capability_comparison_semantics.json",
    "adoption_replay": "tests/tooling/fixtures/adoption_legibility/adoption_replay_semantics.json",
    "public_claim_policy": "tests/tooling/fixtures/adoption_legibility/public_claim_policy.json",
    "metadata_publication": "scripts/publish_objc3c_adoption_legibility_metadata.py",
}
OWNER_CONTRACTS = {
    "public_claim_owner": {
        "source_contract": OWNER_SPLIT["public_claim_policy"],
        "artifact_section": "candidate_claims",
        "publication_projection": "evaluator_publication.candidate_claim_classes",
        "blocker_projection": "claim_audit.blocker_metadata.public_claim_policy",
    },
    "adoption_comparison_owner": {
        "source_contract": OWNER_SPLIT["capability_comparison"],
        "artifact_section": "comparison_matrix",
        "publication_projection": "evaluator_publication.comparison_axes",
        "blocker_projection": "claim_audit.blocker_metadata.capability_comparison",
    },
    "adoption_replay_owner": {
        "source_contract": OWNER_SPLIT["adoption_replay"],
        "artifact_section": "adoption_replay",
        "publication_projection": "evaluator_publication.adoption_replay_phases",
        "blocker_projection": "claim_audit.blocker_metadata.adoption_replay",
    },
    "publication_owner": {
        "source_contract": OWNER_SPLIT["metadata_publication"],
        "artifact_section": "public_workflow",
        "publication_projection": "evaluator_publication",
        "blocker_projection": "claim_audit.blocker_metadata.metadata_publication",
    },
}
BLOCKER_METADATA = {
    "public_claim_policy": {
        "owner": "public_claim_owner",
        "blocked_when": "candidate claim omits support class, evidence paths, or forbidden-claim demotion",
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "capability_comparison": {
        "owner": "adoption_comparison_owner",
        "blocked_when": "comparison wording widens beyond conformance, performance, package, support, and checked-in example evidence",
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "adoption_replay": {
        "owner": "adoption_replay_owner",
        "blocked_when": "same-major adoption replay lacks replay fields, package bridge, revert target, or public workflow action",
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "metadata_publication": {
        "owner": "publication_owner",
        "blocked_when": "publication widens claims beyond generated adoption evidence",
        "release_blocker_field": "claim_audit.release_blockers",
    },
}


@dataclass(frozen=True)
class EvidenceStep:
    name: str
    command: list[str]


@dataclass(frozen=True)
class RequiredReport:
    name: str
    path: Path


@dataclass(frozen=True)
class AdoptionLegibilityEvidencePaths:
    root: Path
    artifact_path: Path
    publication_path: Path
    summary_path: Path
    boundary_contract: Path
    public_claim_policy: Path
    comparison_semantics: Path
    adoption_replay_semantics: Path
    artifact_contract: Path
    boundary_summary: Path
    public_claim_summary: Path
    comparison_summary: Path
    adoption_replay_summary: Path
    artifact_contract_summary: Path

    @classmethod
    def for_root(cls, root: Path = ROOT) -> "AdoptionLegibilityEvidencePaths":
        report_root = root / "tmp" / "reports" / "adoption-legibility"
        artifact_root = root / "tmp" / "artifacts" / "adoption-legibility"
        fixture_root = root / "tests" / "tooling" / "fixtures" / "adoption_legibility"
        return cls(
            root=root,
            artifact_path=artifact_root / "adoption-legibility-evidence.json",
            publication_path=artifact_root / "evaluator-publication.json",
            summary_path=report_root / "evidence-summary.json",
            boundary_contract=fixture_root / "boundary_inventory.json",
            public_claim_policy=fixture_root / "public_claim_policy.json",
            comparison_semantics=fixture_root / "capability_comparison_semantics.json",
            adoption_replay_semantics=fixture_root / "adoption_replay_semantics.json",
            artifact_contract=fixture_root / "artifact_contract.json",
            boundary_summary=report_root / "boundary-inventory-summary.json",
            public_claim_summary=report_root / "public-claim-policy-summary.json",
            comparison_summary=report_root / "capability-comparison-summary.json",
            adoption_replay_summary=report_root / "adoption-replay-summary.json",
            artifact_contract_summary=report_root / "artifact-contract-summary.json",
        )

    def steps(self) -> tuple[EvidenceStep, ...]:
        return (
            EvidenceStep("boundary-inventory", python_script_command("scripts/build_adoption_legibility_boundary_inventory_summary.py")),
            EvidenceStep("public-claim-policy", python_script_command("scripts/build_adoption_legibility_public_claim_policy_summary.py")),
            EvidenceStep("capability-comparison", python_script_command("scripts/build_adoption_legibility_capability_comparison_summary.py")),
            EvidenceStep("adoption-replay", python_script_command("scripts/build_adoption_legibility_adoption_replay_summary.py")),
            EvidenceStep("artifact-contract", python_script_command("scripts/build_adoption_legibility_artifact_contract_summary.py")),
        )

    def required_reports(self) -> tuple[RequiredReport, ...]:
        return (
            RequiredReport("boundary", self.boundary_summary),
            RequiredReport("public_claim", self.public_claim_summary),
            RequiredReport("comparison", self.comparison_summary),
            RequiredReport("adoption_replay", self.adoption_replay_summary),
            RequiredReport("artifact_contract", self.artifact_contract_summary),
        )

    def contracts(self) -> tuple[Path, ...]:
        return (
            self.boundary_contract,
            self.public_claim_policy,
            self.comparison_semantics,
            self.adoption_replay_semantics,
            self.artifact_contract,
        )
