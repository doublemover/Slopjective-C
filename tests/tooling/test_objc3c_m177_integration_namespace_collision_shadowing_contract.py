import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m177_integration_namespace_collision_shadowing_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M177 integration namespace collision and shadowing diagnostics contract",
        "check:objc3c:m177-namespace-collision-shadowing-contracts",
        "check:compiler-closeout:m177",
        "tests/tooling/test_objc3c_m177_frontend_namespace_collision_shadowing_parser_contract.py",
        "tests/tooling/test_objc3c_m177_sema_namespace_collision_shadowing_contract.py",
        "tests/tooling/test_objc3c_m177_lowering_namespace_collision_shadowing_contract.py",
        "tests/tooling/test_objc3c_m177_validation_namespace_collision_shadowing_contract.py",
        "tests/tooling/test_objc3c_m177_integration_namespace_collision_shadowing_contract.py",
    ):
        assert text in library_api_doc


def test_m177_integration_namespace_collision_shadowing_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m177-namespace-collision-shadowing-contracts" in scripts
    assert scripts["check:objc3c:m177-namespace-collision-shadowing-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m177_frontend_namespace_collision_shadowing_parser_contract.py "
        "tests/tooling/test_objc3c_m177_sema_namespace_collision_shadowing_contract.py "
        "tests/tooling/test_objc3c_m177_lowering_namespace_collision_shadowing_contract.py "
        "tests/tooling/test_objc3c_m177_validation_namespace_collision_shadowing_contract.py "
        "tests/tooling/test_objc3c_m177_integration_namespace_collision_shadowing_contract.py -q"
    )

    assert "check:compiler-closeout:m177" in scripts
    assert scripts["check:compiler-closeout:m177"] == (
        "npm run check:objc3c:m177-namespace-collision-shadowing-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m177" in scripts["check:task-hygiene"]

    assert "Enforce M177 namespace collision/shadowing packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m177" in workflow
    assert "Run M177 namespace collision/shadowing integration gate" in workflow
    assert "npm run check:objc3c:m177-namespace-collision-shadowing-contracts" in workflow
