"""Markdown rendering for validation surface inventory reports."""

from __future__ import annotations

from typing import Any


def render_markdown(report: dict[str, Any]) -> str:
    measured = report["measured_counts"]
    class_counts = report["classification_counts"]["retention_class"]
    kind_counts = report["classification_counts"]["surface_kind"]
    migration_map = report["migration_map"]
    retained_static_guards = report["retained_static_guards"]
    unreferenced_entries = report["unreferenced_check_surfaces"]

    lines = [
        "# validation-surface-inventory Validation Surface Inventory",
        "",
        f"- generated_at: `{report['generated_at']}`",
        f"- package_bridge_count: `{measured['package_bridge_count']}`",
        f"- check_py_files: `{measured['check_py_files']}`",
        f"- test_check_py_files: `{measured['test_check_py_files']}`",
        f"- validation_ps1_files: `{measured['validation_ps1_files']}`",
        f"- shared_acceptance_harness_suite_count: `{measured['shared_acceptance_harness_suite_count']}`",
        f"- retained_static_guard_count: `{measured['retained_static_guard_count']}`",
        f"- executable_validation_count: `{measured['executable_validation_count']}`",
        "",
        "## Retained static guard classes",
    ]
    for key, value in sorted(class_counts.items()):
        if key.startswith("retain:"):
            lines.append(f"- `{key}`: `{value}`")
    lines.extend(["", "## Surface-kind counts"])
    for key, value in sorted(kind_counts.items()):
        lines.append(f"- `{key}`: `{value}`")
    lines.extend(["", "## Acceptance-first truth owners"])
    lines.extend(f"- {item}" for item in migration_map["acceptance_first_truth"])
    lines.extend(["", "## Retained static guards"])
    for entry in retained_static_guards:
        lines.append(f"- `{entry['path']}` -> `{entry['retention_class']}`: {entry['unique_value']}")
    lines.extend(["", "## Unreferenced check surfaces"])
    if unreferenced_entries:
        lines.extend(f"- `{item}`" for item in unreferenced_entries)
    else:
        lines.append("- none")
    lines.extend(["", "## Non-goals"])
    lines.extend(f"- {item}" for item in report["non_goals"])
    lines.append("")
    lines.append("Next issue: `validation-policy-summary`")
    return "\n".join(lines)
