from __future__ import annotations

import json
from pathlib import Path
import re
import sys


REPO_ROOT = Path(__file__).resolve().parents[1]
CONTRACT_PATH = (
    REPO_ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m315"
    / "m315_c001_source_of_truth_and_generated_artifact_contract_and_architecture_freeze_contract.json"
)
B001_POLICY_PATH = (
    REPO_ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m315"
    / "m315_b001_stable_feature_surface_identifier_and_annotation_policy_contract_and_architecture_freeze_policy.json"
)
B005_RESULT_PATH = (
    REPO_ROOT
    / "spec"
    / "planning"
    / "compiler"
    / "m315"
    / "m315_b005_comment_constexpr_and_contract_string_decontamination_sweep_edge_case_and_compatibility_completion_result.json"
)
SUMMARY_PATH = (
    REPO_ROOT
    / "tmp"
    / "reports"
    / "m315"
    / "M315-C001"
    / "source_of_truth_generated_artifact_contract_summary.json"
)

NEXT_ISSUE_PATHS = [
    REPO_ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h",
    REPO_ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp",
    REPO_ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp",
    REPO_ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp",
]
ISSUE_KEY_PATH = REPO_ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
M248_PATHS = [
    REPO_ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h",
    REPO_ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "objc3_final_readiness_gate_core_feature_implementation_surface.h",
]
TRANSITIONAL_SOURCE_MODEL_PATHS = [
    REPO_ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.h",
    REPO_ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp",
]
DEPENDENCY_ARRAY_PATH = (
    REPO_ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_types.h"
)

MILESTONE_REF_RE = re.compile(r"\bM\d{3}-[A-E]\d{3}\b")
LEGACY_FIELD_RE = re.compile(r";([a-z_]+_issue)=M\d{3}-[A-E]\d{3}")
REPLACEMENT_FIELD_RE = re.compile(r"^[a-z][a-z0-9_]*$")
PROHIBITED_FIELD_TOKEN_RE = re.compile(r"(?:^|_)(?:m\d{3}|issue\d+)(?:_|$)", re.IGNORECASE)


def read_json(path: Path) -> dict:
    return json.loads(path.read_text(encoding="utf-8"))


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def collect_next_issue_refs() -> list[str]:
    observed: set[str] = set()
    for path in NEXT_ISSUE_PATHS:
        text = read_text(path)
        observed.update(
            re.findall(r"next_issue[^M]*(M\d{3}-[A-E]\d{3})", text)
        )
        observed.update(
            re.findall(
                r'kObjc3RuntimeLiveDispatchGateNextIssue[^"]*"?(M\d{3}-[A-E]\d{3})',
                text,
            )
        )
    return sorted(observed)


def collect_issue_key_fields() -> list[str]:
    return sorted(
        field
        for field in set(LEGACY_FIELD_RE.findall(read_text(ISSUE_KEY_PATH)))
        if field != "next_issue"
    )


def collect_dependency_refs() -> list[str]:
    text = read_text(DEPENDENCY_ARRAY_PATH)
    match = re.search(
        r"portability_dependency_issue_ids\s*=\s*\{(?P<body>.*?)\};",
        text,
        re.DOTALL,
    )
    if not match:
        return []
    return sorted(set(MILESTONE_REF_RE.findall(match.group("body"))))


def collect_m248_legacy_hits(contract: dict) -> list[str]:
    text = "\n".join(read_text(path) for path in M248_PATHS)
    observed = {
        legacy
        for legacy in contract["replacement_contracts"]["legacy_m248_surface_identifier"][
            "legacy_to_replacement"
        ]
        if legacy in text
    }
    return sorted(observed)


def collect_transitional_literals(contract: dict) -> list[str]:
    text = "\n".join(read_text(path) for path in TRANSITIONAL_SOURCE_MODEL_PATHS)
    observed = {
        entry["legacy"]
        for entry in contract["replacement_contracts"]["transitional_source_model"][
            "legacy_literals"
        ]
        if entry["legacy"] in text
    }
    return sorted(observed)


