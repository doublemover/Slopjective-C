from __future__ import annotations

import json
from pathlib import Path
import sys

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS = ROOT / "scripts"
if str(SCRIPTS) not in sys.path:
    sys.path.insert(0, str(SCRIPTS))

import check_objc3c_cross_lane_e2e as runner  # noqa: E402
from objc3c_cross_lane_selection import resolve_execution_plan, safe_run_id, validate_manifest_inventory  # noqa: E402

MANIFEST = ROOT / "tests/tooling/fixtures/cross_lane_e2e/manifest.json"


def test_actual_manifest_inventory_and_provider_consumer_dependency_closure() -> None:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    rows = validate_manifest_inventory(manifest)
    assert len(rows) == 5
    plan = resolve_execution_plan(manifest, ROOT, ["text_collections_package"], "consumer-compile")
    ids = plan["selected_node_ids"]
    provider = "text_collections_package:provider:CrossLaneFixtureProvider"
    assert provider in ids
    assert "text_collections_package:consumer" in ids
    assert "text_collections_package:link-run" in ids
    assert "distribution_package_lifecycle:distribution" in plan["unselected_node_ids"]


def test_phase_selector_keeps_link_run_before_debugger_and_reflection() -> None:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    plan = resolve_execution_plan(manifest, ROOT, ["object_reflection_debugger"], "debugger")
    ids = plan["selected_node_ids"]
    assert ids == [
        "object_reflection_debugger:consumer",
        "object_reflection_debugger:link-run",
        "object_reflection_debugger:debugger",
        "object_reflection_debugger:reflection",
    ]


def test_selection_rejects_unknown_ids_and_unsafe_run_id() -> None:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    try:
        resolve_execution_plan(manifest, ROOT, ["missing-family"])
    except ValueError as error:
        assert "unknown family" in str(error)
    else:
        raise AssertionError("unknown family selector accepted")
    try:
        safe_run_id("../escape")
    except ValueError:
        pass
    else:
        raise AssertionError("unsafe run id accepted")


def test_run_checked_streams_output_to_isolated_log(tmp_path: Path) -> None:
    runner.RUN_CONTEXT["output_root"] = tmp_path
    runner.RUN_LOG_MAP.clear()
    result = runner.run_checked(
        [sys.executable, "-c", "print('out'); import sys; print('err', file=sys.stderr)"],
        cwd=ROOT,
        log_path=ROOT / "tmp/example.log",
        domain="synthetic.node",
        timeout_seconds=5,
    )
    assert result.returncode == 0
    assert result.stdout.strip() == "out"
    assert result.stderr.strip() == "err"
    logs = list((tmp_path / "logs").glob("*.log"))
    assert len(logs) == 1
    assert "stdout" not in logs[0].read_text(encoding="utf-8") or "out" in logs[0].read_text(encoding="utf-8")
    runner.RUN_CONTEXT.clear()
    runner.RUN_LOG_MAP.clear()


def test_run_checked_timeout_kills_child_and_keeps_partial_log(tmp_path: Path) -> None:
    runner.RUN_CONTEXT["output_root"] = tmp_path
    try:
        runner.run_checked(
            [sys.executable, "-c", "import time; print('started', flush=True); time.sleep(20)"],
            cwd=ROOT,
            log_path=ROOT / "tmp/timeout.log",
            domain="synthetic.timeout",
            timeout_seconds=0.2,
        )
    except RuntimeError as error:
        assert "timed out" in str(error)
    else:
        raise AssertionError("timeout was not enforced")
    assert "started" in next((tmp_path / "logs").glob("*.log")).read_text(encoding="utf-8")
    runner.RUN_CONTEXT.clear()
    runner.RUN_LOG_MAP.clear()


def test_failed_provider_blocks_consumer_and_unselected_families_remain_distinct() -> None:
    from objc3c_cross_lane_selection import execution_node_statuses
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    plan = resolve_execution_plan(manifest, ROOT, ["text_collections_package"], "consumer-compile")
    statuses = execution_node_statuses(
        plan,
        {"text_collections_package": {"status": "FAILED"}},
        [{"family_id": "text_collections_package", "error": "provider package module compile failed"}],
    )
    by_id = {row["node_id"]: row["status"] for row in statuses}
    assert by_id["text_collections_package:provider:CrossLaneFixtureProvider"] == "failed"
    assert by_id["text_collections_package:consumer"] == "dependency-blocked"
    assert by_id["distribution_package_lifecycle:distribution"] == "unselected"


def test_manifest_rejects_paths_escaping_workspace_root(tmp_path: Path) -> None:
    manifest = {
        "families": [{
            "family_id": "unsafe",
            "source": "../outside.objc3",
            "native_meta": "meta.json",
            "workspace_manifest": "workspace.json",
            "expectation": "expectation.json",
        }]
    }
    try:
        validate_manifest_inventory(manifest, tmp_path)
    except ValueError as error:
        assert "escapes" in str(error)
    else:
        raise AssertionError("outside manifest path accepted")
