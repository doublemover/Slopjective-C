"""Select and run native compiler fixtures, compiled packages, and editor checks."""
from __future__ import annotations

import argparse
import copy
import hashlib
import json
import math
import os
import sys
import tempfile
from datetime import datetime, timezone
from pathlib import Path
from typing import Any

from scripts.objc3c_cross_lane_execution import (
    compute_node_identities, execute_node_plan, validate_reuse_report,
)
from scripts.objc3c_cross_lane_selection import safe_run_id
from scripts.objc3c_native_validation.artifacts import (
    NodeResultError, read_node_result, write_node_result,
)
from scripts.objc3c_native_validation.identity import resolve_toolchain, verify_toolchain_snapshot
from scripts.objc3c_native_validation.plan import build_plan
from scripts.objc3c_native_validation.worker import result_path


def write_json(path: Path, payload: Any) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    descriptor, temporary = tempfile.mkstemp(prefix=".native-", suffix=".json", dir=path.parent)
    try:
        with os.fdopen(descriptor, "w", encoding="utf-8", newline="\n") as stream:
            json.dump(payload, stream, indent=2, sort_keys=True)
            stream.write("\n")
        os.replace(temporary, path)
    finally:
        if os.path.exists(temporary):
            os.unlink(temporary)


def inventory_identity(plan: dict[str, Any]) -> str:
    # Selection changes do not invalidate independent completed nodes. Source
    # contents enter each node identity, not this full graph-shape identity.
    encoded = json.dumps(plan["nodes"], sort_keys=True, separators=(",", ":")).encode()
    return hashlib.sha256(encoded).hexdigest()


def prepare_reuse(report: dict[str, Any], *, repo: Path, plan: dict[str, Any],
                  identities: dict[str, str], toolchain: dict[str, Any],
                  inventory: str, run_id: str) -> tuple[dict[str, Any], dict[str, str]]:
    if (not isinstance(report, dict) or report.get("native_validation_schema") != 1 or
            report.get("validation_complete") is not True):
        raise ValueError("native reuse report has no completed artifact, source, and toolchain validation")
    candidates = validate_reuse_report(
        report, plan=plan, node_identities=identities,
        build_identity=toolchain["build_identity"], inventory_identity=inventory,
        current_run_id=run_id)
    invalid: dict[str, str] = {}
    for node_id, prior in candidates.items():
        try:
            path = Path(prior.get("result_path", ""))
            if not path.is_absolute() or not path.resolve().is_relative_to(repo):
                raise NodeResultError("reuse node result is missing or outside the repository")
            saved = read_node_result(path, node_id=node_id, node_identity=identities[node_id],
                                     build_identity=toolchain["build_identity"], allowed_roots=[repo])
            if saved["run_id"] != report["run_id"]:
                raise NodeResultError("reuse node result belongs to another run")
        except (NodeResultError, OSError, ValueError) as error:
            invalid[node_id] = str(error)
    for node in plan["nodes"]:
        damaged = [dependency for dependency in node.get("depends_on", []) if dependency in invalid]
        if damaged:
            invalid[node["node_id"]] = "dependency artifacts invalidated: " + ", ".join(damaged)
    prepared = copy.deepcopy(report)
    for prior in prepared["node_records"]:
        if prior["node_id"] in invalid:
            # The shared executor reruns identity mismatches. Preserve the
            # original complete report on disk and never label corrupt data reused.
            prior["node_identity"] = "invalidated-artifacts"
    return prepared, invalid


