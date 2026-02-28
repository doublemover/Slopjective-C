from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m142_integration_cli_c_api_parity_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M142 integration CLI and C API parity harness",
        "npm run check:objc3c:m142-cli-c-api-parity",
        "check:compiler-closeout:m142",
        "tests/tooling/test_objc3c_m142_frontend_cli_c_api_parity_contract.py",
        "tests/tooling/test_objc3c_m142_sema_cli_c_api_parity_contract.py",
        "tests/tooling/test_objc3c_m142_lowering_cli_c_api_parity_contract.py",
        "tests/tooling/test_objc3c_m142_validation_cli_c_api_parity_contract.py",
        "tests/tooling/test_objc3c_m142_integration_cli_c_api_parity_contract.py",
        "objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)",
        "objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()",
        "OBJC3C_FRONTEND_VERSION_STRING",
        "OBJC3C_FRONTEND_ABI_VERSION",
    ):
        assert text in library_api_doc


def test_m142_integration_cli_c_api_parity_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)

    assert '"check:objc3c:m142-cli-c-api-parity"' in package_json
    assert (
        "npm run check:compiler-closeout:m142 && python -m pytest "
        "tests/tooling/test_objc3c_m142_frontend_cli_c_api_parity_contract.py "
        "tests/tooling/test_objc3c_m142_sema_cli_c_api_parity_contract.py "
        "tests/tooling/test_objc3c_m142_lowering_cli_c_api_parity_contract.py "
        "tests/tooling/test_objc3c_m142_validation_cli_c_api_parity_contract.py "
        "tests/tooling/test_objc3c_m142_integration_cli_c_api_parity_contract.py -q"
    ) in package_json

    assert "Run M142 CLI and C API parity gate" in workflow
    assert "npm run check:objc3c:m142-cli-c-api-parity" in workflow
