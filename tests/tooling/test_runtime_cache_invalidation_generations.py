from __future__ import annotations

from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
RUNTIME = ROOT / "native" / "objc3c" / "src" / "runtime"


def _read(relative_path: str) -> str:
    return (RUNTIME / relative_path).read_text(encoding="utf-8")


def test_method_cache_entries_stamp_all_runtime_mutation_generations() -> None:
    records = _read("dispatch/runtime_resolution_records.h")
    helper = _read("state/runtime_cache_invalidation.h")
    resolution = _read("dispatch/method_cache_resolution.cpp")
    fast_path = _read("dispatch/method_fast_path_seed.cpp")

    for field in (
        "cache_class_graph_generation",
        "cache_category_attachment_generation",
        "cache_protocol_declaration_generation",
        "cache_storage_surface_generation",
        "cache_method_surface_generation",
    ):
        assert field in records
        assert field in helper

    assert "StampMethodCacheMutationGenerationsUnlocked(cache_entry, state)" in resolution
    assert "StampMethodCacheMutationGenerationsUnlocked(cache_entry, state)" in fast_path
    assert "!MethodCacheMutationGenerationsMatchUnlocked(state, entry)" in resolution
    assert "OBJC3_RUNTIME_DISPATCH_STATUS_STALE_METHOD_CACHE" in resolution


def test_mutation_points_bump_named_runtime_generations() -> None:
    class_graph = _read("classes/class_graph.cpp")
    category_attachment = _read("classes/category_attachment.cpp")
    property_layout = _read("storage/property_layout_realization.cpp")
    registration = _read("images/registration.cpp")

    assert "BumpRuntimeClassGraphGenerationUnlocked(state)" in class_graph
    assert "BumpRuntimeMethodSurfaceGenerationUnlocked(state)" in class_graph
    assert "BumpRuntimeCategoryAttachmentGenerationUnlocked(state)" in category_attachment
    assert "BumpRuntimeMethodSurfaceGenerationUnlocked(state)" in category_attachment
    assert "BumpRuntimeStorageSurfaceGenerationUnlocked(state)" in property_layout
    assert "BumpRuntimeMethodSurfaceGenerationUnlocked(state)" in property_layout
    assert "BumpRuntimeProtocolDeclarationGenerationUnlocked(state)" in registration


def test_property_lookup_cache_is_generation_checked() -> None:
    records = _read("dispatch/runtime_resolution_records.h")
    helper = _read("state/runtime_cache_invalidation.h")
    lookup = _read("storage/property_lookup.cpp")

    for field in (
        "cache_class_graph_generation",
        "cache_category_attachment_generation",
        "cache_storage_surface_generation",
    ):
        assert field in records
        assert field in helper

    assert "PropertyLookupCacheMutationGenerationsMatchUnlocked" in lookup
    assert "StampPropertyLookupCacheMutationGenerationsUnlocked(entry, state)" in lookup


def test_generation_snapshot_surfaces_are_private_and_deterministic() -> None:
    dispatch_contract = _read("dispatch/dispatch_snapshot_contracts.h")
    object_contract = _read("classes/runtime_object_snapshot_contracts.h")
    cache_snapshot = _read("dispatch/method_cache_snapshots.cpp")
    graph_snapshot = _read("classes/class_graph_snapshots.cpp")

    for field in (
        "class_graph_generation",
        "category_attachment_generation",
        "protocol_declaration_generation",
        "storage_surface_generation",
        "method_surface_generation",
    ):
        assert field in dispatch_contract
        assert field in object_contract
        assert f"snapshot->{field}" in cache_snapshot
        assert f"snapshot->{field}" in graph_snapshot
