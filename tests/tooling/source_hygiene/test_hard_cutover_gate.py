from __future__ import annotations

import json
from pathlib import Path

from scripts.source_hygiene.gate_contracts import (
    HARD_CUTOVER_GATE_ID,
    REQUIRED_RESIDUE_CLASSES,
    RETIRED_ALLOWLIST_REPORT_FIELDS,
)
from scripts.source_hygiene.hard_cutover_gate import (
    HARD_CUTOVER_GATE_CLI_CONTRACT_ID,
    HARD_CUTOVER_GATE_SUMMARY_FIELDS,
    format_gate_summary,
    hard_cutover_gate_cli_contract_payload,
)
from scripts.source_hygiene.generated_reports import (
    GENERATED_TRUTH_BOUNDARY_CLAIM_POLICY,
    GENERATED_TRUTH_BOUNDARY_CONTRACT_ID,
)
from scripts.source_hygiene.patterns_cutover import HARD_CUTOVER_RESIDUE_PATTERNS
from scripts.source_hygiene.patterns_implementation_fallbacks import (
    IMPLEMENTATION_FALLBACK_PATTERNS,
)
from scripts.source_hygiene.patterns_implementation_legacy import (
    IMPLEMENTATION_LEGACY_PATTERNS,
)
from scripts.source_hygiene.patterns_implementation_migration import (
    IMPLEMENTATION_MIGRATION_PATTERNS,
)
from scripts.source_hygiene.patterns_implementation_shims import IMPLEMENTATION_SHIM_PATTERNS
from scripts.source_hygiene.patterns_public_aliases import PUBLIC_ALIAS_PATTERNS
from scripts.source_hygiene.patterns_public_claims import PUBLIC_CLAIM_PATTERNS
from scripts.source_hygiene.patterns_public_fallbacks import PUBLIC_FALLBACK_PATTERNS
from scripts.source_hygiene.patterns_public_legacy import PUBLIC_LEGACY_PATTERNS
from scripts.source_hygiene.patterns_public_migration import PUBLIC_MIGRATION_PATTERNS
from scripts.source_hygiene.patterns_public_projection import PUBLIC_PROJECTION_PATTERNS
from scripts.source_hygiene.patterns_public_shims import PUBLIC_SHIM_PATTERNS
from scripts.source_hygiene.roots import DEFAULT_SCAN_ROOTS
from scripts.source_hygiene.scanner import build_report, write_reports
from scripts.source_hygiene.scan_config import SOURCE_HYGIENE_SCAN_CONFIG_CONTRACT_ID
from scripts.source_hygiene.report_writer import (
    REPORT_SUMMARY_FIELDS,
    REPORT_WRITER_CONTRACT_ID,
    report_writer_contract_payload,
)
from scripts.source_hygiene.owners import (
    SOURCE_HYGIENE_BLOCKER_METADATA_OWNER,
    SOURCE_HYGIENE_GENERATED_REPORT_OWNER,
    SOURCE_HYGIENE_PATTERN_OWNER,
    SOURCE_HYGIENE_SCAN_ROOT_OWNER,
)


def write(path: Path, text: str) -> None:
    path.parent.mkdir(parents=True, exist_ok=True)
    path.write_text(text, encoding="utf-8")


def test_public_claim_patterns_are_split_by_owner_modules() -> None:
    owner_patterns = (
        *PUBLIC_ALIAS_PATTERNS,
        *PUBLIC_FALLBACK_PATTERNS,
        *PUBLIC_PROJECTION_PATTERNS,
        *PUBLIC_LEGACY_PATTERNS,
        *PUBLIC_SHIM_PATTERNS,
        *PUBLIC_MIGRATION_PATTERNS,
    )

    assert PUBLIC_CLAIM_PATTERNS == owner_patterns
    assert len({pattern.pattern_id for pattern in PUBLIC_CLAIM_PATTERNS}) == len(
        PUBLIC_CLAIM_PATTERNS
    )


