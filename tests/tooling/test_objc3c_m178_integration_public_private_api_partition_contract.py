import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

LIBRARY_API_DOC = ROOT / "docs" / "objc3c-native" / "src" / "library-api.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m178_integration_public_private_api_partition_is_documented() -> None:
    library_api_doc = _read(LIBRARY_API_DOC)

    for text in (
        "## M178 integration public/private API partition semantics contract",
        "check:objc3c:m178-public-private-api-partition-contracts",
        "check:compiler-closeout:m178",
        "tests/tooling/test_objc3c_m178_frontend_public_private_api_partition_parser_contract.py",
        "tests/tooling/test_objc3c_m178_sema_public_private_api_partition_contract.py",
        "tests/tooling/test_objc3c_m178_integration_public_private_api_partition_contract.py",
        "M178-C001 and M178-D001 outputs are not yet landed in this workspace.",
        "while remaining forward-compatible for future M178-C001/M178-D001 additions.",
    ):
        assert text in library_api_doc


def test_m178_integration_public_private_api_partition_gate_is_wired() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)
    scripts = json.loads(package_json)["scripts"]

    assert "check:objc3c:m178-public-private-api-partition-contracts" in scripts
    assert scripts["check:objc3c:m178-public-private-api-partition-contracts"] == (
        "python -m pytest tests/tooling/test_objc3c_m178_frontend_public_private_api_partition_parser_contract.py "
        "tests/tooling/test_objc3c_m178_sema_public_private_api_partition_contract.py "
        "tests/tooling/test_objc3c_m178_integration_public_private_api_partition_contract.py -q"
    )

    assert "check:compiler-closeout:m178" in scripts
    assert scripts["check:compiler-closeout:m178"] == (
        "npm run check:objc3c:m178-public-private-api-partition-contracts && "
        "python scripts/ci/check_task_hygiene.py && python scripts/build_objc3c_native_docs.py --check"
    )
    assert "check:compiler-closeout:m178" in scripts["check:task-hygiene"]

    assert "Enforce M178 public/private API partition packet/docs contract" in workflow
    assert "npm run check:compiler-closeout:m178" in workflow
    assert "Run M178 public/private API partition integration gate" in workflow
    assert "npm run check:objc3c:m178-public-private-api-partition-contracts" in workflow
