import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m154_integration_property_synthesis_ivar_binding_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M154 integration property synthesis ivar binding contract",
        "check:objc3c:m154-property-synthesis-ivar-bindings",
        "check:compiler-closeout:m154",
        "tests/tooling/test_objc3c_m154_frontend_property_synthesis_ivar_binding_contract.py",
        "tests/tooling/test_objc3c_m154_sema_property_synthesis_ivar_binding_contract.py",
        "tests/tooling/test_objc3c_m154_lowering_property_synthesis_ivar_binding_contract.py",
        "tests/tooling/test_objc3c_m154_validation_property_synthesis_ivar_binding_contract.py",
        "tests/tooling/test_objc3c_m154_integration_property_synthesis_ivar_binding_contract.py",
    ):
        assert text in library_api_doc


def test_m154_integration_property_synthesis_ivar_binding_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m154-property-synthesis-ivar-bindings" in scripts
    assert scripts["check:objc3c:m154-property-synthesis-ivar-bindings"] == (
        "python -m pytest tests/tooling/test_objc3c_m154_frontend_property_synthesis_ivar_binding_contract.py "
        "tests/tooling/test_objc3c_m154_sema_property_synthesis_ivar_binding_contract.py "
        "tests/tooling/test_objc3c_m154_lowering_property_synthesis_ivar_binding_contract.py "
        "tests/tooling/test_objc3c_m154_validation_property_synthesis_ivar_binding_contract.py "
        "tests/tooling/test_objc3c_m154_integration_property_synthesis_ivar_binding_contract.py -q"
    )

    assert "check:compiler-closeout:m154" in scripts
    assert scripts["check:compiler-closeout:m154"] == (
        "npm run check:objc3c:m154-property-synthesis-ivar-bindings && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m154" in scripts["check:task-hygiene"]

    assert "Enforce M154 property-synthesis/ivar-binding packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m154" in workflow
    assert "Run M154 property-synthesis/ivar-binding integration gate" in workflow
    assert "npm run check:objc3c:m154-property-synthesis-ivar-bindings" in workflow
