from objc3c_sema_extraction_behavior import (
    assert_cmake_registers_sema_target,
    assert_pipeline_uses_sema_contract_types,
    assert_sema_contract_exports_explicit_type_metadata_handoff_surface,
    assert_sema_module_exists_and_pipeline_uses_api,
)


def test_sema_module_exists_and_pipeline_uses_api() -> None:
    assert_sema_module_exists_and_pipeline_uses_api()


def test_pipeline_uses_sema_contract_types() -> None:
    assert_pipeline_uses_sema_contract_types()


def test_sema_contract_exports_explicit_type_metadata_handoff_surface() -> None:
    assert_sema_contract_exports_explicit_type_metadata_handoff_surface()


def test_cmake_registers_sema_target() -> None:
    assert_cmake_registers_sema_target()
