"""Result classification for runnable interop end-to-end validation."""

from __future__ import annotations

from .assertions import expect


def assert_runtime_interop_link_plan(
    *,
    consumer_link_plan: dict[str, object],
    provider_bridge_json: dict[str, object],
    provider_import_surface: dict[str, object],
) -> None:
    expect(
        consumer_link_plan.get("module_image_count") == 2
        and consumer_link_plan.get("direct_import_input_count") == 1,
        "packaged interop consumer link plan drifted from the two-image mixed-module topology",
    )
    expect(
        consumer_link_plan.get("expected_interop_bridge_header_artifact_relative_path")
        == "module.interop-bridge.h"
        and consumer_link_plan.get("expected_interop_bridge_module_artifact_relative_path")
        == "module.interop-bridge.modulemap"
        and consumer_link_plan.get("expected_interop_bridge_artifact_relative_path")
        == "module.interop-bridge.json",
        "packaged interop consumer link plan drifted from the bridge artifact paths",
    )
    expect(
        consumer_link_plan.get("interop_ffi_imported_module_count") == 1
        and consumer_link_plan.get("interop_header_module_bridge_imported_module_count") == 1,
        "packaged interop consumer link plan drifted from the imported ffi and bridge module counts",
    )
    expect(
        isinstance(provider_import_surface.get("module_name"), str)
        and provider_import_surface.get("module_name") != ""
        and provider_bridge_json.get("header_artifact_relative_path") == "module.interop-bridge.h"
        and provider_bridge_json.get("module_artifact_relative_path") == "module.interop-bridge.modulemap"
        and provider_bridge_json.get("bridge_artifact_relative_path") == "module.interop-bridge.json",
        "packaged interop provider artifacts drifted from the emitted provider identity or bridge artifact paths",
    )


def assert_header_bridge_link_plan(header_consumer_link_plan: dict[str, object]) -> None:
    expect(
        "m274_header_module_bridge_provider"
        in header_consumer_link_plan.get(
            "interop_header_module_bridge_imported_module_names_lexicographic", []
        ),
        "packaged header-bridge consumer link plan drifted from the imported bridge provider identity",
    )


__all__ = ["assert_header_bridge_link_plan", "assert_runtime_interop_link_plan"]
