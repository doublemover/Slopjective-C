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
        "synthesized_accessor_entries=8" in facts.ll_text,
        "expected inherited instance allocation fixture to preserve eight synthesized accessors",
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
    expect(facts.initialized_new == 1048578, "expected builtin new to allocate the third deterministic runtime instance identity")
    expect(facts.first_alloc != facts.second_alloc, "expected alloc to materialize distinct receiver identities")
    expect(
        facts.payload.get("first_init_result", {}).get("status_code") == 0
        and facts.payload.get("first_init_result", {}).get("object_reference") == facts.first_alloc,
        "expected first init to mark the allocated receiver initialized and return the same object identity",
    )
    expect(
        facts.payload.get("initialized_new_result", {}).get("status_code") == 0
        and facts.payload.get("initialized_new_result", {}).get("object_reference") == facts.initialized_new,
        "expected builtin new to allocate and initialize a runtime receiver",
    )
    expect(
        facts.payload.get("double_init_result", {}).get("status_code") == -4
        and facts.payload.get("double_init_result", {}).get("diagnostic_code") == "O3RT004",
        "expected double init to fail closed as malformed runtime lifecycle metadata",
    )
    expect(facts.payload.get("set_base_count_first") == 0, "expected inherited baseCount setter dispatch to return zero")
    expect(facts.payload.get("base_count_value_first") == 21, "expected first inherited baseCount getter to read its written value")
    expect(facts.payload.get("base_count_value_second_before") == 0, "expected second inherited baseCount getter to start from zero-filled storage")
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
    expect(facts.payload.get("set_base_count_second") == 0, "expected second inherited baseCount setter dispatch to return zero")
    expect(facts.payload.get("base_count_value_first_after_second") == 21, "expected second inherited baseCount write not to affect the first instance")
    expect(facts.payload.get("base_count_value_second_after") == 84, "expected second inherited baseCount getter to read its own written value")
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
        facts.selector_state.get("selector_table_entry_count", 0) >= 8,
        "expected inherited plus Widget synthesized accessor selectors in the selector table",
    )
    expect(
        facts.selector_state.get("metadata_backed_selector_count", 0) >= 8,
        "expected selector table to distinguish metadata-backed selectors",
    )


