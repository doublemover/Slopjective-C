from __future__ import annotations

import json
from collections import Counter
from collections.abc import Callable
from dataclasses import dataclass
from typing import Any

from objc3c_tooling.reports import expected_json_report, markdown_table

SUMMARY_CONTRACT_ID = "objc3c.support.classification.generator.summary.v1"
SUMMARY_SCRIPT_PATH = "scripts/build_objc3c_support_classification.py"

CANONICAL_SUPPORT_CLASSES = {
    "supported": {
        "claim_class": "production-claimable",
        "fail_closed": False,
        "release_blocking": False,
    },
    "experimental": {
        "claim_class": "preview-or-candidate-only",
        "fail_closed": False,
        "release_blocking": False,
    },
    "unsupported": {
        "claim_class": "fail-closed",
        "fail_closed": True,
        "release_blocking": False,
    },
    "release-blocking": {
        "claim_class": "blocked",
        "fail_closed": True,
        "release_blocking": True,
    },
}


class ContractError(RuntimeError):
    pass


@dataclass(frozen=True)
class SupportClassificationReport:
    summary: dict[str, Any]
    markdown: str
    json_report: str
    console_json: str
    status: str


@dataclass(frozen=True)
class SupportClassificationSource:
    contract: dict[str, Any]
    contract_path: str
    contract_sha256: str


def require_list(contract: dict[str, Any], key: str) -> list[Any]:
    value = contract.get(key)
    if not isinstance(value, list):
        raise ContractError(f"contract field `{key}` must be a list")
    return value


def require_string(value: Any, label: str) -> str:
    if not isinstance(value, str) or not value:
        raise ContractError(f"{label} must be a non-empty string")
    return value


def validate_support_classes(contract: dict[str, Any]) -> set[str]:
    rows = require_list(contract, "support_classes")
    classes: set[str] = set()
    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ContractError(f"support_classes[{index}] must be an object")
        support_class = require_string(
            row.get("class"),
            f"support_classes[{index}].class",
        )
        require_string(row.get("meaning"), f"support_classes[{index}].meaning")
        if support_class not in CANONICAL_SUPPORT_CLASSES:
            raise ContractError(
                f"unknown support class `{support_class}` in support_classes[{index}]"
            )
        classes.add(support_class)
    missing = set(CANONICAL_SUPPORT_CLASSES) - classes
    if missing:
        raise ContractError(f"missing canonical support classes: {sorted(missing)}")
    return classes


def validate_evidence_families(contract: dict[str, Any]) -> set[str]:
    rows = require_list(contract, "evidence_families")
    families: set[str] = set()
    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ContractError(f"evidence_families[{index}] must be an object")
        family = require_string(row.get("family"), f"evidence_families[{index}].family")
        require_string(row.get("report"), f"evidence_families[{index}].report")
        require_string(
            row.get("required_contract_id"),
            f"evidence_families[{index}].required_contract_id",
        )
        if family in families:
            raise ContractError(f"duplicate evidence family `{family}`")
        families.add(family)
    return families


def normalize_checked_in_path(
    path_text: str,
    *,
    label: str,
    path_exists: Callable[[str], bool],
) -> str:
    if path_text.startswith("tmp/") or path_text.startswith("tmp\\"):
        raise ContractError(f"{label} must not use tmp as source truth")
    if not path_exists(path_text):
        raise ContractError(f"missing checked-in surface path `{path_text}`")
    return path_text.replace("\\", "/")


def validate_surface_paths(
    paths: list[Any],
    row_label: str,
    *,
    path_exists: Callable[[str], bool],
) -> list[str]:
    out: list[str] = []
    for index, raw_path in enumerate(paths):
        path_text = require_string(raw_path, f"{row_label}.checked_in_surfaces[{index}]")
        out.append(
            normalize_checked_in_path(
                path_text,
                label=f"{row_label}.checked_in_surfaces[{index}]",
                path_exists=path_exists,
            )
        )
    return out


