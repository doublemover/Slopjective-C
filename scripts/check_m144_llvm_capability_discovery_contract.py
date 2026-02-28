#!/usr/bin/env python3
"""Fail-closed validator for M144 LLVM capability discovery wiring."""

from __future__ import annotations

import argparse
import json
import sys
from dataclasses import dataclass
from pathlib import Path
from typing import Sequence

ROOT = Path(__file__).resolve().parents[1]
MODE = "m144-llvm-capability-discovery-contract-v1"

ARTIFACTS: dict[str, Path] = {
    "probe_script": ROOT / "scripts" / "probe_objc3c_llvm_capabilities.py",
    "parity_script": ROOT / "scripts" / "check_objc3c_library_cli_parity.py",
    "routing_source": ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_llvm_capability_routing.cpp",
    "cli_source": ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.cpp",
    "probe_test": ROOT / "tests" / "tooling" / "test_probe_objc3c_llvm_capabilities.py",
    "parity_test": ROOT / "tests" / "tooling" / "test_objc3c_library_cli_parity.py",
    "routing_test": ROOT / "tests" / "tooling" / "test_objc3c_driver_llvm_capability_routing_extraction.py",
    "driver_cli_test": ROOT / "tests" / "tooling" / "test_objc3c_driver_cli_extraction.py",
    "cli_fragment": ROOT / "docs" / "objc3c-native" / "src" / "10-cli.md",
    "semantics_fragment": ROOT / "docs" / "objc3c-native" / "src" / "30-semantics.md",
    "artifacts_fragment": ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md",
    "tests_fragment": ROOT / "docs" / "objc3c-native" / "src" / "60-tests.md",
    "contract_doc": ROOT / "docs" / "contracts" / "llvm_capability_discovery_expectations.md",
    "package_json": ROOT / "package.json",
}

REQUIRED_SNIPPETS: dict[str, tuple[tuple[str, str], ...]] = {
    "probe_script": (
        ("M144-PROBE-01", 'MODE = "objc3c-llvm-capabilities-v2"'),
        ("M144-PROBE-02", '"sema_type_system_parity"'),
        ("M144-PROBE-03", '"deterministic_type_metadata_handoff"'),
        ("M144-PROBE-04", '"llc capability probe failed: --filetype=obj support not detected"'),
    ),
    "parity_script": (
        ("M144-PAR-01", 'LLVM_CAPABILITY_MODE = "objc3c-llvm-capabilities-v2"'),
        ("M144-PAR-02", '"--llvm-capabilities-summary"'),
        ("M144-PAR-03", '"--route-cli-backend-from-capabilities"'),
        (
            "M144-PAR-04",
            "capability routing fail-closed: --route-cli-backend-from-capabilities requires --llvm-capabilities-summary",
        ),
        ("M144-PAR-05", "capability routing fail-closed: sema/type-system parity capability unavailable:"),
    ),
    "routing_source": (
        ("M144-ROUTE-01", "objc3c-llvm-capabilities-v2"),
        ("M144-ROUTE-02", "llvm capability summary mode mismatch: expected objc3c-llvm-capabilities-v2"),
        (
            "M144-ROUTE-03",
            "capability routing fail-closed: --objc3-route-backend-from-capabilities requires --llvm-capabilities-summary",
        ),
        (
            "M144-ROUTE-04",
            "summary.llc_supports_filetype_obj ? Objc3IrObjectBackend::kLLVMDirect : Objc3IrObjectBackend::kClang",
        ),
        ("M144-ROUTE-05", "options.clang_path = summary.clang_path;"),
        ("M144-ROUTE-06", "options.llc_path = summary.llc_path;"),
    ),
    "cli_source": (
        ("M144-CLI-01", "--llvm-capabilities-summary <path>"),
        ("M144-CLI-02", "--objc3-route-backend-from-capabilities"),
        ("M144-CLI-03", "options.llvm_capabilities_summary = argv[++i];"),
        ("M144-CLI-04", "options.route_backend_from_capabilities = true;"),
    ),
    "probe_test": (
        ("M144-TST-01", "test_probe_passes_when_clang_and_llc_capabilities_are_detected"),
        ("M144-TST-02", "test_package_wires_llvm_capability_probe_script"),
    ),
    "parity_test": (
        ("M144-TST-03", "test_parity_source_mode_routes_backend_from_capabilities_when_enabled"),
        ("M144-TST-04", "test_parity_source_mode_fail_closes_when_capability_parity_is_unavailable"),
        (
            "M144-TST-05",
            "test_parity_source_mode_fail_closes_when_capability_routing_is_requested_without_summary",
        ),
    ),
    "routing_test": (
        ("M144-TST-06", "test_driver_llvm_capability_routing_is_fail_closed_and_mode_pinned"),
    ),
    "driver_cli_test": (
        ("M144-TST-07", "--llvm-capabilities-summary <path>"),
        ("M144-TST-08", "--objc3-route-backend-from-capabilities"),
    ),
    "cli_fragment": (
        ("M144-DOC-CLI-01", "## LLVM capability discovery and backend routing (M144-E001)"),
        ("M144-DOC-CLI-02", "--llvm-capabilities-summary <path>"),
        ("M144-DOC-CLI-03", "--objc3-route-backend-from-capabilities"),
    ),
    "semantics_fragment": (
        ("M144-DOC-SEM-01", "## LLVM capability discovery contract (M144-E001)"),
        ("M144-DOC-SEM-02", "objc3c-llvm-capabilities-v2"),
        ("M144-DOC-SEM-03", "python scripts/check_m144_llvm_capability_discovery_contract.py"),
    ),
    "artifacts_fragment": (
        ("M144-DOC-ART-01", "## LLVM capability discovery artifacts (M144-E001)"),
        ("M144-DOC-ART-02", "tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json"),
        ("M144-DOC-ART-03", "npm run check:compiler-closeout:m144"),
    ),
    "tests_fragment": (
        ("M144-DOC-TST-01", "npm run test:objc3c:m144-llvm-capability-discovery"),
        ("M144-DOC-TST-02", "npm run check:objc3c:llvm-capabilities"),
        ("M144-DOC-TST-03", "npm run check:compiler-closeout:m144"),
    ),
    "contract_doc": (
        ("M144-DOC-CON-01", "Contract ID: `objc3c-llvm-capability-discovery-contract/m144-v1`"),
        ("M144-DOC-CON-02", "| `M144-CAP-04` |"),
        ("M144-DOC-CON-03", "npm run check:compiler-closeout:m144"),
    ),
}

