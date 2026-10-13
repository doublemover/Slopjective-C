from __future__ import annotations

import json
from pathlib import Path
import sys

REPO_ROOT = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO_ROOT / "scripts"))

from objc3c_cross_lane_selection import resolve_execution_plan, safe_run_id  # noqa: E402
from objc3c_cross_lane_execution import (  # noqa: E402
    NODE_REPORT_CONTRACT_ID,
    compute_node_identities,
    execute_node_plan,
    run_node_command,
    validate_reuse_report,
)

MANIFEST = REPO_ROOT / "tests/tooling/fixtures/cross_lane_e2e/manifest.json"


def test_phase_selects_only_matching_nodes_and_their_dependencies() -> None:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    plan = resolve_execution_plan(manifest, REPO_ROOT, ["text_collections_package"], "consumer-compile")
    assert plan["selected_node_ids"] == [
        "text_collections_package:provider:CrossLaneFixtureProvider",
        "text_collections_package:consumer",
    ]
    assert "text_collections_package:link-run" in plan["unselected_node_ids"]
    assert "distribution_package_lifecycle:distribution" in plan["unselected_node_ids"]
    assert all(node["phase"] not in ("package-producer", "clean-install-consumer", "distribution")
               for node in plan["selected_nodes"])
    assert len(plan["nodes"]) > len(plan["selected_nodes"])


def test_distribution_is_a_real_producer_clean_consumer_then_validation_chain() -> None:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    plan = resolve_execution_plan(manifest, REPO_ROOT,
        ["distribution_package_lifecycle"], "clean-install-consumer")
    ids = plan["selected_node_ids"]
    assert ids[-2:] == ["distribution_package_lifecycle:package-producer",
                       "distribution_package_lifecycle:clean-install-consumer"]
    producer = next(node for node in plan["nodes"] if node["node_id"] == ids[-2])
    consumer = next(node for node in plan["nodes"] if node["node_id"] == ids[-1])
    assert producer["phase"] == "package-producer"
    assert consumer["phase"] == "clean-install-consumer"
    assert consumer["depends_on"] == [producer["node_id"]]


def test_existing_runner_routes_non_distribution_selection_to_only_selected_workers(tmp_path: Path) -> None:
    import check_objc3c_cross_lane_e2e as runner
    import objc3c_cross_lane_execution as execution

    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    observed: list[list[str]] = []
    original = execution.execute_node_plan

    def capture(plan, **kwargs):
        observed.extend(kwargs["command_for_node"](node) for node in plan["selected_nodes"])
        return {"status": "PASS", "partial_report": False, "run_id": "runner-probe",
                "run_directory": str(tmp_path / "runner-probe"), "node_records": [],
                "selected_node_ids": list(plan["selected_node_ids"])}

    execution.execute_node_plan = capture
    try:
        runner.execute_selected_nodes(manifest, families=["text_collections_package"],
            phase="consumer-compile", node_ids=[], run_id="runner-probe",
            output_base=tmp_path, build_identity="fixture-build", timeout_seconds=5)
    finally:
        execution.execute_node_plan = original
    assert len(observed) == 2
    assert all("--_node-worker" in command for command in observed)
    worker_ids = [command[command.index("--_node-worker") + 1] for command in observed]
    assert worker_ids == ["text_collections_package:provider:CrossLaneFixtureProvider",
                          "text_collections_package:consumer"]
    assert all("package-producer" not in " ".join(command) and "validate-package-install" not in " ".join(command)
               for command in observed)


def test_existing_runtime_reuse_rejects_missing_or_wrong_identity(tmp_path: Path) -> None:
    from scripts.objc3c_workflow.composite_step_runtime_reuse import _valid_runtime_report

    report = tmp_path / "runtime-report.json"
    base = {"status": "PASS", "runtime_reuse_identity": {}, "options_digest": "expected",
            "case_labels": [], "fixture_ids": []}
    report.write_text(json.dumps(base), encoding="utf-8")
    valid, reason = _valid_runtime_report(report, set(), "expected")
    assert not valid and "identity" in reason
    base["runtime_reuse_identity"] = {"contract_id": "wrong", "native_executable": "stale"}
    report.write_text(json.dumps(base), encoding="utf-8")
    valid, reason = _valid_runtime_report(report, set(), "expected")
    assert not valid and reason


