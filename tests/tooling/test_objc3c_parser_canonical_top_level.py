import json
import subprocess
from pathlib import Path


REPO_ROOT = Path(__file__).resolve().parents[2]
NATIVE_EXE = REPO_ROOT / "artifacts" / "bin" / "objc3c-native.exe"
CONFORMANCE_FIXTURE = REPO_ROOT / "tests" / "conformance" / "parser" / "TUV-01.json"


def test_canonical_top_level_function_fixture_compiles_without_unsupported_statement(
    tmp_path: Path,
) -> None:
    assert NATIVE_EXE.exists(), "native compiler binary must exist before running parser canonical test"

    fixture_payload = json.loads(CONFORMANCE_FIXTURE.read_text(encoding="utf-8"))
    source_text = fixture_payload["source"]
    assert isinstance(source_text, str)

    source_path = tmp_path / "tuv_01.objc3"
    out_dir = tmp_path / "out"
    source_path.write_text(source_text, encoding="utf-8")
    out_dir.mkdir(parents=True, exist_ok=True)

    completed = subprocess.run(
        [
            str(NATIVE_EXE),
            str(source_path),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ],
        capture_output=True,
        text=True,
        check=False,
    )

    diagnostics_path = out_dir / "module.diagnostics.txt"
    diagnostics_text = diagnostics_path.read_text(encoding="utf-8") if diagnostics_path.exists() else ""

    assert completed.returncode == 0, (
        "expected canonical parser path to accept a basic Objective-C 3 top-level function fixture; "
        f"stdout={completed.stdout!r} stderr={completed.stderr!r} diagnostics={diagnostics_text!r}"
    )
    assert "unsupported Objective-C 3 statement" not in diagnostics_text
    assert (out_dir / "module.ll").exists()


def test_removed_optional_template_alias_is_a_canonical_diagnostic(tmp_path: Path) -> None:
    assert NATIVE_EXE.exists(), "native compiler binary must exist before running parser canonical test"

    source_path = tmp_path / "negative_optional_template_alias.objc3"
    out_dir = tmp_path / "out"
    source_path.write_text(
        "module NegativeOptionalTemplateAlias;\n\n"
        "fn main(value: optional<int>) -> i32 {\n"
        "  return value;\n"
        "}\n",
        encoding="utf-8",
    )
    out_dir.mkdir(parents=True, exist_ok=True)

    completed = subprocess.run(
        [
            str(NATIVE_EXE),
            str(source_path),
            "--out-dir",
            str(out_dir),
            "--emit-prefix",
            "module",
        ],
        capture_output=True,
        text=True,
        check=False,
    )

    diagnostics_path = out_dir / "module.diagnostics.txt"
    diagnostics_text = diagnostics_path.read_text(encoding="utf-8") if diagnostics_path.exists() else ""

    assert completed.returncode != 0
    assert "O3C004" in diagnostics_text
    assert "optional<T> aliases are rejected; use canonical Optional<T> spelling" in diagnostics_text
