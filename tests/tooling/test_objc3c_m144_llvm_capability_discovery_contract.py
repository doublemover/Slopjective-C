from __future__ import annotations

import json
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
PACKAGE_JSON = ROOT / "package.json"
CLI_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "10-cli.md"
SEMANTICS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md"
ARTIFACTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
TESTS_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md"
CONTRACT_DOC = ROOT / "docs" / "contracts" / "llvm_capability_discovery_expectations.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m144_package_and_docs_wiring_contract() -> None:
    payload = json.loads(_read(PACKAGE_JSON))
    scripts = payload["scripts"]

    assert "check:objc3c:llvm-capabilities" in scripts
    assert "scripts/probe_objc3c_llvm_capabilities.py" in scripts["check:objc3c:llvm-capabilities"]
    assert "tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json" in scripts[
        "check:objc3c:llvm-capabilities"
    ]

    assert "check:objc3c:library-cli-parity:source:m144" in scripts
    assert "--work-dir tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/work" in scripts[
        "check:objc3c:library-cli-parity:source:m144"
    ]
    assert "--llvm-capabilities-summary tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json" in scripts[
        "check:objc3c:library-cli-parity:source:m144"
    ]
    assert "--route-cli-backend-from-capabilities" in scripts["check:objc3c:library-cli-parity:source:m144"]

    assert "test:objc3c:m144-llvm-capability-discovery" in scripts
    assert "tests/tooling/test_probe_objc3c_llvm_capabilities.py" in scripts[
        "test:objc3c:m144-llvm-capability-discovery"
    ]
    assert "test_parity_source_mode_routes_backend_from_capabilities_when_enabled" in scripts[
        "test:objc3c:m144-llvm-capability-discovery"
    ]
    assert "tests/tooling/test_objc3c_driver_llvm_capability_routing_extraction.py" in scripts[
        "test:objc3c:m144-llvm-capability-discovery"
    ]
    assert "tests/tooling/test_check_m144_llvm_capability_discovery_contract.py" in scripts[
        "test:objc3c:m144-llvm-capability-discovery"
    ]

    assert "check:compiler-closeout:m144" in scripts
    assert "python scripts/check_m144_llvm_capability_discovery_contract.py" in scripts[
        "check:compiler-closeout:m144"
    ]
    assert "npm run test:objc3c:m144-llvm-capability-discovery" in scripts["check:compiler-closeout:m144"]
    assert '--glob "docs/contracts/llvm_capability_discovery_expectations.md"' in scripts[
        "check:compiler-closeout:m144"
    ]
    assert "check:compiler-closeout:m144" in scripts["check:task-hygiene"]

    assert "## LLVM capability discovery and backend routing (M144-E001)" in _read(CLI_FRAGMENT)
    assert "## LLVM capability discovery contract (M144-E001)" in _read(SEMANTICS_FRAGMENT)
    assert "## LLVM capability discovery artifacts (M144-E001)" in _read(ARTIFACTS_FRAGMENT)
    assert "npm run test:objc3c:m144-llvm-capability-discovery" in _read(TESTS_FRAGMENT)
    assert "npm run check:compiler-closeout:m144" in _read(TESTS_FRAGMENT)
    assert "Contract ID: `objc3c-llvm-capability-discovery-contract/m144-v1`" in _read(CONTRACT_DOC)
