import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m167_integration_block_abi_invoke_trampoline_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M167 integration block ABI invoke-trampoline contract",
        "check:objc3c:m167-block-abi-invoke-trampoline-contracts",
        "check:compiler-closeout:m167",
        "tests/tooling/test_objc3c_m167_frontend_block_abi_invoke_trampoline_parser_contract.py",
        "tests/tooling/test_objc3c_m167_sema_block_abi_invoke_trampoline_contract.py",
        "tests/tooling/test_objc3c_m167_lowering_block_abi_invoke_trampoline_contract.py",
        "tests/tooling/test_objc3c_m167_validation_block_abi_invoke_trampoline_contract.py",
        "tests/tooling/test_objc3c_m167_integration_block_abi_invoke_trampoline_contract.py",
    ):
        assert text in library_api_doc


def test_m167_integration_block_abi_invoke_trampoline_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m167-block-abi-invoke-trampoline-contracts" in scripts
    assert scripts["check:objc3c:m167-block-abi-invoke-trampoline-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m167_frontend_block_abi_invoke_trampoline_parser_contract.py "
        "tests/tooling/test_objc3c_m167_sema_block_abi_invoke_trampoline_contract.py "
        "tests/tooling/test_objc3c_m167_lowering_block_abi_invoke_trampoline_contract.py "
        "tests/tooling/test_objc3c_m167_validation_block_abi_invoke_trampoline_contract.py "
        "tests/tooling/test_objc3c_m167_integration_block_abi_invoke_trampoline_contract.py -q"
    )

    assert "check:compiler-closeout:m167" in scripts
    assert scripts["check:compiler-closeout:m167"] == (
        "npm run check:objc3c:m167-block-abi-invoke-trampoline-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m167" in scripts["check:task-hygiene"]

    assert "Enforce M167 block-ABI invoke-trampoline packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m167" in workflow
    assert "Run M167 block ABI invoke-trampoline integration gate" in workflow
    assert "npm run check:objc3c:m167-block-abi-invoke-trampoline-contracts" in workflow
