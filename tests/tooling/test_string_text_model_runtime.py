from pathlib import Path
import os
import subprocess

ROOT = Path(__file__).resolve().parents[2]
RUNTIME_SOURCE = ROOT / "native/objc3c/src/runtime"


def test_string_text_model_runtime_owns_and_validates_utf8_storage(tmp_path: Path) -> None:
    build = tmp_path / "build"
    compiler = os.environ.get("CXX", "clang++")
    commands = (
        [
            "cmake", "-S", str(RUNTIME_SOURCE), "-B", str(build), "-G", "Ninja",
            f"-DCMAKE_CXX_COMPILER={compiler}", "-DBUILD_TESTING=ON",
        ],
        ["cmake", "--build", str(build), "--target", "objc3_text_scalar_boundary_test"],
        [
            "ctest", "--test-dir", str(build), "-R", "^objc3_text_scalar_boundary$",
            "--no-tests=error", "--output-on-failure",
        ],
    )
    for command in commands:
        completed = subprocess.run(command, capture_output=True, text=True, check=False)
        assert completed.returncode == 0, completed.stdout + completed.stderr
