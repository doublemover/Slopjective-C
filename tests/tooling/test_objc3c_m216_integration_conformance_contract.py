from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m216_integration_conformance_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M216 integration conformance suite v1",
        "npm run check:objc3c:m216-conformance-suite-v1",
        "check:objc3c:m217-differential-parity",
        "objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)",
        "objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()",
        "OBJC3C_FRONTEND_VERSION_STRING",
        "OBJC3C_FRONTEND_ABI_VERSION",
        "tests/tooling/test_objc3c_m216_frontend_conformance_contract.py",
        "tests/tooling/test_objc3c_m216_sema_conformance_contract.py",
        "tests/tooling/test_objc3c_m216_lowering_conformance_contract.py",
        "tests/tooling/test_objc3c_m216_validation_conformance_contract.py",
        "tests/tooling/test_objc3c_m216_integration_conformance_contract.py",
    ):
        assert text in library_api_doc


def test_m216_integration_conformance_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)

    assert '"check:objc3c:m216-conformance-suite-v1"' in package_json
    assert (
        "npm run check:objc3c:m217-differential-parity && python -m pytest "
        "tests/tooling/test_objc3c_m216_frontend_conformance_contract.py "
        "tests/tooling/test_objc3c_m216_sema_conformance_contract.py "
        "tests/tooling/test_objc3c_m216_lowering_conformance_contract.py "
        "tests/tooling/test_objc3c_m216_validation_conformance_contract.py "
        "tests/tooling/test_objc3c_m216_integration_conformance_contract.py -q"
    ) in package_json

    assert "Run M216 conformance suite v1 integration gate" in workflow
    assert "npm run check:objc3c:m216-conformance-suite-v1" in workflow
