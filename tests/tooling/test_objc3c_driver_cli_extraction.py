from objc3c_driver_cli_behavior import (
    assert_cli_default_out_dir_is_tmp_governed,
    assert_cli_exposes_ir_object_backend_flag_and_enum,
    assert_cmake_registers_driver_target,
    assert_cmake_target_linkage_topology_is_split_by_stage,
    assert_driver_cli_module_exists_and_main_calls_it,
)


def test_driver_cli_module_exists_and_main_calls_it() -> None:
    assert_driver_cli_module_exists_and_main_calls_it()


def test_cmake_registers_driver_target() -> None:
    assert_cmake_registers_driver_target()


def test_cmake_target_linkage_topology_is_split_by_stage() -> None:
    assert_cmake_target_linkage_topology_is_split_by_stage()


def test_cli_exposes_ir_object_backend_flag_and_enum() -> None:
    assert_cli_exposes_ir_object_backend_flag_and_enum()


def test_cli_default_out_dir_is_tmp_governed() -> None:
    assert_cli_default_out_dir_is_tmp_governed()