def test_implementation_residue_patterns_are_split_by_owner_modules() -> None:
    owner_patterns = (
        *IMPLEMENTATION_SHIM_PATTERNS,
        *IMPLEMENTATION_FALLBACK_PATTERNS,
        *IMPLEMENTATION_MIGRATION_PATTERNS,
        *IMPLEMENTATION_LEGACY_PATTERNS,
    )

    assert HARD_CUTOVER_RESIDUE_PATTERNS == owner_patterns
    assert len({pattern.pattern_id for pattern in HARD_CUTOVER_RESIDUE_PATTERNS}) == len(
        HARD_CUTOVER_RESIDUE_PATTERNS
    )


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


def test_hard_cutover_gate_ignores_retired_tmp_allowlist(tmp_path: Path) -> None:
    write(
        tmp_path / "native/objc3c/src/driver/options.cpp",
        "const char *flag = \"--objc3-compat-mode\";\n",
    )
    write(
        tmp_path / "tmp/source-hygiene-hard-cutover-allowlist.json",
        '{"allowed":[{"pattern_id":"public-compatibility-mode"}]}\n',
    )

    report = build_report(
        root=tmp_path,
        scan_roots=("native/objc3c",),
        excludes=(),
    )

    assert report["ok"] is False
    assert "allowed_finding_count" not in report["stats"]
    assert report["stats"]["active_finding_count"] == 1
    assert report["active_findings"][0]["pattern_id"] == "public-compatibility-mode"


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


