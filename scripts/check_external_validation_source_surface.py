#!/usr/bin/env python3
"""Validate the checked-in external validation source surface."""

from __future__ import annotations

import sys
from pathlib import Path
from typing import Any

from objc3c_shared.json_io import load_json_object as load_json
from objc3c_shared.json_io import write_report_json
from objc3c_tooling.paths import repo_rel


ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "external_validation" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "external-validation" / "source-surface-summary.json"
SOURCE_SURFACE_CONTRACT_ID = "objc3c.external_validation.source.surface.v1"
SCHEMA_VERSION = 1
SUMMARY_CONTRACT_ID = "objc3c.external_validation.source.surface.summary.v1"
EXPECTED_REQUIRED_PATHS = {
    "runbook": "docs/runbooks/objc3c_external_validation.md",
    "source_root": "tests/tooling/fixtures/external_validation",
    "source_readme": "tests/tooling/fixtures/external_validation/README.md",
    "source_check_script": "scripts/check_external_validation_source_surface.py",
    "trust_policy": "tests/tooling/fixtures/external_validation/trust_policy.json",
    "intake_manifest": "tests/tooling/fixtures/external_validation/intake_manifest.json",
    "quarantine_manifest": "tests/tooling/fixtures/external_validation/quarantine_manifest.json",
    "artifact_surface": "tests/tooling/fixtures/external_validation/artifact_surface.json",
}
EXPECTED_ROOTS = (
    "tests/tooling/fixtures/external_validation",
    "tests/tooling/fixtures/objc3c",
    "tests/conformance",
    "docs/runbooks",
)
EXPECTED_FAMILY_IDS = (
    "intake-normalization-boundary",
    "independent-replay-proofs",
    "packaged-reproducibility-surface",
)
EXPECTED_SOURCE_FAMILY_PATHS = {
    "intake-normalization-boundary": (
        "docs/runbooks/objc3c_external_validation.md",
        "tests/tooling/fixtures/external_validation/README.md",
        "tests/tooling/fixtures/external_validation/source_surface.json",
        "tests/tooling/fixtures/external_validation/trust_policy.json",
        "tests/tooling/fixtures/external_validation/intake_manifest.json",
        "tests/tooling/fixtures/external_validation/quarantine_manifest.json",
        "tests/tooling/fixtures/external_validation/artifact_surface.json",
        "tests/tooling/fixtures/objc3c",
        "tests/conformance/corpus_surface.json",
        "tests/conformance/longitudinal_suites.json",
        "docs/runbooks/objc3c_conformance_corpus.md",
    ),
    "independent-replay-proofs": (
        "scripts/check_objc3c_parser_replay_proof.ps1",
        "scripts/check_objc3c_diagnostics_replay_proof.ps1",
        "scripts/check_objc3c_lowering_replay_proof.ps1",
        "scripts/check_objc3c_execution_replay_proof.ps1",
        "tests/conformance/parser/manifest.json",
        "tests/conformance/diagnostics/manifest.json",
        "tests/conformance/lowering_abi/manifest.json",
        "tests/conformance/module_roundtrip/manifest.json",
        "tests/tooling/fixtures/objc3c",
    ),
    "packaged-reproducibility-surface": (
        "scripts/check_objc3c_conformance_corpus_integration.py",
        "scripts/check_objc3c_runnable_conformance_corpus_end_to_end.py",
        "scripts/package_objc3c_runnable_toolchain.ps1",
        "docs/runbooks/objc3c_conformance_corpus.md",
        "tests/conformance/corpus_surface.json",
    ),
}
EXPECTED_CONTRACT_IDS = {
    "trust_policy": "objc3c.external_validation.trust.policy.v1",
    "intake_manifest": "objc3c.external_validation.intake.manifest.v1",
    "quarantine_manifest": "objc3c.external_validation.quarantine.manifest.v1",
    "artifact_surface": "objc3c.external_validation.artifact.surface.v1",
}
EXPECTED_TRUST_STATES = ("candidate", "accepted", "quarantined", "rejected")
EXPECTED_PUBLISHABLE_TRUST_STATES = ("accepted",)
EXPECTED_REQUIRED_PROVENANCE_FIELDS = (
    "origin",
    "captured_at_utc",
    "captured_by",
    "license",
    "normalized_from",
)
EXPECTED_INTAKE_SURFACES = ("conformance-case", "replay-contract")
EXPECTED_INTAKE_FAMILIES = ("parser", "module-roundtrip")
EXPECTED_QUARANTINE_TRUST_STATES = ("quarantined", "rejected")
EXPECTED_DISCLOSURE_MODES = ("internal-only", "redacted-summary", "blocked")
EXPECTED_ESCALATION_TARGETS = ("license-review", "maintainer-review", "security-review")
EXPECTED_ARTIFACT_ROOT = "tmp/artifacts/external-validation"
EXPECTED_REPORT_ROOT = "tmp/reports/external-validation"


