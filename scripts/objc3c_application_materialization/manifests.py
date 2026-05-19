"""Manifest payload builders for application and stdlib materializations."""

from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import display_path, repo_rel

from .contract_models import (
    CANONICAL_WORKSPACE_CONTRACT_ID,
    CANONICAL_WORKSPACE_SUMMARY_CONTRACT_ID,
    PROJECT_TEMPLATE_CONTRACT_ID,
    PROJECT_TEMPLATE_HARNESS_CONTRACT_ID,
    STDLIB_WORKSPACE_SUMMARY_CONTRACT_ID,
    CanonicalWorkspaceMaterialization,
    ProjectTemplatePaths,
    StdlibWorkspaceMaterialization,
)


def canonical_workspace_manifest_payload(
    *,
    root: Path,
    contract: dict[str, object],
    contract_path: Path,
    portfolio_path: Path,
    stdlib_program_surface_path: Path,
    materialization: CanonicalWorkspaceMaterialization,
) -> dict[str, object]:
    return {
        "contract_id": CANONICAL_WORKSPACE_CONTRACT_ID,
        "schema_version": 1,
        "canonical_application_architecture_contract": repo_rel(contract_path, root=root),
        "portfolio_manifest": repo_rel(portfolio_path, root=root),
        "stdlib_program_surface": repo_rel(stdlib_program_surface_path, root=root),
        "workspace_root": repo_rel(materialization.output_root, root=root),
        "readme": repo_rel(materialization.readme, root=root),
        "architecture_layers": contract["architecture_layers"],
        "included_examples": materialization.included_examples,
        "public_actions": contract["required_evidence_actions"],
        "package_bridge": "objc3c",
        "copied_roots": [
            "examples",
            "stdlib",
        ],
    }


def canonical_workspace_summary_payload(
    *,
    root: Path,
    contract: dict[str, object],
    contract_path: Path,
    stdlib_program_surface: dict[str, object],
    materialization: CanonicalWorkspaceMaterialization,
) -> dict[str, object]:
    return {
        "contract_id": CANONICAL_WORKSPACE_SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "workspace_contract_id": CANONICAL_WORKSPACE_CONTRACT_ID,
        "canonical_application_architecture_contract": repo_rel(contract_path, root=root),
        "workspace_root": repo_rel(materialization.output_root, root=root),
        "workspace_manifest": repo_rel(materialization.workspace_manifest, root=root),
        "readme": repo_rel(materialization.readme, root=root),
        "example_count": len(materialization.included_examples),
        "architecture_layer_count": len(contract["architecture_layers"]),
        "copied_path_count": len(materialization.copied_paths),
        "included_examples": materialization.included_examples,
        "public_actions": contract["required_evidence_actions"],
        "package_bridge": "objc3c",
        "stdlib_publish_model": stdlib_program_surface.get("publish_model"),
    }


def project_template_manifest_payload(
    *,
    root: Path,
    example_id: str,
    example_record: dict[str, Any],
    paths: ProjectTemplatePaths,
    application_architecture_contracts: dict[str, Path],
) -> dict[str, object]:
    return {
        "contract_id": PROJECT_TEMPLATE_CONTRACT_ID,
        "schema_version": 1,
        "example_id": example_id,
        "source_origin": str(example_record["source"]),
        "template_root": display_path(paths.template_root, root=root),
        "template_source": display_path(paths.template_source, root=root),
        "template_readme": display_path(paths.template_readme, root=root),
        "tutorial_guides": [
            "docs/tutorials/getting_started.md",
            "docs/tutorials/build_run_verify.md",
            "docs/tutorials/guided_walkthrough.md",
        ],
        "application_architecture_testing_contracts": {
            "first_party_testing": display_path(
                application_architecture_contracts["first_party_testing"], root=root
            ),
            "project_template_workspace": display_path(
                application_architecture_contracts["project_template_workspace"], root=root
            ),
            "canonical_application_architecture": display_path(
                application_architecture_contracts[
                    "canonical_application_architecture"
                ],
                root=root,
            ),
        },
        "public_actions": [
            "materialize-project-template",
            "materialize-playground-workspace",
            "benchmark-runtime-inspector",
            "inspect-bonus-tool-integration",
        ],
        "recommended_validation_actions": [
            "validate-showcase",
            "validate-runnable-showcase",
            "validate-stdlib-program",
            "validate-runnable-stdlib-program",
        ],
    }


def project_harness_payload(
    *,
    root: Path,
    failures: list[str],
    paths: ProjectTemplatePaths,
    integration_report: str,
    playground_workspace: str,
    benchmark_report: str,
) -> dict[str, object]:
    return {
        "contract_id": PROJECT_TEMPLATE_HARNESS_CONTRACT_ID,
        "schema_version": 1,
        "ok": not failures,
        "failures": failures,
        "template_contract": display_path(paths.template_manifest, root=root),
        "template_source": display_path(paths.template_source, root=root),
        "integration_report": integration_report,
        "playground_workspace": playground_workspace,
        "benchmark_report": benchmark_report,
        "public_actions": [
            "materialize-project-template",
            "inspect-bonus-tool-integration",
            "materialize-playground-workspace",
            "benchmark-runtime-inspector",
        ],
    }


def stdlib_workspace_summary_payload(
    *,
    root: Path,
    workspace_path: Path,
    workspace: dict[str, Any],
    materialization: StdlibWorkspaceMaterialization,
) -> dict[str, object]:
    unique_copied_paths = sorted(dict.fromkeys(materialization.copied_paths))
    return {
        "contract_id": STDLIB_WORKSPACE_SUMMARY_CONTRACT_ID,
        "schema_version": 1,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "output_root": repo_rel(materialization.output_root, root=root),
        "workspace_contract": repo_rel(workspace_path, root=root),
        "module_inventory": repo_rel(root / str(workspace["module_inventory"]), root=root),
        "stability_policy": repo_rel(root / str(workspace["stability_policy"]), root=root),
        "package_surface": repo_rel(root / str(workspace["package_surface"]), root=root),
        "modules": [module.to_payload() for module in materialization.modules],
        "copied_paths": unique_copied_paths,
        "copied_file_count": len(unique_copied_paths),
    }
