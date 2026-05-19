from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.probe_compile import find_clangxx
from objc3c_tooling.public_workflow_output import extract_output_value, extract_report_paths

from .constants import SUMMARY_CONTRACT_ID
from .models import BonusPackageSurface, ExampleRunResult


def build_summary_payload(
    *,
    package_result: Any,
    capability_probe_result: Any,
    package_root: Path,
    manifest_path: Path,
    surface: BonusPackageSurface,
    capability_report: Path,
    example_results: list[ExampleRunResult],
) -> dict[str, Any]:
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_bonus_experience_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "showcase_portfolio": repo_rel(surface.showcase_portfolio),
        "guided_walkthrough_manifest": repo_rel(surface.guided_walkthrough_manifest),
        "capability_report": repo_rel(capability_report),
        "examples": [entry.payload() for entry in example_results],
        "child_report_paths": extract_report_paths(package_result.stdout)
        + [repo_rel(capability_report)],
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
                "package_root": extract_output_value(package_result.stdout, "package_root"),
                "manifest": extract_output_value(package_result.stdout, "manifest"),
            },
            {
                "action": "probe-packaged-llvm-capabilities",
                "exit_code": capability_probe_result.returncode,
                "summary_path": repo_rel(capability_report),
            },
            {
                "action": "compile-link-run-template-derived-examples",
                "exit_code": 0,
                "clangxx": find_clangxx(),
                "example_ids": [entry.example_id for entry in example_results],
            },
        ],
    }


def write_summary(report_path: Path, payload: dict[str, Any]) -> None:
    report_path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(report_path, payload)
    print(f"summary_path: {repo_rel(report_path)}")
