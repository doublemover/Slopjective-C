"""Imported-runtime replay probe assertion catalog."""

from __future__ import annotations

from .data import PayloadEqualityExpectation
from .data import PayloadMinimumExpectation
from .data import PayloadResolvedMethodExpectation
from .data import PayloadStatusFlagExpectation


PROVIDER_MODULE_NAME = "runtimePackagingProvider"
CONSUMER_MODULE_NAME = "runtimePackagingConsumer"
IMPORTED_PROVIDER_CLASS_NAME = "ImportedProvider"
LOCAL_CONSUMER_CLASS_NAME = "LocalConsumer"

PROVIDER_CLASS_METHOD_OWNER_IDENTITY = (
    "implementation:ImportedProvider::class_method:providerClassValue"
)
IMPORTED_PROTOCOL_METHOD_OWNER_IDENTITY = (
    "implementation:ImportedProvider::class_method:importedProtocolValue"
)
LOCAL_CLASS_METHOD_OWNER_IDENTITY = (
    "implementation:LocalConsumer::class_method:localClassValue"
)

PROVIDER_CLASS_DISPATCH_VALUE = 43
IMPORTED_PROTOCOL_DISPATCH_VALUE = 41
LOCAL_CLASS_DISPATCH_VALUE = 53

POST_RESET_AND_REPLAY_COPY_EXPECTATIONS = (
    PayloadEqualityExpectation(
        "post_reset_registration_copy_status",
        0,
        "expected post-reset registration snapshot copy to succeed",
    ),
    PayloadEqualityExpectation(
        "post_reset_replay_copy_status",
        0,
        "expected post-reset replay snapshot copy to succeed",
    ),
    PayloadEqualityExpectation(
        "post_reset_registered_image_count",
        0,
        "expected reset to clear installed images before replay",
    ),
    PayloadEqualityExpectation(
        "post_reset_retained_bootstrap_image_count",
        2,
        "expected reset to retain both imported and local bootstrap images for replay",
    ),
    PayloadEqualityExpectation(
        "post_reset_generation",
        1,
        "expected reset to advance the reset generation before replay",
    ),
    PayloadEqualityExpectation(
        "replay_status",
        0,
        "expected imported runtime replay to succeed",
    ),
    PayloadEqualityExpectation(
        "post_replay_registration_copy_status",
        0,
        "expected post-replay registration snapshot copy to succeed",
    ),
    PayloadEqualityExpectation(
        "post_replay_image_walk_status",
        0,
        "expected post-replay image-walk snapshot copy to succeed",
    ),
    PayloadEqualityExpectation(
        "post_replay_graph_status",
        0,
        "expected post-replay realized-class graph snapshot copy to succeed",
    ),
    PayloadEqualityExpectation(
        "post_replay_replay_copy_status",
        0,
        "expected post-replay replay snapshot copy to succeed",
    ),
    PayloadEqualityExpectation(
        "post_replay_registered_image_count",
        2,
        "expected replay to restore both imported and local images",
    ),
)

POST_REPLAY_REGISTRATION_EXPECTATIONS = (
    PayloadEqualityExpectation(
        "post_replay_next_expected_registration_order_ordinal",
        3,
        "expected replay to restore the next registration ordinal to three",
    ),
    PayloadEqualityExpectation(
        "post_replay_walked_image_count",
        2,
        "expected replay to walk both imported and local images",
    ),
    PayloadEqualityExpectation(
        "post_replay_last_walked_module_name",
        CONSUMER_MODULE_NAME,
        "expected replay to walk the local image last",
    ),
    PayloadEqualityExpectation(
        "post_replay_realized_class_count",
        2,
        "expected replay to restore both realized classes",
    ),
)

POST_REPLAY_REPLAY_GENERATION_EXPECTATION = PayloadMinimumExpectation(
    "post_replay_replay_generation",
    1,
    0,
    "expected replay to advance the replay generation",
)

POST_REPLAY_BOOTSTRAP_EXPECTATIONS = (
    PayloadEqualityExpectation(
        "post_replay_retained_bootstrap_image_count",
        2,
        "expected replay to preserve both retained bootstrap images",
    ),
)

POST_REPLAY_METADATA_ENTRY_EXPECTATIONS = (
    PayloadStatusFlagExpectation(
        "post_replay_imported_entry_status",
        "post_replay_imported_entry_found",
        "expected imported provider runtime metadata to survive replay",
    ),
    PayloadStatusFlagExpectation(
        "post_replay_local_entry_status",
        "post_replay_local_entry_found",
        "expected local consumer runtime metadata to survive replay",
    ),
)

POST_REPLAY_MODULE_NAME_EXPECTATIONS = (
    PayloadEqualityExpectation(
        "post_replay_imported_module_name",
        PROVIDER_MODULE_NAME,
        "expected replay to preserve the provider module name",
    ),
    PayloadEqualityExpectation(
        "post_replay_local_module_name",
        CONSUMER_MODULE_NAME,
        "expected replay to preserve the consumer module name",
    ),
)

POST_REPLAY_SELECTOR_TABLE_EXPECTATIONS = (
    PayloadEqualityExpectation(
        "post_replay_selector_table_status",
        0,
        "expected replay selector-table snapshot copy to succeed",
    ),
    PayloadEqualityExpectation(
        "post_replay_selector_table_entry_count",
        3,
        "expected replay to restore three selector entries",
    ),
    PayloadEqualityExpectation(
        "post_replay_selector_metadata_backed_selector_count",
        3,
        "expected replay to restore three metadata-backed selectors",
    ),
)

