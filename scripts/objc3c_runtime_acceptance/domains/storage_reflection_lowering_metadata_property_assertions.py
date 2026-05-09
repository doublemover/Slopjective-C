"""Property metadata assertions for storage/reflection lowering cases."""

from __future__ import annotations

from typing import Any

from objc3c_runtime_acceptance.expectation_matching import expect


def find_property_entry(
    entries: list[dict[str, Any]],
    owner_kind: str,
    owner_name: str,
    property_name: str,
) -> dict[str, Any]:
    for entry in entries:
        if (
            entry.get("owner_kind") == owner_kind
            and entry.get("owner_name") == owner_name
            and entry.get("property_name") == property_name
        ):
            return entry
    raise RuntimeError(
        f"missing property entry for {owner_kind}:{owner_name}:{property_name}"
    )


def expect_property_lowering(
    manifest: dict[str, Any],
    owner_kind: str,
    owner_name: str,
    property_name: str,
    synthesizes_executable_accessors: bool,
    getter_helper_symbol: str,
    setter_helper_symbol: str,
) -> None:
    runtime_metadata_records = manifest.get("runtime_metadata_source_records", {})
    property_records = runtime_metadata_records.get("properties", [])
    expect(
        isinstance(property_records, list),
        "expected runtime metadata source records to publish property entries",
    )
    property_record = find_property_entry(
        property_records, owner_kind, owner_name, property_name
    )
    expect(
        property_record.get("synthesizes_executable_accessors")
        is synthesizes_executable_accessors,
        f"expected runtime metadata property record to preserve synthesized-accessor lowering truth for {owner_kind}:{owner_name}:{property_name}",
    )
    expect(
        property_record.get("getter_storage_runtime_helper_symbol")
        == getter_helper_symbol,
        f"expected runtime metadata property record to preserve getter helper lowering for {owner_kind}:{owner_name}:{property_name}",
    )
    expect(
        property_record.get("setter_storage_runtime_helper_symbol")
        == setter_helper_symbol,
        f"expected runtime metadata property record to preserve setter helper lowering for {owner_kind}:{owner_name}:{property_name}",
    )

    source_graph = manifest.get("objc_executable_metadata_source_graph")
    if not isinstance(source_graph, dict):
        source_graph = manifest.get("executable_metadata_source_graph")
    if not isinstance(source_graph, dict):
        source_graph = (
            manifest.get("frontend", {})
            .get("pipeline", {})
            .get("semantic_surface", {})
            .get("objc_executable_metadata_source_graph", {})
        )
    property_nodes = source_graph.get("property_node_entries", [])
    expect(
        isinstance(property_nodes, list),
        "expected executable metadata source graph to publish property nodes",
    )
    property_node = find_property_entry(
        property_nodes, owner_kind, owner_name, property_name
    )
    expect(
        property_node.get("synthesizes_executable_accessors")
        is synthesizes_executable_accessors,
        f"expected executable metadata property node to preserve synthesized-accessor lowering truth for {owner_kind}:{owner_name}:{property_name}",
    )
    expect(
        property_node.get("getter_storage_runtime_helper_symbol")
        == getter_helper_symbol,
        f"expected executable metadata property node to preserve getter helper lowering for {owner_kind}:{owner_name}:{property_name}",
    )
    expect(
        property_node.get("setter_storage_runtime_helper_symbol")
        == setter_helper_symbol,
        f"expected executable metadata property node to preserve setter helper lowering for {owner_kind}:{owner_name}:{property_name}",
    )


__all__ = ["expect_property_lowering", "find_property_entry"]
