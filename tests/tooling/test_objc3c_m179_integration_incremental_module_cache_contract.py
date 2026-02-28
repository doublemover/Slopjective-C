import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m179_integration_incremental_module_cache_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M179 integration incremental module cache and invalidation contract",
        "check:objc3c:m179-incremental-module-cache-contracts",
        "check:compiler-closeout:m179",
        "tests/tooling/test_objc3c_m179_frontend_incremental_module_cache_parser_contract.py",
        "tests/tooling/test_objc3c_m179_sema_incremental_module_cache_contract.py",
        "tests/tooling/test_objc3c_m179_integration_incremental_module_cache_contract.py",
        "M179-C001 and M179-D001 outputs are not yet landed in this workspace.",
        "while remaining forward-compatible for future M179-C001/M179-D001 additions.",
    ):
        assert text in library_api_doc


def test_m179_integration_incremental_module_cache_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m179-incremental-module-cache-contracts" in scripts
    assert scripts["check:objc3c:m179-incremental-module-cache-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m179_frontend_incremental_module_cache_parser_contract.py "
        "tests/tooling/test_objc3c_m179_sema_incremental_module_cache_contract.py "
        "tests/tooling/test_objc3c_m179_integration_incremental_module_cache_contract.py -q"
    )

    assert "check:compiler-closeout:m179" in scripts
    assert scripts["check:compiler-closeout:m179"] == (
        "npm run check:objc3c:m179-incremental-module-cache-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m179" in scripts["check:task-hygiene"]

    assert "Enforce M179 incremental module cache/invalidation packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m179" in workflow
    assert "Run M179 incremental module cache/invalidation integration gate" in workflow
    assert "npm run check:objc3c:m179-incremental-module-cache-contracts" in workflow