def classify_surfaces(
    contract: dict[str, Any],
    support_classes: set[str],
    evidence_families: set[str],
    *,
    path_exists: Callable[[str], bool],
) -> list[dict[str, Any]]:
    rows = require_list(contract, "support_matrix")
    classifications: list[dict[str, Any]] = []
    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ContractError(f"support_matrix[{index}] must be an object")
        row_label = f"support_matrix[{index}]"
        surface = require_string(row.get("surface"), f"{row_label}.surface")
        current_class = require_string(
            row.get("current_class"),
            f"{row_label}.current_class",
        )
        if current_class not in support_classes:
            raise ContractError(
                f"{row_label}.current_class `{current_class}` is not a defined support class"
            )
        checked_in_surfaces = validate_surface_paths(
            require_list(row, "checked_in_surfaces"),
            row_label,
            path_exists=path_exists,
        )
        required_families = [
            require_string(
                family,
                f"{row_label}.required_evidence_families[{family_index}]",
            )
            for family_index, family in enumerate(
                require_list(row, "required_evidence_families")
            )
        ]
        unknown_families = sorted(set(required_families) - evidence_families)
        if unknown_families:
            raise ContractError(
                f"{row_label} references unknown evidence families: {unknown_families}"
            )
        policy = CANONICAL_SUPPORT_CLASSES[current_class]
        classifications.append(
            {
                "surface": surface,
                "current_class": current_class,
                "claim_class": policy["claim_class"],
                "fail_closed": policy["fail_closed"],
                "release_blocking": policy["release_blocking"],
                "required_evidence_families": required_families,
                "checked_in_surfaces": checked_in_surfaces,
            }
        )
    return classifications


def validate_demotion_triggers(
    contract: dict[str, Any],
    support_classes: set[str],
    evidence_families: set[str],
) -> list[dict[str, Any]]:
    rows = require_list(contract, "demotion_triggers")
    triggers: list[dict[str, Any]] = []
    for index, row in enumerate(rows):
        if not isinstance(row, dict):
            raise ContractError(f"demotion_triggers[{index}] must be an object")
        trigger = require_string(
            row.get("trigger"),
            f"demotion_triggers[{index}].trigger",
        )
        demotes_to = require_string(
            row.get("demotes_to"), f"demotion_triggers[{index}].demotes_to"
        )
        if demotes_to not in support_classes:
            raise ContractError(
                f"demotion trigger `{trigger}` targets unknown class `{demotes_to}`"
            )
        required_families = [
            require_string(family, f"demotion_triggers[{index}].required_families[{i}]")
            for i, family in enumerate(require_list(row, "required_families"))
        ]
        unknown_families = sorted(set(required_families) - evidence_families)
        if unknown_families:
            raise ContractError(
                f"demotion trigger `{trigger}` references unknown evidence families: {unknown_families}"
            )
        triggers.append(
            {
                "trigger": trigger,
                "demotes_to": demotes_to,
                "required_families": required_families,
            }
        )
    return triggers


def validate_public_claim_surfaces(
    contract: dict[str, Any],
    *,
    path_exists: Callable[[str], bool],
) -> list[str]:
    surfaces: list[str] = []
    for index, raw_path in enumerate(require_list(contract, "public_claim_surfaces")):
        path_text = require_string(raw_path, f"public_claim_surfaces[{index}]")
        if path_text.startswith("tmp/") or path_text.startswith("tmp\\"):
            raise ContractError(
                f"public_claim_surfaces[{index}] must not use tmp as source truth"
            )
        if not path_exists(path_text):
            raise ContractError(f"missing public claim surface `{path_text}`")
        surfaces.append(path_text.replace("\\", "/"))
    return surfaces


def require_existing_contract_path(
    contract: dict[str, Any],
    *,
    key: str,
    path_exists: Callable[[str], bool],
) -> str:
    path_text = require_string(contract.get(key), key)
    if not path_exists(path_text):
        raise ContractError(f"{key} path is missing: `{path_text}`")
    return path_text.replace("\\", "/")


