"""Instance allocation layout assertions for storage/reflection runtime probes."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

from .storage_reflection_runtime_layout_payload import InstanceAllocationLayoutPayload


def assert_instance_allocation_layout_payload(
    facts: InstanceAllocationLayoutPayload,
) -> None:
    _assert_instance_allocation_ir_surface(facts)
    _assert_instance_allocation_manifest_surfaces(facts)
    _assert_instance_allocation_runtime_values(facts)
    _assert_instance_allocation_runtime_tables(facts)
    _assert_instance_allocation_graph_state(facts)
    _assert_instance_allocation_cache_entries(facts)


def _assert_instance_allocation_ir_surface(
    facts: InstanceAllocationLayoutPayload,
) -> None:
    expect(
        "; runtime_instance_allocation_layout_support = "
        "contract=objc3c.runtime.instance.allocation.layout.support.v1"
        in facts.ll_text,
        "expected LLVM IR to publish the runtime instance allocation/layout support surface",
    )
    expect(
        "; runtime_property_layout_consumption = "
        "contract=objc3c.runtime.property.layout.consumption.freeze.v1"
        in facts.ll_text,
        "expected LLVM IR to preserve the property/layout consumption surface coupled to instance allocation",
    )
    expect(
        "synthesized_accessor_entries=6" in facts.ll_text,
        "expected instance allocation fixture to preserve six synthesized accessors",
    )
    expect(
        "property_descriptor_entries=" in facts.ll_text,
        "expected instance allocation fixture to publish property descriptors",
    )
    expect(
        "ivar_layout_owner_entries=" in facts.ll_text,
        "expected instance allocation fixture to publish ivar layout owners",
    )


def _assert_instance_allocation_manifest_surfaces(
    facts: InstanceAllocationLayoutPayload,
) -> None:
    runtime_surface = facts.manifest.get(
        "runtime_property_ivar_storage_accessor_source_surface",
        {},
    )
    expect(
        runtime_surface.get("storage_semantics_model")
        == "interface-owned-property-layout-slots-sizes-alignment-init-order-and-reverse-destruction-order-remain-deterministic-before-runtime-allocation",
        "expected compile manifest to keep the source layout model coupled to runtime allocation",
    )
    synthesized_surface = facts.manifest.get(
        "executable_synthesized_accessor_property_lowering_surface",
        {},
    )
    expect(
        synthesized_surface.get("storage_model")
        == "synthesized-getter-setter-bodies-lower-directly-to-runtime-current-property-helper-calls-without-storage-globals",
        "expected synthesized accessor lowering to route storage through runtime helpers",
    )


def _assert_instance_allocation_runtime_values(
    facts: InstanceAllocationLayoutPayload,
) -> None:
    expect(facts.first_alloc == 1048576, "expected first runtime instance identity to start at 1048576")
    expect(facts.second_alloc == 1048577, "expected second runtime instance identity to increment deterministically")
    expect(facts.first_alloc != facts.second_alloc, "expected alloc to materialize distinct receiver identities")
    expect(facts.payload.get("set_count_first") == 0, "expected first count setter dispatch to return zero")
    expect(facts.payload.get("count_value_first") == 37, "expected first count getter to read its written value")
    expect(facts.payload.get("count_value_second_before") == 0, "expected second count getter to start from zero-filled storage")
    expect(facts.payload.get("set_enabled_first") == 0, "expected first enabled setter dispatch to return zero")
    expect(facts.payload.get("enabled_value_first") == 1, "expected first enabled getter to read its written value")
    expect(facts.payload.get("enabled_value_second") == 0, "expected second enabled getter to start from zero-filled storage")
    expect(facts.payload.get("set_value_first") == 0, "expected first strong value setter dispatch to return zero")
    expect(facts.payload.get("value_result_first") == 55, "expected first strong value getter to read its retained slot value")
    expect(facts.payload.get("value_result_second_before") == 0, "expected second strong value getter to start from nil/zero storage")
    expect(facts.payload.get("set_count_second") == 0, "expected second count setter dispatch to return zero")
    expect(facts.payload.get("count_value_first_after_second") == 37, "expected second count write not to affect the first instance")
    expect(facts.payload.get("count_value_second_after") == 9, "expected second count getter to read its own written value")
    expect(facts.payload.get("set_value_second") == 0, "expected second strong value setter dispatch to return zero")
    expect(facts.payload.get("value_result_first_after_second") == 55, "expected second value write not to affect the first instance")
    expect(facts.payload.get("value_result_second_after") == 91, "expected second value getter to read its own written value")


def _assert_instance_allocation_runtime_tables(
    facts: InstanceAllocationLayoutPayload,
) -> None:
    expect(
        facts.registration_state.get("registered_image_count", 0) >= 1,
        "expected registered image state for instance allocation probe",
    )
    expect(
        facts.registration_state.get("registered_descriptor_total", 0) >= 1,
        "expected registered descriptors for instance allocation probe",
    )
    expect(
        facts.selector_state.get("selector_table_entry_count", 0) >= 6,
        "expected synthesized accessor selectors in the selector table",
    )
    expect(
        facts.selector_state.get("metadata_backed_selector_count", 0) >= 6,
        "expected selector table to distinguish metadata-backed selectors",
    )


def _assert_instance_allocation_graph_state(
    facts: InstanceAllocationLayoutPayload,
) -> None:
    expect(facts.graph_state.get("realized_class_count") == 1, "expected one realized Widget class")
    expect(facts.graph_state.get("root_class_count") == 1, "expected Widget to be realized as a root class")
    expect(facts.graph_state.get("receiver_class_binding_count") == 1, "expected one class receiver binding")
    expect(facts.graph_state.get("live_instance_count") == 2, "expected two live runtime instances")
    expect(
        facts.graph_state.get("last_allocated_receiver_identity") == facts.second_alloc,
        "expected graph state to record the last allocated receiver",
    )
    expect(facts.graph_state.get("last_allocated_base_identity") == 1024, "expected graph state to record the Widget class base identity")
    expect(facts.graph_state.get("last_allocated_instance_size_bytes") == 16, "expected Widget instance storage size to remain 16 bytes")
    expect(facts.graph_state.get("last_allocated_class_name") == "Widget", "expected graph state to record the allocated class name")
    expect(facts.widget_entry.get("found") == 1, "expected Widget realized class entry to be queryable")
    expect(facts.widget_entry.get("base_identity") == 1024, "expected Widget base identity to remain deterministic")
    expect(facts.widget_entry.get("is_root_class") == 1, "expected Widget fixture to be a root class")
    expect(facts.widget_entry.get("implementation_backed") == 1, "expected Widget entry to be implementation backed")
    expect(facts.widget_entry.get("runtime_property_accessor_count") == 3, "expected Widget to publish three runtime-backed property accessors")
    expect(facts.widget_entry.get("runtime_instance_size_bytes") == 16, "expected Widget entry to publish 16 bytes of instance storage")
    expect(facts.widget_entry.get("class_owner_identity") == "class:Widget", "expected Widget class owner identity")
    expect(facts.widget_entry.get("metaclass_owner_identity") == "metaclass:Widget", "expected Widget metaclass owner identity")


def _assert_instance_allocation_cache_entries(
    facts: InstanceAllocationLayoutPayload,
) -> None:
    expect(
        facts.count_entry.get("found") == 1 and facts.count_entry.get("resolved") == 1,
        "expected count getter cache entry to resolve",
    )
    expect(facts.count_entry.get("dispatch_family_is_class") == 0, "expected count getter dispatch to be instance-family")
    expect(facts.count_entry.get("normalized_receiver_identity") == 1025, "expected instance dispatch to normalize to Widget instance identity")
    expect(facts.count_entry.get("parameter_count") == 0, "expected count getter cache entry to preserve zero parameters")
    expect(
        facts.count_entry.get("resolved_owner_identity")
        == "implementation:Widget::instance_method:count",
        "expected count getter cache entry to preserve synthesized owner identity",
    )
    expect(
        facts.set_count_entry.get("found") == 1
        and facts.set_count_entry.get("resolved") == 1,
        "expected setCount setter cache entry to resolve",
    )
    expect(facts.set_count_entry.get("dispatch_family_is_class") == 0, "expected setCount setter dispatch to be instance-family")
    expect(facts.set_count_entry.get("normalized_receiver_identity") == 1025, "expected setter dispatch to normalize to Widget instance identity")
    expect(facts.set_count_entry.get("parameter_count") == 1, "expected setCount setter cache entry to preserve one parameter")
    expect(
        facts.set_count_entry.get("resolved_owner_identity")
        == "implementation:Widget::instance_method:setCount:",
        "expected setCount setter cache entry to preserve synthesized owner identity",
    )


__all__ = ["assert_instance_allocation_layout_payload"]
