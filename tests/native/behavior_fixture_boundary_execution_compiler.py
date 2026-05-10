import subprocess
from pathlib import Path

from behavior_fixture_boundary_support import BehaviorFixture, NATIVE_EXE


def assert_native_compiler_available() -> None:
    assert NATIVE_EXE.exists(), "native compiler binary must exist before running behavior fixtures"


def compile_fixture(fixture: BehaviorFixture, out_dir: Path) -> tuple[int, str]:
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