def main() -> int:
    contract = read_json(CONTRACT_PATH)
    b001 = read_json(B001_POLICY_PATH)
    b005 = read_json(B005_RESULT_PATH)
    durable_id_re = re.compile(
        contract.get("durable_identifier_regex", b001["durable_identifier_grammar"]["regex"])
    )

    observed_next_issue_refs = collect_next_issue_refs()
    observed_issue_key_fields = collect_issue_key_fields()
    observed_dependency_refs = collect_dependency_refs()
    observed_m248_legacy_hits = collect_m248_legacy_hits(contract)
    observed_transitional_literals = collect_transitional_literals(contract)

    summary = {
        "observed_next_issue_ref_count": len(observed_next_issue_refs),
        "observed_next_issue_refs": observed_next_issue_refs,
        "observed_issue_key_field_count": len(observed_issue_key_fields),
        "observed_issue_key_fields": observed_issue_key_fields,
        "observed_dependency_ref_count": len(observed_dependency_refs),
        "observed_dependency_refs": observed_dependency_refs,
        "observed_m248_identifier_count": len(observed_m248_legacy_hits),
        "observed_m248_identifiers": observed_m248_legacy_hits,
        "observed_transitional_literal_count": len(observed_transitional_literals),
        "observed_transitional_literals": observed_transitional_literals,
        "residual_class_expectations": contract["residual_class_expectations"],
    }
    SUMMARY_PATH.parent.mkdir(parents=True, exist_ok=True)
    SUMMARY_PATH.write_text(json.dumps(summary, indent=2) + "\n", encoding="utf-8")

    errors: list[str] = []

    expected_counts = {
        key: value["count"]
        for key, value in b005["allowed_residual_classes"].items()
        if key in contract["residual_class_expectations"]
    }
    if expected_counts != contract["residual_class_expectations"]:
        errors.append("contract residual-class expectations drifted from M315-B005 baseline")

    if observed_next_issue_refs != contract["replacement_contracts"]["next_issue_schema_field"]["observed_legacy_refs"]:
        errors.append("observed next_issue refs drifted from frozen contract inventory")

    if observed_dependency_refs != contract["replacement_contracts"]["dependency_issue_array"]["observed_legacy_refs"]:
        errors.append("observed dependency issue refs drifted from frozen contract inventory")

    expected_issue_fields = sorted(
        contract["replacement_contracts"]["issue_key_schema_field"]["legacy_to_replacement"].keys()
    )
    if observed_issue_key_fields != expected_issue_fields:
        errors.append("observed *_issue field names drifted from frozen contract inventory")

    expected_m248_keys = sorted(
        contract["replacement_contracts"]["legacy_m248_surface_identifier"]["legacy_to_replacement"].keys()
    )
    if observed_m248_legacy_hits != expected_m248_keys:
        errors.append("observed legacy m248 identifiers drifted from frozen contract inventory")

    expected_transitional_literals = sorted(
        entry["legacy"]
        for entry in contract["replacement_contracts"]["transitional_source_model"]["legacy_literals"]
    )
    if observed_transitional_literals != expected_transitional_literals:
        errors.append("observed transitional source-model literals drifted from frozen contract inventory")

    replacement_fields = [
        contract["replacement_contracts"]["dependency_issue_array"]["replacement_field"],
        contract["replacement_contracts"]["next_issue_schema_field"]["replacement_field"],
    ]
    replacement_fields.extend(
        contract["replacement_contracts"]["issue_key_schema_field"]["legacy_to_replacement"].values()
    )
    m248_replacements = contract["replacement_contracts"]["legacy_m248_surface_identifier"][
        "legacy_to_replacement"
    ]
    replacement_fields.extend(
        value
        for key, value in m248_replacements.items()
        if key[0].islower() and ":" not in key and not key.startswith("Build")
    )

    for field in replacement_fields:
        if ":" in field:
            continue
        if not REPLACEMENT_FIELD_RE.fullmatch(field):
            errors.append(f"replacement field is not snake_case-compatible: {field}")
        if PROHIBITED_FIELD_TOKEN_RE.search(field):
            errors.append(f"replacement field still contains legacy token residue: {field}")

    durable_ids = list(contract["generated_artifact_contract"]["stable_registry_family_ids"].values())
    durable_ids.extend(
        entry["replacement"]
        for entry in contract["replacement_contracts"]["transitional_source_model"]["legacy_literals"]
    )
    for durable_id in durable_ids:
        if not durable_id_re.fullmatch(durable_id):
            errors.append(f"durable identifier does not satisfy M315-B001 grammar: {durable_id}")

    if errors:
        for error in errors:
            print(f"ERROR: {error}", file=sys.stderr)
        print(f"Summary written to {SUMMARY_PATH}", file=sys.stderr)
        return 1

    print("M315-C001 source-of-truth/generated-artifact contract summary:")
    print(json.dumps(summary, indent=2))
    print(f"Summary written to {SUMMARY_PATH}")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
