from pathlib import Path

from behavior_fixture_boundary_execution_compiler import (
    assert_native_compiler_available,
    compile_fixture,
)
from behavior_fixture_boundary_execution_strict_errors import (
    assert_strict_error_fixture_result,
)
from behavior_fixture_boundary_execution_success import (
    assert_successful_fixture_result,
)
from behavior_fixture_boundary_support import load_behavior_fixture_catalog


def test_behavior_fixture_slice_executes_compile_and_strict_error_contracts(tmp_path: Path) -> None:
    assert_native_compiler_available()

    driver_fixtures = load_behavior_fixture_catalog().compiler_driver_fixtures()
    assert driver_fixtures
    assert {fixture.owner_phase for fixture in driver_fixtures} >= {
        "parser",
        "sema",
        "lowering",
        "ir",
        "e2e",
    }

    for index, fixture in enumerate(driver_fixtures):
        return_code, output = compile_fixture(fixture, tmp_path / f"driver-{index}")
        if fixture.is_strict:
            assert_strict_error_fixture_result(fixture, return_code, output)
        else:
            assert_successful_fixture_result(return_code, output)
