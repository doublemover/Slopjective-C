import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m169_integration_block_copy_dispose_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M169 integration block copy-dispose helper contract",
        "check:objc3c:m169-block-copy-dispose-contracts",
        "check:compiler-closeout:m169",
        "tests/tooling/test_objc3c_m169_frontend_block_copy_dispose_helper_parser_contract.py",
        "tests/tooling/test_objc3c_m169_sema_block_copy_dispose_contract.py",
        "tests/tooling/test_objc3c_m169_lowering_block_copy_dispose_contract.py",
        "tests/tooling/test_objc3c_m169_validation_block_copy_dispose_contract.py",
        "tests/tooling/test_objc3c_m169_integration_block_copy_dispose_contract.py",
    ):
        assert text in library_api_doc


def test_m169_integration_block_copy_dispose_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m169-block-copy-dispose-contracts" in scripts
    assert scripts["check:objc3c:m169-block-copy-dispose-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m169_frontend_block_copy_dispose_helper_parser_contract.py "
        "tests/tooling/test_objc3c_m169_sema_block_copy_dispose_contract.py "
        "tests/tooling/test_objc3c_m169_lowering_block_copy_dispose_contract.py "
        "tests/tooling/test_objc3c_m169_validation_block_copy_dispose_contract.py "
        "tests/tooling/test_objc3c_m169_integration_block_copy_dispose_contract.py -q"
    )

    assert "check:compiler-closeout:m169" in scripts
    assert scripts["check:compiler-closeout:m169"] == (
        "npm run check:objc3c:m169-block-copy-dispose-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m169" in scripts["check:task-hygiene"]

    assert "Enforce M169 block-copy-dispose packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m169" in workflow
    assert "Run M169 block copy-dispose integration gate" in workflow
    assert "npm run check:objc3c:m169-block-copy-dispose-contracts" in workflow