POST_REPLAY_SELECTOR_ENTRY_EXPECTATIONS = (
    PayloadStatusFlagExpectation(
        "post_replay_provider_selector_status",
        "post_replay_provider_selector_found",
        "expected provider selector metadata to survive replay",
    ),
    PayloadStatusFlagExpectation(
        "post_replay_imported_protocol_selector_status",
        "post_replay_imported_protocol_selector_found",
        "expected imported protocol selector metadata to survive replay",
    ),
    PayloadStatusFlagExpectation(
        "post_replay_local_selector_status",
        "post_replay_local_selector_found",
        "expected local selector metadata to survive replay",
    ),
)

POST_REPLAY_METHOD_CACHE_EXPECTATIONS = (
    PayloadEqualityExpectation(
        "post_replay_method_cache_state_status",
        0,
        "expected replay method-cache snapshot copy to succeed",
    ),
    PayloadEqualityExpectation(
        "post_replay_method_cache_entry_count",
        3,
        "expected replay to restore three method-cache entries",
    ),
    PayloadEqualityExpectation(
        "post_replay_method_cache_live_dispatch_count",
        3,
        "expected replay to restore three live dispatch entries",
    ),
    PayloadEqualityExpectation(
        "post_replay_method_cache_strict_dispatch_error_count",
        0,
        "expected replay to avoid metadata-backed strict dispatch errors",
    ),
    PayloadEqualityExpectation(
        "post_replay_method_cache_last_selector",
        "localClassValue",
        "expected replay to preserve the last resolved selector",
    ),
    PayloadEqualityExpectation(
        "post_replay_method_cache_last_resolved_class_name",
        LOCAL_CONSUMER_CLASS_NAME,
        "expected replay to preserve the last resolved method-cache class name",
    ),
    PayloadEqualityExpectation(
        "post_replay_method_cache_last_resolved_owner_identity",
        LOCAL_CLASS_METHOD_OWNER_IDENTITY,
        "expected replay to preserve the last resolved method-cache owner identity",
    ),
)

POST_REPLAY_METHOD_ENTRY_EXPECTATIONS = (
    PayloadResolvedMethodExpectation(
        "post_replay_provider_method_status",
        "post_replay_provider_method_found",
        "post_replay_provider_method_resolved",
        "expected provider class method metadata to resolve after replay",
    ),
    PayloadResolvedMethodExpectation(
        "post_replay_imported_protocol_method_status",
        "post_replay_imported_protocol_method_found",
        "post_replay_imported_protocol_method_resolved",
        "expected imported protocol method metadata to resolve after replay",
    ),
    PayloadResolvedMethodExpectation(
        "post_replay_local_method_status",
        "post_replay_local_method_found",
        "post_replay_local_method_resolved",
        "expected local class method metadata to resolve after replay",
    ),
)

POST_REPLAY_METHOD_OWNER_EXPECTATIONS = (
    PayloadEqualityExpectation(
        "post_replay_provider_method_owner_identity",
        PROVIDER_CLASS_METHOD_OWNER_IDENTITY,
        "expected provider class method metadata to preserve its resolved owner identity after replay",
    ),
    PayloadEqualityExpectation(
        "post_replay_imported_protocol_method_owner_identity",
        IMPORTED_PROTOCOL_METHOD_OWNER_IDENTITY,
        "expected imported protocol method metadata to preserve its resolved owner identity after replay",
    ),
    PayloadEqualityExpectation(
        "post_replay_local_method_owner_identity",
        LOCAL_CLASS_METHOD_OWNER_IDENTITY,
        "expected local class method metadata to preserve its resolved owner identity after replay",
    ),
)


__all__ = [
    "CONSUMER_MODULE_NAME",
    "IMPORTED_PROTOCOL_DISPATCH_VALUE",
    "IMPORTED_PROTOCOL_METHOD_OWNER_IDENTITY",
    "IMPORTED_PROVIDER_CLASS_NAME",
    "LOCAL_CLASS_DISPATCH_VALUE",
    "LOCAL_CLASS_METHOD_OWNER_IDENTITY",
    "LOCAL_CONSUMER_CLASS_NAME",
    "POST_REPLAY_BOOTSTRAP_EXPECTATIONS",
    "POST_REPLAY_METADATA_ENTRY_EXPECTATIONS",
    "POST_REPLAY_METHOD_CACHE_EXPECTATIONS",
    "POST_REPLAY_METHOD_ENTRY_EXPECTATIONS",
    "POST_REPLAY_METHOD_OWNER_EXPECTATIONS",
    "POST_REPLAY_MODULE_NAME_EXPECTATIONS",
    "POST_REPLAY_REGISTRATION_EXPECTATIONS",
    "POST_REPLAY_REPLAY_GENERATION_EXPECTATION",
    "POST_REPLAY_SELECTOR_ENTRY_EXPECTATIONS",
    "POST_REPLAY_SELECTOR_TABLE_EXPECTATIONS",
    "POST_RESET_AND_REPLAY_COPY_EXPECTATIONS",
    "PROVIDER_CLASS_DISPATCH_VALUE",
    "PROVIDER_CLASS_METHOD_OWNER_IDENTITY",
    "PROVIDER_MODULE_NAME",
]
