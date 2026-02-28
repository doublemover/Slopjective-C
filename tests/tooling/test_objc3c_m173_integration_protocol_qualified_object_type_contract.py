import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m173_integration_protocol_qualified_object_type_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M173 integration protocol-qualified object type contract",
        "check:objc3c:m173-protocol-qualified-object-type-contracts",
        "check:compiler-closeout:m173",
        "tests/tooling/test_objc3c_m173_frontend_protocol_qualified_object_type_parser_contract.py",
        "tests/tooling/test_objc3c_m173_sema_protocol_qualified_object_type_contract.py",
        "tests/tooling/test_objc3c_m173_lowering_protocol_qualified_object_type_contract.py",
        "tests/tooling/test_objc3c_m173_validation_protocol_qualified_object_type_contract.py",
        "tests/tooling/test_objc3c_m173_integration_protocol_qualified_object_type_contract.py",
    ):
        assert text in library_api_doc


def test_m173_integration_protocol_qualified_object_type_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m173-protocol-qualified-object-type-contracts" in scripts
    assert scripts["check:objc3c:m173-protocol-qualified-object-type-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m173_frontend_protocol_qualified_object_type_parser_contract.py "
        "tests/tooling/test_objc3c_m173_sema_protocol_qualified_object_type_contract.py "
        "tests/tooling/test_objc3c_m173_lowering_protocol_qualified_object_type_contract.py "
        "tests/tooling/test_objc3c_m173_validation_protocol_qualified_object_type_contract.py "
        "tests/tooling/test_objc3c_m173_integration_protocol_qualified_object_type_contract.py -q"
    )

    assert "check:compiler-closeout:m173" in scripts
    assert scripts["check:compiler-closeout:m173"] == (
        "npm run check:objc3c:m173-protocol-qualified-object-type-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m173" in scripts["check:task-hygiene"]

    assert "Enforce M173 protocol-qualified object-type packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m173" in workflow
    assert "Run M173 protocol-qualified object type integration gate" in workflow
    assert "npm run check:objc3c:m173-protocol-qualified-object-type-contracts" in workflow
