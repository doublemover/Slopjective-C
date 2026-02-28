import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m172_integration_nullability_flow_warning_precision_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M172 integration nullability flow warning precision contract",
        "check:objc3c:m172-nullability-flow-warning-precision-contracts",
        "check:compiler-closeout:m172",
        "tests/tooling/test_objc3c_m172_frontend_nullability_flow_parser_contract.py",
        "tests/tooling/test_objc3c_m172_sema_nullability_flow_warning_precision_contract.py",
        "tests/tooling/test_objc3c_m172_lowering_nullability_flow_warning_precision_contract.py",
        "tests/tooling/test_objc3c_m172_validation_nullability_flow_warning_precision_contract.py",
        "tests/tooling/test_objc3c_m172_integration_nullability_flow_warning_precision_contract.py",
    ):
        assert text in library_api_doc


def test_m172_integration_nullability_flow_warning_precision_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m172-nullability-flow-warning-precision-contracts" in scripts
    assert scripts["check:objc3c:m172-nullability-flow-warning-precision-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m172_frontend_nullability_flow_parser_contract.py "
        "tests/tooling/test_objc3c_m172_sema_nullability_flow_warning_precision_contract.py "
        "tests/tooling/test_objc3c_m172_lowering_nullability_flow_warning_precision_contract.py "
        "tests/tooling/test_objc3c_m172_validation_nullability_flow_warning_precision_contract.py "
        "tests/tooling/test_objc3c_m172_integration_nullability_flow_warning_precision_contract.py -q"
    )

    assert "check:compiler-closeout:m172" in scripts
    assert scripts["check:compiler-closeout:m172"] == (
        "npm run check:objc3c:m172-nullability-flow-warning-precision-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m172" in scripts["check:task-hygiene"]

    assert "Enforce M172 nullability-flow warning-precision packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m172" in workflow
    assert "Run M172 nullability flow warning precision integration gate" in workflow
    assert "npm run check:objc3c:m172-nullability-flow-warning-precision-contracts" in workflow
