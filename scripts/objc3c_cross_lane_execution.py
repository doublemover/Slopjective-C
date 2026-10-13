"""Node execution, per-node identity, and strict completed-report reuse.

This helper deliberately does not choose compiler commands. The cross-lane
runner supplies a command for each selected DAG node after resolving the
manifest and build identity.
"""
from __future__ import annotations

import hashlib
import json
import os
import re
import signal
import subprocess
import threading
import time
import tempfile
import shutil
from pathlib import Path
from queue import Queue
from typing import Any, Callable, Iterable, Sequence

from objc3c_cross_lane_selection import safe_run_id

NODE_REPORT_CONTRACT_ID = "objc3c.cross_lane_e2e.node_report.v1"
_LOG_LIMIT = 10 * 1024 * 1024
_MAX_TIMEOUT_SECONDS = 24 * 60 * 60


def _canonical_digest(value: Any) -> str:
    encoded = json.dumps(value, sort_keys=True, separators=(",", ":"), ensure_ascii=False)
    return hashlib.sha256(encoded.encode("utf-8")).hexdigest()


def _contained_file(root: Path, value: str) -> Path:
    candidate = Path(value)
    if not candidate.is_absolute():
        candidate = root / candidate
    resolved = candidate.resolve()
    try:
        resolved.relative_to(root.resolve())
    except ValueError as error:
        raise ValueError(f"node identity path escapes input root: {value}") from error
    if not resolved.is_file():
        raise ValueError(f"node identity input is missing: {value}")
    return resolved


