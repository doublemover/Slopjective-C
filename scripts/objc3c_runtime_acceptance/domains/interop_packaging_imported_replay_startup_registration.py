"""Imported-runtime packaging startup registration assertions."""

from __future__ import annotations

from typing import Any

from ..expectation_matching import expect


def assert_imported_runtime_startup_registration_state(
    payload: dict[str, Any],
    link_plan: dict[str, Any],
) -> None:
    expect(
        payload.get("startup_registration_copy_status") == 0,
        "expected imported-runtime startup registration snapshot copy to succeed",
    )
    expect(
        payload.get("startup_registered_image_count") == 2,
        "expected imported-runtime startup to install two images",
    )
    expect(
        payload.get("startup_registered_image_count")
        == link_plan.get("module_image_count"),
        "expected imported-runtime startup image count to match the cross-module link plan",
    )
    expect(
        payload.get("startup_next_expected_registration_order_ordinal") == 3,
        "expected imported-runtime startup to advance the next registration ordinal to three",
    )
    expect(
        payload.get("startup_image_walk_status") == 0,
        "expected imported-runtime startup image-walk snapshot copy to succeed",
    )
    expect(
        payload.get("startup_walked_image_count") == 2,
        "expected imported-runtime startup to walk both imported and local images",
    )
    expect(
        payload.get("startup_last_walked_module_name") == "runtimePackagingConsumer",
        "expected imported-runtime startup to walk the local image last",
    )
    expect(
        payload.get("startup_graph_status") == 0,
        "expected imported-runtime startup realized-class graph snapshot copy to succeed",
    )
    expect(
        payload.get("startup_realized_class_count") == 2,
        "expected imported-runtime startup to realize both imported and local classes",
    )
    expect(
        payload.get("startup_root_class_count") == 2,
        "expected imported-runtime startup to publish both classes as roots",
    )
    expect(
        payload.get("startup_metaclass_edge_count") == 0,
        "expected imported-runtime startup realized-class graph to avoid metaclass edges",
    )


__all__ = ["assert_imported_runtime_startup_registration_state"]
