import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m161_integration_ownership_qualifier_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M161 integration ownership-qualifier semantics contract",
        "check:objc3c:m161-ownership-qualifier-contracts",
        "check:compiler-closeout:m161",
        "tests/tooling/test_objc3c_m161_frontend_ownership_qualifier_parser_contract.py",
        "tests/tooling/test_objc3c_m161_sema_ownership_qualifier_contract.py",
        "tests/tooling/test_objc3c_m161_lowering_ownership_qualifier_contract.py",
        "tests/tooling/test_objc3c_m161_validation_ownership_qualifier_contract.py",
        "tests/tooling/test_objc3c_m161_integration_ownership_qualifier_contract.py",
    ):
        assert text in library_api_doc


def test_m161_integration_ownership_qualifier_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m161-ownership-qualifier-contracts" in scripts
    assert scripts["check:objc3c:m161-ownership-qualifier-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m161_frontend_ownership_qualifier_parser_contract.py "
        "tests/tooling/test_objc3c_m161_sema_ownership_qualifier_contract.py "
        "tests/tooling/test_objc3c_m161_lowering_ownership_qualifier_contract.py "
        "tests/tooling/test_objc3c_m161_validation_ownership_qualifier_contract.py "
        "tests/tooling/test_objc3c_m161_integration_ownership_qualifier_contract.py -q"
    )

    assert "check:compiler-closeout:m161" in scripts
    assert scripts["check:compiler-closeout:m161"] == (
        "npm run check:objc3c:m161-ownership-qualifier-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m161" in scripts["check:task-hygiene"]

    assert "Enforce M161 ownership-qualifier packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m161" in workflow
    assert "Run M161 ownership-qualifier integration gate" in workflow
    assert "npm run check:objc3c:m161-ownership-qualifier-contracts" in workflow
