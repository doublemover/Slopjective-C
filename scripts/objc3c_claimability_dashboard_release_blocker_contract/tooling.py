from __future__ import annotations

try:
    from objc3c_tooling.paths import display_path as repo_rel
    from objc3c_tooling.reports import expected_json_report, write_report_outputs
except ModuleNotFoundError:
    from scripts.objc3c_tooling.paths import display_path as repo_rel
    from scripts.objc3c_tooling.reports import (
        expected_json_report,
        write_report_outputs,
    )

__all__ = ["expected_json_report", "repo_rel", "write_report_outputs"]
