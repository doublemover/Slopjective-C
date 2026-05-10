from __future__ import annotations

try:
    from objc3c_tooling.json_io import require_json_object as load_json
    from objc3c_tooling.json_io import write_json_file
    from objc3c_tooling.paths import normalize_rel_path, repo_rel
    from objc3c_tooling.probe_compile import find_clangxx
    from objc3c_tooling.public_workflow_output import extract_output_value
    from objc3c_tooling.public_workflow_output import extract_report_paths
    from objc3c_tooling.subprocesses import run_capture
except ModuleNotFoundError as exc:
    if exc.name != "objc3c_tooling":
        raise
    from scripts.objc3c_tooling.json_io import require_json_object as load_json
    from scripts.objc3c_tooling.json_io import write_json_file
    from scripts.objc3c_tooling.paths import normalize_rel_path, repo_rel
    from scripts.objc3c_tooling.probe_compile import find_clangxx
    from scripts.objc3c_tooling.public_workflow_output import extract_output_value
    from scripts.objc3c_tooling.public_workflow_output import extract_report_paths
    from scripts.objc3c_tooling.subprocesses import run_capture


__all__ = [
    "extract_output_value",
    "extract_report_paths",
    "find_clangxx",
    "load_json",
    "normalize_rel_path",
    "repo_rel",
    "run_capture",
    "write_json_file",
]
