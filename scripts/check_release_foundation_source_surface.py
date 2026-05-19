#!/usr/bin/env python3
"""Validate the checked-in release-foundation source surface."""

from __future__ import annotations

import sys
from pathlib import Path

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_tooling.paths import repo_rel

ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "release_foundation" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "release-foundation" / "source-surface-summary.json"
SOURCE_SURFACE_CONTRACT_ID = "objc3c.release.foundation.source.surface.v1"
SOURCE_SURFACE_KIND = "release-foundation-source-surface"
SUMMARY_CONTRACT_ID = "objc3c.release.foundation.source.surface.summary.v1"
EXPECTED_RUNBOOK = "docs/runbooks/objc3c_release_foundation.md"

EXPECTED_CONTRACT_IDS = {
    "artifact_taxonomy": "objc3c.release.foundation.artifact.taxonomy.v1",
    "distribution_trust_model": "objc3c.release.foundation.distribution.trust.model.v1",
    "distribution_audit": "objc3c.release.foundation.distribution.audit.v1",
    "reproducibility_policy": "objc3c.release.foundation.reproducibility.policy.v1",
    "release_payload_policy": "objc3c.release.foundation.payload.policy.v1",
    "provenance_policy": "objc3c.release.foundation.provenance.policy.v1",
    "workflow_surface": "objc3c.release.foundation.workflow.surface.v1",
    "schema_surface": "objc3c.release.foundation.schema.surface.v1",
}

EXPECTED_REQUIRED_PATHS = {
    "runbook": EXPECTED_RUNBOOK,
    "artifact_taxonomy": "tests/tooling/fixtures/release_foundation/artifact_taxonomy.json",
    "distribution_trust_model": "tests/tooling/fixtures/release_foundation/distribution_trust_model.json",
    "distribution_audit": "tests/tooling/fixtures/release_foundation/distribution_audit.json",
    "reproducibility_policy": "tests/tooling/fixtures/release_foundation/reproducibility_policy.json",
    "release_payload_policy": "tests/tooling/fixtures/release_foundation/release_payload_policy.json",
    "provenance_policy": "tests/tooling/fixtures/release_foundation/provenance_policy.json",
    "workflow_surface": "tests/tooling/fixtures/release_foundation/workflow_surface.json",
    "schema_surface": "tests/tooling/fixtures/release_foundation/schema_surface.json",
}

EXPECTED_CHECKED_IN_SOURCES = (
    "docs/runbooks/objc3c_release_foundation.md",
    "scripts/package_objc3c_runnable_toolchain.ps1",
    "scripts/check_release_evidence.py",
    "scripts/build_objc3c_native.ps1",
    "docs/runbooks/objc3c_maintainer_workflows.md",
)

EXPECTED_UPSTREAM_SURFACES = (
    "spec/conformance/release_evidence_gate_maintenance.md",
    "docs/runbooks/objc3c_public_command_surface.md",
    "tmp/build-objc3c-native/repo_superclean_source_of_truth.json",
)
GENERATED_UPSTREAM_SURFACES = frozenset(
    {"tmp/build-objc3c-native/repo_superclean_source_of_truth.json"}
)

EXPECTED_BUILD_SCRIPTS = (
    "scripts/check_release_foundation_source_surface.py",
    "scripts/check_release_foundation_schema_surface.py",
    "scripts/build_objc3c_release_manifest.py",
    "scripts/publish_objc3c_release_provenance.py",
    "scripts/check_objc3c_release_foundation_integration.py",
)

EXPECTED_MACHINE_OWNED_OUTPUT_ROOTS = (
    "tmp/reports/release-foundation",
    "tmp/artifacts/release-foundation",
    "tmp/pkg/objc3c-native-runnable-toolchain",
)

EXPECTED_EXPLICIT_NON_GOALS = (
    "no second installer-shaped payload",
    "no hand-maintained digest manifest",
    "no release claim that bypasses the runnable package manifest",
)


def fail(message: str) -> int:
    print(f"release-foundation-source-surface: {message}", file=sys.stderr)
    return 1


def require_exact_path(source_surface: dict[str, object], field_name: str) -> str | None:
    raw_path = source_surface.get(field_name)
    expected_path = EXPECTED_REQUIRED_PATHS[field_name]
    if raw_path != expected_path:
        fail(f"{field_name} drifted from required path {expected_path}")
        return None
    return expected_path


def require_existing_file(path: str, label: str) -> bool:
    target = ROOT / path
    if not target.is_file():
        fail(f"{label} referenced missing file {path}")
        return False
    return True


def require_existing_path(path: str, label: str) -> bool:
    target = ROOT / path
    if not target.exists():
        fail(f"{label} referenced missing path {path}")
        return False
    return True