def test_debugger_phase_includes_link_dependencies_not_sibling_reflection() -> None:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    plan = resolve_execution_plan(manifest, REPO_ROOT, ["object_reflection_debugger"], "debugger")
    assert plan["selected_node_ids"] == [
        "object_reflection_debugger:consumer",
        "object_reflection_debugger:link-run",
        "object_reflection_debugger:debugger",
    ]
    assert "object_reflection_debugger:reflection" in plan["unselected_node_ids"]


def test_node_selector_selects_exact_target_and_dependency_closure() -> None:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    target = "text_collections_package:link-run"
    plan = resolve_execution_plan(manifest, REPO_ROOT, node_ids=[target])
    assert plan["selected_node_ids"] == [
        "text_collections_package:provider:CrossLaneFixtureProvider",
        "text_collections_package:consumer",
        target,
    ]
    assert "object_reflection_debugger:link-run" in plan["unselected_node_ids"]


def test_selector_rejects_unknown_and_unsafe_ids() -> None:
    manifest = json.loads(MANIFEST.read_text(encoding="utf-8"))
    try:
        resolve_execution_plan(manifest, REPO_ROOT, node_ids=["missing:node"])
    except ValueError as error:
        assert "unknown node" in str(error)
    else:
        raise AssertionError("unknown node selector accepted")
    try:
        safe_run_id("../escape")
    except ValueError:
        pass
    else:
        raise AssertionError("unsafe run id accepted")


def test_provider_edges_are_ordered_and_cycles_are_rejected(tmp_path: Path) -> None:
    workspace = {"package_modules": [
        {"module_name": "Owner", "package_id": "fixture:owner", "source": "owner.c"},
        {"module_name": "Dependency", "package_id": "fixture:dependency", "source": "dependency.c"},
    ], "package_edges": [{"from": "fixture:owner", "to": "fixture:dependency"}]}
    (tmp_path / "workspace.json").write_text(json.dumps(workspace))
    (tmp_path / "expectation.json").write_text("{}\n")
    manifest = {"families": [{"family_id": "fixture", "source": "consumer.c",
        "native_meta": "meta.json", "workspace_manifest": "workspace.json", "expectation": "expectation.json"}]}
    plan = resolve_execution_plan(manifest, tmp_path, node_ids=["fixture:provider:Owner"])
    assert plan["selected_node_ids"] == ["fixture:provider:Dependency", "fixture:provider:Owner"]
    workspace["package_edges"].append({"from": "fixture:dependency", "to": "fixture:owner"})
    (tmp_path / "workspace.json").write_text(json.dumps(workspace))
    try:
        resolve_execution_plan(manifest, tmp_path)
    except ValueError as error:
        assert "cycle" in str(error)
    else:
        raise AssertionError("provider dependency cycle accepted")


def _tiny_plan() -> dict:
    return {
        "nodes": [
            {"node_id": "family:a", "family_id": "family", "phase": "provider", "source": "a.c", "depends_on": []},
            {"node_id": "family:b", "family_id": "family", "phase": "consumer-compile", "source": "b.c", "depends_on": ["family:a"]},
            {"node_id": "other:c", "family_id": "other", "phase": "link-run", "source": "c.c", "depends_on": []},
        ],
        "selected_nodes": [],
        "selected_node_ids": ["family:a", "family:b"],
        "unselected_node_ids": ["other:c"],
    }


