#!/usr/bin/env python3
from __future__ import annotations

import json
import subprocess
from collections import Counter, defaultdict
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

ROOT = Path(__file__).resolve().parents[1]
REPORT_DIR = ROOT / "tmp" / "reports" / "m313" / "M313-A001"
JSON_OUT = REPORT_DIR / "validation_surface_inventory.json"
MD_OUT = REPORT_DIR / "validation_surface_inventory.md"
PACKAGE_JSON = ROOT / "package.json"
WORKFLOW_RUNNER = ROOT / "scripts" / "objc3c_public_workflow_runner.py"
TASK_HYGIENE_GATE = ROOT / "scripts" / "ci" / "run_task_hygiene_gate.py"
ACCEPTANCE_HARNESS = ROOT / "scripts" / "shared_compiler_runtime_acceptance_harness.py"
CHECK_ROOTS = [ROOT / "scripts", ROOT / "tests", ROOT / "native", ROOT / "docs", ROOT / "showcase", ROOT / "stdlib"]

STATIC_GUARD_OVERRIDES: dict[str, tuple[str, str]] = {
    "scripts/ci/check_task_hygiene.py": (
        "retain:task-hygiene",
        "prevents removed script families, stale prototype roots, live bytecode, and budget drift from silently returning",
    ),
    "scripts/check_repo_superclean_surface.py": (
        "retain:repo-shape",
        "guards canonical repo roots, generated output boundaries, and build-owned source-of-truth publication",
    ),
    "scripts/check_documentation_surface.py": (
        "retain:docs-surface",
        "guards reader-facing docs, site structure, command appendix accessibility, and documented surface boundaries",
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


def repo_rel(path: Path) -> str:
    return path.relative_to(ROOT).as_posix()


def run_json(command: list[str]) -> Any:
    completed = subprocess.run(
        command,
        cwd=ROOT,
        capture_output=True,
        text=True,
        encoding="utf-8",
        errors="replace",
        check=False,
    )
    if completed.returncode != 0:
        raise RuntimeError(completed.stderr or completed.stdout or f"command failed: {' '.join(command)}")
    return json.loads(completed.stdout)


def all_check_py_files() -> list[Path]:
    files: list[Path] = []
    for root in CHECK_ROOTS:
        if root.exists():
            files.extend(path for path in root.rglob("check_*.py") if path.is_file())
    return sorted(files)


def all_test_check_py_files() -> list[Path]:
    files: list[Path] = []
    for root in CHECK_ROOTS:
        if root.exists():
            files.extend(path for path in root.rglob("test_check_*.py") if path.is_file())
    return sorted(files)


def all_validation_ps1_files() -> list[Path]:
    scripts_root = ROOT / "scripts"
    if not scripts_root.exists():
        return []
    files = [path for path in scripts_root.rglob("check_*.ps1") if path.is_file()]
    files.extend(path for path in scripts_root.rglob("run_*.ps1") if path.is_file() and "fixture_matrix" in path.name)
    return sorted(set(files))


def package_scripts() -> dict[str, str]:
    payload = json.loads(PACKAGE_JSON.read_text(encoding="utf-8"))
    return dict(payload.get("scripts", {}))


def acceptance_harness_catalog() -> dict[str, Any]:
    return run_json(["python", str(ACCEPTANCE_HARNESS), "--list-suites"])


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
    package_hits = any(basename in command or rel.replace("/", "\\") in command or rel in command for command in package.values())
    workflow_hits = basename in workflow_text or rel in workflow_text or rel.replace("/", "\\") in workflow_text
    hygiene_hits = basename in hygiene_text or rel in hygiene_text or rel.replace("/", "\\") in hygiene_text
    return {
        "package_json": package_hits,
        "workflow_runner": workflow_hits,
        "task_hygiene_gate": hygiene_hits,
    }


def main() -> int:
    REPORT_DIR.mkdir(parents=True, exist_ok=True)
    package = package_scripts()
    workflow_text = WORKFLOW_RUNNER.read_text(encoding="utf-8")
    hygiene_text = TASK_HYGIENE_GATE.read_text(encoding="utf-8")
    acceptance_catalog = acceptance_harness_catalog()

    check_files = all_check_py_files()
    test_check_files = all_test_check_py_files()
    ps1_files = all_validation_ps1_files()

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

    package_test_scripts = {name: command for name, command in package.items() if name.startswith("test:")}
    package_check_scripts = {name: command for name, command in package.items() if name.startswith("check:")}
    package_build_scripts = {name: command for name, command in package.items() if name.startswith("build:")}

    retained_static_guards = [entry for entry in inventory_entries if entry["surface_kind"] == "retained_static_guard"]
    migration_map = {
        "acceptance_first_truth": [
            "scripts/shared_compiler_runtime_acceptance_harness.py",
            "scripts/objc3c_public_workflow_runner.py validate-* and test-* actions",
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

    report = {
        "issue": "M313-A001",
        "generated_at": datetime.now(timezone.utc).isoformat(),
        "measured_counts": {
            "package_scripts_total": len(package),
            "package_test_scripts": len(package_test_scripts),
            "package_check_scripts": len(package_check_scripts),
            "package_build_scripts": len(package_build_scripts),
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
        "migration_map": migration_map,
        "retained_static_guards": retained_static_guards,
        "unreferenced_check_surfaces": unreferenced_entries,
        "acceptance_harness_catalog": acceptance_catalog,
        "validation_ps1_files": [repo_rel(path) for path in ps1_files],
        "package_script_inventory": {
            "test": sorted(package_test_scripts),
            "check": sorted(package_check_scripts),
        },
        "check_surface_inventory": inventory_entries,
        "non_goals": [
            "This inventory does not collapse or rename validation commands yet; that belongs to later M313 issues.",
            "This inventory does not delete executable validators; it classifies retained static guards versus acceptance-first truth surfaces.",
            "This inventory does not rewrite CI scheduling; that belongs to M313-D001 and M313-D002.",
        ],
        "next_issue": "M313-B001",
    }

    JSON_OUT.write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")

    lines = [
        "# M313-A001 Validation Surface Inventory",
        "",
        f"- generated_at: `{report['generated_at']}`",
        f"- package_scripts_total: `{report['measured_counts']['package_scripts_total']}`",
        f"- package_test_scripts: `{report['measured_counts']['package_test_scripts']}`",
        f"- package_check_scripts: `{report['measured_counts']['package_check_scripts']}`",
        f"- check_py_files: `{report['measured_counts']['check_py_files']}`",
        f"- test_check_py_files: `{report['measured_counts']['test_check_py_files']}`",
        f"- validation_ps1_files: `{report['measured_counts']['validation_ps1_files']}`",
        f"- shared_acceptance_harness_suite_count: `{report['measured_counts']['shared_acceptance_harness_suite_count']}`",
        f"- retained_static_guard_count: `{report['measured_counts']['retained_static_guard_count']}`",
        f"- executable_validation_count: `{report['measured_counts']['executable_validation_count']}`",
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
    lines.append("Next issue: `M313-B001`")
    MD_OUT.write_text("\n".join(lines), encoding="utf-8")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
