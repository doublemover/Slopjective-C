"""Classification and reference matching for validation inventory surfaces."""

from __future__ import annotations

from pathlib import Path

STATIC_GUARD_OVERRIDES: dict[str, tuple[str, str]] = {
    "scripts/ci/check_task_hygiene.py": (
        "retain:task-hygiene",
        "prevents removed script families, stale prototype roots, live bytecode, and budget drift from silently returning",
    ),
    "scripts/check_repo_superclean_surface.py": (
        "retain:repo-shape",
        "guards canonical repo roots, generated output boundaries, and build-owned source-of-truth publication",
    ),
    "scripts/check_showcase_surface.py": (
        "retain:product-surface",
        "guards showcase source-of-truth structure and compile-coupled example boundaries",
    ),
    "scripts/check_stdlib_surface.py": (
        "retain:product-surface",
        "guards stdlib roots, module inventory, package alias mapping, and import/lowering contract shape",
    ),
    "scripts/check_stress_source_surface.py": (
        "retain:source-surface-contract",
        "guards stress source roots and machine-owned artifact boundaries before execution begins",
    ),
    "scripts/check_external_validation_source_surface.py": (
        "retain:source-surface-contract",
        "guards external-validation intake and trust-policy source-of-truth structure",
    ),
    "scripts/check_public_conformance_reporting_source_surface.py": (
        "retain:source-surface-contract",
        "guards public conformance reporting source-surface inputs and checked-in contracts",
    ),
    "scripts/check_public_conformance_schema_surface.py": (
        "retain:schema-contract",
        "guards public conformance publication schemas before report generation and publication",
    ),
    "scripts/check_performance_governance_source_surface.py": (
        "retain:source-surface-contract",
        "guards performance governance source inputs, policy roots, and checked-in contract layout",
    ),
    "scripts/check_performance_governance_schema_surface.py": (
        "retain:schema-contract",
        "guards performance governance schemas before dashboard/report publication",
    ),
    "scripts/check_release_foundation_source_surface.py": (
        "retain:source-surface-contract",
        "guards release-foundation source inputs, policy roots, and payload contract layout",
    ),
    "scripts/check_release_foundation_schema_surface.py": (
        "retain:schema-contract",
        "guards release-foundation schemas before manifest/SBOM/attestation publication",
    ),
    "scripts/check_packaging_channels_source_surface.py": (
        "retain:source-surface-contract",
        "guards packaging-channel source roots, supported-platform inputs, and workflow-surface contracts",
    ),
    "scripts/check_packaging_channels_schema_surface.py": (
        "retain:schema-contract",
        "guards packaging-channel schemas before package-manifest and install-receipt publication",
    ),
    "scripts/check_release_operations_source_surface.py": (
        "retain:source-surface-contract",
        "guards release-operations source inputs, version/update policy roots, and workflow metadata contracts",
    ),
    "scripts/check_release_operations_schema_surface.py": (
        "retain:schema-contract",
        "guards release-operations schemas before update-manifest and publication metadata generation",
    ),
    "scripts/check_distribution_credibility_source_surface.py": (
        "retain:source-surface-contract",
        "guards distribution-credibility source roots and checked-in trust-signal contracts",
    ),
    "scripts/check_distribution_credibility_schema_surface.py": (
        "retain:schema-contract",
        "guards distribution-credibility schemas before dashboard/trust-report publication",
    ),
}


def classify_check_py(rel: str) -> dict[str, str]:
    if rel in STATIC_GUARD_OVERRIDES:
        retention_class, unique_value = STATIC_GUARD_OVERRIDES[rel]
        return {
            "surface_kind": "retained_static_guard",
            "retention_class": retention_class,
            "unique_value": unique_value,
            "successor_surface": "retained as static policy guard inside the acceptance-first model",
        }
    if rel.endswith("_source_surface.py"):
        return {
            "surface_kind": "retained_static_guard",
            "retention_class": "retain:source-surface-contract",
            "unique_value": "guards checked-in source-surface contract roots before integration or publication runs",
            "successor_surface": "retained as preflight contract guard ahead of executable validation",
        }
    if rel.endswith("_schema_surface.py"):
        return {
            "surface_kind": "retained_static_guard",
            "retention_class": "retain:schema-contract",
            "unique_value": "guards machine-owned schema compatibility before generated publication or evidence runs",
            "successor_surface": "retained as preflight contract guard ahead of executable validation",
        }
    if rel.endswith("_integration.py"):
        return {
            "surface_kind": "executable_integration_validator",
            "retention_class": "migrate:acceptance-first-integration",
            "unique_value": "runs the integrated live workflow path and should be primary executable truth rather than a static checker",
            "successor_surface": "shared harnesses plus public workflow validate-* actions",
        }
    if rel.endswith("_end_to_end.py"):
        return {
            "surface_kind": "executable_runnable_validator",
            "retention_class": "migrate:acceptance-first-runnable",
            "unique_value": "proves staged runnable or packaged end-to-end behavior and should remain executable evidence, not static policy",
            "successor_surface": "shared harnesses plus runnable validate-* actions",
        }
    if "conformance" in rel or "acceptance" in rel:
        return {
            "surface_kind": "executable_acceptance_validator",
            "retention_class": "migrate:acceptance-first-suite",
            "unique_value": "encodes runnable conformance or acceptance semantics and belongs in the executable truth layer",
            "successor_surface": "shared compiler/runtime acceptance harness and public workflow validate/test actions",
        }
    return {
        "surface_kind": "executable_validator",
        "retention_class": "migrate:acceptance-first-executable",
        "unique_value": "performs live validation work and should be treated as executable evidence rather than a retained static guard",
        "successor_surface": "shared harnesses and public workflow actions",
    }


def referenced_by(package: dict[str, str], workflow_text: str, hygiene_text: str, rel: str) -> dict[str, bool]:
    basename = Path(rel).name
    package_hits = any(
        basename in command or rel.replace("/", "\\") in command or rel in command for command in package.values()
    )
    workflow_hits = basename in workflow_text or rel in workflow_text or rel.replace("/", "\\") in workflow_text
    hygiene_hits = basename in hygiene_text or rel in hygiene_text or rel.replace("/", "\\") in hygiene_text
    return {
        "package_json": package_hits,
        "workflow_runner": workflow_hits,
        "task_hygiene_gate": hygiene_hits,
    }
