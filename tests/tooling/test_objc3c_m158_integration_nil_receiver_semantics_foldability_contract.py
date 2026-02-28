import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m158_integration_nil_receiver_semantics_foldability_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M158 integration nil-receiver semantics/foldability contract",
        "check:objc3c:m158-nil-receiver-semantics-foldability-contracts",
        "check:compiler-closeout:m158",
        "tests/tooling/test_objc3c_m158_frontend_nil_receiver_semantics_foldability_contract.py",
        "tests/tooling/test_objc3c_m158_sema_nil_receiver_semantics_foldability_contract.py",
        "tests/tooling/test_objc3c_m158_lowering_nil_receiver_semantics_foldability_contract.py",
        "tests/tooling/test_objc3c_m158_validation_nil_receiver_semantics_foldability_contract.py",
        "tests/tooling/test_objc3c_m158_integration_nil_receiver_semantics_foldability_contract.py",
    ):
        assert text in library_api_doc


def test_m158_integration_nil_receiver_semantics_foldability_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m158-nil-receiver-semantics-foldability-contracts" in scripts
    assert scripts["check:objc3c:m158-nil-receiver-semantics-foldability-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m158_frontend_nil_receiver_semantics_foldability_contract.py "
        "tests/tooling/test_objc3c_m158_sema_nil_receiver_semantics_foldability_contract.py "
        "tests/tooling/test_objc3c_m158_lowering_nil_receiver_semantics_foldability_contract.py "
        "tests/tooling/test_objc3c_m158_validation_nil_receiver_semantics_foldability_contract.py "
        "tests/tooling/test_objc3c_m158_integration_nil_receiver_semantics_foldability_contract.py -q"
    )

    assert "check:compiler-closeout:m158" in scripts
    assert scripts["check:compiler-closeout:m158"] == (
        "npm run check:objc3c:m158-nil-receiver-semantics-foldability-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m158" in scripts["check:task-hygiene"]

    assert "Enforce M158 nil-receiver semantics/foldability packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m158" in workflow
    assert "Run M158 nil-receiver semantics/foldability integration gate" in workflow
    assert "npm run check:objc3c:m158-nil-receiver-semantics-foldability-contracts" in workflow
