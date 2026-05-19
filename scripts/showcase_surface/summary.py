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
        "presentation": dict(workspace_payload.get("presentation", {})),
        "out_dir": repo_relative(out_dir),
        "artifacts": {
            label: repo_relative(path) for label, path in required_artifacts.items()
        },
    }


def build_summary_payload(
    *,
    portfolio_payload: dict[str, Any],
    selected_examples: list[dict[str, object]],
    compile_results: list[dict[str, object]],
) -> dict[str, object]:
    return {
        "contract_id": "objc3c.showcase.surface.summary.v1",
        "schema_version": 1,
        "portfolio_contract_id": portfolio_payload["contract_id"],
        "showcase_root": portfolio_payload["showcase_root"],
        "machine_output_root": portfolio_payload["machine_output_root"],
        "machine_report_root": portfolio_payload["machine_report_root"],
        "package_stage_root": portfolio_payload["package_stage_root"],
        "selected_example_ids": [
            entry["id"] for entry in selected_examples if isinstance(entry.get("id"), str)
        ],
        "examples": compile_results,
    }
