from __future__ import annotations

from pathlib import Path
import sys

import pytest

REPO = Path(__file__).resolve().parents[2]
sys.path.insert(0, str(REPO / "scripts"))

from scripts.objc3c_native_validation.plan import build_plan  # noqa: E402
from objc3c_cross_lane_execution import compute_node_identities  # noqa: E402
import objc3c_cross_lane_execution as execution  # noqa: E402


@pytest.fixture
def inventory_repo(tmp_path: Path) -> Path:
    # Mock the complete tracked input inventory, without compilers or outputs.
    paths = {path for node in build_plan(REPO)["nodes"] for path in node["identity_inputs"]}
    for relative in sorted(paths):
        path = tmp_path / relative
        path.parent.mkdir(parents=True, exist_ok=True)
        path.write_text(f"fixture input: {relative}\n", encoding="utf-8")
    return tmp_path


def identities(plan: dict, repo: Path) -> dict[str, str]:
    return compute_node_identities(plan, input_root=repo, build_identity="sealed-build",
                                   inventory_identity="full-node-shapes")


def test_default_full_inventory_is_dependency_first_and_deterministic(inventory_repo: Path) -> None:
    plan = build_plan(inventory_repo)
    assert plan == build_plan(inventory_repo, families=["editor", "native-package", "carriers"],
                              drivers=["objc3c-clang", "clang-cl", "clang"])
    assert len(plan["nodes"]) == 18
    assert plan["selected_node_ids"] == [node["node_id"] for node in plan["nodes"]]
    assert plan["selected_families"] == ["carriers", "editor", "native-package"]
    assert plan["selection_complete"] is True
    seen = set()
    for node in plan["nodes"]:
        assert set(node["depends_on"]) <= seen
        assert node["source"] in node["identity_inputs"]
        for relative in node["identity_inputs"]:
            assert not Path(relative).is_absolute()
            assert (inventory_repo / relative).is_file()
            assert relative.split("/")[0] not in {"out", "temp", "tmp", "artifacts"}
        seen.add(node["node_id"])
    assert len(identities(plan, inventory_repo)) == 18


def test_exact_consumer_selection_keeps_full_inventory_and_adds_provider(inventory_repo: Path) -> None:
    plan = build_plan(inventory_repo, families=["native-package"],
                      node_ids=["native-package:consumer-source"])
    assert len(plan["nodes"]) == 18
    assert plan["selected_node_ids"] == ["native-package:provider", "native-package:consumer-source"]
    consumer = plan["selected_nodes"][-1]
    assert consumer["operation"] == "package-consumer"
    assert consumer["mode"] == "source-directed"
    assert "native-package:consumer-eager" in plan["unselected_node_ids"]
    assert plan["selection_complete"] is True


def test_native_driver_omits_unsupported_baselines(inventory_repo: Path) -> None:
    plan = build_plan(inventory_repo, families=["carriers"], drivers=["objc3c-clang"])
    assert plan["selected_node_ids"] == ["carriers:match:objc3c-clang", "carriers:transfer:objc3c-clang"]
    assert len(plan["nodes"]) == 18
    assert all(node["driver"] == "objc3c-clang" for node in plan["selected_nodes"])


@pytest.mark.parametrize("filters, message", [
    ({"families": ["carrier"]}, "unknown family"),
    ({"drivers": ["clang++"]}, "unknown driver"),
    ({"node_ids": ["carriers:match"]}, "unknown node"),
    ({"families": ["native-package"], "drivers": ["clang-cl"]}, "do not match"),
    ({"families": ["editor"], "drivers": ["objc3c-clang"]}, "do not match"),
    ({"families": ["editor"], "node_ids": ["native-package:provider"]}, "conflicts"),
    ({"drivers": ["clang"], "node_ids": ["carriers:match:clang-cl"]}, "conflicts"),
    ({"node_ids": ["carriers:match:clang", "carriers:transfer:clang-cl"], "drivers": ["clang"]}, "conflicts"),
    ({"families": "carriers"}, "sequence"),
    ({"drivers": [""]}, "nonempty"),
])
def test_invalid_or_conflicting_selectors_fail_closed(inventory_repo: Path, filters: dict, message: str) -> None:
    with pytest.raises(ValueError, match=message):
        build_plan(inventory_repo, **filters)


