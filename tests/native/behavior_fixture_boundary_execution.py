import subprocess
from pathlib import Path

from behavior_fixture_boundary_support import (
    BehaviorFixture,
    NATIVE_EXE,
    load_behavior_fixture_catalog,
)


def _compile_fixture(fixture: BehaviorFixture, out_dir: Path) -> tuple[int, str]:
    completed = subprocess.run(
        [
            str(NATIVE_EXE),
            str(fixture.source_path),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
            *fixture.native_compile_args,
        ],
        capture_output=True,
        text=True,
        check=False,
    )
    diagnostics_path = out_dir / "module.diagnostics.txt"
    diagnostics = diagnostics_path.read_text(encoding="utf-8") if diagnostics_path.exists() else ""
    return completed.returncode, diagnostics + completed.stdout + completed.stderr


def test_behavior_fixture_slice_executes_compile_and_strict_error_contracts(tmp_path: Path) -> None:
    assert NATIVE_EXE.exists(), "native compiler binary must exist before running behavior fixtures"

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
        return_code, output = _compile_fixture(fixture, tmp_path / f"driver-{index}")
        if fixture.is_strict:
            assert return_code != 0
            assert fixture.expected_diagnostic_code in output
            for token in fixture.required_tokens:
                assert token in output
        else:
            assert return_code == 0, output