def fail(message: str) -> int:
    print(f"external-validation-source-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def require_path(relative_path: str, *, kind: str) -> Path:
    path = ROOT / relative_path
    if not path.exists():
        raise RuntimeError(f"missing {kind}: {relative_path}")
    return path


def require_exact_path(source_surface: dict[str, object], field_name: str) -> str | None:
    expected_path = EXPECTED_REQUIRED_PATHS[field_name]
    if source_surface.get(field_name) != expected_path:
        fail(f"{field_name} drifted")
        return None
    return expected_path


def require_exact_list(
    source_surface: dict[str, object],
    field_name: str,
    expected_items: tuple[str, ...],
) -> tuple[str, ...] | None:
    if source_surface.get(field_name) != list(expected_items):
        fail(f"{field_name} drifted")
        return None
    return expected_items


def require_contract_id(payload: dict[str, object], field_name: str) -> bool:
    expected_contract_id = EXPECTED_CONTRACT_IDS[field_name]
    if payload.get("contract_id") != expected_contract_id:
        fail(f"{field_name} contract_id drifted")
        return False
    if payload.get("schema_version") != SCHEMA_VERSION:
        fail(f"{field_name} schema_version drifted")
        return False
    return True


def main() -> int:
    if not SOURCE_SURFACE.is_file():
        return fail(f"missing source surface contract: {repo_rel(SOURCE_SURFACE)}")

    surface = load_json(SOURCE_SURFACE)
    if surface.get("contract_id") != SOURCE_SURFACE_CONTRACT_ID:
        return fail("contract_id drifted")
    if surface.get("schema_version") != SCHEMA_VERSION:
        return fail("schema_version drifted")

    checked_roots = require_exact_list(surface, "checked_in_roots", EXPECTED_ROOTS)
    if checked_roots is None:
        return 1

    required_paths: dict[str, str] = {}
    checked_paths: list[str] = []
    for field_name in EXPECTED_REQUIRED_PATHS:
        relative_path = require_exact_path(surface, field_name)
        if relative_path is None:
            return 1
        required_paths[field_name] = relative_path
        require_path(relative_path, kind=field_name)
        checked_paths.append(relative_path)
    for root in checked_roots:
        require_path(root, kind="checked-in root")
        checked_paths.append(root)

    trust_policy = load_json(ROOT / required_paths["trust_policy"])
    if not require_contract_id(trust_policy, "trust_policy"):
        return 1
    if trust_policy.get("allowed_trust_states") != list(EXPECTED_TRUST_STATES):
        return fail("trust policy allowed_trust_states drifted")
    if trust_policy.get("publishable_trust_states") != list(EXPECTED_PUBLISHABLE_TRUST_STATES):
        return fail("trust policy publishable_trust_states drifted")
    if trust_policy.get("required_provenance_fields") != list(EXPECTED_REQUIRED_PROVENANCE_FIELDS):
        return fail("trust policy required_provenance_fields drifted")

    intake_manifest = load_json(ROOT / required_paths["intake_manifest"])
    if not require_contract_id(intake_manifest, "intake_manifest"):
        return 1
    entries = intake_manifest.get("entries")
    if not isinstance(entries, list) or len(entries) < 3:
        return fail("intake manifest entries drifted")
    allowed_trust_states = set(EXPECTED_TRUST_STATES)
    allowed_surfaces = set(EXPECTED_INTAKE_SURFACES)
    allowed_families = set(EXPECTED_INTAKE_FAMILIES)
    intake_entry_summaries: list[dict[str, Any]] = []
    for entry in entries:
        if not isinstance(entry, dict):
            return fail("intake manifest contains a non-object entry")
        fixture_id = entry.get("fixture_id")
        trust_state = entry.get("trust_state")
        normalized_surface = entry.get("normalized_surface")
        family = entry.get("family")
        provenance = entry.get("provenance")
        replay_script = entry.get("replay_script")
        if not isinstance(fixture_id, str) or not fixture_id:
            return fail("intake manifest entry missing fixture_id")
        if trust_state not in allowed_trust_states:
            return fail(f"{fixture_id} references unknown trust_state")
        if normalized_surface not in allowed_surfaces:
            return fail(f"{fixture_id} references unknown normalized_surface")
        if family not in allowed_families:
            return fail(f"{fixture_id} references unknown family")
        if not isinstance(provenance, dict):
            return fail(f"{fixture_id} is missing provenance")
        for field_name in EXPECTED_REQUIRED_PROVENANCE_FIELDS:
            if not isinstance(provenance.get(field_name), str) or not provenance.get(field_name):
                return fail(f"{fixture_id} is missing provenance.{field_name}")
        if not isinstance(replay_script, str) or not replay_script:
            return fail(f"{fixture_id} is missing replay_script")
        require_path(replay_script, kind=f"{fixture_id} replay script")
        checked_paths.append(replay_script)
        summary_entry = {
            "fixture_id": fixture_id,
            "trust_state": trust_state,
            "normalized_surface": normalized_surface,
            "family": family,
            "replay_script": replay_script,
        }
        if normalized_surface == "conformance-case":
            normalized_case_path = entry.get("normalized_case_path")
            if not isinstance(normalized_case_path, str) or not normalized_case_path:
                return fail(f"{fixture_id} is missing normalized_case_path")
            require_path(normalized_case_path, kind=f"{fixture_id} normalized case")
            checked_paths.append(normalized_case_path)
            summary_entry["normalized_case_path"] = normalized_case_path
        else:
            normalized_contract_path = entry.get("normalized_contract_path")
            coverage_anchor = entry.get("coverage_anchor")
            if not isinstance(normalized_contract_path, str) or not normalized_contract_path:
                return fail(f"{fixture_id} is missing normalized_contract_path")
            if not isinstance(coverage_anchor, str) or not coverage_anchor:
                return fail(f"{fixture_id} is missing coverage_anchor")
            require_path(normalized_contract_path, kind=f"{fixture_id} normalized contract")
            require_path(coverage_anchor, kind=f"{fixture_id} coverage anchor")
            checked_paths.extend([normalized_contract_path, coverage_anchor])
            summary_entry["normalized_contract_path"] = normalized_contract_path
            summary_entry["coverage_anchor"] = coverage_anchor
        intake_entry_summaries.append(summary_entry)

    quarantine_manifest = load_json(ROOT / required_paths["quarantine_manifest"])
    if not require_contract_id(quarantine_manifest, "quarantine_manifest"):
        return 1
    quarantine_entries = quarantine_manifest.get("entries")
    if not isinstance(quarantine_entries, list) or len(quarantine_entries) < 3:
        return fail("quarantine manifest entries drifted")
    allowed_disclosure_modes = set(EXPECTED_DISCLOSURE_MODES)
    allowed_escalation_targets = set(EXPECTED_ESCALATION_TARGETS)
    quarantine_entry_summaries: list[dict[str, Any]] = []
    for entry in quarantine_entries:
        if not isinstance(entry, dict):
            return fail("quarantine manifest contains a non-object entry")
        fixture_id = entry.get("fixture_id")
        trust_state = entry.get("trust_state")
        diagnostic_id = entry.get("diagnostic_id")
        escalation_target = entry.get("escalation_target")
        disclosure_compatibility = entry.get("disclosure_compatibility")
        reason = entry.get("reason")
        normalized_contract_path = entry.get("normalized_contract_path")
        replay_script = entry.get("replay_script")
        if not isinstance(fixture_id, str) or not fixture_id:
            return fail("quarantine manifest entry missing fixture_id")
        if trust_state not in set(EXPECTED_QUARANTINE_TRUST_STATES):
            return fail(f"{fixture_id} uses an invalid quarantine trust_state")
        if not isinstance(diagnostic_id, str) or not diagnostic_id.startswith("OBJC3-EXTERNAL-EVIDENCE-"):
            return fail(f"{fixture_id} is missing an external-evidence diagnostic_id")
        if escalation_target not in allowed_escalation_targets:
            return fail(f"{fixture_id} references unknown escalation_target")
        if disclosure_compatibility not in allowed_disclosure_modes:
            return fail(f"{fixture_id} references unknown disclosure_compatibility")
        if not isinstance(reason, str) or not reason:
            return fail(f"{fixture_id} is missing reason")
        if not isinstance(normalized_contract_path, str) or not normalized_contract_path:
            return fail(f"{fixture_id} is missing normalized_contract_path")
        if not isinstance(replay_script, str) or not replay_script:
            return fail(f"{fixture_id} is missing replay_script")
        require_path(normalized_contract_path, kind=f"{fixture_id} normalized contract")
        require_path(replay_script, kind=f"{fixture_id} replay script")
        checked_paths.extend([normalized_contract_path, replay_script])
        quarantine_entry_summaries.append(
            {
                "fixture_id": fixture_id,
                "trust_state": trust_state,
                "diagnostic_id": diagnostic_id,
                "escalation_target": escalation_target,
                "disclosure_compatibility": disclosure_compatibility,
                "normalized_contract_path": normalized_contract_path,
                "replay_script": replay_script,
            }
        )

    artifact_surface = load_json(ROOT / required_paths["artifact_surface"])
    if not require_contract_id(artifact_surface, "artifact_surface"):
        return 1
    if artifact_surface.get("artifact_root") != EXPECTED_ARTIFACT_ROOT:
        return fail("artifact surface artifact_root drifted")
    if artifact_surface.get("report_root") != EXPECTED_REPORT_ROOT:
        return fail("artifact surface report_root drifted")

    families = surface.get("source_families")
    if not isinstance(families, list) or len(families) != len(EXPECTED_FAMILY_IDS):
        return fail("source_families drifted")

    family_summaries: list[dict[str, Any]] = []
    observed_family_ids: list[str] = []
    for family in families:
        if not isinstance(family, dict):
            return fail("source_families contains a non-object entry")
        family_id = family.get("family_id")
        coverage_goal = family.get("coverage_goal")
        source_paths = family.get("source_paths")
        if not isinstance(family_id, str) or not family_id:
            return fail("source_families entry missing family_id")
        if not isinstance(coverage_goal, str) or not coverage_goal:
            return fail(f"{family_id} is missing coverage_goal")
        if not isinstance(source_paths, list) or not source_paths:
            return fail(f"{family_id} is missing source_paths")
        expected_source_paths = EXPECTED_SOURCE_FAMILY_PATHS.get(family_id)
        if expected_source_paths is None or source_paths != list(expected_source_paths):
            return fail(f"{family_id} source_paths drifted")
        observed_family_ids.append(family_id)
        family_checked_paths: list[str] = []
        for source_path in source_paths:
            if not isinstance(source_path, str) or not source_path:
                return fail(f"{family_id} contains a non-string source path")
            require_path(source_path, kind=f"{family_id} source path")
            checked_paths.append(source_path)
            family_checked_paths.append(source_path)
        family_summaries.append(
            {
                "family_id": family_id,
                "source_path_count": len(family_checked_paths),
                "source_paths": family_checked_paths,
            }
        )

    if observed_family_ids != list(EXPECTED_FAMILY_IDS):
        return fail("source_families inventory drifted")

    checked_paths.append(repo_rel(SOURCE_SURFACE))
    unique_checked_paths = sorted(set(checked_paths))
    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": "PASS",
        "source_surface": repo_rel(SOURCE_SURFACE),
        "runbook": required_paths["runbook"],
        "source_root": required_paths["source_root"],
        "source_readme": required_paths["source_readme"],
        "source_check_script": required_paths["source_check_script"],
        "trust_policy": required_paths["trust_policy"],
        "intake_manifest": required_paths["intake_manifest"],
        "quarantine_manifest": required_paths["quarantine_manifest"],
        "artifact_surface": required_paths["artifact_surface"],
        "checked_in_roots": list(checked_roots),
        "expected_family_ids": list(EXPECTED_FAMILY_IDS),
        "artifact_root": EXPECTED_ARTIFACT_ROOT,
        "report_root": EXPECTED_REPORT_ROOT,
        "family_summaries": family_summaries,
        "intake_entry_summaries": intake_entry_summaries,
        "quarantine_entry_summaries": quarantine_entry_summaries,
        "checked_path_count": len(unique_checked_paths),
        "checked_paths": unique_checked_paths,
    }
    write_report_json(SUMMARY_PATH, summary, sort_keys=False)
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("external-validation-source-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