def test_missing_unselected_input_is_not_hidden_by_filter(inventory_repo: Path) -> None:
    (inventory_repo / "tests/tooling/fixtures/objc3_editor/reserved_intrinsics.mm").unlink()
    with pytest.raises(ValueError, match="input is missing"):
        build_plan(inventory_repo, node_ids=["native-package:provider"])


@pytest.mark.parametrize("module", [
    "__init__.py", "cli.py", "worker.py", "artifacts.py", "identity.py",
    "fixture_nodes.py", "package_nodes.py",
])
def test_implemented_modules_are_required_identity_inputs(inventory_repo: Path, module: str) -> None:
    (inventory_repo / "scripts/objc3c_native_validation" / module).unlink()
    with pytest.raises(ValueError, match="input is missing"):
        build_plan(inventory_repo)


def test_missing_implementation_sources_are_not_silently_omitted(inventory_repo: Path) -> None:
    directory = inventory_repo / "native/objc3c/src/clang_foundation"
    for path in directory.rglob("*"):
        if path.is_file():
            path.unlink()
    with pytest.raises(ValueError, match="no source inputs"):
        build_plan(inventory_repo)


def test_operation_sources_invalidate_only_owning_nodes_and_dependents(inventory_repo: Path) -> None:
    directory = inventory_repo / "scripts/objc3c_native_validation"
    directory.mkdir(parents=True, exist_ok=True)
    fixture = directory / "fixture_nodes.py"
    package = directory / "package_nodes.py"
    fixture.write_text("fixture implementation one\n", encoding="utf-8")
    package.write_text("package implementation one\n", encoding="utf-8")
    plan = build_plan(inventory_repo)
    before = identities(plan, inventory_repo)
    package.write_text("package implementation two\n", encoding="utf-8")
    after_package = identities(plan, inventory_repo)
    changed = {node_id for node_id in before if before[node_id] != after_package[node_id]}
    assert changed == {"native-package:provider", "native-package:consumer-eager", "native-package:consumer-source"}
    fixture.write_text("fixture implementation two\n", encoding="utf-8")
    after_fixture = identities(plan, inventory_repo)
    changed = {node_id for node_id in before if after_package[node_id] != after_fixture[node_id]}
    assert len(changed) == 14 and all(node_id.startswith("carriers:") for node_id in changed)


def test_package_fixture_changes_follow_provider_dependency_only(inventory_repo: Path) -> None:
    plan = build_plan(inventory_repo)
    expected_sources = {
        "native-package:provider": "tests/clang_foundation/native_package/provider.m",
        "native-package:consumer-eager": "tests/clang_foundation/native_package/consumer-eager.m",
        "native-package:consumer-source": "tests/clang_foundation/native_package/consumer-source.m",
    }
    package_nodes = {node["node_id"]: node for node in plan["nodes"]
                     if node["family_id"] == "native-package"}
    for node_id, source in expected_sources.items():
        assert package_nodes[node_id]["source"] == source
        assert set(package_nodes[node_id]["identity_inputs"]) & set(expected_sources.values()) == {source}

    before = identities(plan, inventory_repo)
    provider = inventory_repo / expected_sources["native-package:provider"]
    provider.write_text("provider fixture changed\n", encoding="utf-8")
    after_provider = identities(plan, inventory_repo)
    assert {node_id for node_id in before if before[node_id] != after_provider[node_id]} == set(expected_sources)

    eager = inventory_repo / expected_sources["native-package:consumer-eager"]
    eager.write_text("eager consumer fixture changed\n", encoding="utf-8")
    after_eager = identities(plan, inventory_repo)
    assert {node_id for node_id in before if after_provider[node_id] != after_eager[node_id]} == {
        "native-package:consumer-eager"}

    source = inventory_repo / expected_sources["native-package:consumer-source"]
    source.write_text("source-directed consumer fixture changed\n", encoding="utf-8")
    after_source = identities(plan, inventory_repo)
    assert {node_id for node_id in before if after_eager[node_id] != after_source[node_id]} == {
        "native-package:consumer-source"}


