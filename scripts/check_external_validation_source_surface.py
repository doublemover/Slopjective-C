#!/usr/bin/env python3
"""Validate the checked-in external validation source surface."""

from __future__ import annotations

import json
import sys
from datetime import datetime, timezone
from pathlib import Path
from typing import Any


ROOT = Path(__file__).resolve().parents[1]
SOURCE_SURFACE = ROOT / "tests" / "tooling" / "fixtures" / "external_validation" / "source_surface.json"
SUMMARY_PATH = ROOT / "tmp" / "reports" / "external-validation" / "source-surface-summary.json"
SUMMARY_CONTRACT_ID = "objc3c.external_validation.source.surface.summary.v1"
EXPECTED_ROOTS = [
    "tests/tooling/fixtures/external_validation",
    "tests/tooling/fixtures/objc3c",
    "tests/conformance",
    "docs/runbooks",
]
EXPECTED_FAMILY_IDS = [
    "intake-normalization-boundary",
    "independent-replay-proofs",
    "packaged-reproducibility-surface",
]


def fail(message: str) -> int:
    print(f"external-validation-source-surface: FAIL\n- {message}", file=sys.stderr)
    return 1


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def load_json(path: Path) -> dict[str, Any]:
    payload = json.loads(path.read_text(encoding="utf-8"))
    if not isinstance(payload, dict):
        raise RuntimeError(f"JSON object expected at {repo_rel(path)}")
    return payload


def require_path(relative_path: str, *, kind: str) -> Path:
    path = ROOT / relative_path
    if not path.exists():
        raise RuntimeError(f"missing {kind}: {relative_path}")
    return path


