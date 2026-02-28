from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m224_integration_release_readiness_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M224 integration/release-readiness (1.0 ABI/version gates)",
        "objc3c_frontend_is_abi_compatible(OBJC3C_FRONTEND_ABI_VERSION)",
        "objc3c_frontend_version().abi_version == objc3c_frontend_abi_version()",
        "OBJC3C_FRONTEND_MIN_COMPATIBILITY_ABI_VERSION",
        "OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION",
        "npm run check:objc3c:m224-integration-release-readiness",
    ):
        assert text in library_api_doc


def test_m224_integration_release_readiness_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)

    assert '"check:objc3c:m224-integration-release-readiness"' in package_json
    assert (
        "npm run test:objc3c:m222-compatibility-migration && npm run check:objc3c:library-cli-parity:golden "
        "&& python -m pytest "
        "tests/tooling/test_objc3c_m224_frontend_release_contract.py "
        "tests/tooling/test_objc3c_m224_sema_release_contract.py "
        "tests/tooling/test_objc3c_m224_lowering_release_contract.py "
        "tests/tooling/test_objc3c_m224_integration_release_contract.py -q"
    ) in package_json

    assert "Run M224 integration/release-readiness ABI+version gate" in workflow
    assert "npm run check:objc3c:m224-integration-release-readiness" in workflow
