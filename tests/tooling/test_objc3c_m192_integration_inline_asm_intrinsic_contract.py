import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m192_integration_inline_asm_intrinsic_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M192 integration inline-asm and intrinsic governance gating contract",
        "check:objc3c:m192-inline-asm-intrinsic-contracts",
        "check:compiler-closeout:m192",
        "tests/tooling/test_objc3c_m192_integration_inline_asm_intrinsic_contract.py",
    ):
        assert text in library_api_doc


def test_m192_integration_inline_asm_intrinsic_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m192-inline-asm-intrinsic-contracts" in scripts
    assert scripts["check:objc3c:m192-inline-asm-intrinsic-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m192_frontend_inline_asm_intrinsic_parser_contract.py "
        "tests/tooling/test_objc3c_m192_sema_inline_asm_intrinsic_contract.py "
        "tests/tooling/test_objc3c_m192_lowering_inline_asm_intrinsic_contract.py "
        "tests/tooling/test_objc3c_m192_validation_inline_asm_intrinsic_contract.py "
        "tests/tooling/test_objc3c_m192_conformance_inline_asm_intrinsic_contract.py "
        "tests/tooling/test_objc3c_m192_integration_inline_asm_intrinsic_contract.py -q"
    )

    assert "check:compiler-closeout:m192" in scripts
    assert scripts["check:compiler-closeout:m192"] == (
        "npm run check:objc3c:m192-inline-asm-intrinsic-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m192" in scripts["check:task-hygiene"]
