"""Runtime payload and reflection assertions for property execution probes."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .storage_reflection_runtime_property_execution_payload import (
    PropertyExecutionPayload,
)


def assert_property_execution_runtime_payload(facts: PropertyExecutionPayload) -> None:
    _assert_materialized_widget_values(facts)
    _assert_runtime_backed_properties(facts)
    _assert_reflected_property_selectors(facts)
    _assert_reflected_property_ownership(facts)
    _assert_runtime_registry_resolution(facts)
    _assert_runtime_method_cache_resolution(facts)


def _assert_materialized_widget_values(facts: PropertyExecutionPayload) -> None:
    expect(facts.payload.get("widget_instance", 0) != 0, "expected alloc to materialize a Widget instance")
    expect(facts.payload.get("count_value") == 37, "expected synthesized count getter to return the stored value")
    expect(facts.payload.get("enabled_value") == 1, "expected synthesized enabled getter to return the stored value")
    expect(facts.payload.get("value_result") == 55, "expected synthesized strong property getter to return the stored value")
    expect(facts.widget_entry.get("found") == 1, "expected Widget to be realized during property execution")
    expect(facts.widget_entry.get("runtime_property_accessor_count", 0) >= 4,
           "expected Widget to publish runtime-backed synthesized accessors")
    expect(facts.registry_state.get("slot_backed_property_count", 0) >= 4,
           "expected property execution fixture to register four slot-backed properties")


def _assert_runtime_backed_properties(facts: PropertyExecutionPayload) -> None:
    expect(facts.count_property.get("has_runtime_getter") == 1 and facts.count_property.get("has_runtime_setter") == 1,
           "expected count property to execute through runtime-backed synthesized accessors")
    expect(facts.enabled_property.get("has_runtime_getter") == 1 and facts.enabled_property.get("has_runtime_setter") == 1,
           "expected enabled property to execute through runtime-backed synthesized accessors")
    expect(facts.value_property.get("has_runtime_getter") == 1 and facts.value_property.get("has_runtime_setter") == 1,
           "expected value property to execute through runtime-backed synthesized accessors")
    expect(facts.token_property.get("has_runtime_getter") == 1 and facts.token_property.get("setter_available") == 0,
           "expected readonly token property to expose only the synthesized getter")


def _assert_reflected_property_selectors(facts: PropertyExecutionPayload) -> None:
    expect(facts.count_property.get("property_name") == "count",
           "expected count property reflection to stay coherent")
    expect(facts.count_property.get("effective_getter_selector") == "count",
           "expected count getter selector reflection to stay coherent")
    expect(facts.count_property.get("effective_setter_selector") == "setCount:",
           "expected count setter selector reflection to stay coherent")
    expect(facts.enabled_property.get("effective_getter_selector") == "enabled",
           "expected enabled getter selector reflection to stay coherent")
    expect(facts.enabled_property.get("effective_setter_selector") == "setEnabled:",
           "expected enabled setter selector reflection to stay coherent")
    expect(facts.value_property.get("effective_getter_selector") == "currentValue",
           "expected value getter selector reflection to stay coherent")
    expect(facts.value_property.get("effective_setter_selector") == "setCurrentValue:",
           "expected value setter selector reflection to stay coherent")
    expect(facts.token_property.get("effective_getter_selector") == "tokenValue",
           "expected token getter selector reflection to stay coherent")


def _assert_reflected_property_ownership(facts: PropertyExecutionPayload) -> None:
    expect(facts.count_property.get("getter_owner_identity"), "expected count getter owner identity to be published")
    expect(facts.count_property.get("setter_owner_identity"), "expected count setter owner identity to be published")
    expect(facts.enabled_property.get("getter_owner_identity"), "expected enabled getter owner identity to be published")
    expect(facts.enabled_property.get("setter_owner_identity"), "expected enabled setter owner identity to be published")
    expect(facts.value_property.get("getter_owner_identity"), "expected value getter owner identity to be published")
    expect(facts.value_property.get("setter_owner_identity"), "expected value setter owner identity to be published")
    expect(facts.token_property.get("getter_owner_identity"), "expected token getter owner identity to be published")
    expect(facts.token_property.get("setter_owner_identity") is None,
           "did not expect readonly token property to publish a setter owner identity")
    expect(facts.count_property.get("base_identity") == facts.widget_entry.get("base_identity"),
           "expected count property base identity to match the realized Widget class")
    expect(facts.enabled_property.get("base_identity") == facts.widget_entry.get("base_identity"),
           "expected enabled property base identity to match the realized Widget class")
    expect(facts.value_property.get("base_identity") == facts.widget_entry.get("base_identity"),
           "expected value property base identity to match the realized Widget class")
    expect(facts.token_property.get("base_identity") == facts.widget_entry.get("base_identity"),
           "expected token property base identity to match the realized Widget class")


def _assert_runtime_registry_resolution(facts: PropertyExecutionPayload) -> None:
    expect(facts.registry_state.get("last_resolved_class_name") == "Widget",
           "expected property registry to resolve Widget during live accessor execution")
    expect(facts.registry_state.get("last_resolved_owner_identity"),
           "expected property registry to publish the resolved owner identity")


def _assert_runtime_method_cache_resolution(facts: PropertyExecutionPayload) -> None:
    expect(facts.count_method.get("resolved") == 1 and facts.count_method.get("parameter_count") == 0,
           "expected count getter dispatch to resolve live through the runtime cache")
    expect(facts.enabled_method.get("resolved") == 1 and facts.enabled_method.get("parameter_count") == 0,
           "expected enabled getter dispatch to resolve live through the runtime cache")
    expect(facts.value_method.get("resolved") == 1 and facts.value_method.get("parameter_count") == 0,
           "expected currentValue getter dispatch to resolve live through the runtime cache")
    expect(facts.token_method.get("resolved") == 1 and facts.token_method.get("parameter_count") == 0,
           "expected tokenValue getter dispatch to resolve live through the runtime cache")
    expect(facts.count_method.get("resolved_owner_identity") == facts.count_property.get("getter_owner_identity"),
           "expected count getter cache ownership to match reflected property ownership")
    expect(facts.enabled_method.get("resolved_owner_identity") == facts.enabled_property.get("getter_owner_identity"),
           "expected enabled getter cache ownership to match reflected property ownership")
    expect(facts.value_method.get("resolved_owner_identity") == facts.value_property.get("getter_owner_identity"),
           "expected currentValue getter cache ownership to match reflected property ownership")
    expect(facts.token_method.get("resolved_owner_identity") == facts.token_property.get("getter_owner_identity"),
           "expected tokenValue getter cache ownership to match reflected property ownership")


__all__ = ["assert_property_execution_runtime_payload"]
