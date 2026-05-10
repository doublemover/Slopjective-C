"""Imported-runtime packaging startup protocol-query assertions."""

from __future__ import annotations

from typing import Any

from ..expectation_matching import expect


def assert_imported_runtime_startup_protocol_query(payload: dict[str, Any]) -> None:
    expect(
        payload.get("protocol_query_status") == 0,
        "expected imported-runtime startup protocol-conformance query snapshot copy to succeed",
    )
    expect(
        payload.get("protocol_query_class_found") == 1
        and payload.get("protocol_query_protocol_found") == 1
        and payload.get("protocol_query_conforms") == 1,
        "expected imported provider protocol conformance to survive cross-module startup",
    )
    expect(
        payload.get("protocol_query_visited_protocol_count") == 1,
        "expected imported-runtime startup to visit one protocol during conformance evaluation",
    )
    expect(
        payload.get("protocol_query_attached_category_count") == 0,
        "expected imported-runtime startup to avoid category-backed protocol conformance",
    )
    expect(
        payload.get("protocol_query_matched_protocol_owner_identity") == "",
        "expected imported-runtime startup protocol conformance to leave the matched protocol owner identity empty",
    )


__all__ = ["assert_imported_runtime_startup_protocol_query"]