def _assert_instance_allocation_graph_state(
    facts: InstanceAllocationLayoutPayload,
) -> None:
    expect(facts.graph_state.get("realized_class_count") == 2, "expected realized Base and Widget classes")
    expect(facts.graph_state.get("root_class_count") == 1, "expected only Base to be realized as the root class")
    expect(facts.graph_state.get("receiver_class_binding_count") == 2, "expected Base and Widget receiver bindings")
    expect(facts.graph_state.get("live_instance_count") == 3, "expected alloc/new to leave three live runtime instances")
    expect(
        facts.graph_state.get("last_allocated_receiver_identity") == facts.initialized_new,
        "expected graph state to record the last allocated receiver from builtin new",
    )
    expect(
        facts.graph_state.get("last_allocated_base_identity") == facts.widget_entry.get("base_identity"),
        "expected graph state to record the Widget class base identity",
    )
    expect(
        facts.graph_state.get("last_allocated_instance_size_bytes") == facts.widget_entry.get("runtime_instance_size_bytes"),
        "expected graph state to record the inherited Widget instance storage size",
    )
    expect(
        facts.graph_state.get("last_allocated_allocation_ordinal") == 3,
        "expected graph state to preserve the deterministic allocation ordinal from builtin new",
    )
    expect(facts.graph_state.get("last_allocated_class_name") == "Widget", "expected graph state to record the allocated class name")
    expect(
        facts.graph_state.get("last_initialized_receiver_identity") == facts.initialized_new
        and facts.graph_state.get("last_initialized_initialization_ordinal") == 2,
        "expected graph state to preserve the latest initialized receiver and deterministic init ordinal",
    )
    expect(
        facts.graph_state.get("last_instance_lifecycle_failure_reason")
        == "runtime instance already initialized",
        "expected graph state to preserve the double-init fail-closed lifecycle reason",
    )
    expect(facts.base_entry.get("found") == 1, "expected Base realized class entry to be queryable")
    expect(facts.base_entry.get("is_root_class") == 1, "expected Base fixture to be the root class")
    expect(facts.base_entry.get("runtime_property_accessor_count") == 1, "expected Base to publish the inherited baseCount accessor pair")
    expect(facts.base_entry.get("runtime_instance_size_bytes", 0) > 0, "expected Base entry to publish non-zero inherited storage floor")
    expect(facts.widget_entry.get("found") == 1, "expected Widget realized class entry to be queryable")
    expect(facts.widget_entry.get("is_root_class") == 0, "expected Widget fixture to inherit from Base")
    expect(facts.widget_entry.get("has_super_node") == 1, "expected Widget entry to publish a realized superclass edge")
    expect(
        facts.widget_entry.get("super_base_identity") == facts.base_entry.get("base_identity"),
        "expected Widget superclass base identity to match Base",
    )
    expect(facts.widget_entry.get("implementation_backed") == 1, "expected Widget entry to be implementation backed")
    expect(facts.widget_entry.get("runtime_property_accessor_count") == 3, "expected Widget to publish three runtime-backed property accessors")
    expect(
        facts.widget_entry.get("runtime_instance_size_bytes", 0)
        >= facts.base_entry.get("runtime_instance_size_bytes", 0),
        "expected Widget instance storage to include at least Base storage",
    )
    expect(facts.widget_entry.get("class_owner_identity") == "class:Widget", "expected Widget class owner identity")
    expect(facts.widget_entry.get("metaclass_owner_identity") == "metaclass:Widget", "expected Widget metaclass owner identity")
    expect(facts.first_instance.get("found") == 1, "expected first instance snapshot to be queryable")
    expect(facts.second_instance.get("found") == 1, "expected second instance snapshot to be queryable")
    expect(facts.initialized_new_instance.get("found") == 1, "expected builtin new instance snapshot to be queryable")
    expect(facts.first_instance.get("class_name") == "Widget", "expected first instance to record Widget class")
    expect(facts.second_instance.get("class_name") == "Widget", "expected second instance to record Widget class")
    expect(facts.initialized_new_instance.get("class_name") == "Widget", "expected builtin new instance to record Widget class")
    expect(
        facts.first_instance.get("initialized") == 1
        and facts.first_instance.get("initialization_ordinal") == 1,
        "expected first init to publish deterministic initialization state",
    )
    expect(
        facts.second_instance.get("initialized") == 0
        and facts.second_instance.get("initialization_ordinal") == 0,
        "expected raw alloc without init to remain explicitly uninitialized",
    )
    expect(
        facts.initialized_new_instance.get("initialized") == 1
        and facts.initialized_new_instance.get("initialization_ordinal") == 2,
        "expected builtin new to publish deterministic initialization state",
    )
    expect(
        facts.first_instance.get("normalized_receiver_identity")
        == facts.widget_entry.get("base_identity", 0) + 1
        and facts.first_instance.get("class_receiver_identity")
        == facts.widget_entry.get("base_identity", 0) + 2,
        "expected instance snapshots to expose normalized instance/class receiver identities",
    )
    expect(
        facts.first_instance.get("class_owner_identity") == "class:Widget"
        and facts.first_instance.get("metaclass_owner_identity") == "metaclass:Widget"
        and facts.first_instance.get("instance_isa_owner_identity") == "class:Widget"
        and facts.first_instance.get("class_object_isa_owner_identity") == "metaclass:Widget",
        "expected instance snapshots to expose class/metaclass isa owner identities",
    )
    expect(
        facts.first_instance.get("instance_size_bytes") == facts.widget_entry.get("runtime_instance_size_bytes"),
        "expected first instance allocation size to match Widget runtime layout",
    )
    expect(
        facts.second_instance.get("instance_size_bytes") == facts.widget_entry.get("runtime_instance_size_bytes"),
        "expected second instance allocation size to match Widget runtime layout",
    )
    expect(
        facts.first_instance.get("zero_initialized_storage_byte_count", 0)
        < facts.first_instance.get("storage_size_bytes", 0),
        "expected first instance storage snapshot to observe post-write non-zero bytes",
    )
    expect(
        facts.second_instance.get("zero_initialized_storage_byte_count", 0)
        < facts.second_instance.get("storage_size_bytes", 0),
        "expected second instance storage snapshot to observe post-write non-zero bytes",
    )
    expect(
        facts.base_count_property.get("found") == 1 and facts.base_count_property.get("inherited") == 1,
        "expected Widget baseCount reflection to resolve inherited Base storage",
    )
    expect(
        facts.count_property.get("found") == 1 and facts.count_property.get("inherited") == 0,
        "expected Widget count reflection to resolve local Widget storage",
    )
    expect(
        facts.count_property.get("inherited_size_bytes") == facts.base_count_property.get("owner_size_bytes"),
        "expected Widget count inherited size to equal Base owner storage size",
    )
    expect(
        facts.count_property.get("offset_bytes", 0) >= facts.count_property.get("inherited_size_bytes", 0),
        "expected Widget count offset to start at or after inherited storage",
    )
    expect(
        facts.value_property.get("owner_size_bytes", 0) == facts.widget_entry.get("runtime_instance_size_bytes"),
        "expected reflected value owner size to match Widget instance size",
    )


