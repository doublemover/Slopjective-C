import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m156_integration_message_send_selector_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M156 integration message-send selector-lowering contract",
        "check:objc3c:m156-message-send-selector-lowering-contracts",
        "check:compiler-closeout:m156",
        "tests/tooling/test_objc3c_m156_frontend_message_send_selector_lowering_contract.py",
        "tests/tooling/test_objc3c_m156_sema_message_send_selector_lowering_contract.py",
        "tests/tooling/test_objc3c_m156_lowering_message_send_selector_lowering_contract.py",
        "tests/tooling/test_objc3c_m156_validation_message_send_selector_lowering_contract.py",
        "tests/tooling/test_objc3c_m156_integration_message_send_selector_lowering_contract.py",
    ):
        assert text in library_api_doc


def test_m156_integration_message_send_selector_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m156-message-send-selector-lowering-contracts" in scripts
    assert scripts["check:objc3c:m156-message-send-selector-lowering-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m156_frontend_message_send_selector_lowering_contract.py "
        "tests/tooling/test_objc3c_m156_sema_message_send_selector_lowering_contract.py "
        "tests/tooling/test_objc3c_m156_lowering_message_send_selector_lowering_contract.py "
        "tests/tooling/test_objc3c_m156_validation_message_send_selector_lowering_contract.py "
        "tests/tooling/test_objc3c_m156_integration_message_send_selector_lowering_contract.py -q"
    )

    assert "check:compiler-closeout:m156" in scripts
    assert scripts["check:compiler-closeout:m156"] == (
        "npm run check:objc3c:m156-message-send-selector-lowering-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m156" in scripts["check:task-hygiene"]

    assert "Enforce M156 message-send selector-lowering packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m156" in workflow
    assert "Run M156 message-send selector-lowering integration gate" in workflow
    assert "npm run check:objc3c:m156-message-send-selector-lowering-contracts" in workflow
