from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel

from .constants import SUMMARY_CONTRACT_ID
from .models import StdlibCompileResult, StdlibPackageSurface


def build_summary_payload(
    *,
    package_result: Any,
    package_root: Path,
    manifest_path: Path,
    surface: StdlibPackageSurface,
    compile_results: list[StdlibCompileResult],
) -> dict[str, Any]:
    artifact_filenames = surface.lowering_import_surface_payload["artifact_filenames"]
    manifest = surface.manifest
    command_surfaces = surface.command_surfaces
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_stdlib_foundation_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "packaged_command_surfaces": {
            "build_stdlib": command_surfaces["build_stdlib"],
            "check_stdlib_surface": command_surfaces["check_stdlib_surface"],
            "stdlib": command_surfaces["stdlib"],
            "stdlib_e2e": command_surfaces["stdlib_e2e"],
        },
        "packaged_stdlib_surface": {
            "program_contract": repo_rel(surface.stdlib_program_contract),
            "program_runbook": repo_rel(surface.stdlib_program_runbook),
            "program_site_entry": repo_rel(surface.stdlib_program_site_entry),
            "workspace_manifest": repo_rel(surface.workspace_manifest),
            "module_inventory": repo_rel(surface.module_inventory),
            "stability_policy": repo_rel(surface.stability_policy),
            "package_surface": repo_rel(surface.package_surface),
            "compatibility_gates": repo_rel(surface.compatibility_gates),
            "lowering_import_surface": repo_rel(surface.lowering_import_surface),
            "advanced_helper_package_surface": repo_rel(
                surface.advanced_helper_package_surface
            ),
            "artifact_filenames": artifact_filenames,
            "import_surface": surface.lowering_import_surface_payload["import_surface"],
        },
        "packaged_stdlib_compatibility_gates": manifest["stdlib_compatibility_gate_summary"],
        "packaged_stdlib_program_surface": {
            "publish_inputs": surface.packaged_publish_inputs,
            "command_surfaces": manifest["stdlib_program_command_surfaces"],
            "examples": manifest["stdlib_program_examples"],
        },
        "advanced_helper_surface": {
            "modules": manifest["advanced_helper_modules"],
            "command_surfaces": manifest["advanced_helper_command_surfaces"],
            "profile_gates": manifest["advanced_helper_profile_gates"],
        },
        "compile_results": [result.payload() for result in compile_results],
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
            },
            {
                "action": "compile-packaged-stdlib-smoke-sources",
                "compiled_module_count": len(compile_results),
            },
        ],
    }


def write_summary(report_path: Path, payload: dict[str, Any]) -> None:
    report_path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(report_path, payload)
    print(f"summary_path: {repo_rel(report_path)}")
    print("objc3c-runnable-stdlib-foundation: PASS")