def test_shared_execution_source_invalidates_all_nodes(inventory_repo: Path) -> None:
    plan = build_plan(inventory_repo)
    before = identities(plan, inventory_repo)
    (inventory_repo / "scripts/objc3c_cross_lane_execution.py").write_text("executor changed\n", encoding="utf-8")
    after = identities(plan, inventory_repo)
    assert all(before[node_id] != after[node_id] for node_id in before)


def test_fixture_edit_preserves_unrelated_suite_reuse(inventory_repo: Path) -> None:
    plan = build_plan(inventory_repo)
    before = identities(plan, inventory_repo)
    (inventory_repo / "tests/clang_foundation/match-native-runtime.mm").write_text("match changed\n", encoding="utf-8")
    after = identities(plan, inventory_repo)
    changed = {node_id for node_id in before if before[node_id] != after[node_id]}
    assert changed == {f"carriers:match:{driver}" for driver in ("clang", "clang-cl", "objc3c-clang")}


def test_existing_executor_accepts_full_inventory_and_selected_closure(
    inventory_repo: Path, monkeypatch: pytest.MonkeyPatch,
) -> None:
    plan = build_plan(inventory_repo, node_ids=["native-package:consumer-eager"])
    observed = []

    def qualify(node, command, *, cwd, log_path, timeout_seconds):
        observed.append(node["node_id"])
        log_path.parent.mkdir(parents=True, exist_ok=True)
        log_path.write_text("mock qualification\n", encoding="utf-8")
        return {"exit_code": 0, "timed_out": False, "log_path": str(log_path)}

    monkeypatch.setattr(execution, "run_node_command", qualify)
    report = execution.execute_node_plan(
        plan, run_id="mock-native-plan", output_root=inventory_repo / "out/runs",
        build_identity="sealed-build", inventory_identity="full-node-shapes",
        node_identities=identities(plan, inventory_repo),
        command_for_node=lambda node: ["mock-worker", node["operation"]],
        cwd=inventory_repo, timeout_seconds=1,
    )
    assert observed == ["native-package:provider", "native-package:consumer-eager"]
    assert report["status"] == "PASS" and report["selection_complete"] is True
    assert len(report["node_inventory"]) == len(report["node_records"]) == 18
    assert {record["node_id"] for record in report["node_records"]
            if record["status"] == "completed"} == set(observed)


def test_identity_symlink_cannot_escape_repo(inventory_repo: Path, tmp_path: Path) -> None:
    outside = tmp_path.parent / (tmp_path.name + "-outside.py")
    outside.write_text("outside source\n", encoding="utf-8")
    target = inventory_repo / "scripts/objc3c_cross_lane_execution.py"
    target.unlink()
    try:
        target.symlink_to(outside)
    except OSError as error:
        pytest.skip(f"symlinks unavailable: {error}")
    with pytest.raises(ValueError, match="escapes repository"):
        build_plan(inventory_repo)


@pytest.mark.parametrize("separator", [[], ["--"]])
def test_public_workflow_route_forwards_native_foundation_filters(
    monkeypatch: pytest.MonkeyPatch, separator: list[str],
) -> None:
    from scripts.objc3c_workflow.cli_dispatch import dispatch_cli
    from scripts.objc3c_workflow.actions import application_conformance

    observed = []
    monkeypatch.setattr(application_conformance, "run", lambda command: observed.append(command) or 0)
    arguments = ["validate-native-foundation", *separator, "--family", "native-package", "--list"]
    original = list(arguments)
    assert dispatch_cli(arguments) == 0
    assert arguments == original
    assert observed == [[sys.executable, "-B", str(REPO / "scripts/check_objc3c_native_validation.py"),
                         "--family", "native-package", "--list"]]