REQUIRED_PACKAGE_SCRIPTS: dict[str, tuple[str, ...]] = {
    "check:objc3c:llvm-capabilities": (
        "python scripts/probe_objc3c_llvm_capabilities.py",
        "--summary-out tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json",
    ),
    "check:objc3c:library-cli-parity:source:m144": (
        "python scripts/check_objc3c_library_cli_parity.py",
        "--work-dir tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/work",
        "--summary-out tmp/artifacts/compilation/objc3c-native/m144/library-cli-parity/summary.json",
        "--llvm-capabilities-summary tmp/artifacts/objc3c-native/m144/llvm_capabilities/summary.json",
        "--route-cli-backend-from-capabilities",
    ),
    "test:objc3c:m144-llvm-capability-discovery": (
        "tests/tooling/test_probe_objc3c_llvm_capabilities.py",
        "tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_routes_backend_from_capabilities_when_enabled",
        "tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_fail_closes_when_capability_parity_is_unavailable",
        "tests/tooling/test_objc3c_library_cli_parity.py::test_parity_source_mode_fail_closes_when_capability_routing_is_requested_without_summary",
        "tests/tooling/test_objc3c_driver_llvm_capability_routing_extraction.py",
        "tests/tooling/test_objc3c_driver_cli_extraction.py",
        "tests/tooling/test_objc3c_m144_llvm_capability_discovery_contract.py",
        "tests/tooling/test_check_m144_llvm_capability_discovery_contract.py",
    ),
    "check:compiler-closeout:m144": (
        "python scripts/check_m144_llvm_capability_discovery_contract.py",
        "npm run test:objc3c:m144-llvm-capability-discovery",
        '--glob "docs/contracts/llvm_capability_discovery_expectations.md"',
    ),
    "check:task-hygiene": (
        "check:compiler-closeout:m144",
    ),
}


@dataclass(frozen=True)
class Finding:
    artifact: str
    check_id: str
    detail: str


