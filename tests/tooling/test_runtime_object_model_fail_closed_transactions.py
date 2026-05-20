from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
RUNTIME = ROOT / "native" / "objc3c" / "src" / "runtime"


def _read(relative_path: str) -> str:
    return (RUNTIME / relative_path).read_text(encoding="utf-8")


def test_realized_class_graph_selects_one_global_preferred_bundle_per_class() -> None:
    class_graph = _read("classes/class_graph.cpp")

    assert "CollectClassGraphBundleCandidates(ordered_images, class_name)" in class_graph
    assert "SelectPreferredClassGraphBundleCandidate(candidates, class_name" in class_graph
    assert "ResolveInterfaceOwnerIdentityForClassGraphCandidates(" in class_graph
    assert "duplicate implementation-backed class metadata for " in class_graph
    assert "duplicate declaration-only class metadata for " in class_graph
    assert "class/metaclass owner edge is incomplete for " in class_graph
    assert "state.realized_class_node_indices_by_name[class_name].push_back(" in class_graph


def test_registration_table_walk_rolls_back_selector_and_keypath_mutations() -> None:
    table_walk = _read("images/registration_table_walk.cpp")

    assert "RuntimeRegistrationTableWalkMutationCheckpoint" in table_walk
    assert "selector_index_by_name(state.selector_index_by_name)" in table_walk
    assert "selector_slots(state.selector_slots)" in table_walk
    assert "keypath_slots(state.keypath_slots)" in table_walk
    assert "metadata_provider_edge_count(state.metadata_provider_edge_count)" in table_walk
    assert "image_backed_keypath_count(state.image_backed_keypath_count)" in table_walk
    assert "mutation_checkpoint.Restore(state)" in table_walk
    assert (
        table_walk.count("mutation_checkpoint.Restore(state)") >= 2
    ), "keypath and selector failures must both roll back partial materialization"
