from __future__ import annotations

import json
from pathlib import Path

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
        "Allowed fallback behavior:\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "deterministic-fallback-wording"


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
