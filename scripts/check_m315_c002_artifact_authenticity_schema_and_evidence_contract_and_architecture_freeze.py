from __future__ import annotations

import json
from pathlib import Path
import re
import sys


REPO_ROOT = Path(__file__).resolve().parents[1]
SCHEMA_PATH = (
    REPO_ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m315"
    / "m315_c002_artifact_authenticity_schema_and_evidence_contract_and_architecture_freeze_schema.json"
)
A003_PATH = (
    REPO_ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m315"
    / "m315_a003_artifact_authenticity_and_provenance_classification_inventory_source_completion_inventory.json"
)
B001_PATH = (
    REPO_ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m315"
    / "m315_b001_stable_feature_surface_identifier_and_annotation_policy_contract_and_architecture_freeze_policy.json"
)
B004_PATH = (
    REPO_ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m315"
    / "m315_b004_synthetic_versus_genuine_ir_fixture_compatibility_semantics_core_feature_implementation_registry.json"
)
C001_PATH = (
    REPO_ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m315"
    / "m315_c001_source_of_truth_and_generated_artifact_contract_and_architecture_freeze_contract.json"
)
SUMMARY_PATH = (
    REPO_ROOT
    / "tmp"
    / "reports"
    / "m315"
    / "M315-C002"
    / "artifact_authenticity_schema_summary.json"
)

NO_MILESTONE_TOKEN_RE = re.compile(r"\bM\d{3}-[A-E]\d{3}\b")


def read_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def validate_example(schema: dict, class_id: str, example: dict, durable_id_re: re.Pattern[str]) -> list[str]:
    errors: list[str] = []
    required_fields = schema["authenticity_classes"][class_id]["required_fields"]
    for field in required_fields:
        if field not in example:
            errors.append(f"{class_id} example missing required field: {field}")

    if example.get("provenance_class") != class_id:
        errors.append(f"{class_id} example provenance_class mismatch")

    if example.get("provenance_mode") not in schema["authenticity_classes"][class_id]["allowed_provenance_modes"]:
        errors.append(f"{class_id} example provenance_mode not allowed")

    artifact_family_id = example.get("artifact_family_id")
    if artifact_family_id and not durable_id_re.fullmatch(artifact_family_id):
        errors.append(f"{class_id} example artifact_family_id violates durable id grammar")

    generator_surface_id = example.get("generator_surface_id")
    if generator_surface_id and not durable_id_re.fullmatch(generator_surface_id):
        errors.append(f"{class_id} example generator_surface_id violates durable id grammar")

    if class_id == "generated_replay":
        if NO_MILESTONE_TOKEN_RE.search(example.get("regeneration_recipe", "")):
            errors.append("generated_replay example regeneration recipe must not embed milestone refs")

    if class_id == "legacy_generated_replay_bridge":
        if example.get("migration_owner_issue") != "M315-C003":
            errors.append("legacy replay bridge example must point at M315-C003 owner issue")

    return errors


def main() -> int:
    schema = read_json(SCHEMA_PATH)
    a003 = read_json(A003_PATH)
    b001 = read_json(B001_PATH)
    b004 = read_json(B004_PATH)
    c001 = read_json(C001_PATH)
    durable_id_re = re.compile(b001["durable_identifier_grammar"]["regex"])

    errors: list[str] = []

    expected_aliases = {
        "generated_or_report_artifact": "generated_report",
        "replay_candidate_missing_provenance": "legacy_generated_replay_bridge",
    }
    if schema["legacy_class_aliases"] != expected_aliases:
        errors.append("legacy class alias map drifted")

    expected_classes = {
        "synthetic_fixture",
        "sample_or_example_artifact",
        "generated_report",
        "generated_replay",
        "legacy_generated_replay_bridge",
        "schema_policy_contract",
        "historical_archive",
    }
    if set(schema["authenticity_classes"]) != expected_classes:
        errors.append("stable authenticity class set drifted")

    if schema["proof_rules"]["proof_eligible_classes"] != ["generated_replay"]:
        errors.append("proof-eligible classes must remain restricted to generated_replay")

    if schema["proof_rules"]["proof_required_mode"] != "tool_generated_regenerable":
        errors.append("proof-required provenance mode drifted")

    if schema["baseline_alignment"]["a003_class_totals"] != a003["authenticity_class_counts"]:
        errors.append("A003 baseline alignment drifted")

    if schema["baseline_alignment"]["b004_ll_fixture_totals"] != b004["ll_fixture_totals"]:
        errors.append("B004 ll fixture totals drifted")

    if c001["generated_artifact_contract"]["authenticity_schema_owner_issue"] != "M315-C002":
        errors.append("C001 authenticity schema owner issue drifted")

    for class_id, class_schema in schema["authenticity_classes"].items():
        if not class_schema["required_fields"]:
            errors.append(f"{class_id} required field set must not be empty")
        if class_schema["proof_eligible"] and class_id != "generated_replay":
            errors.append(f"{class_id} must not be proof-eligible")

    for class_id, example in schema["example_envelopes"].items():
        errors.extend(validate_example(schema, class_id, example, durable_id_re))

    ll_required_keys = schema["transport_contract"]["ll_required_keys"]
    if sorted(ll_required_keys) != [
        "generated_replay",
        "legacy_generated_replay_bridge",
        "synthetic_fixture",
    ]:
        errors.append("ll required-key contract drifted")

    for class_id, keys in ll_required_keys.items():
        missing = [
            key
            for key in keys
            if key not in schema["authenticity_classes"][class_id]["required_fields"]
        ]
        if missing:
            errors.append(f"{class_id} ll required keys are not in required_fields: {missing}")

    summary = {
        "stable_class_count": len(schema["authenticity_classes"]),
        "provenance_mode_count": len(schema["provenance_modes"]),
        "proof_eligible_classes": schema["proof_rules"]["proof_eligible_classes"],
        "legacy_class_aliases": schema["legacy_class_aliases"],
        "a003_replay_candidate_missing_provenance": a003["authenticity_class_counts"]["replay_candidate_missing_provenance"],
        "b004_replay_without_frontend_header": b004["ll_fixture_totals"]["replay_without_frontend_header"],
        "c001_authenticity_schema_owner_issue": c001["generated_artifact_contract"]["authenticity_schema_owner_issue"],
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    if errors:
        for error in errors:
            print(f"ERROR: {error}", file=sys.stderr)
        print(f"Summary written to {SUMMARY_PATH}", file=sys.stderr)
        return 1

    print("M315-C002 artifact authenticity schema summary:")
    print(json.dumps(summary, indent=2))
    print(f"Summary written to {SUMMARY_PATH}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
