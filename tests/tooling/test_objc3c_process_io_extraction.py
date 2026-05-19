from objc3c_process_io_behavior import (
    assert_cmake_registers_io_target,
    assert_cmake_registers_runtime_abi_target,
    assert_io_process_exposes_llc_direct_object_emission_path,
    assert_io_process_module_exists_and_main_uses_it,
    assert_runtime_registration_symbol_json_uses_owner_record_contract,
)


def test_io_process_module_exists_and_main_uses_it() -> None:
    assert_io_process_module_exists_and_main_uses_it()


def test_cmake_registers_io_target() -> None:
    assert_cmake_registers_io_target()


def test_cmake_registers_runtime_abi_target() -> None:
    assert_cmake_registers_runtime_abi_target()


def test_io_process_exposes_llc_direct_object_emission_path() -> None:
    assert_io_process_exposes_llc_direct_object_emission_path()


def test_runtime_registration_symbol_json_uses_owner_record_contract() -> None:
    assert_runtime_registration_symbol_json_uses_owner_record_contract()
