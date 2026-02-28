from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PACKAGE_JSON = ROOT / "package.json"
CLI_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "10-cli.md"
SEMANTICS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
ARTIFACTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
TESTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
CONTRACT_DOC = ROOT / "docs" / "contracts" / "artifact_tmp_governance_expectations.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m143_package_and_docs_wiring_contract() -> None:
    package_payload = json.loads(_read(PACKAGE_JSON))
    scripts = package_payload["scripts"]

    assert "check:objc3c:library-cli-parity:source:m143" in scripts
    assert "--work-dir tmp/artifacts/objc3c-native/m143/library-cli-parity/work" in scripts[
        "check:objc3c:library-cli-parity:source:m143"
    ]
    assert "--work-key hello-default" in scripts["check:objc3c:library-cli-parity:source:m143"]
    assert "--summary-out tmp/artifacts/compilation/objc3c-native/m143/library-cli-parity/summary.json" in scripts[
        "check:objc3c:library-cli-parity:source:m143"
    ]

    assert "test:objc3c:m143-artifact-governance" in scripts
    assert "tests/tooling/test_objc3c_m143_artifact_tmp_governance_contract.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_check_m143_artifact_tmp_governance_contract.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_objc3c_parser_extraction.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_objc3c_parser_ast_builder_extraction.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_objc3c_sema_extraction.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_objc3c_sema_pass_manager_extraction.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_objc3c_frontend_types_extraction.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]
    assert "tests/tooling/test_objc3c_m143_sema_type_system_tmp_governance_contract.py" in scripts[
        "test:objc3c:m143-artifact-governance"
    ]

    assert "check:compiler-closeout:m143" in scripts
    assert "python scripts/check_m143_artifact_tmp_governance_contract.py" in scripts[
        "check:compiler-closeout:m143"
    ]
    assert "npm run test:objc3c:m143-artifact-governance" in scripts["check:compiler-closeout:m143"]
    assert '--glob "docs/contracts/artifact_tmp_governance_expectations.md"' in scripts[
        "check:compiler-closeout:m143"
    ]
    assert "check:compiler-closeout:m143" in scripts["check:task-hygiene"]

    assert "## Artifact tmp-path governance (M143-D001)" in _read(CLI_FRAGMENT)
    assert "## Artifact tmp-path governance contract (M143-D001)" in _read(SEMANTICS_FRAGMENT)
    assert "Parser/AST-facing lane-A closeout coverage" in _read(SEMANTICS_FRAGMENT)
    assert "m143-sema-type-system-default" in _read(SEMANTICS_FRAGMENT)
    assert "## Artifact tmp-path governance artifacts (M143-D001)" in _read(ARTIFACTS_FRAGMENT)
    assert "tmp/artifacts/compilation/objc3c-native/m143/sema-pass-manager-diagnostics-bus-contract/<run_id>/summary.json" in _read(
        ARTIFACTS_FRAGMENT
    )
    assert "tests/tooling/test_objc3c_parser_extraction.py" in _read(TESTS_FRAGMENT)
    assert "tests/tooling/test_objc3c_parser_ast_builder_extraction.py" in _read(TESTS_FRAGMENT)
    assert "m143-sema-type-system-default" in _read(TESTS_FRAGMENT)
    assert "npm run check:compiler-closeout:m143" in _read(TESTS_FRAGMENT)
    assert "Contract ID: `objc3c-artifact-tmp-governance-contract/m143-v1`" in _read(CONTRACT_DOC)
    assert "| `M143-A001` |" in _read(CONTRACT_DOC)
    assert "| `M143-B001` |" in _read(CONTRACT_DOC)
