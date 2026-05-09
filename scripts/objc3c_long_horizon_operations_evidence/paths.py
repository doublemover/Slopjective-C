from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from objc3c_tooling.paths import ROOT
from objc3c_tooling.subprocesses import python_script_command


ARTIFACT_CONTRACT_ID = "objc3c.long_horizon_operations.evidence.v1"
SUMMARY_CONTRACT_ID = "objc3c.long_horizon_operations.evidence.summary.v1"
OWNER_SPLIT = {
    "boundary_inventory": "tests/tooling/fixtures/long_horizon_operations/boundary_inventory.json",
    "artifact_contract": "tests/tooling/fixtures/long_horizon_operations/artifact_contract.json",
    "deprecation_support_policy": "tests/tooling/fixtures/long_horizon_operations/deprecation_compatibility_policy.json",
    "conversion_replay_revert_support_window": "tests/tooling/fixtures/long_horizon_operations/migration_rollback_support_window_semantics.json",
    "aging_release_cadence": "tests/tooling/fixtures/long_horizon_operations/aging_regression_release_cadence_criteria.json",
    "metadata_publication": "scripts/publish_objc3c_long_horizon_operations_metadata.py",
}
OWNER_CONTRACTS = {
    "deprecation_owner": {
        "source_contract": OWNER_SPLIT["deprecation_support_policy"],
        "artifact_section": "claim_audit",
        "publication_projection": "operator_publication.support_window_summary",
        "blocker_projection": "claim_audit.blocker_metadata.deprecation_support_policy",
    },
    "revert_owner": {
        "source_contract": OWNER_SPLIT["conversion_replay_revert_support_window"],
        "artifact_section": "revert_readiness",
        "publication_projection": "operator_publication.revert_channels",
        "blocker_projection": "claim_audit.blocker_metadata.revert_readiness",
    },
    "cadence_owner": {
        "source_contract": OWNER_SPLIT["aging_release_cadence"],
        "artifact_section": "aging_regression",
        "publication_projection": "operator_publication.support_window_summary",
        "blocker_projection": "claim_audit.blocker_metadata.aging_release_cadence",
    },
    "publication_owner": {
        "source_contract": OWNER_SPLIT["metadata_publication"],
        "artifact_section": "claim_audit",
        "publication_projection": "operator_publication",
        "blocker_projection": "claim_audit.blocker_metadata.metadata_publication",
    },
}
BLOCKER_METADATA = {
    "deprecation_support_policy": {
        "owner": "deprecation_owner",
        "blocked_when": "advertised support lacks current support-window authority or generated replay evidence",
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "revert_readiness": {
        "owner": "revert_owner",
        "blocked_when": "advertised channel lacks generated revert guidance or transport",
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "aging_release_cadence": {
        "owner": "cadence_owner",
        "blocked_when": "cadence claim lacks package, application, soak, freshness, or revert evidence",
        "release_blocker_field": "claim_audit.release_blockers",
    },
    "metadata_publication": {
        "owner": "publication_owner",
        "blocked_when": "operator publication widens support claims beyond generated long-horizon evidence",
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
class LongHorizonEvidencePaths:
    root: Path
    artifact_path: Path
    summary_path: Path
    boundary_summary: Path
    deprecation_summary: Path
    conversion_summary: Path
    aging_summary: Path
    artifact_contract_summary: Path
    update_manifest: Path
    upgrade_support_report: Path
    package_integration: Path
    package_lock_summary: Path
    application_architecture_integration: Path
    performance_governance_integration: Path
    conformance_corpus_integration: Path
    stress_integration: Path
    external_validation_integration: Path
    public_conformance_integration: Path

    @classmethod
    def for_root(cls, root: Path = ROOT) -> "LongHorizonEvidencePaths":
        reports = root / "tmp" / "reports"
        artifacts = root / "tmp" / "artifacts"
        long_horizon_reports = reports / "long-horizon-operations"
        return cls(
            root=root,
            artifact_path=artifacts / "long-horizon-operations" / "long-horizon-operations-evidence.json",
            summary_path=long_horizon_reports / "evidence-summary.json",
            boundary_summary=long_horizon_reports / "boundary-inventory-summary.json",
            deprecation_summary=long_horizon_reports / "deprecation-support-policy-summary.json",
            conversion_summary=long_horizon_reports / "conversion-replay-revert-support-window-summary.json",
            aging_summary=long_horizon_reports / "aging-regression-release-cadence-summary.json",
            artifact_contract_summary=long_horizon_reports / "artifact-contract-summary.json",
            update_manifest=artifacts / "release-operations" / "update-manifest" / "objc3c-update-manifest.json",
            upgrade_support_report=artifacts
            / "release-operations"
            / "publication"
            / "objc3c-upgrade-support-report.json",
            package_integration=reports / "package-ecosystem" / "integration-summary.json",
            package_lock_summary=reports / "package-ecosystem" / "package-lock-summary.json",
            application_architecture_integration=reports
            / "application-architecture-testing"
            / "runnable-template-canonical-app-summary.json",
            performance_governance_integration=reports / "performance-governance" / "integration-summary.json",
            conformance_corpus_integration=reports / "conformance" / "corpus-integration-summary.json",
            stress_integration=reports / "stress" / "integration-summary.json",
            external_validation_integration=reports / "external-validation" / "integration-summary.json",
            public_conformance_integration=reports / "public-conformance" / "integration-summary.json",
        )

    def steps(self) -> tuple[EvidenceStep, ...]:
        return (
            EvidenceStep("boundary-inventory", python_script_command("scripts/build_long_horizon_operations_boundary_inventory_summary.py")),
            EvidenceStep("deprecation-policy", python_script_command("scripts/build_long_horizon_operations_deprecation_policy_summary.py")),
            EvidenceStep("conversion-replay-revert-support-window", python_script_command("scripts/build_long_horizon_operations_migration_rollback_summary.py")),
            EvidenceStep("aging-cadence", python_script_command("scripts/build_long_horizon_operations_aging_cadence_summary.py")),
            EvidenceStep("artifact-contract", python_script_command("scripts/build_long_horizon_operations_artifact_contract_summary.py")),
            EvidenceStep("package-ecosystem-integration", python_script_command("scripts/check_objc3c_package_ecosystem_integration.py")),
            EvidenceStep("application-architecture-integration", python_script_command("scripts/check_objc3c_application_architecture_integration.py")),
            EvidenceStep("performance-governance-integration", python_script_command("scripts/check_objc3c_performance_governance_integration.py")),
            EvidenceStep("conformance-corpus-integration", python_script_command("scripts/check_objc3c_conformance_corpus_integration.py")),
            EvidenceStep("stress-integration", python_script_command("scripts/check_objc3c_stress_integration.py")),
            EvidenceStep("external-validation-integration", python_script_command("scripts/check_objc3c_external_validation_integration.py")),
            EvidenceStep("public-conformance-integration", python_script_command("scripts/check_objc3c_public_conformance_reporting_integration.py")),
        )

    def required_reports(self) -> tuple[RequiredReport, ...]:
        return (
            RequiredReport("boundary", self.boundary_summary),
            RequiredReport("deprecation", self.deprecation_summary),
            RequiredReport("conversion", self.conversion_summary),
            RequiredReport("aging", self.aging_summary),
            RequiredReport("artifact_contract", self.artifact_contract_summary),
            RequiredReport("package", self.package_integration),
            RequiredReport("application_architecture", self.application_architecture_integration),
            RequiredReport("performance_governance", self.performance_governance_integration),
            RequiredReport("conformance_corpus", self.conformance_corpus_integration),
            RequiredReport("stress", self.stress_integration),
            RequiredReport("external_validation", self.external_validation_integration),
            RequiredReport("public_conformance", self.public_conformance_integration),
        )

    def soak_report_paths(self) -> tuple[Path, ...]:
        return (
            self.conformance_corpus_integration,
            self.stress_integration,
            self.external_validation_integration,
            self.public_conformance_integration,
        )

    def package_and_app_evidence_paths(self) -> tuple[Path, ...]:
        return (
            self.package_integration,
            self.package_lock_summary,
            self.application_architecture_integration,
        )