def build_support_classification_summary(
    source: SupportClassificationSource,
    *,
    path_exists: Callable[[str], bool],
) -> dict[str, Any]:
    contract = source.contract
    support_classes = validate_support_classes(contract)
    evidence_families = validate_evidence_families(contract)
    classifications = classify_surfaces(
        contract,
        support_classes,
        evidence_families,
        path_exists=path_exists,
    )
    demotion_triggers = validate_demotion_triggers(
        contract, support_classes, evidence_families
    )
    public_claim_surfaces = validate_public_claim_surfaces(
        contract,
        path_exists=path_exists,
    )

    runbook = require_existing_contract_path(
        contract,
        key="runbook",
        path_exists=path_exists,
    )
    legacy_summary_script = require_existing_contract_path(
        contract,
        key="summary_script",
        path_exists=path_exists,
    )

    class_counts = Counter(row["current_class"] for row in classifications)
    source_truth_paths = sorted(
        {
            source.contract_path,
            runbook,
            SUMMARY_SCRIPT_PATH,
            legacy_summary_script,
            *public_claim_surfaces,
            *(
                surface
                for row in classifications
                for surface in row["checked_in_surfaces"]
            ),
        }
    )
    tmp_source_truth_paths = [
        path_text
        for path_text in source_truth_paths
        if path_text.startswith("tmp/") or path_text.startswith("tmp\\")
    ]
    fail_closed_surfaces = [
        row["surface"] for row in classifications if row["fail_closed"]
    ]
    release_blocking_surfaces = [
        row["surface"] for row in classifications if row["release_blocking"]
    ]

    checks = {
        "canonical_support_classes_defined": set(CANONICAL_SUPPORT_CLASSES)
        == support_classes,
        "all_classifications_use_known_classes": all(
            row["current_class"] in support_classes for row in classifications
        ),
        "all_required_evidence_families_are_declared": all(
            set(row["required_evidence_families"]).issubset(evidence_families)
            for row in classifications
        ),
        "demotion_targets_are_known_classes": all(
            row["demotes_to"] in support_classes for row in demotion_triggers
        ),
        "source_truth_excludes_tmp": not tmp_source_truth_paths,
        "has_supported_surface": class_counts["supported"] > 0,
        "has_fail_closed_surface": bool(fail_closed_surfaces),
    }
    status = "PASS" if all(checks.values()) else "FAIL"

    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "status": status,
        "source_contract_id": contract["contract_id"],
        "source_contract_path": source.contract_path,
        "source_contract_sha256": source.contract_sha256,
        "runbook": runbook,
        "summary_script": SUMMARY_SCRIPT_PATH,
        "legacy_support_matrix_summary_script": legacy_summary_script,
        "support_classes": sorted(support_classes),
        "support_class_count": len(support_classes),
        "evidence_families": sorted(evidence_families),
        "evidence_family_count": len(evidence_families),
        "classification_count": len(classifications),
        "class_counts": dict(sorted(class_counts.items())),
        "classifications": classifications,
        "demotion_triggers": demotion_triggers,
        "public_claim_surfaces": public_claim_surfaces,
        "source_truth_paths": source_truth_paths,
        "tmp_source_truth_paths": tmp_source_truth_paths,
        "fail_closed_surfaces": fail_closed_surfaces,
        "release_blocking_surfaces": release_blocking_surfaces,
        "checks": checks,
    }


def render_support_classification_markdown(summary: dict[str, Any]) -> str:
    rows = markdown_table(
        ["Surface", "Class", "Claim Class", "Required Evidence"],
        [
            [
                row["surface"],
                row["current_class"],
                row["claim_class"],
                ", ".join(row["required_evidence_families"]) or "none",
            ]
            for row in summary["classifications"]
        ],
    )
    classification_table = "\n".join(rows)
    return (
        "# Objective-C 3.0 Support Classification Summary\n\n"
        f"- Contract: `{summary['source_contract_id']}`\n"
        f"- Status: `{summary['status']}`\n"
        f"- Support classes: `{summary['support_class_count']}`\n"
        f"- Evidence families: `{summary['evidence_family_count']}`\n"
        f"- Classified surfaces: `{summary['classification_count']}`\n"
        f"- Source truth excludes tmp: `{summary['checks']['source_truth_excludes_tmp']}`\n\n"
        "## Class Counts\n\n"
        + "\n".join(
            f"- `{key}`: `{value}`" for key, value in summary["class_counts"].items()
        )
        + "\n\n"
        "## Surface Classifications\n\n"
        f"{classification_table}\n"
    )


def build_support_classification_report(
    source: SupportClassificationSource,
    *,
    path_exists: Callable[[str], bool],
) -> SupportClassificationReport:
    summary = build_support_classification_summary(source, path_exists=path_exists)
    markdown = render_support_classification_markdown(summary)
    return SupportClassificationReport(
        summary=summary,
        markdown=markdown,
        json_report=expected_json_report(summary, sort_keys=False),
        console_json=json.dumps(summary, indent=2),
        status=str(summary["status"]),
    )
