import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m175_integration_generic_metadata_abi_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M175 integration generic metadata emission and ABI checks contract",
        "check:objc3c:m175-generic-metadata-abi-contracts",
        "check:compiler-closeout:m175",
        "tests/tooling/test_objc3c_m175_frontend_generic_metadata_abi_parser_contract.py",
        "tests/tooling/test_objc3c_m175_sema_generic_metadata_abi_contract.py",
        "tests/tooling/test_objc3c_m175_lowering_generic_metadata_abi_contract.py",
        "tests/tooling/test_objc3c_m175_validation_generic_metadata_abi_contract.py",
        "tests/tooling/test_objc3c_m175_integration_generic_metadata_abi_contract.py",
    ):
        assert text in library_api_doc


def test_m175_integration_generic_metadata_abi_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m175-generic-metadata-abi-contracts" in scripts
    assert scripts["check:objc3c:m175-generic-metadata-abi-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m175_frontend_generic_metadata_abi_parser_contract.py "
        "tests/tooling/test_objc3c_m175_sema_generic_metadata_abi_contract.py "
        "tests/tooling/test_objc3c_m175_lowering_generic_metadata_abi_contract.py "
        "tests/tooling/test_objc3c_m175_validation_generic_metadata_abi_contract.py "
        "tests/tooling/test_objc3c_m175_integration_generic_metadata_abi_contract.py -q"
    )

    assert "check:compiler-closeout:m175" in scripts
    assert scripts["check:compiler-closeout:m175"] == (
        "npm run check:objc3c:m175-generic-metadata-abi-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m175" in scripts["check:task-hygiene"]

    assert "Enforce M175 generic metadata ABI packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m175" in workflow
    assert "Run M175 generic metadata ABI integration gate" in workflow
    assert "npm run check:objc3c:m175-generic-metadata-abi-contracts" in workflow
