"""Summary report rendering for runnable developer-tooling end-to-end validation."""

from __future__ import annotations

from datetime import datetime
from datetime import timezone
from pathlib import Path
from typing import Any

from objc3c_tooling.json_io import write_json_file
from objc3c_tooling.paths import normalize_rel_path
from objc3c_tooling.paths import repo_rel
from objc3c_tooling.public_workflow_output import extract_output_value
from objc3c_tooling.public_workflow_output import extract_report_paths

from .constants import REPORT_PATH
from .constants import RUNNER_PATH
from .constants import SUMMARY_CONTRACT_ID


def build_summary_payload(
    *,
    package_result: Any,
    inspect_result: Any,
    format_result: Any,
    workspace_result: Any,
    integrated_result: Any,
    manifest_path: Path,
    package_root: Path,
    dump_path_text: str,
    format_summary_path_text: str,
    workspace_path_text: str,
    integrated_summary_path_text: str,
    public_actions: Any,
    package_bridge: str,
    manifest_package_bridge: Any,
) -> dict[str, Any]:
    return {
        "contract_id": SUMMARY_CONTRACT_ID,
        "generated_at_utc": datetime.now(timezone.utc).isoformat(),
        "status": "PASS",
        "runner_path": RUNNER_PATH,
        "package_manifest_path": repo_rel(manifest_path),
        "package_root": repo_rel(package_root),
        "packaged_editor_surface_path": repo_rel(
            package_root / normalize_rel_path(str(dump_path_text))
        ),
        "packaged_formatter_summary_path": repo_rel(
            package_root / normalize_rel_path(str(format_summary_path_text))
        ),
        "packaged_workspace_path": repo_rel(
            package_root / normalize_rel_path(str(workspace_path_text))
        ),
        "packaged_integration_summary_path": repo_rel(
            package_root / normalize_rel_path(str(integrated_summary_path_text))
        ),
        "packaged_public_actions": public_actions,
        "package_bridge": package_bridge,
        "packaged_package_bridge": manifest_package_bridge,
        "child_report_paths": [
            *extract_report_paths(package_result.stdout),
            *extract_report_paths(inspect_result.stdout),
            *extract_report_paths(format_result.stdout),
            *extract_report_paths(workspace_result.stdout),
            *extract_report_paths(integrated_result.stdout),
        ],
        "steps": [
            {
                "action": "package-runnable-toolchain",
                "exit_code": package_result.returncode,
                "package_root": extract_output_value(package_result.stdout, "package_root"),
                "manifest": extract_output_value(package_result.stdout, "manifest"),
            },
            {
                "action": "inspect-editor-tooling",
                "exit_code": inspect_result.returncode,
                "dump_path": dump_path_text,
            },
            {
                "action": "format-objc3c",
                "exit_code": format_result.returncode,
                "summary_path": format_summary_path_text,
            },
            {
                "action": "materialize-playground-workspace",
                "exit_code": workspace_result.returncode,
                "workspace_path": workspace_path_text,
            },
            {
                "action": "validate-developer-tooling",
                "exit_code": integrated_result.returncode,
                "summary_path": integrated_summary_path_text,
            },
        ],
    }


def write_summary(payload: dict[str, Any]) -> None:
    REPORT_PATH.parent.mkdir(parents=True, exist_ok=True)
    write_json_file(REPORT_PATH, payload)


__all__ = ["build_summary_payload", "write_summary"]
