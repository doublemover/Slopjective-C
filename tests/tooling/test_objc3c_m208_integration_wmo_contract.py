from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m208_integration_wmo_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M208 integration whole-module optimization controls",
        "npm run check:objc3c:m208-whole-module-optimization",
        "check:objc3c:m209-pgo-hooks",
        "objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)",
        "objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()",
        "OBJC3C_FRONTEND_VERSION_STRING",
        "OBJC3C_FRONTEND_ABI_VERSION",
        "tests/tooling/test_objc3c_m208_frontend_wmo_contract.py",
        "tests/tooling/test_objc3c_m208_sema_wmo_contract.py",
        "tests/tooling/test_objc3c_m208_lowering_wmo_contract.py",
        "tests/tooling/test_objc3c_m208_validation_wmo_contract.py",
        "tests/tooling/test_objc3c_m208_integration_wmo_contract.py",
    ):
        assert text in library_api_doc


def test_m208_integration_wmo_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)

    assert '"check:objc3c:m208-whole-module-optimization"' in package_json
    assert (
        "npm run check:objc3c:m209-pgo-hooks && python -m pytest "
        "tests/tooling/test_objc3c_m208_frontend_wmo_contract.py "
        "tests/tooling/test_objc3c_m208_sema_wmo_contract.py "
        "tests/tooling/test_objc3c_m208_lowering_wmo_contract.py "
        "tests/tooling/test_objc3c_m208_validation_wmo_contract.py "
        "tests/tooling/test_objc3c_m208_integration_wmo_contract.py -q"
    ) in package_json

    assert "Run M208 whole-module optimization integration gate" in workflow
    assert "npm run check:objc3c:m208-whole-module-optimization" in workflow