def ensure_generated_upstream_surface(path: str) -> bool:
    target = ROOT / path
    if path not in GENERATED_UPSTREAM_SURFACES:
        return True
    if not target.is_file():
        fail(
            f"generated upstream surface is missing {path}; "
            "run the native build contract surface before checking release foundation"
        )
        return False
    return True


def require_exact_list(
    source_surface: dict[str, object],
    field_name: str,
    expected_items: tuple[str, ...],
) -> tuple[str, ...] | None:
    items = source_surface.get(field_name)
    if items != list(expected_items):
        fail(f"{field_name} drifted from required entries")
        return None
    return expected_items


def main() -> int:
    if not SOURCE_SURFACE.is_file():
        return fail(f"missing source surface {repo_rel(SOURCE_SURFACE)}")
    source_surface = load_json(SOURCE_SURFACE)
    if source_surface.get("contract_id") != SOURCE_SURFACE_CONTRACT_ID:
        return fail("unexpected source surface contract_id")
    if source_surface.get("surface_kind") != SOURCE_SURFACE_KIND:
        return fail("unexpected source surface kind")

    checked_paths: list[str] = [repo_rel(SOURCE_SURFACE)]
    for field_name, expected_contract_id in EXPECTED_CONTRACT_IDS.items():
        raw_path = require_exact_path(source_surface, field_name)
        if raw_path is None:
            return 1
        if not require_existing_file(raw_path, field_name):
            return 1
        payload = load_json(ROOT / raw_path)
        if payload.get("contract_id") != expected_contract_id:
            return fail(f"{field_name} drifted from expected contract id {expected_contract_id}")
        checked_paths.append(raw_path)

    runbook = require_exact_path(source_surface, "runbook")
    if runbook is None:
        return 1
    if not require_existing_file(runbook, "runbook"):
        return 1
    checked_paths.append(runbook)

    checked_in_sources = require_exact_list(
        source_surface,
        "checked_in_sources",
        EXPECTED_CHECKED_IN_SOURCES,
    )
    upstream_surfaces = require_exact_list(
        source_surface,
        "upstream_surfaces",
        EXPECTED_UPSTREAM_SURFACES,
    )
    build_scripts = require_exact_list(source_surface, "build_scripts", EXPECTED_BUILD_SCRIPTS)
    machine_owned_output_roots = require_exact_list(
        source_surface,
        "machine_owned_output_roots",
        EXPECTED_MACHINE_OWNED_OUTPUT_ROOTS,
    )
    explicit_non_goals = require_exact_list(
        source_surface,
        "explicit_non_goals",
        EXPECTED_EXPLICIT_NON_GOALS,
    )
    if (
        checked_in_sources is None
        or upstream_surfaces is None
        or build_scripts is None
        or machine_owned_output_roots is None
        or explicit_non_goals is None
    ):
        return 1

    for list_name, items in (
        ("checked_in_sources", checked_in_sources),
        ("upstream_surfaces", upstream_surfaces),
        ("build_scripts", build_scripts),
    ):
        for raw_path in items:
            if list_name == "upstream_surfaces" and not ensure_generated_upstream_surface(raw_path):
                return 1
            if not require_existing_path(raw_path, list_name):
                return 1
            checked_paths.append(raw_path)

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "source_surface": repo_rel(SOURCE_SURFACE),
        "runbook": EXPECTED_RUNBOOK,
        "artifact_taxonomy": EXPECTED_REQUIRED_PATHS["artifact_taxonomy"],
        "distribution_trust_model": EXPECTED_REQUIRED_PATHS["distribution_trust_model"],
        "distribution_audit": EXPECTED_REQUIRED_PATHS["distribution_audit"],
        "reproducibility_policy": EXPECTED_REQUIRED_PATHS["reproducibility_policy"],
        "release_payload_policy": EXPECTED_REQUIRED_PATHS["release_payload_policy"],
        "provenance_policy": EXPECTED_REQUIRED_PATHS["provenance_policy"],
        "workflow_surface": EXPECTED_REQUIRED_PATHS["workflow_surface"],
        "schema_surface": EXPECTED_REQUIRED_PATHS["schema_surface"],
        "checked_in_sources": list(checked_in_sources),
        "upstream_surfaces": list(upstream_surfaces),
        "build_scripts": list(build_scripts),
        "machine_owned_output_roots": list(machine_owned_output_roots),
        "explicit_non_goals": list(explicit_non_goals),
        "checked_path_count": len(sorted(set(checked_paths))),
        "checked_paths": sorted(set(checked_paths)),
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("release-foundation-source-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
