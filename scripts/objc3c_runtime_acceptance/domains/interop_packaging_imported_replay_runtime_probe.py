"""Imported-runtime packaging post-replay probe assertions."""

from __future__ import annotations

from typing import Any

from ..assertions import expect
from objc3c_runtime_acceptance.domains.interop_packaging_imported_replay_startup_probe import (
    ImportedRuntimeStartupDispatchValues,
)


def assert_imported_runtime_replay_probe_payload(
    payload: dict[str, Any],
    link_plan: dict[str, Any],
    provider_identity: str,
    consumer_identity: str,
    startup_values: ImportedRuntimeStartupDispatchValues,
) -> None:
    expect(
        payload.get("post_reset_registration_copy_status") == 0,
        "expected post-reset registration snapshot copy to succeed",
    )
    expect(
        payload.get("post_reset_replay_copy_status") == 0,
        "expected post-reset replay snapshot copy to succeed",
    )
    expect(
        payload.get("post_reset_registered_image_count") == 0,
        "expected reset to clear installed images before replay",
    )
    expect(
        payload.get("post_reset_retained_bootstrap_image_count") == 2,
        "expected reset to retain both imported and local bootstrap images for replay",
    )
    expect(
        payload.get("post_reset_generation") == 1,
        "expected reset to advance the reset generation before replay",
    )
    expect(
        payload.get("replay_status") == 0,
        "expected imported runtime replay to succeed",
    )
    expect(
        payload.get("post_replay_registration_copy_status") == 0,
        "expected post-replay registration snapshot copy to succeed",
    )
    expect(
        payload.get("post_replay_image_walk_status") == 0,
        "expected post-replay image-walk snapshot copy to succeed",
    )
    expect(
        payload.get("post_replay_graph_status") == 0,
        "expected post-replay realized-class graph snapshot copy to succeed",
    )
    expect(
        payload.get("post_replay_replay_copy_status") == 0,
        "expected post-replay replay snapshot copy to succeed",
    )
    expect(
        payload.get("post_replay_registered_image_count") == 2,
        "expected replay to restore both imported and local images",
    )
    expect(
        payload.get("post_replay_registered_image_count")
        == link_plan.get("module_image_count"),
        "expected replay image count to match the cross-module link plan",
    )
    expect(
        payload.get("post_replay_next_expected_registration_order_ordinal") == 3,
        "expected replay to restore the next registration ordinal to three",
    )
    expect(
        payload.get("post_replay_walked_image_count") == 2,
        "expected replay to walk both imported and local images",
    )
    expect(
        payload.get("post_replay_last_walked_module_name") == "runtimePackagingConsumer",
        "expected replay to walk the local image last",
    )
    expect(
        payload.get("post_replay_realized_class_count") == 2,
        "expected replay to restore both realized classes",
    )
    expect(
        payload.get("post_replay_replay_generation", 0) >= 1,
        "expected replay to advance the replay generation",
    )
    expect(
        payload.get("post_replay_retained_bootstrap_image_count") == 2,
        "expected replay to preserve both retained bootstrap images",
    )
    expect(
        payload.get("post_replay_imported_entry_status") == 0
        and payload.get("post_replay_imported_entry_found") == 1,
        "expected imported provider runtime metadata to survive replay",
    )
    expect(
        payload.get("post_replay_local_entry_status") == 0
        and payload.get("post_replay_local_entry_found") == 1,
        "expected local consumer runtime metadata to survive replay",
    )
    expect(
        payload.get("post_replay_imported_module_name") == "runtimePackagingProvider",
        "expected replay to preserve the provider module name",
    )
    expect(
        payload.get("post_replay_imported_translation_unit_identity_key")
        == provider_identity,
        "expected replay to preserve the provider translation unit identity key",
    )
    expect(
        payload.get("post_replay_local_module_name") == "runtimePackagingConsumer",
        "expected replay to preserve the consumer module name",
    )
    expect(
        payload.get("post_replay_local_translation_unit_identity_key")
        == consumer_identity,
        "expected replay to preserve the consumer translation unit identity key",
    )
    expect(
        payload.get("post_replay_imported_provider_class_value")
        == startup_values.imported_provider_class_value
        == 43,
        "expected imported provider class dispatch value to survive replay",
    )
    expect(
        payload.get("post_replay_imported_provider_protocol_value")
        == startup_values.imported_provider_protocol_value
        == 41,
        "expected imported provider protocol dispatch value to survive replay",
    )
    expect(
        payload.get("post_replay_local_consumer_class_value")
        == startup_values.local_consumer_class_value
        == 53,
        "expected local consumer class dispatch value to survive replay",
    )
    expect(
        payload.get("post_replay_selector_table_status") == 0,
        "expected replay selector-table snapshot copy to succeed",
    )
    expect(
        payload.get("post_replay_selector_table_entry_count") == 3,
        "expected replay to restore three selector entries",
    )
    expect(
        payload.get("post_replay_selector_metadata_backed_selector_count") == 3,
        "expected replay to restore three metadata-backed selectors",
    )
    expect(
        payload.get("post_replay_provider_selector_status") == 0
        and payload.get("post_replay_provider_selector_found") == 1,
        "expected provider selector metadata to survive replay",
    )
    expect(
        payload.get("post_replay_imported_protocol_selector_status") == 0
        and payload.get("post_replay_imported_protocol_selector_found") == 1,
        "expected imported protocol selector metadata to survive replay",
    )
    expect(
        payload.get("post_replay_local_selector_status") == 0
        and payload.get("post_replay_local_selector_found") == 1,
        "expected local selector metadata to survive replay",
    )
    expect(
        payload.get("post_replay_method_cache_state_status") == 0,
        "expected replay method-cache snapshot copy to succeed",
    )
    expect(
        payload.get("post_replay_method_cache_entry_count") == 3,
        "expected replay to restore three method-cache entries",
    )
    expect(
        payload.get("post_replay_method_cache_live_dispatch_count") == 3,
        "expected replay to restore three live dispatch entries",
    )
    expect(
        payload.get("post_replay_method_cache_strict_dispatch_error_count") == 0,
        "expected replay to avoid metadata-backed strict dispatch errors",
    )
    expect(
        payload.get("post_replay_method_cache_last_selector") == "localClassValue",
        "expected replay to preserve the last resolved selector",
    )
    expect(
        payload.get("post_replay_method_cache_last_resolved_class_name")
        == "LocalConsumer",
        "expected replay to preserve the last resolved method-cache class name",
    )
    expect(
        payload.get("post_replay_method_cache_last_resolved_owner_identity")
        == "implementation:LocalConsumer::class_method:localClassValue",
        "expected replay to preserve the last resolved method-cache owner identity",
    )
    expect(
        payload.get("post_replay_provider_method_status") == 0
        and payload.get("post_replay_provider_method_found") == 1
        and payload.get("post_replay_provider_method_resolved") == 1,
        "expected provider class method metadata to resolve after replay",
    )
    expect(
        payload.get("post_replay_provider_method_owner_identity")
        == "implementation:ImportedProvider::class_method:providerClassValue",
        "expected provider class method metadata to preserve its resolved owner identity after replay",
    )
    expect(
        payload.get("post_replay_imported_protocol_method_status") == 0
        and payload.get("post_replay_imported_protocol_method_found") == 1
        and payload.get("post_replay_imported_protocol_method_resolved") == 1,
        "expected imported protocol method metadata to resolve after replay",
    )
    expect(
        payload.get("post_replay_imported_protocol_method_owner_identity")
        == "implementation:ImportedProvider::class_method:importedProtocolValue",
        "expected imported protocol method metadata to preserve its resolved owner identity after replay",
    )
    expect(
        payload.get("post_replay_local_method_status") == 0
        and payload.get("post_replay_local_method_found") == 1
        and payload.get("post_replay_local_method_resolved") == 1,
        "expected local class method metadata to resolve after replay",
    )
    expect(
        payload.get("post_replay_local_method_owner_identity")
        == "implementation:LocalConsumer::class_method:localClassValue",
        "expected local class method metadata to preserve its resolved owner identity after replay",
    )


__all__ = ["assert_imported_runtime_replay_probe_payload"]