def file_digest(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for chunk in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(chunk)
    return digest.hexdigest()


def compute_node_identities(
    plan: dict[str, Any],
    *,
    input_root: Path,
    build_identity: str,
    inventory_identity: str,
) -> dict[str, str]:
    """Bind each node to its own declared inputs, build and dependency outputs."""
    if not build_identity or not inventory_identity:
        raise ValueError("build and inventory identities are required")
    identities: dict[str, str] = {}
    for node in plan["nodes"]:
        node_id = str(node["node_id"])
        inputs = node.get("identity_inputs")
        if not isinstance(inputs, list) or not inputs:
            source = node.get("source")
            if not isinstance(source, str) or not source:
                raise ValueError(f"{node_id} has no identity inputs")
            inputs = [source]
        files = []
        for raw in inputs:
            if not isinstance(raw, str) or not raw:
                raise ValueError(f"{node_id} has a malformed identity input")
            path = _contained_file(input_root, raw)
            files.append({"path": path.relative_to(input_root.resolve()).as_posix(),
                          "sha256": file_digest(path)})
        dependencies = node.get("depends_on", [])
        if not isinstance(dependencies, list) or any(dep not in identities for dep in dependencies):
            raise ValueError(f"{node_id} dependencies are not in dependency-first order")
        identities[node_id] = _canonical_digest({
            "node": {key: node.get(key) for key in ("node_id", "family_id", "phase", "source")},
            "inputs": files,
            "build_identity": build_identity,
            "inventory_identity": inventory_identity,
            "dependencies": {dep: identities[dep] for dep in dependencies},
        })
    return identities


def validate_reuse_report(
    report: Any,
    *,
    plan: dict[str, Any],
    node_identities: dict[str, str],
    build_identity: str,
    inventory_identity: str,
    current_run_id: str,
) -> dict[str, dict[str, Any]]:
    """Reject incomplete or mismatched reports; return exact reusable nodes."""
    if not isinstance(report, dict) or report.get("contract_id") != NODE_REPORT_CONTRACT_ID:
        raise ValueError("reuse report contract is missing or unsupported")
    if report.get("status") != "PASS" or report.get("partial_report") is not False:
        raise ValueError("partial or failed reports cannot be reused")
    if report.get("selection_complete") is not True:
        raise ValueError("reuse report selection is incomplete")
    old_run_id = report.get("run_id")
    if not isinstance(old_run_id, str) or safe_run_id(old_run_id) != old_run_id:
        raise ValueError("reuse report run identity is malformed")
    if old_run_id == current_run_id:
        raise ValueError("run identity must be unique; cannot reuse the current run")
    if report.get("build_identity") != build_identity:
        raise ValueError("reuse report build identity mismatch")
    if report.get("inventory_identity") != inventory_identity:
        raise ValueError("reuse report full-inventory identity mismatch")

    expected_inventory = {str(node["node_id"]) for node in plan["nodes"]}
    inventory = report.get("node_inventory")
    if not isinstance(inventory, list) or any(not isinstance(item, str) for item in inventory):
        raise ValueError("reuse report full node inventory is missing")
    if len(inventory) != len(set(inventory)) or set(inventory) != expected_inventory:
        raise ValueError("reuse report node inventory is partial or changed")
    records = report.get("node_records")
    if not isinstance(records, list):
        raise ValueError("reuse report node records are missing")
    by_id: dict[str, dict[str, Any]] = {}
    for record in records:
        if not isinstance(record, dict) or not isinstance(record.get("node_id"), str):
            raise ValueError("reuse report contains a malformed node record")
        node_id = record["node_id"]
        if node_id in by_id or node_id not in expected_inventory:
            raise ValueError("reuse report contains duplicate or unknown node records")
        by_id[node_id] = record
    if set(by_id) != expected_inventory:
        raise ValueError("reuse report node records are partial")

    reusable: dict[str, dict[str, Any]] = {}
    for node_id in plan["selected_node_ids"]:
        record = by_id.get(node_id)
        if record is None or record.get("status") == "unselected":
            continue
        if record.get("status") not in ("completed", "reused"):
            raise ValueError(f"reuse report has incomplete node {node_id}")
        if record.get("node_identity") != node_identities.get(node_id):
            # A completed report may still contain reusable independent nodes.
            # Mismatched nodes rerun; dependency identities invalidate their
            # downstream nodes without discarding unaffected providers.
            continue
        if record.get("build_identity") != build_identity:
            raise ValueError(f"reuse report node build identity mismatch: {node_id}")
        reusable[node_id] = record
    return reusable


def _terminate_tree(process: subprocess.Popen[str]) -> None:
    if process.poll() is not None:
        return
    if os.name == "nt":
        subprocess.run(["taskkill", "/PID", str(process.pid), "/T", "/F"],
                       capture_output=True, check=False)
        return
    try:
        os.killpg(process.pid, signal.SIGTERM)
    except ProcessLookupError:
        return
    try:
        process.wait(timeout=2)
    except subprocess.TimeoutExpired:
        try:
            os.killpg(process.pid, signal.SIGKILL)
        except ProcessLookupError:
            pass


def run_node_command(
    node: dict[str, Any],
    command: Sequence[str],
    *,
    cwd: Path,
    log_path: Path,
    timeout_seconds: float,
) -> dict[str, Any]:
    """Stream merged child output to a node-owned log and bound its deadline."""
    if timeout_seconds <= 0 or timeout_seconds > _MAX_TIMEOUT_SECONDS:
        raise ValueError("node timeout must be positive and at most 24 hours")
    if not command or any(not isinstance(token, str) or not token for token in command):
        raise ValueError("node command must be a non-empty string argument list")
    node_id = str(node["node_id"])
    if not re.sub(r"[^A-Za-z0-9._-]+", "-", node_id).strip("-"):
        raise ValueError("node id cannot name a log")
    log_path.parent.mkdir(parents=True, exist_ok=True)
    creation_flags = subprocess.CREATE_NEW_PROCESS_GROUP if os.name == "nt" else 0
    try:
        process = subprocess.Popen(
            list(command), cwd=cwd, text=True, encoding="utf-8", errors="replace",
            stdout=subprocess.PIPE, stderr=subprocess.STDOUT, bufsize=1,
            start_new_session=os.name != "nt", creationflags=creation_flags)
    except OSError as error:
        with log_path.open("w", encoding="utf-8", newline="") as log:
            log.write("command: " + " ".join(map(str, command)) + "\n")
            log.write("node_id: " + node_id + "\n")
            log.write(f"spawn_error: {error}\nexit_code: 127\ntimeout: false\n")
        return {"exit_code": 127, "timed_out": False, "log_path": str(log_path),
                "spawn_error": str(error)}
    events: Queue[str | None] = Queue(maxsize=1024)

    def pump() -> None:
        assert process.stdout is not None
        try:
            for line in iter(process.stdout.readline, ""):
                events.put(line)
        finally:
            events.put(None)

    reader = threading.Thread(target=pump, daemon=True)
    reader.start()
    deadline = time.monotonic() + timeout_seconds
    timed_out = False
    bytes_written = 0
    ended = False
    with log_path.open("w", encoding="utf-8", newline="") as log:
        log.write("command: " + " ".join(map(str, command)) + "\n")
        log.write("node_id: " + node_id + "\n")
        while not ended or process.poll() is None:
            if not timed_out and process.poll() is None and time.monotonic() >= deadline:
                timed_out = True
                _terminate_tree(process)
            try:
                line = events.get(timeout=0.1)
            except Exception:
                if process.poll() is not None and ended:
                    break
                continue
            if line is None:
                ended = True
                continue
            encoded = line.encode("utf-8", errors="replace")
            if bytes_written + len(encoded) <= _LOG_LIMIT:
                log.write(line)
                bytes_written += len(encoded)
            elif bytes_written <= _LOG_LIMIT:
                log.write("\n[node log truncated at 10 MiB; output drained]\n")
                bytes_written = _LOG_LIMIT + 1
        process.wait()
        reader.join(timeout=1)
        exit_code = 124 if timed_out else int(process.returncode or 0)
        log.write(f"\nexit_code: {exit_code}\ntimeout: {str(timed_out).lower()}\n")
    return {"exit_code": exit_code, "timed_out": timed_out, "log_path": str(log_path)}


def execute_node_plan(
    plan: dict[str, Any],
    *,
    run_id: str,
    output_root: Path,
    build_identity: str,
    inventory_identity: str,
    node_identities: dict[str, str],
    command_for_node: Callable[[dict[str, Any]], Sequence[str]],
    cwd: Path,
    timeout_seconds: float,
    reuse_report: Any = None,
    on_reuse: Callable[[str, dict[str, Any], Path], None] | None = None,
) -> dict[str, Any]:
    """Execute selected DAG nodes, block dependents, and retain full inventory."""
    safe_run_id(run_id)
    reusable = validate_reuse_report(
        reuse_report, plan=plan, node_identities=node_identities,
        build_identity=build_identity, inventory_identity=inventory_identity,
        current_run_id=run_id) if reuse_report is not None else {}
    run_dir = output_root / run_id
    run_dir.mkdir(parents=True, exist_ok=False)
    selected = set(plan["selected_node_ids"])
    statuses: dict[str, dict[str, Any]] = {}
    for node in plan["nodes"]:
        node_id = str(node["node_id"])
        statuses[node_id] = {"node_id": node_id,
                             "status": "selected" if node_id in selected else "unselected"}

    def make_report(*, complete: bool = False) -> dict[str, Any]:
        records = [statuses[str(node["node_id"])] for node in plan["nodes"]]
        failed = any(record["status"] == "failed" for record in records)
        blocked = any(record["status"] == "dependency-blocked" for record in records)
        done = complete and not failed and not blocked and all(
            statuses[node_id]["status"] in ("completed", "reused")
            for node_id in plan["selected_node_ids"])
        return {
            "contract_id": NODE_REPORT_CONTRACT_ID,
            "status": "PASS" if done else ("FAIL" if complete or failed or blocked else "RUNNING"),
            "partial_report": not done,
            "selection_complete": done,
            "run_id": run_id,
            "run_directory": str(run_dir),
            "build_identity": build_identity,
            "inventory_identity": inventory_identity,
            "node_inventory": [str(node["node_id"]) for node in plan["nodes"]],
            "selected_node_ids": list(plan["selected_node_ids"]),
            "node_records": records,
        }

    def persist_report(payload: dict[str, Any]) -> None:
        report_path = run_dir / "summary.json"
        fd, temporary_name = tempfile.mkstemp(prefix=".summary-", suffix=".json", dir=run_dir)
        try:
            with os.fdopen(fd, "w", encoding="utf-8", newline="") as stream:
                json.dump(payload, stream, indent=2, sort_keys=True)
                stream.write("\n")
            os.replace(temporary_name, report_path)
        finally:
            if os.path.exists(temporary_name):
                os.unlink(temporary_name)

    persist_report(make_report())
    for node in plan["nodes"]:
        node_id = str(node["node_id"])
        if node_id not in selected:
            continue
        failed_dependencies = [dep for dep in node.get("depends_on", [])
                               if statuses.get(dep, {}).get("status") not in ("completed", "reused")]
        if failed_dependencies:
            statuses[node_id] = {"node_id": node_id, "status": "dependency-blocked",
                                 "blocked_by": failed_dependencies}
            persist_report(make_report())
            continue
        prior = reusable.get(node_id)
        safe_name = re.sub(r"[^A-Za-z0-9._-]+", "-", node_id).strip("-")
        suffix = hashlib.sha256(node_id.encode("utf-8")).hexdigest()[:10]
        log_path = run_dir / "logs" / f"{safe_name}-{suffix}.log"
        if prior is not None:
            try:
                if on_reuse is not None:
                    on_reuse(node_id, prior, run_dir)
                statuses[node_id] = {**prior, "status": "reused",
                                     "reused_from_run_id": reuse_report["run_id"]}
            except Exception as error:
                log_path.parent.mkdir(parents=True, exist_ok=True)
                log_path.write_text(f"node artifact reuse failed: {error}\nexit_code: 127\ntimeout: false\n", encoding="utf-8")
                statuses[node_id] = {"node_id": node_id, "status": "failed",
                                     "node_identity": node_identities[node_id],
                                     "build_identity": build_identity,
                                     "exit_code": 127, "timed_out": False,
                                     "reuse_error": str(error), "log_path": str(log_path)}
            persist_report(make_report())
            continue
        try:
            command = command_for_node(node)
            result = run_node_command(node, command, cwd=cwd,
                                      log_path=log_path, timeout_seconds=timeout_seconds)
        except Exception as error:
            log_path.parent.mkdir(parents=True, exist_ok=True)
            log_path.write_text(f"node setup failed: {error}\nexit_code: 127\ntimeout: false\n",
                                encoding="utf-8")
            result = {"exit_code": 127, "timed_out": False,
                      "log_path": str(log_path), "setup_error": str(error)}
        status = "completed" if result["exit_code"] == 0 and not result["timed_out"] else "failed"
        statuses[node_id] = {"node_id": node_id, "status": status,
                             "node_identity": node_identities[node_id],
                             "build_identity": build_identity, **result}
        persist_report(make_report())
    payload = make_report(complete=True)
    persist_report(payload)
    return payload


__all__ = [
    "NODE_REPORT_CONTRACT_ID", "compute_node_identities", "validate_reuse_report",
    "run_node_command", "execute_node_plan",
]
