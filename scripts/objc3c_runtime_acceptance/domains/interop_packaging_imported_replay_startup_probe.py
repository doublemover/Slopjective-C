"""Imported-runtime packaging startup probe assertions."""

from __future__ import annotations

from dataclasses import dataclass
from typing import Any

from ..expectation_matching import expect


@dataclass(frozen=True)
class ImportedRuntimeStartupDispatchValues:
    imported_provider_class_value: int
    imported_provider_protocol_value: int
    local_consumer_class_value: int


def assert_imported_runtime_startup_probe_payload(
    payload: dict[str, Any],
    link_plan: dict[str, Any],
) -> ImportedRuntimeStartupDispatchValues:
    expect(
        payload.get("startup_registration_copy_status") == 0,
        "expected imported-runtime startup registration snapshot copy to succeed",
    )
    expect(
        payload.get("startup_registered_image_count") == 2,
        "expected imported-runtime startup to install two images",
    )
    expect(
        payload.get("startup_registered_image_count") == link_plan.get("module_image_count"),
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
    expect(
        payload.get("imported_entry_status") == 0
        and payload.get("imported_entry_found") == 1,
        "expected imported provider runtime metadata to be realized at startup",
    )
    expect(
        payload.get("local_entry_status") == 0
        and payload.get("local_entry_found") == 1,
        "expected local consumer runtime metadata to be realized at startup",
    )
    expect(
        payload.get("imported_registration_order_ordinal") == 1,
        "expected imported provider runtime metadata to preserve registration ordinal one",
    )
    expect(
        payload.get("local_registration_order_ordinal") == 2,
        "expected local consumer runtime metadata to preserve registration ordinal two",
    )
    expect(
        payload.get("imported_direct_protocol_count") == 1,
        "expected imported provider class to publish one direct protocol",
    )
    expect(
        payload.get("imported_attached_protocol_count") == 0,
        "expected imported provider class to publish no attached protocols",
    )
    expect(
        payload.get("imported_runtime_property_accessor_count") == 0,
        "expected imported provider class to publish no runtime property accessors",
    )
    expect(
        payload.get("imported_module_name") == "runtimePackagingProvider",
        "expected imported provider class entry to preserve the provider module name",
    )
    expect(
        isinstance(payload.get("imported_translation_unit_identity_key"), str)
        and payload.get("imported_translation_unit_identity_key") != "",
        "expected imported provider class entry to publish a non-empty translation unit identity key",
    )
    expect(
        isinstance(payload.get("imported_class_owner_identity"), str)
        and payload.get("imported_class_owner_identity") != "",
        "expected imported provider class owner identity to be non-empty",
    )
    expect(
        payload.get("local_direct_protocol_count") == 0,
        "expected local consumer class to publish no direct protocols",
    )
    expect(
        payload.get("local_attached_protocol_count") == 0,
        "expected local consumer class to publish no attached protocols",
    )
    expect(
        payload.get("local_runtime_property_accessor_count") == 0,
        "expected local consumer class to publish no runtime property accessors",
    )
    expect(
        payload.get("local_module_name") == "runtimePackagingConsumer",
        "expected local consumer class entry to preserve the consumer module name",
    )
    expect(
        isinstance(payload.get("local_translation_unit_identity_key"), str)
        and payload.get("local_translation_unit_identity_key") != "",
        "expected local consumer class entry to publish a non-empty translation unit identity key",
    )
    expect(
        isinstance(payload.get("local_class_owner_identity"), str)
        and payload.get("local_class_owner_identity") != "",
        "expected local consumer class owner identity to be non-empty",
    )
    expect(
        payload.get("protocol_query_attached_category_count") == 0,
        "expected imported-runtime protocol conformance query to publish no attached categories",
    )
    imported_provider_class_value = payload.get("imported_provider_class_value")
    imported_provider_protocol_value = payload.get("imported_provider_protocol_value")
    local_consumer_class_value = payload.get("local_consumer_class_value")
    expect(
        isinstance(imported_provider_class_value, int)
        and imported_provider_class_value == 43,
        "expected imported provider class dispatch to execute the provider class method",
    )
    expect(
        isinstance(imported_provider_protocol_value, int)
        and imported_provider_protocol_value == 41,
        "expected imported provider protocol method dispatch to execute the provider implementation",
    )
    expect(
        isinstance(local_consumer_class_value, int)
        and local_consumer_class_value == 53,
        "expected local consumer class dispatch to execute the local class method",
    )
    expect(
        payload.get("selector_table_status") == 0,
        "expected imported-runtime startup selector-table snapshot copy to succeed",
    )
    expect(
        payload.get("selector_table_entry_count") == 3,
        "expected imported-runtime startup to publish three selector entries",
    )
    expect(
        payload.get("selector_metadata_backed_selector_count") == 3,
        "expected imported-runtime startup to publish three metadata-backed selectors",
    )
    expect(
        payload.get("selector_dynamic_selector_count") == 0,
        "expected imported-runtime startup to avoid dynamic selector entries",
    )
    expect(
        payload.get("provider_selector_status") == 0
        and payload.get("provider_selector_found") == 1,
        "expected provider class selector metadata to be installed at startup",
    )
    expect(
        payload.get("provider_selector_metadata_backed") == 1,
        "expected provider class selector metadata to stay metadata-backed",
    )
    expect(
        payload.get("provider_selector_provider_count") == 1,
        "expected provider class selector metadata to name one provider",
    )
    expect(
        payload.get("provider_selector_first_ordinal") == 1,
        "expected provider class selector metadata to retain the provider registration ordinal",
    )
    expect(
        payload.get("provider_selector_last_ordinal") == 1,
        "expected provider class selector metadata to end at the provider registration ordinal",
    )
    expect(
        payload.get("imported_protocol_selector_status") == 0
        and payload.get("imported_protocol_selector_found") == 1,
        "expected imported protocol selector metadata to be installed at startup",
    )
    expect(
        payload.get("imported_protocol_selector_metadata_backed") == 1,
        "expected imported protocol selector metadata to stay metadata-backed",
    )
    expect(
        payload.get("imported_protocol_selector_provider_count") == 1,
        "expected imported protocol selector metadata to name one provider",
    )
    expect(
        payload.get("imported_protocol_selector_first_ordinal") == 1,
        "expected imported protocol selector metadata to retain the provider registration ordinal",
    )
    expect(
        payload.get("imported_protocol_selector_last_ordinal") == 1,
        "expected imported protocol selector metadata to end at the provider registration ordinal",
    )
    expect(
        payload.get("local_selector_status") == 0
        and payload.get("local_selector_found") == 1,
        "expected local class selector metadata to be installed at startup",
    )
    expect(
        payload.get("local_selector_metadata_backed") == 1,
        "expected local class selector metadata to stay metadata-backed",
    )
    expect(
        payload.get("local_selector_provider_count") == 1,
        "expected local class selector metadata to name one provider",
    )
    expect(
        payload.get("local_selector_first_ordinal") == 2,
        "expected local class selector metadata to retain the local registration ordinal",
    )
    expect(
        payload.get("local_selector_last_ordinal") == 2,
        "expected local class selector metadata to end at the local registration ordinal",
    )
    expect(
        payload.get("method_cache_state_status") == 0,
        "expected imported-runtime startup method-cache snapshot copy to succeed",
    )
    expect(
        payload.get("method_cache_entry_count") == 3,
        "expected imported-runtime startup to publish three method-cache entries",
    )
    expect(
        payload.get("method_cache_live_dispatch_count") == 3,
        "expected imported-runtime startup to publish three live dispatch entries",
    )
    expect(
        payload.get("method_cache_strict_dispatch_error_count") == 0,
        "expected imported-runtime startup to avoid metadata-backed strict dispatch errors",
    )
    expect(
        payload.get("method_cache_last_selector") == "localClassValue",
        "expected imported-runtime startup to publish the last resolved selector",
    )
    expect(
        payload.get("method_cache_last_resolved_class_name") == "LocalConsumer",
        "expected imported-runtime startup to resolve the last method-cache class name",
    )
    expect(
        payload.get("method_cache_last_resolved_owner_identity")
        == "implementation:LocalConsumer::class_method:localClassValue",
        "expected imported-runtime startup to resolve the last method-cache owner identity",
    )
    expect(
        payload.get("provider_method_status") == 0
        and payload.get("provider_method_found") == 1
        and payload.get("provider_method_resolved") == 1,
        "expected provider class method metadata to resolve at startup",
    )
    expect(
        payload.get("provider_method_owner_identity")
        == "implementation:ImportedProvider::class_method:providerClassValue",
        "expected provider class method metadata to publish the resolved owner identity at startup",
    )
    expect(
        payload.get("imported_protocol_method_status") == 0
        and payload.get("imported_protocol_method_found") == 1
        and payload.get("imported_protocol_method_resolved") == 1,
        "expected imported protocol method metadata to resolve at startup",
    )
    expect(
        payload.get("imported_protocol_method_owner_identity")
        == "implementation:ImportedProvider::class_method:importedProtocolValue",
        "expected imported protocol method metadata to publish the resolved owner identity at startup",
    )
    expect(
        payload.get("local_method_status") == 0
        and payload.get("local_method_found") == 1
        and payload.get("local_method_resolved") == 1,
        "expected local class method metadata to resolve at startup",
    )
    expect(
        payload.get("local_method_owner_identity")
        == "implementation:LocalConsumer::class_method:localClassValue",
        "expected local class method metadata to publish the resolved owner identity at startup",
    )
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

    return ImportedRuntimeStartupDispatchValues(
        imported_provider_class_value=imported_provider_class_value,
        imported_provider_protocol_value=imported_provider_protocol_value,
        local_consumer_class_value=local_consumer_class_value,
    )


__all__ = [
    "ImportedRuntimeStartupDispatchValues",
    "assert_imported_runtime_startup_probe_payload",
]
