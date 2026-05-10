"""Inventory model construction for validation surface reports."""

from __future__ import annotations

from collections import Counter
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.paths import repo_rel

from .classification import classify_check_py, referenced_by
from .discovery import (
    acceptance_harness_catalog,
    all_check_py_files,
    all_test_check_py_files,
    all_validation_ps1_files,
    package_scripts,
    workflow_backend_text,
)
from .filtering import retained_static_guards as filter_retained_static_guards
from .paths import TASK_HYGIENE_GATE


def migration_map() -> dict[str, Any]:
    return {
        "acceptance_first_truth": [
            "scripts/shared_compiler_runtime_acceptance_harness.py",
            "scripts.objc3c_workflow validate-* and test-* actions",
            "scripts/check_objc3c_*_integration.py and scripts/check_objc3c_runnable_*_end_to_end.py executable flows",
            "PowerShell runtime suites such as scripts/check_objc3c_native_execution_smoke.ps1, scripts/check_objc3c_execution_replay_proof.ps1, and scripts/check_objc3c_native_recovery_contract.ps1",
        ],
        "retained_static_guard_classes": [
            "retain:task-hygiene",
            "retain:repo-shape",
            "retain:docs-surface",
            "retain:product-surface",
            "retain:source-surface-contract",
            "retain:schema-contract",
        ],
        "legacy_namespace_target": "legacy checker surfaces should be classified as active, migration-only, archival, or prohibited in generated reports rather than presented as primary truth",
    }


def non_goals() -> list[str]:
    return [
        "This inventory does not collapse or rename validation commands yet; that belongs to later validation-consolidation steps.",
        "This inventory does not delete executable validators; it classifies retained static guards versus acceptance-first truth surfaces.",
        "This inventory does not rewrite CI scheduling; that belongs to validation-ci-topology and validation-ci-topology-integration.",
    ]


def build_inventory_entries(
    check_files: list[Path],
    *,
    package: dict[str, str],
    workflow_text: str,
    hygiene_text: str,
) -> tuple[list[dict[str, Any]], Counter[str], Counter[str], list[str]]:
    inventory_entries: list[dict[str, Any]] = []
    class_counts: Counter[str] = Counter()
    kind_counts: Counter[str] = Counter()
    unreferenced_entries: list[str] = []

    for path in check_files:
        rel = repo_rel(path)
        classification = classify_check_py(rel)
        refs = referenced_by(package, workflow_text, hygiene_text, rel)
        if not any(refs.values()):
            unreferenced_entries.append(rel)
        entry = {
            "path": rel,
            **classification,
            "references": refs,
        }
        inventory_entries.append(entry)
        class_counts[classification["retention_class"]] += 1
        kind_counts[classification["surface_kind"]] += 1

    return inventory_entries, class_counts, kind_counts, unreferenced_entries


def build_report(*, generated_at: str | None = None) -> dict[str, Any]:
    package = package_scripts()
    workflow_text = workflow_backend_text()
    hygiene_text = TASK_HYGIENE_GATE.read_text(encoding="utf-8")
    acceptance_catalog = acceptance_harness_catalog()

    check_files = all_check_py_files()
    test_check_files = all_test_check_py_files()
    ps1_files = all_validation_ps1_files()

    inventory_entries, class_counts, kind_counts, unreferenced_entries = build_inventory_entries(
        check_files,
        package=package,
        workflow_text=workflow_text,
        hygiene_text=hygiene_text,
    )

    package_bridge = "objc3c"
    package_bridge_count = 1 if package_bridge in package else 0
    retained_static_guards = filter_retained_static_guards(inventory_entries)

    return {
        "issue": "validation-surface-inventory",
        "generated_at": generated_at or datetime.now(timezone.utc).isoformat(),
        "measured_counts": {
            "package_bridge_count": package_bridge_count,
            "check_py_files": len(check_files),
            "test_check_py_files": len(test_check_files),
            "validation_ps1_files": len(ps1_files),
            "shared_acceptance_harness_suite_count": acceptance_catalog.get("suite_count", 0),
            "retained_static_guard_count": len(retained_static_guards),
            "executable_validation_count": len(check_files) - len(retained_static_guards),
        },
        "classification_counts": {
            "surface_kind": dict(sorted(kind_counts.items())),
            "retention_class": dict(sorted(class_counts.items())),
        },
        "migration_map": migration_map(),
        "retained_static_guards": retained_static_guards,
        "unreferenced_check_surfaces": unreferenced_entries,
        "acceptance_harness_catalog": acceptance_catalog,
        "validation_ps1_files": [repo_rel(path) for path in ps1_files],
        "package_bridge": package_bridge if package_bridge_count else "",
        "check_surface_inventory": inventory_entries,
        "non_goals": non_goals(),
        "next_issue": "validation-policy-summary",
    }
