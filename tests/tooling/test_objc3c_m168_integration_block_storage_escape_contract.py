import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m168_integration_block_storage_escape_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M168 integration block storage escape contract",
        "check:objc3c:m168-block-storage-escape-contracts",
        "check:compiler-closeout:m168",
        "tests/tooling/test_objc3c_m168_frontend_block_storage_escape_parser_contract.py",
        "tests/tooling/test_objc3c_m168_sema_block_storage_escape_contract.py",
        "tests/tooling/test_objc3c_m168_lowering_block_storage_escape_contract.py",
        "tests/tooling/test_objc3c_m168_validation_block_storage_escape_contract.py",
        "tests/tooling/test_objc3c_m168_integration_block_storage_escape_contract.py",
    ):
        assert text in library_api_doc


def test_m168_integration_block_storage_escape_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m168-block-storage-escape-contracts" in scripts
    assert scripts["check:objc3c:m168-block-storage-escape-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m168_frontend_block_storage_escape_parser_contract.py "
        "tests/tooling/test_objc3c_m168_sema_block_storage_escape_contract.py "
        "tests/tooling/test_objc3c_m168_lowering_block_storage_escape_contract.py "
        "tests/tooling/test_objc3c_m168_validation_block_storage_escape_contract.py "
        "tests/tooling/test_objc3c_m168_integration_block_storage_escape_contract.py -q"
    )

    assert "check:compiler-closeout:m168" in scripts
    assert scripts["check:compiler-closeout:m168"] == (
        "npm run check:objc3c:m168-block-storage-escape-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m168" in scripts["check:task-hygiene"]

    assert "Enforce M168 block-storage-escape packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m168" in workflow
    assert "Run M168 block storage escape integration gate" in workflow
    assert "npm run check:objc3c:m168-block-storage-escape-contracts" in workflow
