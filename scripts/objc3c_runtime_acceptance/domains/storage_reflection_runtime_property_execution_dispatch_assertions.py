"""Property dispatch assertions for storage/reflection execution probes."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .storage_reflection_runtime_property_execution_payload import (
    PropertyExecutionPayload,
)


def assert_property_execution_dispatches(facts: PropertyExecutionPayload) -> None:
    _assert_count_dispatch(facts)
    _assert_enabled_dispatch(facts)
    _assert_value_dispatch(facts)
    _assert_token_dispatch(facts)


def _assert_count_dispatch(facts: PropertyExecutionPayload) -> None:
    expect(facts.set_count_dispatch.get("last_dispatch_path") == "slow-path-live",
           "expected setCount: to execute through live synthesized accessor resolution")
    expect(facts.set_count_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setCount: to execute through the runtime property-setter builtin")
    expect(facts.set_count_dispatch.get("last_property_name") == facts.count_property.get("property_name"),
           "expected setCount: dispatch property name to match reflected property metadata")
    expect(facts.set_count_dispatch.get("last_property_base_identity") == facts.count_property.get("base_identity"),
           "expected setCount: dispatch base identity to match reflected property metadata")
    expect(facts.set_count_dispatch.get("last_property_slot_index") == facts.count_property.get("slot_index"),
           "expected setCount: dispatch slot index to match reflected property metadata")
    expect(facts.set_count_dispatch.get("last_selector") == facts.count_property.get("effective_setter_selector"),
           "expected setCount: dispatch selector to match reflected property metadata")
    expect(facts.set_count_dispatch.get("last_resolved_owner_identity") == facts.count_property.get("setter_owner_identity"),
           "expected setCount: dispatch ownership to match reflected property metadata")
    expect(facts.set_count_dispatch.get("last_used_builtin") == 1 and facts.set_count_dispatch.get("last_effective_direct_dispatch") == 0,
           "expected setCount: to remain builtin-backed and runtime-dispatched")
    expect(facts.set_count_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setCount: dispatch to report one setter parameter")
    expect(facts.count_dispatch.get("last_dispatch_path") == "slow-path-live",
           "expected count getter to execute through live synthesized accessor resolution")
    expect(facts.count_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected count getter to execute through the runtime property-getter builtin")
    expect(facts.count_dispatch.get("last_property_name") == facts.count_property.get("property_name"),
           "expected count getter dispatch property name to match reflected property metadata")
    expect(facts.count_dispatch.get("last_property_base_identity") == facts.count_property.get("base_identity"),
           "expected count getter dispatch base identity to match reflected property metadata")
    expect(facts.count_dispatch.get("last_property_slot_index") == facts.count_property.get("slot_index"),
           "expected count getter dispatch slot index to match reflected property metadata")
    expect(facts.count_dispatch.get("last_selector") == facts.count_property.get("effective_getter_selector"),
           "expected count getter dispatch selector to match reflected property metadata")
    expect(facts.count_dispatch.get("last_resolved_owner_identity") == facts.count_property.get("getter_owner_identity"),
           "expected count getter dispatch ownership to match reflected property metadata")
    expect(facts.count_dispatch.get("last_used_builtin") == 1 and facts.count_dispatch.get("last_effective_direct_dispatch") == 0,
           "expected count getter to remain builtin-backed and runtime-dispatched")
    expect(facts.count_dispatch.get("last_resolved_parameter_count") == 0,
           "expected count getter dispatch to report zero getter parameters")


def _assert_enabled_dispatch(facts: PropertyExecutionPayload) -> None:
    expect(facts.set_enabled_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setEnabled: to execute through the runtime property-setter builtin")
    expect(facts.set_enabled_dispatch.get("last_property_name") == facts.enabled_property.get("property_name"),
           "expected setEnabled: dispatch property name to match reflected property metadata")
    expect(facts.set_enabled_dispatch.get("last_property_base_identity") == facts.enabled_property.get("base_identity"),
           "expected setEnabled: dispatch base identity to match reflected property metadata")
    expect(facts.set_enabled_dispatch.get("last_property_slot_index") == facts.enabled_property.get("slot_index"),
           "expected setEnabled: dispatch slot index to match reflected property metadata")
    expect(facts.set_enabled_dispatch.get("last_selector") == facts.enabled_property.get("effective_setter_selector"),
           "expected setEnabled: dispatch selector to match reflected property metadata")
    expect(facts.set_enabled_dispatch.get("last_resolved_owner_identity") == facts.enabled_property.get("setter_owner_identity"),
           "expected setEnabled: dispatch ownership to match reflected property metadata")
    expect(facts.set_enabled_dispatch.get("last_used_builtin") == 1 and facts.set_enabled_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setEnabled: to remain builtin-backed and report one setter parameter")
    expect(facts.enabled_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected enabled getter to execute through the runtime property-getter builtin")
    expect(facts.enabled_dispatch.get("last_property_name") == facts.enabled_property.get("property_name"),
           "expected enabled getter dispatch property name to match reflected property metadata")
    expect(facts.enabled_dispatch.get("last_property_base_identity") == facts.enabled_property.get("base_identity"),
           "expected enabled getter dispatch base identity to match reflected property metadata")
    expect(facts.enabled_dispatch.get("last_property_slot_index") == facts.enabled_property.get("slot_index"),
           "expected enabled getter dispatch slot index to match reflected property metadata")
    expect(facts.enabled_dispatch.get("last_selector") == facts.enabled_property.get("effective_getter_selector"),
           "expected enabled getter dispatch selector to match reflected property metadata")
    expect(facts.enabled_dispatch.get("last_resolved_owner_identity") == facts.enabled_property.get("getter_owner_identity"),
           "expected enabled getter dispatch ownership to match reflected property metadata")
    expect(facts.enabled_dispatch.get("last_used_builtin") == 1 and facts.enabled_dispatch.get("last_resolved_parameter_count") == 0,
           "expected enabled getter to remain builtin-backed and report zero getter parameters")


def _assert_value_dispatch(facts: PropertyExecutionPayload) -> None:
    expect(facts.set_value_dispatch.get("last_implementation_kind") == "builtin-property-setter",
           "expected setCurrentValue: to execute through the runtime property-setter builtin")
    expect(facts.set_value_dispatch.get("last_property_name") == facts.value_property.get("property_name"),
           "expected setCurrentValue: dispatch property name to match reflected property metadata")
    expect(facts.set_value_dispatch.get("last_property_base_identity") == facts.value_property.get("base_identity"),
           "expected setCurrentValue: dispatch base identity to match reflected property metadata")
    expect(facts.set_value_dispatch.get("last_property_slot_index") == facts.value_property.get("slot_index"),
           "expected setCurrentValue: dispatch slot index to match reflected property metadata")
    expect(facts.set_value_dispatch.get("last_selector") == facts.value_property.get("effective_setter_selector"),
           "expected setCurrentValue: dispatch selector to match reflected property metadata")
    expect(facts.set_value_dispatch.get("last_resolved_owner_identity") == facts.value_property.get("setter_owner_identity"),
           "expected setCurrentValue: dispatch ownership to match reflected property metadata")
    expect(facts.set_value_dispatch.get("last_used_builtin") == 1 and facts.set_value_dispatch.get("last_resolved_parameter_count") == 1,
           "expected setCurrentValue: to remain builtin-backed and report one setter parameter")
    expect(facts.value_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected currentValue getter to execute through the runtime property-getter builtin")
    expect(facts.value_dispatch.get("last_property_name") == facts.value_property.get("property_name"),
           "expected currentValue getter dispatch property name to match reflected property metadata")
    expect(facts.value_dispatch.get("last_property_base_identity") == facts.value_property.get("base_identity"),
           "expected currentValue getter dispatch base identity to match reflected property metadata")
    expect(facts.value_dispatch.get("last_property_slot_index") == facts.value_property.get("slot_index"),
           "expected currentValue getter dispatch slot index to match reflected property metadata")
    expect(facts.value_dispatch.get("last_selector") == facts.value_property.get("effective_getter_selector"),
           "expected currentValue getter dispatch selector to match reflected property metadata")
    expect(facts.value_dispatch.get("last_resolved_owner_identity") == facts.value_property.get("getter_owner_identity"),
           "expected currentValue getter dispatch ownership to match reflected property metadata")
    expect(facts.value_dispatch.get("last_used_builtin") == 1 and facts.value_dispatch.get("last_resolved_parameter_count") == 0,
           "expected currentValue getter to remain builtin-backed and report zero getter parameters")


def _assert_token_dispatch(facts: PropertyExecutionPayload) -> None:
    expect(facts.token_dispatch.get("last_implementation_kind") == "builtin-property-getter",
           "expected tokenValue getter to execute through the runtime property-getter builtin")
    expect(facts.token_dispatch.get("last_property_name") == facts.token_property.get("property_name"),
           "expected tokenValue getter dispatch property name to match reflected property metadata")
    expect(facts.token_dispatch.get("last_property_base_identity") == facts.token_property.get("base_identity"),
           "expected tokenValue getter dispatch base identity to match reflected property metadata")
    expect(facts.token_dispatch.get("last_property_slot_index") == facts.token_property.get("slot_index"),
           "expected tokenValue getter dispatch slot index to match reflected property metadata")
    expect(facts.token_dispatch.get("last_selector") == facts.token_property.get("effective_getter_selector"),
           "expected tokenValue getter dispatch selector to match reflected property metadata")
    expect(facts.token_dispatch.get("last_resolved_owner_identity") == facts.token_property.get("getter_owner_identity"),
           "expected tokenValue getter dispatch ownership to match reflected property metadata")
    expect(facts.token_dispatch.get("last_used_builtin") == 1 and facts.token_dispatch.get("last_resolved_parameter_count") == 0,
           "expected tokenValue getter to remain builtin-backed and report zero getter parameters")


__all__ = ["assert_property_execution_dispatches"]
