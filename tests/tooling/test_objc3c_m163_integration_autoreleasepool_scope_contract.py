import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m163_integration_autoreleasepool_scope_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M163 integration autoreleasepool scope/lifetime semantics contract",
        "check:objc3c:m163-autoreleasepool-scope-contracts",
        "check:compiler-closeout:m163",
        "tests/tooling/test_objc3c_m163_frontend_autorelease_pool_parser_contract.py",
        "tests/tooling/test_objc3c_m163_sema_autorelease_pool_scope_contract.py",
        "tests/tooling/test_objc3c_m163_lowering_autoreleasepool_scope_contract.py",
        "tests/tooling/test_objc3c_m163_validation_autoreleasepool_scope_contract.py",
        "tests/tooling/test_objc3c_m163_integration_autoreleasepool_scope_contract.py",
    ):
        assert text in library_api_doc


def test_m163_integration_autoreleasepool_scope_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m163-autoreleasepool-scope-contracts" in scripts
    assert scripts["check:objc3c:m163-autoreleasepool-scope-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m163_frontend_autorelease_pool_parser_contract.py "
        "tests/tooling/test_objc3c_m163_sema_autorelease_pool_scope_contract.py "
        "tests/tooling/test_objc3c_m163_lowering_autoreleasepool_scope_contract.py "
        "tests/tooling/test_objc3c_m163_validation_autoreleasepool_scope_contract.py "
        "tests/tooling/test_objc3c_m163_integration_autoreleasepool_scope_contract.py -q"
    )

    assert "check:compiler-closeout:m163" in scripts
    assert scripts["check:compiler-closeout:m163"] == (
        "npm run check:objc3c:m163-autoreleasepool-scope-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m163" in scripts["check:task-hygiene"]

    assert "Enforce M163 autoreleasepool scope/lifetime packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m163" in workflow
    assert "Run M163 autoreleasepool scope/lifetime integration gate" in workflow
    assert "npm run check:objc3c:m163-autoreleasepool-scope-contracts" in workflow