def _assert_instance_allocation_cache_entries(
    facts: InstanceAllocationLayoutPayload,
) -> None:
    expect(
        facts.base_count_entry.get("found") == 1 and facts.base_count_entry.get("resolved") == 1,
        "expected inherited baseCount getter cache entry to resolve",
    )
    expect(facts.base_count_entry.get("parameter_count") == 0, "expected baseCount getter cache entry to preserve zero parameters")
    expect(
        facts.base_count_entry.get("resolved_owner_identity")
        == facts.base_count_property.get("getter_owner_identity"),
        "expected inherited baseCount getter cache owner to match reflected property owner",
    )
    expect(
        facts.set_base_count_entry.get("found") == 1
        and facts.set_base_count_entry.get("resolved") == 1,
        "expected inherited setBaseCount setter cache entry to resolve",
    )
    expect(facts.set_base_count_entry.get("parameter_count") == 1, "expected baseCount setter cache entry to preserve one parameter")
    expect(
        facts.set_base_count_entry.get("resolved_owner_identity")
        == facts.base_count_property.get("setter_owner_identity"),
        "expected inherited baseCount setter cache owner to match reflected property owner",
    )
    expect(
        facts.count_entry.get("found") == 1 and facts.count_entry.get("resolved") == 1,
        "expected count getter cache entry to resolve",
    )
    expect(facts.count_entry.get("dispatch_family_is_class") == 0, "expected count getter dispatch to be instance-family")
    expect(
        facts.count_entry.get("normalized_receiver_identity")
        == facts.widget_entry.get("base_identity", 0) + 1,
        "expected count getter dispatch to normalize to Widget instance identity",
    )
    expect(facts.count_entry.get("parameter_count") == 0, "expected count getter cache entry to preserve zero parameters")
    expect(
        facts.count_entry.get("resolved_owner_identity")
        == facts.count_property.get("getter_owner_identity"),
        "expected count getter cache entry to match reflected property owner",
    )
    expect(
        facts.set_count_entry.get("found") == 1
        and facts.set_count_entry.get("resolved") == 1,
        "expected setCount setter cache entry to resolve",
    )
    expect(facts.set_count_entry.get("dispatch_family_is_class") == 0, "expected setCount setter dispatch to be instance-family")
    expect(
        facts.set_count_entry.get("normalized_receiver_identity")
        == facts.widget_entry.get("base_identity", 0) + 1,
        "expected setter dispatch to normalize to Widget instance identity",
    )
    expect(facts.set_count_entry.get("parameter_count") == 1, "expected setCount setter cache entry to preserve one parameter")
    expect(
        facts.set_count_entry.get("resolved_owner_identity")
        == facts.count_property.get("setter_owner_identity"),
        "expected setCount setter cache entry to match reflected property owner",
    )


__all__ = ["assert_instance_allocation_layout_payload"]