def display_path(path: Path) -> str:
    absolute = path.resolve()
    try:
        return absolute.relative_to(ROOT).as_posix()
    except ValueError:
        return absolute.as_posix()


def canonical_json(payload: object) -> str:
    return json.dumps(payload, indent=2) + "\n"


def parse_args(argv: Sequence[str]) -> argparse.Namespace:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument(
        "--summary-out",
        type=Path,
        default=Path("tmp/m144_llvm_capability_discovery_contract_summary.json"),
    )
    return parser.parse_args(argv)


def load_text(path: Path, *, artifact: str) -> str:
    if not path.exists() or not path.is_file():
        raise ValueError(f"{artifact} missing file: {display_path(path)}")
    return path.read_text(encoding="utf-8")


def collect_text_findings(*, artifact: str, text: str) -> list[Finding]:
    findings: list[Finding] = []
    for check_id, snippet in REQUIRED_SNIPPETS.get(artifact, ()):
        if snippet not in text:
            findings.append(
                Finding(artifact, check_id, f"expected snippet missing: {snippet}")
            )
    return findings


def collect_package_findings(package_json: Path) -> list[Finding]:
    payload = json.loads(load_text(package_json, artifact="package_json"))
    scripts = payload.get("scripts")
    if not isinstance(scripts, dict):
        return [
            Finding(
                "package_json",
                "M144-PKG-00",
                "package.json scripts field must be an object",
            )
        ]

    findings: list[Finding] = []
    for script_name, required_tokens in REQUIRED_PACKAGE_SCRIPTS.items():
        script_value = scripts.get(script_name)
        if not isinstance(script_value, str):
            findings.append(
                Finding(
                    "package_json",
                    "M144-PKG-01",
                    f"missing scripts['{script_name}']",
                )
            )
            continue
        for index, token in enumerate(required_tokens, start=1):
            if token not in script_value:
                findings.append(
                    Finding(
                        "package_json",
                        f"M144-PKG-{script_name}-{index}",
                        f"scripts['{script_name}'] missing token: {token}",
                    )
                )
    return findings


def run(argv: Sequence[str]) -> int:
    args = parse_args(argv)
    findings: list[Finding] = []

    for artifact, path in ARTIFACTS.items():
        if artifact == "package_json":
            continue
        findings.extend(
            collect_text_findings(
                artifact=artifact,
                text=load_text(path, artifact=artifact),
            )
        )
    findings.extend(collect_package_findings(ARTIFACTS["package_json"]))
    findings.sort(key=lambda item: (item.artifact, item.check_id, item.detail))

    checks_total = sum(len(items) for items in REQUIRED_SNIPPETS.values()) + sum(
        len(items) for items in REQUIRED_PACKAGE_SCRIPTS.values()
    )
    summary = {
        "mode": MODE,
        "ok": not findings,
        "checks_total": checks_total,
        "checks_passed": checks_total - len(findings),
        "failures": [
            {
                "artifact": finding.artifact,
                "check_id": finding.check_id,
                "detail": finding.detail,
            }
            for finding in findings
        ],
    }

    args.summary_out.parent.mkdir(parents=True, exist_ok=True)
    args.summary_out.write_text(canonical_json(summary), encoding="utf-8")

    if findings:
        print(
            "m144-llvm-capability-discovery-contract: contract drift detected "
            f"({len(findings)} finding(s)).",
            file=sys.stderr,
        )
        for finding in findings:
            print(
                f"- {finding.artifact}:{finding.check_id} {finding.detail}",
                file=sys.stderr,
            )
        print("remediation:", file=sys.stderr)
        print("1. Restore M144 capability-discovery source/docs/package snippets.", file=sys.stderr)
        print("2. Re-run: python scripts/check_m144_llvm_capability_discovery_contract.py", file=sys.stderr)
        print(
            f"3. Inspect summary: {display_path(args.summary_out)}",
            file=sys.stderr,
        )
        return 1

    print("m144-llvm-capability-discovery-contract: OK")
    print(f"- mode={MODE}")
    print(f"- checks_passed={summary['checks_passed']}")
    print("- fail_closed=true")
    print(f"- summary={display_path(args.summary_out)}")
    return 0


def main() -> None:
    raise SystemExit(run(sys.argv[1:]))


if __name__ == "__main__":
    main()
