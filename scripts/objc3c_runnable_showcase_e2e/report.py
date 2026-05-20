from __future__ import annotations

from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from .constants import SUMMARY_CONTRACT_ID
from .models import ShowcaseExecutionResult, ShowcasePackageSurface
from .tooling import (
    extract_output_value,
    extract_report_paths,
    repo_rel,
    write_json_file,
)


def build_summary_payload(
    *,
    package_result: Any,
    package_root: Path,
    manifest_path: Path,
    surface: ShowcasePackageSurface,
    execution_result: ShowcaseExecutionResult,
) -> dict[str, Any]:
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": "scripts/check_objc3c_runnable_showcase_end_to_end.py",
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "showcase_portfolio": repo_rel(surface.showcase_portfolio),
        "showcase_readme": repo_rel(surface.showcase_readme),
        "showcase_demo_packages_manifest": repo_rel(
            surface.showcase_demo_packages_manifest
        ),
        "showcase_demo_package_ids": [
            entry.get("package_id")
            for entry in surface.showcase_demo_packages
            if isinstance(entry, dict)
        ],
        "examples": [entry.payload() for entry in execution_result.examples],
        "child_report_paths": extract_report_paths(package_result.stdout),
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
                "package_root": extract_output_value(package_result.stdout, "package_root"),
                "manifest": extract_output_value(package_result.stdout, "manifest"),
            },
            {
                "action": "compile-link-run-packaged-showcase",
                "exit_code": 0,
                "clangxx": execution_result.clangxx,
            },
        ],
    }


def write_summary(report_path: Path, payload: dict[str, Any]) -> None:
    report_path.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(report_path, payload)
    print(f"summary_path: {repo_rel(report_path)}")


__all__ = ["build_summary_payload", "write_summary"]