def test_hard_cutover_gate_rejects_implementation_compatibility_bridge(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "int compatibility_bridge = 1;\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "compatibility-wrapper-or-bridge-surface"


def test_hard_cutover_gate_rejects_implementation_fallback_handler(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "int fallback_handler = 1;\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "fallback-implementation-surface"


def test_hard_cutover_gate_rejects_implementation_migration_lane(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "int migration_lane = 1;\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "migration-implementation-surface"


def test_hard_cutover_gate_rejects_implementation_legacy_support(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "int legacy_support = 1;\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "legacy-compatibility-support-surface"


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


def test_hard_cutover_gate_rejects_direct_native_compile_wrapper_commands(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "docs/objc3c-native.md",
        "Run pwsh -NoProfile -File scripts/objc3c_native_compile.ps1 sample.objc3.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "direct-native-compile-wrapper-command"


def test_hard_cutover_gate_allows_native_compile_wrapper_as_source_anchor(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "docs/objc3c-native.md",
        "- implementation anchor: `scripts/objc3c_native_compile.ps1`\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is True
    assert report["stats"]["active_finding_count"] == 0


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
    write(
        tmp_path / "stdlib/workspace.json",
        '{"publicScriptAliases": ["test:fast"]}\n',
    )

    report = build_report(
        root=tmp_path,
        scan_roots=("docs", "site", "stdlib"),
        excludes=(),
    )

    assert report["ok"] is False
    assert [finding["pattern_id"] for finding in report["active_findings"]] == [
        "retired-public-script-alias-metadata",
        "retired-public-script-alias-metadata",
        "retired-public-script-alias-metadata",
    ]


def test_hard_cutover_gate_rejects_retired_workflow_registry_facade(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "docs/runbooks/commands.md",
        "Action source: scripts/objc3c_workflow/registry.py\n",
    )
    write(
        tmp_path / "scripts/objc3c_workflow/actions/docs.py",
        "from ..registry import ACTION_SPECS\n",
    )
    write(
        tmp_path / "tests/tooling/test_objc3c_workflow_runner_decomposition.py",
        "from scripts.objc3c_workflow.registry import ACTION_SPECS\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs", "scripts", "tests"), excludes=())

    assert report["ok"] is False
    assert {
        finding["path"]: finding["pattern_id"] for finding in report["active_findings"]
    } == {
        "docs/runbooks/commands.md": "retired-workflow-action-registry-facade",
        "scripts/objc3c_workflow/actions/docs.py": "retired-workflow-action-registry-facade",
        "tests/tooling/test_objc3c_workflow_runner_decomposition.py": "retired-workflow-action-registry-facade",
    }


def test_hard_cutover_default_roots_cover_public_command_truth_surfaces() -> None:
    assert "README.md" in DEFAULT_SCAN_ROOTS
    assert "CONTRIBUTING.md" in DEFAULT_SCAN_ROOTS
    assert "docs" in DEFAULT_SCAN_ROOTS
    assert "showcase" in DEFAULT_SCAN_ROOTS
    assert "spec" in DEFAULT_SCAN_ROOTS
    assert "stdlib" in DEFAULT_SCAN_ROOTS
    assert "site" in DEFAULT_SCAN_ROOTS
    assert "tests" in DEFAULT_SCAN_ROOTS


def test_hard_cutover_policy_data_covers_closure_residue_classes(tmp_path: Path) -> None:
    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())
    residue_classes = {
        pattern["residue_class"] for pattern in report["forbidden_patterns"]
    }

    assert set(REQUIRED_RESIDUE_CLASSES) <= residue_classes
    assert {
        pattern["gate_contract"] for pattern in report["forbidden_patterns"]
    } == {HARD_CUTOVER_GATE_ID}


def test_hard_cutover_report_declares_allowlist_free_contract(tmp_path: Path) -> None:
    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["gate_contract"]["gate_id"] == HARD_CUTOVER_GATE_ID
    assert report["gate_contract"]["closure_issues"] == ["8149", "8150"]
    assert (
        report["gate_contract"]["retired_allowlist_report_fields"]
        == list(RETIRED_ALLOWLIST_REPORT_FIELDS)
    )
    for retired_key in RETIRED_ALLOWLIST_REPORT_FIELDS:
        assert retired_key not in report
        assert retired_key not in report["stats"]


def test_hard_cutover_report_declares_source_owned_contracts(tmp_path: Path) -> None:
    write(
        tmp_path / "docs/support/capability_matrix.md",
        "Backward-compatible aliases remain available.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())
    owner_contract = report["owner_contract"]
    scan_config_contract = report["scan_config_contract"]

    assert owner_contract["scan_root_owner"]["owner_id"] == SOURCE_HYGIENE_SCAN_ROOT_OWNER
    assert scan_config_contract["contract_id"] == SOURCE_HYGIENE_SCAN_CONFIG_CONTRACT_ID
    assert scan_config_contract["path_scope_is_fail_closed"] is True
    assert scan_config_contract["compiled_patterns_are_case_insensitive"] is True
    assert scan_config_contract["scan_roots"] == report["scan_roots"]
    assert scan_config_contract["pattern_count"] == len(report["forbidden_patterns"])
    assert owner_contract["pattern_owner"]["owner_id"] == SOURCE_HYGIENE_PATTERN_OWNER
    assert (
        owner_contract["generated_report_owner"]["owner_id"]
        == SOURCE_HYGIENE_GENERATED_REPORT_OWNER
    )
    assert owner_contract["generated_report_owner"]["generated_boundary_policy"] == {
        "boundary_contract": GENERATED_TRUTH_BOUNDARY_CONTRACT_ID,
        "claim_policy": GENERATED_TRUTH_BOUNDARY_CLAIM_POLICY,
        "generated_output_is_claim_source": False,
    }
    assert (
        owner_contract["blocker_metadata"]["blocker_owner"]
        == SOURCE_HYGIENE_BLOCKER_METADATA_OWNER
    )
    assert report["active_findings"][0]["pattern_owner"] == SOURCE_HYGIENE_PATTERN_OWNER
    assert "pattern_owner_surface" in report["active_findings"][0]
    assert {
        boundary["owner_id"] for boundary in report["generated_truth_boundaries"]
    } == {SOURCE_HYGIENE_GENERATED_REPORT_OWNER}
    assert {
        boundary["contract_id"] for boundary in report["generated_truth_boundaries"]
    } == {GENERATED_TRUTH_BOUNDARY_CONTRACT_ID}
    assert {
        boundary["generated_output_is_claim_source"]
        for boundary in report["generated_truth_boundaries"]
    } == {False}
    assert {
        boundary["claim_policy"] for boundary in report["generated_truth_boundaries"]
    } == {GENERATED_TRUTH_BOUNDARY_CLAIM_POLICY}


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


def test_hard_cutover_gate_rejects_projected_retired_behavior_claims(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "docs/support/capability_matrix.md",
        "Future compatibility aliases will be supported after closeout.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "projected-retired-behavior-claim"
    assert report["active_findings"][0]["residue_class"] == "projected-behavior-claim"


def test_hard_cutover_gate_rejects_backward_compatible_alias_claims(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "docs/support/capability_matrix.md",
        "Backward-compatible aliases remain available.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "backward-compatible-alias-wording"
    assert report["active_findings"][0]["residue_class"] == "alias-residue"


def test_hard_cutover_gate_rejects_public_compatibility_shim_support_claims(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "docs/support/capability_matrix.md",
        "Compatibility shim support remains accepted.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert {
        finding["pattern_id"] for finding in report["active_findings"]
    } >= {"public-compatibility-shim-support-claim", "shim-wording"}


def test_hard_cutover_gate_rejects_public_migration_lane_support_claims(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "docs/support/capability_matrix.md",
        "Migration-lane support remains enabled.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "public-migration-lane-support-claim"
    assert report["active_findings"][0]["residue_class"] == "shim-fallback-language"


def test_hard_cutover_gate_rejects_legacy_compatibility_public_text(
    tmp_path: Path,
) -> None:
    write(
        tmp_path / "docs/support/capability_matrix.md",
        "Legacy compatibility text remains authoritative.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "legacy-compatibility-text"
    assert report["active_findings"][0]["residue_class"] == "legacy-compatibility-text"


def test_hard_cutover_report_matches_schema_shape(tmp_path: Path) -> None:
    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())
    json_path = tmp_path / "tmp/reports/source_hygiene/hard-cutover/report.json"
    text_path = tmp_path / "tmp/reports/source_hygiene/hard-cutover/report.txt"

    write_reports(report, json_path, text_path)
    payload = json.loads(json_path.read_text(encoding="utf-8"))
    text = text_path.read_text(encoding="utf-8")
    writer_contract = report_writer_contract_payload()

    assert payload["schema_version"] == "source-hygiene-hard-cutover-report-v1"
    assert isinstance(payload["forbidden_patterns"], list)
    assert isinstance(payload["active_findings"], list)
    assert payload["stats"]["active_finding_count"] == 0
    assert writer_contract["contract_id"] == REPORT_WRITER_CONTRACT_ID
    assert writer_contract["summary_fields"] == list(REPORT_SUMMARY_FIELDS)
    assert text.startswith("schema_version:")
    assert [line.split(":", 1)[0] for line in text.splitlines()[:5]] == list(
        REPORT_SUMMARY_FIELDS
    )


def test_hard_cutover_gate_cli_summary_declares_owner_contract(tmp_path: Path) -> None:
    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())
    contract = hard_cutover_gate_cli_contract_payload()
    summary = format_gate_summary(
        report=report,
        json_path=tmp_path / "tmp/reports/source_hygiene/hard-cutover/report.json",
        text_path=tmp_path / "tmp/reports/source_hygiene/hard-cutover/report.txt",
        root=tmp_path,
    )

    assert contract["contract_id"] == HARD_CUTOVER_GATE_CLI_CONTRACT_ID
    assert contract["summary_fields"] == list(HARD_CUTOVER_GATE_SUMMARY_FIELDS)
    assert [line.split(":", 1)[0] for line in summary] == list(
        HARD_CUTOVER_GATE_SUMMARY_FIELDS
    )
    assert summary[0] == (
        "source_hygiene_cli_contract: source-hygiene-hard-cutover-cli-v1"
    )
    assert summary[-1] == "generated_truth_boundary_findings: 0"


def test_hard_cutover_gate_excludes_canonical_config_registry(tmp_path: Path) -> None:
    write(
        tmp_path / "native/objc3c/src/config/objc3_language_profile.h",
        'const char *removed = "--objc3-compat-mode";\n',
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",))

    assert report["ok"] is True
    assert report["stats"]["active_finding_count"] == 0
