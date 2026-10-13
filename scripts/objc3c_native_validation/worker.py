"""Execute one selected native node and publish its explicit artifact inventory."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
import re
import sys
from pathlib import Path
from typing import Any

from scripts.objc3c_native_validation.artifacts import read_node_result, write_node_result
from scripts.objc3c_native_validation.identity import ENVIRONMENT_KEYS


def apply_worker_environment(environment: dict[str, str]) -> None:
    if not isinstance(environment, dict) or any(not isinstance(key, str) or not isinstance(value, str)
                                               for key, value in environment.items()):
        raise ValueError("snapshot environment must contain string keys and values")
    # These optional driver inputs are deliberately outside the admitted
    # command lines. Do not let the caller silently inject flags/search paths.
    ambient_inputs = {"CL", "_CL_", "LINK", "CPATH", "C_INCLUDE_PATH", "CPLUS_INCLUDE_PATH",
                      "OBJC_INCLUDE_PATH", "LIBRARY_PATH", "COMPILER_PATH", "GCC_EXEC_PREFIX",
                      "CCC_OVERRIDE_OPTIONS", "CCC_ADD_ARGS", "SDKROOT", "MACOSX_DEPLOYMENT_TARGET"}
    replaced = ambient_inputs | {key.upper() for key in ENVIRONMENT_KEYS}
    for key in list(os.environ):
        if key.upper() in replaced:
            del os.environ[key]
    os.environ.update(environment)


def node_directory(run_dir: Path, node_id: str) -> Path:
    name = re.sub(r"[^A-Za-z0-9._-]", "-", node_id)
    suffix = hashlib.sha256(node_id.encode()).hexdigest()[:8]
    return run_dir / "nodes" / f"{name}-{suffix}"


def result_path(run_dir: Path, node_id: str) -> Path:
    return node_directory(run_dir, node_id) / "result.json"


def run_editor_node(repo: Path, output_dir: Path, toolchain: dict[str, Any],
                    timeout: float) -> dict[str, Any]:
    from scripts.objc3c_cross_lane_execution import run_node_command
    work = output_dir / "editor"
    report_path = work / "results.json"
    command = [sys.executable, "-B", "-m", "scripts.objc3c_editor_tooling.qualification",
               "--repo", str(repo), "--work-dir", str(work), "--report", str(report_path)]
    log = output_dir / "editor.log"
    execution = run_node_command({"node_id": "editor:qualification"}, command,
                                 cwd=repo, log_path=log, timeout_seconds=timeout)
    if execution["exit_code"] or execution["timed_out"]:
        raise RuntimeError(f"editor qualification failed; see {log}")
    report = json.loads(report_path.read_text(encoding="utf-8-sig"))
    if report.get("status") != "passed" or report.get("build_identity") != toolchain["sdk_build_id"]:
        raise ValueError("editor qualification did not publish a passing result for this SDK")
    artifacts = {"execution_log": log}
    artifacts.update({"editor/" + path.relative_to(work).as_posix(): path
                      for path in sorted(work.rglob("*")) if path.is_file()})
    return {"artifacts": artifacts,
            "data": {"runtime_count": 0, "editor_cases": len(report["results"]),
                     "command": command, "report": str(report_path)}}


def execute_worker(context_path: Path, node_id: str) -> None:
    context_path = context_path.resolve(strict=True)
    context = json.loads(context_path.read_text(encoding="utf-8"))
    if context.get("contract_id") != "objc3c.native_validation.context.v1":
        raise ValueError("unsupported native validation context")
    repo = Path(context["repo"]).resolve(strict=True)
    run_dir = context_path.parent
    if not run_dir.is_relative_to(repo) or run_dir.name != context["run_id"]:
        raise ValueError("native validation run must be isolated within the repository")
    nodes = {node["node_id"]: node for node in context["plan"]["nodes"]}
    if node_id not in context["plan"]["selected_node_ids"]:
        raise ValueError("worker node is not selected")
    node = dict(nodes[node_id])
    node["timeout_seconds"] = context["timeout_seconds"]
    output_dir = node_directory(run_dir, node_id)
    output_dir.mkdir(parents=True, exist_ok=False)
    toolchain = context["toolchain"]
    # This worker is a private process. All descendants, including editor
    # tools, must inherit the environment whose native inputs were admitted.
    apply_worker_environment(toolchain.get("environment", {}))
    identities = context["node_identities"]
    dependencies = {
        dependency: read_node_result(result_path(run_dir, dependency), node_id=dependency,
                                      node_identity=identities[dependency],
                                      build_identity=toolchain["build_identity"],
                                      allowed_roots=[repo])
        for dependency in node.get("depends_on", [])
    }
    operation = node["operation"]
    if operation == "fixture":
        from scripts.objc3c_native_validation.fixture_nodes import run_fixture_node
        result = run_fixture_node(repo, node, output_dir, toolchain)
    elif operation in ("package-provider", "package-consumer"):
        from scripts.objc3c_native_validation.package_nodes import run_package_node
        result = run_package_node(repo, node, output_dir, toolchain, dependencies)
    elif operation == "editor":
        result = run_editor_node(repo, output_dir, toolchain, context["timeout_seconds"])
    else:
        raise ValueError(f"unknown native validation operation: {operation}")
    write_node_result(result_path(run_dir, node_id), node_id=node_id,
                      node_identity=identities[node_id],
                      build_identity=toolchain["build_identity"], run_id=context["run_id"],
                      artifacts=result["artifacts"], data=result["data"], allowed_roots=[repo])
    print(f"Native validation passed: {node_id}", flush=True)


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--context", required=True, type=Path)
    parser.add_argument("--node", required=True)
    args = parser.parse_args()
    try:
        execute_worker(args.context, args.node)
        return 0
    except Exception as error:
        print(f"Native validation failed: {args.node}: {error}", file=sys.stderr, flush=True)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
