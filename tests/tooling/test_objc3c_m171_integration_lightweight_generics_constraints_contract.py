import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m171_integration_lightweight_generics_constraints_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M171 integration lightweight generics constraints contract",
        "check:objc3c:m171-lightweight-generics-constraints-contracts",
        "check:compiler-closeout:m171",
        "tests/tooling/test_objc3c_m171_frontend_lightweight_generics_parser_contract.py",
        "tests/tooling/test_objc3c_m171_sema_lightweight_generics_constraints_contract.py",
        "tests/tooling/test_objc3c_m171_lowering_lightweight_generics_constraints_contract.py",
        "tests/tooling/test_objc3c_m171_validation_lightweight_generics_constraints_contract.py",
        "tests/tooling/test_objc3c_m171_integration_lightweight_generics_constraints_contract.py",
    ):
        assert text in library_api_doc


def test_m171_integration_lightweight_generics_constraints_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m171-lightweight-generics-constraints-contracts" in scripts
    assert scripts["check:objc3c:m171-lightweight-generics-constraints-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m171_frontend_lightweight_generics_parser_contract.py "
        "tests/tooling/test_objc3c_m171_sema_lightweight_generics_constraints_contract.py "
        "tests/tooling/test_objc3c_m171_lowering_lightweight_generics_constraints_contract.py "
        "tests/tooling/test_objc3c_m171_validation_lightweight_generics_constraints_contract.py "
        "tests/tooling/test_objc3c_m171_integration_lightweight_generics_constraints_contract.py -q"
    )

    assert "check:compiler-closeout:m171" in scripts
    assert scripts["check:compiler-closeout:m171"] == (
        "npm run check:objc3c:m171-lightweight-generics-constraints-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m171" in scripts["check:task-hygiene"]

    assert "Enforce M171 lightweight-generics constraints packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m171" in workflow
    assert "Run M171 lightweight generics constraints integration gate" in workflow
    assert "npm run check:objc3c:m171-lightweight-generics-constraints-contracts" in workflow
