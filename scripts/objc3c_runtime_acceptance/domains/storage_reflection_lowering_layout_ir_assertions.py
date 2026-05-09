"""LLVM IR assertions for storage/reflection lowering layout cases."""

from __future__ import annotations

from objc3c_runtime_acceptance.expectation_matching import expect

_PROPERTY_ACCESSOR_LAYOUT_IR_SNIPPETS = (
    (
        "; executable_property_accessor_layout_lowering = "
        "contract=objc3c.executable.property.accessor.layout.lowering.v1",
        "the executable property accessor/layout lowering summary",
    ),
    (
        "property_metadata_entries=6;ivar_metadata_entries=3;"
        "property_attribute_profiles=6;accessor_ownership_profiles=6;"
        "synthesized_binding_entries=6;ivar_layout_entries=3",
        "the accessor/layout lowering inventory counts",
    ),
    (
        "; executable_ivar_layout_emission = "
        "contract=objc3c.executable.ivar.layout.emission.v1",
        "the executable ivar layout emission summary",
    ),
    (
        "offset_global_entries=3;layout_table_entries=1;layout_owner_entries=1",
        "the ivar layout emission inventory counts",
    ),
    (
        "; executable_synthesized_accessor_property_lowering = "
        "contract=objc3c.executable.synthesized.accessor.property.lowering.v1",
        "the synthesized accessor lowering summary",
    ),
    (
        "synthesized_accessor_entries=6",
        "the synthesized accessor entry count",
    ),
    (
        "define i32 @objc3_method_Widget_instance_count() {",
        "the synthesized count getter body",
    ),
    (
        "define void @objc3_method_Widget_instance_setCount_(i32 %arg0) {",
        "the synthesized count setter body",
    ),
    (
        "@__objc3_meta_ivar_layout_table_0000 = private global",
        "the emitted ivar layout table",
    ),
    (
        "@__objc3_meta_ivar_offset_0000",
        "the emitted ivar offset globals",
    ),
)

_SYNTHESIZED_ACCESSOR_CODEGEN_IR_SNIPPETS = {
    "count getter": "define i32 @objc3_method_Widget_instance_count()",
    "count setter": "define void @objc3_method_Widget_instance_setCount_(i32 %arg0)",
    "enabled getter": "define i1 @objc3_method_Widget_instance_enabled()",
    "enabled setter": "define void @objc3_method_Widget_instance_setEnabled_(i1 %arg0)",
    "value getter": "define i32 @objc3_method_Widget_instance_value()",
    "value setter": "define void @objc3_method_Widget_instance_setValue_(i32 %arg0)",
    "getter runtime read": "call i32 @objc3_runtime_read_current_property_i32()",
    "setter runtime write": "call void @objc3_runtime_write_current_property_i32(i32 %arg0)",
    "bool setter coercion": "%objc3_property_value = zext i1 %arg0 to i32",
    "strong getter retain": (
        "%objc3_property_retained = call i32 @objc3_runtime_retain_i32(i32 %objc3_property_slot)"
    ),
    "strong getter autorelease": (
        "%objc3_property_autoreleased = call i32 @objc3_runtime_autorelease_i32(i32 %objc3_property_retained)"
    ),
    "strong setter exchange": (
        "%objc3_property_previous = call i32 @objc3_runtime_exchange_current_property_i32(i32 %objc3_property_retained)"
    ),
    "strong setter release": (
        "%objc3_property_release = call i32 @objc3_runtime_release_i32(i32 %objc3_property_previous)"
    ),
    "count descriptor getter binding": (
        "ptr @objc3_method_Widget_instance_count, ptr @objc3_method_Widget_instance_setCount_"
    ),
    "enabled descriptor getter binding": (
        "ptr @objc3_method_Widget_instance_enabled, ptr @objc3_method_Widget_instance_setEnabled_"
    ),
    "value descriptor getter binding": (
        "ptr @objc3_method_Widget_instance_value, ptr @objc3_method_Widget_instance_setValue_"
    ),
}


def assert_property_accessor_layout_ir(ll_text: str) -> None:
    for snippet, label in _PROPERTY_ACCESSOR_LAYOUT_IR_SNIPPETS:
        expect(
            snippet in ll_text,
            f"expected synthesized accessor/layout lowering fixture LLVM IR to publish {label}",
        )


def assert_synthesized_accessor_codegen_ir_snippets(ll_text: str) -> None:
    for label, snippet in _SYNTHESIZED_ACCESSOR_CODEGEN_IR_SNIPPETS.items():
        expect(snippet in ll_text, f"expected synthesized accessor codegen to emit {label}")


def assert_synthesized_accessor_codegen_surface_banners(ll_text: str) -> None:
    expect(
        "; dispatch_and_synthesized_accessor_lowering_surface = "
        "contract_id=objc3c.lowering.dispatch_and_synthesized_accessor_surface.v1"
        in ll_text,
        "expected LLVM IR banner to publish dispatch and synthesized-accessor lowering surface",
    )
    expect(
        "property_synthesis_sites=3" in ll_text,
        "expected LLVM IR banner to report three synthesized properties",
    )
    expect(
        "property_descriptor_count=6" in ll_text,
        "expected LLVM IR banner to report synthesized property descriptor count",
    )
    expect(
        "member_table_emission_ready=true" in ll_text,
        "expected LLVM IR banner to report member table emission readiness",
    )
    expect(
        "; synthesized_getter_setter_llvm_ir_generation_surface = "
        "contract_id=objc3c.synthesized.getter.setter.llvm.ir.generation.v1"
        in ll_text,
        "expected LLVM IR to publish synthesized getter/setter generation surface",
    )
    expect(
        "getter_definitions=3" in ll_text,
        "expected synthesized accessor fixture to emit three getter definitions",
    )
    expect(
        "setter_definitions=3" in ll_text,
        "expected synthesized accessor fixture to emit three setter definitions",
    )
    expect(
        "read_current_property_calls=3" in ll_text,
        "expected synthesized accessor fixture to emit three current-property reads",
    )
    expect(
        "write_current_property_calls=2" in ll_text,
        "expected synthesized accessor fixture to emit two current-property writes",
    )
    expect(
        "exchange_current_property_calls=1" in ll_text,
        "expected synthesized accessor fixture to emit one strong current-property exchange",
    )
    expect(
        "retain_calls=2" in ll_text,
        "expected synthesized accessor fixture to emit two retain helper calls",
    )
    expect(
        "release_calls=1" in ll_text,
        "expected synthesized accessor fixture to emit one release helper call",
    )
    expect(
        "autorelease_calls=1" in ll_text,
        "expected synthesized accessor fixture to emit one autorelease helper call",
    )


__all__ = [
    "assert_property_accessor_layout_ir",
    "assert_synthesized_accessor_codegen_ir_snippets",
    "assert_synthesized_accessor_codegen_surface_banners",
]
