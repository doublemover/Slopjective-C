from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m148_integration_selector_normalization_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M148 integration selector-normalized method declaration grammar",
        "check:objc3c:m148-selector-normalization",
        "tests/tooling/test_objc3c_m148_frontend_selector_normalization_contract.py",
        "tests/tooling/test_objc3c_m148_sema_selector_normalization_contract.py",
        "tests/tooling/test_objc3c_m148_lowering_selector_normalization_contract.py",
        "tests/tooling/test_objc3c_m148_validation_selector_normalization_contract.py",
        "tests/tooling/test_objc3c_m148_integration_selector_normalization_contract.py",
    ):
        assert text in library_api_doc


def test_m148_integration_selector_normalization_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)

    assert '"check:objc3c:m148-selector-normalization"' in package_json
    assert (
        "python -m pytest tests/tooling/test_objc3c_m148_frontend_selector_normalization_contract.py "
        "tests/tooling/test_objc3c_m148_sema_selector_normalization_contract.py "
        "tests/tooling/test_objc3c_m148_lowering_selector_normalization_contract.py "
        "tests/tooling/test_objc3c_m148_validation_selector_normalization_contract.py "
        "tests/tooling/test_objc3c_m148_integration_selector_normalization_contract.py -q"
    ) in package_json

    assert "Run M148 selector-normalized method declaration grammar gate" in workflow
    assert "npm run check:objc3c:m148-selector-normalization" in workflow