def test_reuse_requires_complete_full_inventory_and_exact_build_and_node_inputs(tmp_path: Path) -> None:
    (tmp_path / "a.c").write_text("int a;\n", encoding="utf-8")
    (tmp_path / "b.c").write_text("int b;\n", encoding="utf-8")
    (tmp_path / "c.c").write_text("int c;\n", encoding="utf-8")
    plan = _tiny_plan()
    ids = compute_node_identities(plan, input_root=tmp_path, build_identity="build-1", inventory_identity="manifest-1")
    report = {
        "contract_id": NODE_REPORT_CONTRACT_ID,
        "status": "PASS",
        "partial_report": False,
        "selection_complete": True,
        "run_id": "prior-run",
        "build_identity": "build-1",
        "inventory_identity": "manifest-1",
        "node_inventory": [node["node_id"] for node in plan["nodes"]],
        "node_records": [
            {"node_id": node_id, "status": "completed", "node_identity": ids[node_id], "build_identity": "build-1"}
            for node_id in plan["selected_node_ids"]
        ] + [{"node_id": "other:c", "status": "unselected"}],
    }
    reused = validate_reuse_report(report, plan=plan, node_identities=ids, build_identity="build-1", inventory_identity="manifest-1", current_run_id="new-run")
    assert set(reused) == set(plan["selected_node_ids"])
    for field, value in (("partial_report", True), ("build_identity", "build-2"), ("inventory_identity", "manifest-2")):
        bad = dict(report)
        bad[field] = value
        try:
            validate_reuse_report(bad, plan=plan, node_identities=ids, build_identity="build-1", inventory_identity="manifest-1", current_run_id="new-run")
        except ValueError:
            pass
        else:
            raise AssertionError(f"mismatched reuse accepted: {field}")
    partial = dict(report)
    partial["node_inventory"] = partial["node_inventory"][:-1]
    try:
        validate_reuse_report(partial, plan=plan, node_identities=ids, build_identity="build-1", inventory_identity="manifest-1", current_run_id="new-run")
    except ValueError as error:
        assert "inventory" in str(error)
    else:
        raise AssertionError("partial full inventory accepted")
    partial_records = dict(report)
    partial_records["node_records"] = partial_records["node_records"][:-1]
    try:
        validate_reuse_report(partial_records, plan=plan, node_identities=ids, build_identity="build-1", inventory_identity="manifest-1", current_run_id="new-run")
    except ValueError as error:
        assert "records are partial" in str(error)
    else:
        raise AssertionError("partial node records accepted")


def test_node_execution_logs_timeout_and_blocks_dependents(tmp_path: Path) -> None:
    plan = _tiny_plan()
    log = tmp_path / "timeout.log"
    timeout = run_node_command(plan["nodes"][0], [sys.executable, "-c", "import time; print('started', flush=True); time.sleep(5)"], cwd=tmp_path, log_path=log, timeout_seconds=0.1)
    assert timeout["timed_out"] is True
    assert timeout["exit_code"] == 124
    assert "started" in log.read_text(encoding="utf-8")

    (tmp_path / "a.c").write_text("int a;\n", encoding="utf-8")
    (tmp_path / "b.c").write_text("int b;\n", encoding="utf-8")
    (tmp_path / "c.c").write_text("int c;\n", encoding="utf-8")
    identities = compute_node_identities(plan, input_root=tmp_path, build_identity="b", inventory_identity="i")
    selected_plan = dict(plan)
    selected_plan["selected_node_ids"] = ["family:a", "family:b"]
    report = execute_node_plan(
        selected_plan,
        run_id="isolated-run",
        output_root=tmp_path / "runs",
        build_identity="b",
        inventory_identity="i",
        node_identities=identities,
        command_for_node=lambda node: [sys.executable, "-c", "import sys; sys.exit(7)" if node["node_id"] == "family:a" else "print('should not run')"],
        cwd=tmp_path,
        timeout_seconds=3,
    )
    by_id = {row["node_id"]: row["status"] for row in report["node_records"]}
    assert report["status"] == "FAIL"
    assert by_id == {"family:a": "failed", "family:b": "dependency-blocked", "other:c": "unselected"}
    logs = list((tmp_path / "runs/isolated-run/logs").glob("family-a-*.log"))
    assert len(logs) == 1
    partial = json.loads((tmp_path / "runs/isolated-run/summary.json").read_text(encoding="utf-8"))
    assert partial["partial_report"] is True
    assert partial["node_records"][0]["exit_code"] == 7