def run_validation(*, repo: Path, plan: dict[str, Any], output_root: Path,
                   run_id: str, timeout_seconds: float, reuse_path: Path | None = None,
                   pinned_build_identity: str | None = None) -> dict[str, Any]:
    repo = repo.resolve(strict=True)
    output_root = output_root.resolve()
    if output_root == repo or not output_root.is_relative_to(repo):
        raise ValueError("output root must be a dedicated directory within the repository")
    safe_run_id(run_id)
    if not math.isfinite(timeout_seconds) or timeout_seconds <= 0 or timeout_seconds > 86400:
        raise ValueError("node timeout must be finite, positive and at most 24 hours")
    run_dir = output_root / run_id
    if run_dir.exists():
        raise ValueError(f"run directory already exists: {run_dir}")
    toolchain = resolve_toolchain(repo, require_editor="editor" in plan["selected_families"])
    if pinned_build_identity and toolchain["build_identity"] != pinned_build_identity:
        raise ValueError("pinned build identity does not match the selected native toolchain")
    inventory = inventory_identity(plan)
    identities = compute_node_identities(plan, input_root=repo,
                                         build_identity=toolchain["build_identity"],
                                         inventory_identity=inventory)
    previous = None
    invalidated: dict[str, str] = {}
    if reuse_path is not None:
        previous, invalidated = prepare_reuse(
            json.loads(reuse_path.read_text(encoding="utf-8-sig")), repo=repo,
            plan=plan, identities=identities, toolchain=toolchain,
            inventory=inventory, run_id=run_id)
    context = {"contract_id": "objc3c.native_validation.context.v1", "repo": str(repo),
               "run_id": run_id, "plan": plan, "toolchain": toolchain,
               "node_identities": identities, "timeout_seconds": timeout_seconds}
    context_path = run_dir / "context.json"

    def command(node: dict[str, Any]) -> list[str]:
        if not context_path.exists():
            write_json(context_path, context)
        print(f"Running {node['node_id']}", flush=True)
        return [sys.executable, "-B", "-m", "scripts.objc3c_native_validation.worker",
                "--context", str(context_path), "--node", node["node_id"]]

    def reuse(node_id: str, prior: dict[str, Any], _: Path) -> None:
        saved = read_node_result(Path(prior["result_path"]), node_id=node_id,
                                 node_identity=identities[node_id],
                                 build_identity=toolchain["build_identity"], allowed_roots=[repo])
        data = {**saved["data"], "reused_from_run_id": saved["run_id"]}
        write_node_result(result_path(run_dir, node_id), node_id=node_id,
                          node_identity=identities[node_id], build_identity=toolchain["build_identity"],
                          run_id=run_id, artifacts={role: Path(value["path"])
                                                   for role, value in saved["artifacts"].items()},
                          data=data, allowed_roots=[repo])
        print(f"Reused {node_id}", flush=True)

    result = execute_node_plan(
        plan, run_id=run_id, output_root=output_root,
        build_identity=toolchain["build_identity"], inventory_identity=inventory,
        node_identities=identities, command_for_node=command, cwd=repo,
        timeout_seconds=timeout_seconds, reuse_report=previous, on_reuse=reuse)
    write_json(context_path, context)
    failures = []

    def invalidate(node_ids: list[str], reason: str) -> None:
        for entry in result["node_records"]:
            if entry["node_id"] in node_ids and entry["status"] in ("completed", "reused"):
                entry.update(status="failed", validation_error=reason, runtime_count=0)

    for entry in result["node_records"]:
        if entry["status"] not in ("completed", "reused"):
            continue
        node_id = entry["node_id"]
        try:
            path = result_path(run_dir, node_id)
            saved = read_node_result(path, node_id=node_id, node_identity=identities[node_id],
                                     build_identity=toolchain["build_identity"], allowed_roots=[repo])
            entry.update(result_path=str(path), artifact_count=len(saved["artifacts"]),
                         runtime_count=saved["data"].get("runtime_count", 0))
        except (NodeResultError, OSError, ValueError) as error:
            entry.update(status="failed", artifact_error=str(error), runtime_count=0)
            failures.append(f"{node_id}: {error}")
    try:
        verify_toolchain_snapshot(toolchain)
    except Exception as error:
        reason = f"toolchain changed during validation: {error}"
        failures.append(reason)
        invalidate(plan["selected_node_ids"], reason)
    try:
        current_identities = compute_node_identities(
            plan, input_root=repo, build_identity=toolchain["build_identity"], inventory_identity=inventory)
        changed = [node_id for node_id in plan["selected_node_ids"]
                   if identities[node_id] != current_identities[node_id]]
        if changed:
            reason = "source inputs changed during validation: " + ", ".join(changed)
            failures.append(reason)
            invalidate(changed, reason)
    except (OSError, ValueError) as error:
        reason = f"source inputs could not be verified after validation: {error}"
        failures.append(reason)
        invalidate(plan["selected_node_ids"], reason)
    if failures:
        result.update(status="FAIL", partial_report=True, selection_complete=False,
                      validation_errors=failures)
    result.update(native_validation_schema=1, validation_complete=result["status"] == "PASS",
                  sdk_build_id=toolchain["sdk_build_id"],
                  patch_digest=toolchain["patch_digest"], context_path=str(context_path),
                  invalidated_reuse_nodes=invalidated,
                  scope="Private native migration checks; public capabilities remain unavailable.")
    write_json(run_dir / "summary.json", result)
    return result


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--repo", type=Path, default=Path(__file__).resolve().parents[2])
    parser.add_argument("--family", action="append", default=[])
    parser.add_argument("--node", action="append", default=[])
    parser.add_argument("--driver", action="append", default=[])
    parser.add_argument("--list", action="store_true", help="show the exact plan without requiring built tools")
    parser.add_argument("--output-root", type=Path)
    parser.add_argument("--run-id", default=datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ") + f"-{os.getpid()}")
    parser.add_argument("--timeout-seconds", type=float, default=300)
    parser.add_argument("--reuse-report", type=Path)
    parser.add_argument("--pinned-build-identity")
    args = parser.parse_args(argv)
    try:
        repo = args.repo.resolve(strict=True)
        plan = build_plan(repo, families=args.family, node_ids=args.node, drivers=args.driver)
        if args.list:
            print(json.dumps(plan, indent=2))
            return 0
        output = args.output_root or repo / "out" / "native-validation"
        if not output.is_absolute():
            output = repo / output
        result = run_validation(repo=repo, plan=plan, output_root=output, run_id=args.run_id,
                                timeout_seconds=args.timeout_seconds, reuse_path=args.reuse_report,
                                pinned_build_identity=args.pinned_build_identity)
        counts = {status: sum(entry["status"] == status for entry in result["node_records"])
                  for status in ("completed", "reused", "failed", "dependency-blocked", "unselected")}
        print(json.dumps({"status": result["status"], "build_identity": result["build_identity"],
                          "nodes": counts, "report": str(output / args.run_id / "summary.json")}, indent=2))
        return 0 if result["status"] == "PASS" else 1
    except Exception as error:
        print(f"native foundation validation: {error}", file=sys.stderr)
        return 1


if __name__ == "__main__":
    raise SystemExit(main())
