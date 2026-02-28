import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m164_integration_weak_unowned_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M164 integration weak/unowned semantics contract",
        "check:objc3c:m164-weak-unowned-semantics-contracts",
        "check:compiler-closeout:m164",
        "tests/tooling/test_objc3c_m164_frontend_weak_unowned_parser_contract.py",
        "tests/tooling/test_objc3c_m164_sema_weak_unowned_contract.py",
        "tests/tooling/test_objc3c_m164_lowering_weak_unowned_contract.py",
        "tests/tooling/test_objc3c_m164_validation_weak_unowned_semantics_contract.py",
        "tests/tooling/test_objc3c_m164_integration_weak_unowned_semantics_contract.py",
    ):
        assert text in library_api_doc


def test_m164_integration_weak_unowned_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m164-weak-unowned-semantics-contracts" in scripts
    assert scripts["check:objc3c:m164-weak-unowned-semantics-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m164_frontend_weak_unowned_parser_contract.py "
        "tests/tooling/test_objc3c_m164_sema_weak_unowned_contract.py "
        "tests/tooling/test_objc3c_m164_lowering_weak_unowned_contract.py "
        "tests/tooling/test_objc3c_m164_validation_weak_unowned_semantics_contract.py "
        "tests/tooling/test_objc3c_m164_integration_weak_unowned_semantics_contract.py -q"
    )

    assert "check:compiler-closeout:m164" in scripts
    assert scripts["check:compiler-closeout:m164"] == (
        "npm run check:objc3c:m164-weak-unowned-semantics-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m164" in scripts["check:task-hygiene"]

    assert "Enforce M164 weak/unowned semantics packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m164" in workflow
    assert "Run M164 weak/unowned semantics integration gate" in workflow
    assert "npm run check:objc3c:m164-weak-unowned-semantics-contracts" in workflow