def test_completed_nodes_reuse_only_with_matching_identity_and_unique_run_root(tmp_path: Path) -> None:
    plan = _tiny_plan()
    for name in ("a.c", "b.c", "c.c"):
        (tmp_path / name).write_text(f"int {name[0]};\n", encoding="utf-8")
    identities = compute_node_identities(plan, input_root=tmp_path, build_identity="sealed-build", inventory_identity="manifest")
    first = execute_node_plan(
        plan, run_id="run-one", output_root=tmp_path / "runs", build_identity="sealed-build",
        inventory_identity="manifest", node_identities=identities,
        command_for_node=lambda _node: [sys.executable, "-c", "print('completed')"],
        cwd=tmp_path, timeout_seconds=3)
    assert first["status"] == "PASS" and first["partial_report"] is False
    second = execute_node_plan(
        plan, run_id="run-two", output_root=tmp_path / "runs", build_identity="sealed-build",
        inventory_identity="manifest", node_identities=identities,
        command_for_node=lambda _node: (_ for _ in ()).throw(AssertionError("reused node ran again")),
        cwd=tmp_path, timeout_seconds=3, reuse_report=first)
    assert second["status"] == "PASS"
    assert second["run_directory"] != first["run_directory"]
    assert [row["status"] for row in second["node_records"]] == ["reused", "reused", "unselected"]
    (tmp_path / "b.c").write_text("int changed;\n", encoding="utf-8")
    changed = compute_node_identities(plan, input_root=tmp_path, build_identity="sealed-build", inventory_identity="manifest")
    partial_reuse = validate_reuse_report(first, plan=plan, node_identities=changed,
        build_identity="sealed-build", inventory_identity="manifest", current_run_id="run-three")
    assert set(partial_reuse) == {"family:a"}
    third = execute_node_plan(
        plan, run_id="run-three", output_root=tmp_path / "runs", build_identity="sealed-build",
        inventory_identity="manifest", node_identities=changed,
        command_for_node=lambda node: [sys.executable, "-c", "print('recompiled consumer')" if node["node_id"] == "family:b" else "raise SystemExit('provider should reuse')"],
        cwd=tmp_path, timeout_seconds=3, reuse_report=first)
    assert [row["status"] for row in third["node_records"]] == ["reused", "completed", "unselected"]


def test_failed_artifact_reuse_is_recorded_and_blocks_dependents(tmp_path: Path) -> None:
    plan = _tiny_plan()
    for name in ("a.c", "b.c", "c.c"):
        (tmp_path / name).write_text(f"int {name[0]};\n")
    identities = compute_node_identities(plan, input_root=tmp_path, build_identity="build", inventory_identity="inventory")
    first = execute_node_plan(plan, run_id="first", output_root=tmp_path / "runs",
        build_identity="build", inventory_identity="inventory", node_identities=identities,
        command_for_node=lambda _node: [sys.executable, "-c", "print('ok')"], cwd=tmp_path, timeout_seconds=3)
    def stale_artifacts(_node_id, _record, _run_dir):
        raise ValueError("producer artifact changed")
    second = execute_node_plan(plan, run_id="second", output_root=tmp_path / "runs",
        build_identity="build", inventory_identity="inventory", node_identities=identities,
        command_for_node=lambda _node: (_ for _ in ()).throw(AssertionError("unexpected execution")),
        cwd=tmp_path, timeout_seconds=3, reuse_report=first, on_reuse=stale_artifacts)
    assert second["status"] == "FAIL" and second["partial_report"] is True
    assert [row["status"] for row in second["node_records"]] == ["failed", "dependency-blocked", "unselected"]
    assert second["node_records"][0]["reuse_error"] == "producer artifact changed"


def test_cli_does_not_overwrite_an_existing_run_on_failure(tmp_path: Path, monkeypatch) -> None:
    import check_objc3c_cross_lane_e2e as runner

    run = tmp_path / "prior-run"
    run.mkdir()
    summary = run / "summary.json"
    summary.write_text('{"status":"PASS","sealed":true}\n')
    def reject(**_kwargs):
        raise FileExistsError("existing run")
    monkeypatch.setattr(runner, "validate_manifest", reject)
    assert runner.main(["--output-root", str(tmp_path), "--run-id", "prior-run"]) == 1
    assert json.loads(summary.read_text()) == {"status": "PASS", "sealed": True}