def main() -> int:
    if not SOURCE_SURFACE.is_file():
        return fail(f"missing source surface contract: {repo_rel(SOURCE_SURFACE)}")

    surface = load_json(SOURCE_SURFACE)
    if surface.get("contract_id") != "objc3c.external_validation.source.surface.v1":
        return fail("contract_id drifted")
    if surface.get("schema_version") != 1:
        return fail("schema_version drifted")
    if surface.get("runbook") != "docs/runbooks/objc3c_external_validation.md":
        return fail("runbook drifted")
    if surface.get("source_root") != "tests/tooling/fixtures/external_validation":
        return fail("source_root drifted")
    if surface.get("source_readme") != "tests/tooling/fixtures/external_validation/README.md":
        return fail("source_readme drifted")
    if surface.get("source_check_script") != "scripts/check_external_validation_source_surface.py":
        return fail("source_check_script drifted")
    if surface.get("trust_policy") != "tests/tooling/fixtures/external_validation/trust_policy.json":
        return fail("trust_policy drifted")
    if surface.get("intake_manifest") != "tests/tooling/fixtures/external_validation/intake_manifest.json":
        return fail("intake_manifest drifted")
    if surface.get("quarantine_manifest") != "tests/tooling/fixtures/external_validation/quarantine_manifest.json":
        return fail("quarantine_manifest drifted")
    if surface.get("artifact_surface") != "tests/tooling/fixtures/external_validation/artifact_surface.json":
        return fail("artifact_surface drifted")
    if surface.get("checked_in_roots") != EXPECTED_ROOTS:
        return fail("checked_in_roots drifted")

    require_path("docs/runbooks/objc3c_external_validation.md", kind="runbook")
    require_path("tests/tooling/fixtures/external_validation/README.md", kind="source readme")
    trust_policy_path = require_path(
        "tests/tooling/fixtures/external_validation/trust_policy.json",
        kind="trust policy",
    )
    intake_manifest_path = require_path(
        "tests/tooling/fixtures/external_validation/intake_manifest.json",
        kind="intake manifest",
    )
    quarantine_manifest_path = require_path(
        "tests/tooling/fixtures/external_validation/quarantine_manifest.json",
        kind="quarantine manifest",
    )
    artifact_surface_path = require_path(
        "tests/tooling/fixtures/external_validation/artifact_surface.json",
        kind="artifact surface",
    )
    for root in EXPECTED_ROOTS:
        require_path(root, kind="checked-in root")

    trust_policy = load_json(trust_policy_path)
    if trust_policy.get("contract_id") != "objc3c.external_validation.trust.policy.v1":
        return fail("trust policy contract_id drifted")
    if trust_policy.get("schema_version") != 1:
        return fail("trust policy schema_version drifted")
    if trust_policy.get("allowed_trust_states") != ["candidate", "accepted", "quarantined", "rejected"]:
        return fail("trust policy allowed_trust_states drifted")
    if trust_policy.get("publishable_trust_states") != ["accepted"]:
        return fail("trust policy publishable_trust_states drifted")

    intake_manifest = load_json(intake_manifest_path)
    if intake_manifest.get("contract_id") != "objc3c.external_validation.intake.manifest.v1":
        return fail("intake manifest contract_id drifted")
    if intake_manifest.get("schema_version") != 1:
        return fail("intake manifest schema_version drifted")
    entries = intake_manifest.get("entries")
    if not isinstance(entries, list) or len(entries) < 3:
        return fail("intake manifest entries drifted")
    allowed_trust_states = set(trust_policy["allowed_trust_states"])
    allowed_surfaces = {"conformance-case", "replay-contract"}
    allowed_families = {"parser", "diagnostics", "module-roundtrip"}
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
        for field_name in trust_policy["required_provenance_fields"]:
            if not isinstance(provenance.get(field_name), str) or not provenance.get(field_name):
                return fail(f"{fixture_id} is missing provenance.{field_name}")
        if not isinstance(replay_script, str) or not replay_script:
            return fail(f"{fixture_id} is missing replay_script")
        require_path(replay_script, kind=f"{fixture_id} replay script")
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
            summary_entry["normalized_contract_path"] = normalized_contract_path
            summary_entry["coverage_anchor"] = coverage_anchor
        intake_entry_summaries.append(summary_entry)

    quarantine_manifest = load_json(quarantine_manifest_path)
    if quarantine_manifest.get("contract_id") != "objc3c.external_validation.quarantine.manifest.v1":
        return fail("quarantine manifest contract_id drifted")
    if quarantine_manifest.get("schema_version") != 1:
        return fail("quarantine manifest schema_version drifted")
    quarantine_entries = quarantine_manifest.get("entries")
    if not isinstance(quarantine_entries, list) or len(quarantine_entries) < 3:
        return fail("quarantine manifest entries drifted")
    allowed_disclosure_modes = {"internal-only", "redacted-summary", "blocked"}
    allowed_escalation_targets = {"license-review", "maintainer-review", "security-review"}
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
        if trust_state not in {"quarantined", "rejected"}:
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

    artifact_surface = load_json(artifact_surface_path)
    if artifact_surface.get("contract_id") != "objc3c.external_validation.artifact.surface.v1":
        return fail("artifact surface contract_id drifted")
    if artifact_surface.get("schema_version") != 1:
        return fail("artifact surface schema_version drifted")
    if artifact_surface.get("artifact_root") != "tmp/artifacts/external-validation":
        return fail("artifact surface artifact_root drifted")
    if artifact_surface.get("report_root") != "tmp/reports/external-validation":
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
        observed_family_ids.append(family_id)
        checked_paths: list[str] = []
        for source_path in source_paths:
            if not isinstance(source_path, str) or not source_path:
                return fail(f"{family_id} contains a non-string source path")
            require_path(source_path, kind=f"{family_id} source path")
            checked_paths.append(source_path)
        family_summaries.append(
            {
                "family_id": family_id,
                "source_path_count": len(checked_paths),
                "source_paths": checked_paths,
            }
        )

    if observed_family_ids != EXPECTED_FAMILY_IDS:
        return fail("source_families inventory drifted")

    summary = {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "source_surface_contract": repo_rel(SOURCE_SURFACE),
        "runbook": surface["runbook"],
        "source_check_script": surface["source_check_script"],
        "trust_policy": surface["trust_policy"],
        "intake_manifest": surface["intake_manifest"],
        "quarantine_manifest": surface["quarantine_manifest"],
        "artifact_surface": surface["artifact_surface"],
        "checked_in_roots": EXPECTED_ROOTS,
        "family_summaries": family_summaries,
        "intake_entry_summaries": intake_entry_summaries,
        "quarantine_entry_summaries": quarantine_entry_summaries,
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")
    print(f"summary_path: {repo_rel(SUMMARY_PATH)}")
    print("external-validation-source-surface: OK")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
