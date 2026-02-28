from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

TESTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
DOC_AGGREGATE = ROOT / "docs" / "objc3c-native.md"
PACKAGE_JSON = ROOT / "package.json"
TASK_HYGIENE_WORKFLOW = ROOT / ".github" / "workflows" / "task-hygiene.yml"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m223_operator_quickstart_is_documented() -> None:
    tests_fragment = _read(TESTS_DOC_FRAGMENT)
    aggregate_doc = _read(DOC_AGGREGATE)

    for text in (
        "## M223 operator quickstart (docs+CI parity)",
        "npm run build:objc3c-native",
        "npm run compile:objc3c -- tests/tooling/fixtures/native/hello.objc3",
        "npm run test:objc3c:m222-compatibility-migration",
        "python scripts/build_objc3c_native_docs.py --check",
    ):
        assert text in tests_fragment
        assert text in aggregate_doc


def test_m223_operator_quickstart_is_wired_in_package_and_workflow() -> None:
    package_json = _read(PACKAGE_JSON)
    workflow = _read(TASK_HYGIENE_WORKFLOW)

    assert '"check:objc3c:m223-operator-guides"' in package_json
    assert "npm run test:objc3c:m222-compatibility-migration && python scripts/build_objc3c_native_docs.py --check" in package_json

    assert "Run M223 operator-guides docs+CI parity gate" in workflow
    assert "npm run check:objc3c:m223-operator-guides" in workflow
