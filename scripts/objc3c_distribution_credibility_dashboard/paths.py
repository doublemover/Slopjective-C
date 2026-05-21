from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path

from objc3c_tooling.paths import ROOT


SUMMARY_CONTRACT_ID = "objc3c.distribution.credibility.dashboard.summary.v1"


@dataclass(frozen=True)
class DistributionCredibilityDashboardPaths:
    root: Path
    source_surface: Path
    trust_architecture: Path
    install_doc_surface: Path
    operator_policy: Path
    release_drill_policy: Path
    schema_surface: Path
    artifact_surface: Path
    workflow_surface: Path
    release_foundation_manifest: Path
    package_channels_end_to_end: Path
    package_install_distribution_summary: Path
    release_operations_publication: Path
    release_operations_end_to_end: Path
    release_evidence_index: Path
    release_evidence_check: Path
    output_path: Path

    @classmethod
    def for_root(cls, root: Path = ROOT) -> "DistributionCredibilityDashboardPaths":
        policy_root = root / "tests" / "tooling" / "fixtures" / "distribution_credibility"
        report_root = root / "tmp" / "reports"
        return cls(
            root=root,
            source_surface=policy_root / "source_surface.json",
            trust_architecture=policy_root / "trust_signal_architecture.json",
            install_doc_surface=policy_root / "install_release_doc_surface.json",
            operator_policy=policy_root / "operator_release_policy.json",
            release_drill_policy=policy_root / "release_drill_policy.json",
            schema_surface=policy_root / "schema_surface.json",
            artifact_surface=policy_root / "artifact_surface.json",
            workflow_surface=policy_root / "workflow_surface.json",
            release_foundation_manifest=(
                root
                / "tmp"
                / "artifacts"
                / "release-foundation"
                / "manifest"
                / "objc3c-release-manifest.json"
            ),
            package_channels_end_to_end=report_root / "package-channels" / "end-to-end-summary.json",
            package_install_distribution_summary=(
                report_root / "package-ecosystem" / "install-distribution-credibility-summary.json"
            ),
            release_operations_publication=report_root / "release-operations" / "publication-summary.json",
            release_operations_end_to_end=report_root / "release-operations" / "end-to-end-summary.json",
            release_evidence_index=report_root / "release_evidence" / "evidence-index.json",
            release_evidence_check=root / "scripts" / "check_release_evidence.py",
            output_path=report_root / "distribution-credibility" / "dashboard-summary.json",
        )
