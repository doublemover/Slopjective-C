from __future__ import annotations

import importlib.util
import json
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPT_PATH = (
    ROOT
    / "scripts"
    / "check_m235_b011_qualifier_and_generic_semantic_inference_performance_and_quality_guardrails_contract.py"
)
SPEC = importlib.util.spec_from_file_location(
    "check_m235_b011_qualifier_and_generic_semantic_inference_performance_and_quality_guardrails_contract",
    SCRIPT_PATH,
)
if SPEC is None or SPEC.loader is None:
    raise RuntimeError(
        "Unable to load scripts/check_m235_b011_qualifier_and_generic_semantic_inference_performance_and_quality_guardrails_contract.py"
    )
contract = importlib.util.module_from_spec(SPEC)
sys.modules[SPEC.name] = contract
SPEC.loader.exec_module(contract)


def replace_all(text: str, old: str, new: str) -> str:
    assert old in text
    replaced = text.replace(old, new)
    assert old not in replaced
    return replaced


def write_snippet_fixture(path: Path, snippets: tuple[contract.SnippetCheck, ...]) -> None:
    path.write_text("\n".join(snippet.snippet for snippet in snippets) + "\n", encoding="utf-8")


def supporting_overrides(tmp_path: Path) -> list[str]:
    b010_expectations = tmp_path / "m235_b010_expectations.md"
    write_snippet_fixture(b010_expectations, contract.B010_EXPECTATIONS_SNIPPETS)

    b010_packet = tmp_path / "m235_b010_packet.md"
    write_snippet_fixture(b010_packet, contract.B010_PACKET_SNIPPETS)

    architecture = tmp_path / "ARCHITECTURE.md"
    write_snippet_fixture(architecture, contract.ARCHITECTURE_SNIPPETS)

    lowering = tmp_path / "LOWERING_AND_RUNTIME_CONTRACTS.md"
    write_snippet_fixture(lowering, contract.LOWERING_SPEC_SNIPPETS)

    metadata = tmp_path / "MODULE_METADATA_AND_ABI_TABLES.md"
    write_snippet_fixture(metadata, contract.METADATA_SPEC_SNIPPETS)

    package = tmp_path / "package.json"
    write_snippet_fixture(package, contract.PACKAGE_SNIPPETS)

    b010_checker = tmp_path / "check_m235_b010.py"
    b010_checker.write_text("# fixture\n", encoding="utf-8")

    b010_test = tmp_path / "test_check_m235_b010.py"
    b010_test.write_text("# fixture\n", encoding="utf-8")

    return [
        "--b010-expectations-doc",
        str(b010_expectations),
        "--b010-packet-doc",
        str(b010_packet),
        "--architecture-doc",
        str(architecture),
        "--lowering-spec",
        str(lowering),
        "--metadata-spec",
        str(metadata),
        "--package-json",
        str(package),
        "--b010-checker",
        str(b010_checker),
        "--b010-test",
        str(b010_test),
    ]


def test_contract_passes_with_synthetic_supporting_assets(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    exit_code = contract.run([*supporting_overrides(tmp_path), "--summary-out", str(summary_out)])

    assert exit_code == 0
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["mode"] == "m235-b011-qualifier-and-generic-semantic-inference-performance-and-quality-guardrails-contract-v1"
    assert payload["ok"] is True
    assert payload["checks_total"] >= 58
    assert payload["checks_passed"] == payload["checks_total"]
    assert payload["failures"] == []


def test_contract_default_summary_out_is_under_tmp_reports_m235_b011() -> None:
    args = contract.parse_args([])
    normalized = str(args.summary_out).replace("\\", "/")
    assert normalized.startswith("tmp/reports/m235/M235-B011/")


def test_contract_fails_closed_when_expectations_dependency_token_drifts(tmp_path: Path) -> None:
    drift_doc = tmp_path / "m235_b011_expectations.md"
    drift_doc.write_text(
        replace_all(
            contract.DEFAULT_EXPECTATIONS_DOC.read_text(encoding="utf-8"),
            "Dependencies: `M235-B010`",
            "Dependencies: `M235-B099`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            *supporting_overrides(tmp_path),
            "--expectations-doc",
            str(drift_doc),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-B011-DOC-EXP-03" for failure in payload["failures"])


def test_contract_fails_closed_when_packet_issue_metadata_drifts(tmp_path: Path) -> None:
    drift_packet = tmp_path / "m235_b011_packet.md"
    drift_packet.write_text(
        replace_all(
            contract.DEFAULT_PACKET_DOC.read_text(encoding="utf-8"),
            "Issue: `#5791`",
            "Issue: `#6000`",
        ),
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            *supporting_overrides(tmp_path),
            "--packet-doc",
            str(drift_packet),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-B011-DOC-PKT-03" for failure in payload["failures"])


def test_contract_fails_closed_when_package_readiness_chain_drops_b010(tmp_path: Path) -> None:
    override_args = supporting_overrides(tmp_path)
    drift_package = tmp_path / "drift_package.json"
    drift_package.write_text(
        replace_all(
            contract.PACKAGE_SNIPPETS[2].snippet,
            '"check:objc3c:m235-b011-lane-b-readiness": '
            '"npm run check:objc3c:m235-b010-lane-b-readiness && ',
            '"check:objc3c:m235-b011-lane-b-readiness": "',
        )
        + "\n"
        + "\n".join(
            snippet.snippet for index, snippet in enumerate(contract.PACKAGE_SNIPPETS) if index != 2
        )
        + "\n",
        encoding="utf-8",
    )

    summary_out = tmp_path / "summary.json"
    exit_code = contract.run(
        [
            *override_args,
            "--package-json",
            str(drift_package),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-B011-PKG-03" for failure in payload["failures"])


def test_contract_fails_closed_when_b010_checker_dependency_path_missing(tmp_path: Path) -> None:
    summary_out = tmp_path / "summary.json"
    missing_checker = tmp_path / "missing_b010_checker.py"
    exit_code = contract.run(
        [
            *supporting_overrides(tmp_path),
            "--b010-checker",
            str(missing_checker),
            "--summary-out",
            str(summary_out),
        ]
    )

    assert exit_code == 1
    payload = json.loads(summary_out.read_text(encoding="utf-8"))
    assert payload["ok"] is False
    assert any(failure["check_id"] == "M235-B011-DEP-B010-ARG-01" for failure in payload["failures"])
