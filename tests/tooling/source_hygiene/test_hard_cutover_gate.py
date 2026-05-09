from __future__ import annotations

import json
from pathlib import Path

from scripts.source_hygiene.roots import DEFAULT_SCAN_ROOTS
from scripts.source_hygiene.scanner import build_report, write_reports


def write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def test_hard_cutover_gate_fails_on_active_forbidden_pattern(tmp_path: Path) -> None:
    write(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "int ComputeDispatchResult = 0;\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["stats"]["active_finding_count"] == 1
    assert report["active_findings"][0]["pattern_id"] == "runtime-dispatch-pseudo-success"


def test_hard_cutover_gate_rejects_dotted_runtime_shim_tokens(tmp_path: Path) -> None:
    write(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "const char *contract = \"objc3c.runtime.shim.host.link.v1\";\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "runtime-shim-token"


def test_hard_cutover_gate_rejects_retired_msgsend_symbol(tmp_path: Path) -> None:
    write(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "call i32 @objc3_msgsend_i32(i32 %receiver, ptr %selector)\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "retired-msgsend-compatibility-dispatch"


def test_hard_cutover_gate_rejects_unresolved_dispatch_pseudo_success(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "docs/objc3c-native/src/10-cli.md",
        "Unresolved runtime dispatch pseudo-success remains documented.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "unresolved-dispatch-pseudo-success"


def test_hard_cutover_gate_rejects_compatibility_dispatch_symbol_fields(tmp_path: Path) -> None:
    write(
        tmp_path / "native/objc3c/src/lower/contract.cpp",
        "\n".join(
            [
                "std::string compatibility_runtime_dispatch_symbol;",
                "auto key = runtime_support_library_link_wiring_compatibility_dispatch_symbol;",
            ]
        )
        + "\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert [finding["pattern_id"] for finding in report["active_findings"]] == [
        "retired-msgsend-compatibility-dispatch",
        "retired-msgsend-compatibility-dispatch",
    ]


def test_hard_cutover_gate_rejects_compatibility_dispatch_wording_in_required_roots(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "docs/objc3c-native/src/10-cli.md",
        "Compatibility dispatch symbol remains documented.\n",
    )
    write(tmp_path / "spec/LOWERING_AND_RUNTIME_CONTRACTS.md", "`objc3_msgsend_i32` remains exported.\n")
    write(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "const char *field = \"compatibility_runtime_dispatch_symbol\";\n",
    )
    write(tmp_path / "tests/tooling/test_runtime_dispatch_contract.py", 'TOKEN = "objc3_msgsend_i32"\n')

    report = build_report(root=tmp_path)

    assert report["ok"] is False
    assert report["stats"]["active_finding_count"] == 4
    assert {
        finding["path"]: finding["pattern_id"] for finding in report["active_findings"]
    } == {
        "docs/objc3c-native/src/10-cli.md": "retired-msgsend-compatibility-dispatch",
        "spec/LOWERING_AND_RUNTIME_CONTRACTS.md": "retired-msgsend-compatibility-dispatch",
        "native/objc3c/src/runtime/dispatch.cpp": "retired-msgsend-compatibility-dispatch",
        "tests/tooling/test_runtime_dispatch_contract.py": "retired-msgsend-compatibility-dispatch",
    }


def test_hard_cutover_gate_covers_runtime_docs_spec_and_test_surfaces(tmp_path: Path) -> None:
    write(
        tmp_path / "docs/objc3c-native/src/50-artifacts.md",
        "Runtime dispatch keeps unresolved call pseudo-success metadata.\n",
    )
    write(
        tmp_path / "spec/LOWERING_AND_RUNTIME_CONTRACTS.md",
        "The runtime dispatch lane is fallback-only for unresolved calls.\n",
    )
    write(
        tmp_path / "tests/tooling/test_runtime_dispatch_contract.py",
        'CLAIM = "deterministic arithmetic formula selects dispatch"\n',
    )

    report = build_report(root=tmp_path)

    assert report["ok"] is False
    assert {
        finding["path"]: finding["pattern_id"] for finding in report["active_findings"]
    } == {
        "docs/objc3c-native/src/50-artifacts.md": "unresolved-dispatch-pseudo-success",
        "spec/LOWERING_AND_RUNTIME_CONTRACTS.md": "fallback-only-wording",
        "tests/tooling/test_runtime_dispatch_contract.py": "deterministic-runtime-arithmetic",
    }


def test_hard_cutover_gate_allows_negative_absence_assertions(tmp_path: Path) -> None:
    write(
        tmp_path / "tests/tooling/test_runtime_surface.py",
        "\n".join(
            [
                'assert "objc3_msgsend_i32" not in emitted_ir',
                'assert "compatibility_runtime_dispatch_symbol" not in manifest',
            ]
        )
        + "\n",
    )

    report = build_report(root=tmp_path, scan_roots=("tests",), excludes=())

    assert report["ok"] is True
    assert report["stats"]["active_finding_count"] == 0


def test_hard_cutover_gate_excludes_source_hygiene_violation_fixtures(tmp_path: Path) -> None:
    write(
        tmp_path / "tests/tooling/source_hygiene/fixtures/retired_msgsend_violation.txt",
        "objc3_msgsend_i32 compatibility_runtime_dispatch_symbol\n",
    )

    report = build_report(root=tmp_path, scan_roots=("tests",))

    assert report["ok"] is True
    assert report["stats"]["active_finding_count"] == 0


def test_hard_cutover_gate_allows_temporary_tmp_allowlist(tmp_path: Path) -> None:
    write(
        tmp_path / "native/objc3c/src/driver/options.cpp",
        "const char *flag = \"--objc3-compat-mode\";\n",
    )
    allowlist = tmp_path / "tmp/source-hygiene-hard-cutover-allowlist.json"
    write(
        allowlist,
        json.dumps(
            {
                "allowed": [
                    {
                        "pattern_id": "public-compatibility-mode",
                        "path": "native/objc3c/src/driver/options.cpp",
                    }
                ]
            }
        ),
    )

    report = build_report(
        root=tmp_path,
        scan_roots=("native/objc3c",),
        excludes=(),
        allowlist_path=allowlist,
    )

    assert report["ok"] is True
    assert report["stats"]["allowed_finding_count"] == 1
    assert report["stats"]["active_finding_count"] == 0


def test_hard_cutover_gate_rejects_legacy_language_profile_enum_values(tmp_path: Path) -> None:
    write(
        tmp_path / "native/objc3c/src/pipeline/options.h",
        "enum class Objc3FrontendLanguageProfile { kCanonical = 0u, kLegacy = 1u };\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "legacy-language-profile-enum"


def test_hard_cutover_gate_rejects_plain_compatibility_mode_wording(tmp_path: Path) -> None:
    write(
        tmp_path / "native/objc3c/src/driver/options.cpp",
        "const char *claim = \"compatibility mode is supported\";\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "public-compatibility-mode"


def test_hard_cutover_gate_rejects_retired_public_workflow_runner_path(tmp_path: Path) -> None:
    write(
        tmp_path / "docs/runbooks/commands.md",
        "Run python scripts/objc3c_public_workflow_runner.py test-fast.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "retired-public-workflow-runner"


def test_hard_cutover_gate_rejects_retired_npm_workflow_aliases(tmp_path: Path) -> None:
    write(
        tmp_path / "docs/runbooks/commands.md",
        "Run npm run test:fast.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "retired-npm-workflow-command"


def test_hard_cutover_gate_rejects_retired_public_script_alias_metadata(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "docs/support/capability_matrix.md",
        "The public_scripts alias table remains authoritative.\n",
    )
    write(
        tmp_path / "site/src/index.body.md",
        "The publicScripts metadata field is still displayed.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs", "site"), excludes=())

    assert report["ok"] is False
    assert [finding["pattern_id"] for finding in report["active_findings"]] == [
        "retired-public-script-alias-metadata",
        "retired-public-script-alias-metadata",
    ]


def test_hard_cutover_default_roots_cover_public_command_truth_surfaces() -> None:
    assert "README.md" in DEFAULT_SCAN_ROOTS
    assert "CONTRIBUTING.md" in DEFAULT_SCAN_ROOTS
    assert "docs" in DEFAULT_SCAN_ROOTS
    assert "showcase" in DEFAULT_SCAN_ROOTS
    assert "spec" in DEFAULT_SCAN_ROOTS
    assert "stdlib" in DEFAULT_SCAN_ROOTS
    assert "site" in DEFAULT_SCAN_ROOTS
    assert "tests" in DEFAULT_SCAN_ROOTS


def test_hard_cutover_gate_rejects_legacy_literal_diagnostics_switch(tmp_path: Path) -> None:
    write(
        tmp_path / "native/objc3c/src/pipeline/options.h",
        "bool legacy_literal_diagnostics = false;\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "canonical-rejection-diagnostics-surface"


def test_hard_cutover_gate_rejects_fallback_behavior_wording(tmp_path: Path) -> None:
    write(
        tmp_path / "docs/runbooks/runtime.md",
        "Allowed fallback behaviors:\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "deterministic-fallback-wording"


def test_hard_cutover_gate_rejects_fallback_only_wording(tmp_path: Path) -> None:
    write(
        tmp_path / "spec/LOWERING_AND_RUNTIME_CONTRACTS.md",
        "The runtime dispatch path is fallback-only.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("spec",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "fallback-only-wording"


def test_hard_cutover_gate_rejects_deterministic_arithmetic_wording(tmp_path: Path) -> None:
    write(
        tmp_path / "tests/tooling/test_runtime_dispatch_contract.py",
        'CLAIM = "deterministic arithmetic runtime dispatch formula"\n',
    )

    report = build_report(root=tmp_path, scan_roots=("tests",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "deterministic-runtime-arithmetic"


def test_hard_cutover_report_matches_schema_shape(tmp_path: Path) -> None:
    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())
    json_path = tmp_path / "tmp/reports/source_hygiene/hard-cutover/report.json"
    text_path = tmp_path / "tmp/reports/source_hygiene/hard-cutover/report.txt"

    write_reports(report, json_path, text_path)
    payload = json.loads(json_path.read_text(encoding="utf-8"))

    assert payload["schema_version"] == "source-hygiene-hard-cutover-report-v1"
    assert isinstance(payload["forbidden_patterns"], list)
    assert isinstance(payload["active_findings"], list)
    assert payload["stats"]["active_finding_count"] == 0
    assert text_path.read_text(encoding="utf-8").startswith("schema_version:")


def test_hard_cutover_gate_excludes_canonical_config_registry(tmp_path: Path) -> None:
    write(
        tmp_path / "native/objc3c/src/config/objc3_language_profile.h",
        'const char *removed = "--objc3-compat-mode";\n',
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",))

    assert report["ok"] is True
    assert report["stats"]["active_finding_count"] == 0
