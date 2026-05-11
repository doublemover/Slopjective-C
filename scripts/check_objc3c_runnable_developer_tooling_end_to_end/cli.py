"""CLI orchestration for runnable developer-tooling end-to-end validation."""

from __future__ import annotations

from datetime import datetime

from objc3c_tooling.json_io import require_json_object as load_json
from objc3c_tooling.paths import repo_rel

from .commands import run_package_command
from .constants import CONTRACT_PATH
from .constants import REPORT_PATH
from .constants import ROOT
from .manifest import load_and_validate_manifest
from .paths import package_path
from .summary import build_summary_payload
from .summary import write_summary
from .tooling import run_format_check
from .tooling import run_inspect_editor_tooling_check
from .tooling import run_integrated_validation_check
from .tooling import run_workspace_check


def main() -> int:
    contract = load_json(CONTRACT_PATH)
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-developer-tooling-e2e" / run_id
    manifest_path = (
        package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    )

    package_result = run_package_command(package_root)
    if package_result.returncode != 0:
        raise RuntimeError("runnable toolchain package command failed")

    (
        manifest,
        public_actions,
        package_bridge,
        manifest_package_bridge,
    ) = load_and_validate_manifest(
        contract=contract,
        package_root=package_root,
        manifest_path=manifest_path,
    )

    hello_source = package_path(
        package_root,
        str(manifest["developer_tooling_example_source"]),
    )
    format_source = package_path(
        package_root,
        str(manifest["developer_tooling_formatter_source"]),
    )
    expected_formatted_source = package_path(
        package_root,
        str(manifest["developer_tooling_expected_formatted_source"]),
    )

    inspect_result, dump_path_text = run_inspect_editor_tooling_check(
        package_root=package_root,
        hello_source=hello_source,
        contract=contract,
    )
    format_result, format_summary_path_text = run_format_check(
        package_root=package_root,
        format_source=format_source,
        expected_formatted_source=expected_formatted_source,
    )
    workspace_result, workspace_path_text = run_workspace_check(
        package_root=package_root,
        hello_source=hello_source,
        contract=contract,
    )
    integrated_result, integrated_summary_path_text = run_integrated_validation_check(
        package_root=package_root,
    )

    payload = build_summary_payload(
        package_result=package_result,
        inspect_result=inspect_result,
        format_result=format_result,
        workspace_result=workspace_result,
        integrated_result=integrated_result,
        manifest_path=manifest_path,
        package_root=package_root,
        dump_path_text=dump_path_text,
        format_summary_path_text=format_summary_path_text,
        workspace_path_text=workspace_path_text,
        integrated_summary_path_text=integrated_summary_path_text,
        public_actions=public_actions,
        package_bridge=package_bridge,
        manifest_package_bridge=manifest_package_bridge,
    )
    write_summary(payload)
    print(f"summary_path: {repo_rel(REPORT_PATH)}")
    return 0


__all__ = ["main"]
