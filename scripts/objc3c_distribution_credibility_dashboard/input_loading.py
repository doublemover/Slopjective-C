from __future__ import annotations

from dataclasses import dataclass
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import load_json_object as load_json
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.subprocesses import python_script_command, run_capture

from objc3c_distribution_credibility_dashboard.paths import DistributionCredibilityDashboardPaths


@dataclass(frozen=True)
class DistributionCredibilityDashboardInputs:
    source_surface: dict[str, Any]
    trust_architecture: dict[str, Any]
    install_doc_surface: dict[str, Any]
    operator_policy: dict[str, Any]
    release_drill_policy: dict[str, Any]
    schema_surface: dict[str, Any]
    artifact_surface: dict[str, Any]
    workflow_surface: dict[str, Any]
    release_manifest: dict[str, Any]
    package_channels: dict[str, Any]
    release_operations_publication: dict[str, Any]
    release_operations_end_to_end: dict[str, Any]
    release_evidence: dict[str, Any]
    package_install_distribution: dict[str, Any]


def require_json(path: Path, *, kind: str) -> dict[str, Any]:
    if not path.is_file():
        raise RuntimeError(f"missing {kind}: {repo_rel(path)}")
    return load_json(path)


def ensure_release_evidence_index(paths: DistributionCredibilityDashboardPaths) -> dict[str, Any]:
    if not paths.release_evidence_index.is_file():
        result = run_capture(python_script_command(paths.release_evidence_check))
        if result.returncode != 0:
            raise RuntimeError("failed to generate release-evidence index")
    return require_json(paths.release_evidence_index, kind="release-evidence index")


def load_dashboard_inputs(
    paths: DistributionCredibilityDashboardPaths,
) -> DistributionCredibilityDashboardInputs:
    return DistributionCredibilityDashboardInputs(
        source_surface=require_json(paths.source_surface, kind="source surface"),
        trust_architecture=require_json(paths.trust_architecture, kind="trust architecture"),
        install_doc_surface=require_json(paths.install_doc_surface, kind="install doc surface"),
        operator_policy=require_json(paths.operator_policy, kind="operator policy"),
        release_drill_policy=require_json(paths.release_drill_policy, kind="release drill policy"),
        schema_surface=require_json(paths.schema_surface, kind="schema surface"),
        artifact_surface=require_json(paths.artifact_surface, kind="artifact surface"),
        workflow_surface=require_json(paths.workflow_surface, kind="workflow surface"),
        release_manifest=require_json(paths.release_foundation_manifest, kind="release-foundation manifest"),
        package_channels=require_json(paths.package_channels_end_to_end, kind="package-channels end-to-end summary"),
        release_operations_publication=require_json(
            paths.release_operations_publication,
            kind="release-operations publication summary",
        ),
        release_operations_end_to_end=require_json(
            paths.release_operations_end_to_end,
            kind="release-operations end-to-end summary",
        ),
        release_evidence=ensure_release_evidence_index(paths),
        package_install_distribution=require_json(
            paths.package_install_distribution_summary,
            kind="package install distribution summary",
        ),
    )
