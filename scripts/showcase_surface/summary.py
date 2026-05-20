from __future__ import annotations

from pathlib import Path
from typing import Any

from .paths import ROOT, repo_relative


def required_showcase_artifacts(
    entry: dict[str, object],
    out_dir: Path,
) -> dict[str, Path]:
    return {
        "workspace_manifest": ROOT / str(entry["workspace_manifest"]),
        "compile_provenance": out_dir / "module.compile-provenance.json",
        "llvm_ir": out_dir / "module.ll",
        "object": out_dir / "module.obj",
        "manifest": out_dir / "module.manifest.json",
        "runtime_registration_manifest": out_dir / "module.runtime-registration-manifest.json",
    }


def build_compile_result(
    *,
    entry: dict[str, object],
    demo_package: dict[str, Any],
    module_name: str,
    source: str,
    workspace_payload: dict[str, Any],
    out_dir: Path,
    required_artifacts: dict[str, Path],
) -> dict[str, object]:
    return {
        "example_id": entry["id"],
        "module_name": module_name,
        "source": source,
        "workspace_manifest": str(entry["workspace_manifest"]),
        "story_capabilities": list(entry.get("story_capabilities", [])),
        "stdlib_followup_modules": list(entry.get("stdlib_followup_modules", [])),
        "demo_package": {
            "package_id": demo_package["package_id"],
            "smoke_id": demo_package["smoke_id"],
            "coverage_domain": demo_package["coverage_domain"],
            "expected_exit_code": demo_package["expected_exit_code"],
            "runtime_backed_behavior": list(demo_package["runtime_backed_behavior"]),
            "manifest_inputs": list(demo_package["manifest_inputs"]),
        },
        "presentation": dict(workspace_payload.get("presentation", {})),
        "out_dir": repo_relative(out_dir),
        "artifacts": {
            label: repo_relative(path) for label, path in required_artifacts.items()
        },
    }


def build_summary_payload(
    *,
    portfolio_payload: dict[str, Any],
    demo_packages_payload: dict[str, Any],
    selected_examples: list[dict[str, object]],
    compile_results: list[dict[str, object]],
) -> dict[str, object]:
    packages = [
        entry
        for entry in demo_packages_payload.get("packages", [])
        if isinstance(entry, dict)
    ]
    return {
        "contract_id": "objc3c.showcase.surface.summary.v1",
        "schema_version": 1,
        "portfolio_contract_id": portfolio_payload["contract_id"],
        "demo_packages_contract_id": demo_packages_payload["contract_id"],
        "demo_packages_manifest": portfolio_payload["demo_packages_manifest"],
        "demo_package_ids": [entry["package_id"] for entry in packages],
        "demo_package_coverage_domains": [entry["coverage_domain"] for entry in packages],
        "demo_package_reproducibility": demo_packages_payload["reproducibility_contract"],
        "showcase_root": portfolio_payload["showcase_root"],
        "machine_output_root": portfolio_payload["machine_output_root"],
        "machine_report_root": portfolio_payload["machine_report_root"],
        "package_stage_root": portfolio_payload["package_stage_root"],
        "selected_example_ids": [
            entry["id"] for entry in selected_examples if isinstance(entry.get("id"), str)
        ],
        "examples": compile_results,
    }
