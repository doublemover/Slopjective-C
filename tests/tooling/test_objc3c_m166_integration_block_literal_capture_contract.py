import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m166_integration_block_literal_capture_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M166 integration block literal capture contract",
        "check:objc3c:m166-block-literal-capture-contracts",
        "check:compiler-closeout:m166",
        "tests/tooling/test_objc3c_m166_frontend_block_literal_capture_parser_contract.py",
        "tests/tooling/test_objc3c_m166_sema_block_literal_capture_contract.py",
        "tests/tooling/test_objc3c_m166_lowering_block_literal_capture_contract.py",
        "tests/tooling/test_objc3c_m166_validation_block_literal_capture_contract.py",
        "tests/tooling/test_objc3c_m166_integration_block_literal_capture_contract.py",
    ):
        assert text in library_api_doc


def test_m166_integration_block_literal_capture_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m166-block-literal-capture-contracts" in scripts
    assert scripts["check:objc3c:m166-block-literal-capture-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m166_frontend_block_literal_capture_parser_contract.py "
        "tests/tooling/test_objc3c_m166_sema_block_literal_capture_contract.py "
        "tests/tooling/test_objc3c_m166_lowering_block_literal_capture_contract.py "
        "tests/tooling/test_objc3c_m166_validation_block_literal_capture_contract.py "
        "tests/tooling/test_objc3c_m166_integration_block_literal_capture_contract.py -q"
    )

    assert "check:compiler-closeout:m166" in scripts
    assert scripts["check:compiler-closeout:m166"] == (
        "npm run check:objc3c:m166-block-literal-capture-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m166" in scripts["check:task-hygiene"]

    assert "Enforce M166 block-literal-capture packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m166" in workflow
    assert "Run M166 block literal capture integration gate" in workflow
    assert "npm run check:objc3c:m166-block-literal-capture-contracts" in workflow
