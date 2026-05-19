from __future__ import annotations

from pathlib import Path

from hard_cutover_gate_support import (
    active_path_patterns,
    active_pattern_ids,
    write_fixture,
)
from scripts.source_hygiene.scanner import build_report
from scripts.source_hygiene.violations import SOURCE_HYGIENE_VIOLATION_CONTRACT_ID


def assert_active_forbidden_pattern_fails(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "int ComputeDispatchResult = 0;\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["stats"]["active_finding_count"] == 1
    assert report["active_findings"][0]["pattern_id"] == "runtime-dispatch-pseudo-success"
    assert (
        report["active_findings"][0]["violation_contract"]
        == SOURCE_HYGIENE_VIOLATION_CONTRACT_ID
    )


def assert_dotted_runtime_shim_tokens_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "const char *contract = \"objc3c.runtime.shim.host.link.v1\";\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "runtime-shim-token"


def assert_retired_msgsend_symbol_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "call i32 @objc3_msgsend_i32(i32 %receiver, ptr %selector)\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "retired-msgsend-compatibility-dispatch"
    )


def assert_unresolved_dispatch_pseudo_success_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "docs/objc3c-native/src/10-cli.md",
        "Unresolved runtime dispatch pseudo-success remains documented.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "unresolved-dispatch-pseudo-success"
    )


def assert_compatibility_dispatch_symbol_fields_rejected(tmp_path: Path) -> None:
    write_fixture(
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
    assert active_pattern_ids(report) == [
        "retired-msgsend-compatibility-dispatch",
        "retired-msgsend-compatibility-dispatch",
    ]


def assert_compatibility_dispatch_wording_in_required_roots_rejected(
    tmp_path: Path,
) -> None:
    write_fixture(
        tmp_path / "docs/objc3c-native/src/10-cli.md",
        "Compatibility dispatch symbol remains documented.\n",
    )
    write_fixture(
        tmp_path / "spec/LOWERING_AND_RUNTIME_CONTRACTS.md",
        "`objc3_msgsend_i32` remains exported.\n",
    )
    write_fixture(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "const char *field = \"compatibility_runtime_dispatch_symbol\";\n",
    )
    write_fixture(
        tmp_path / "tests/tooling/test_runtime_dispatch_contract.py",
        'TOKEN = "objc3_msgsend_i32"\n',
    )

    report = build_report(root=tmp_path)

    assert report["ok"] is False
    assert report["stats"]["active_finding_count"] == 4
    assert active_path_patterns(report) == {
        "docs/objc3c-native/src/10-cli.md": "retired-msgsend-compatibility-dispatch",
        "spec/LOWERING_AND_RUNTIME_CONTRACTS.md": (
            "retired-msgsend-compatibility-dispatch"
        ),
        "native/objc3c/src/runtime/dispatch.cpp": (
            "retired-msgsend-compatibility-dispatch"
        ),
        "tests/tooling/test_runtime_dispatch_contract.py": (
            "retired-msgsend-compatibility-dispatch"
        ),
    }


def assert_runtime_docs_spec_and_test_surfaces_covered(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "docs/objc3c-native/src/50-artifacts.md",
        "Runtime dispatch keeps unresolved call pseudo-success metadata.\n",
    )
    write_fixture(
        tmp_path / "spec/LOWERING_AND_RUNTIME_CONTRACTS.md",
        "The runtime dispatch lane is fallback-only for unresolved calls.\n",
    )
    write_fixture(
        tmp_path / "tests/tooling/test_runtime_dispatch_contract.py",
        'CLAIM = "deterministic arithmetic formula selects dispatch"\n',
    )

    report = build_report(root=tmp_path)

    assert report["ok"] is False
    assert active_path_patterns(report) == {
        "docs/objc3c-native/src/50-artifacts.md": (
            "unresolved-dispatch-pseudo-success"
        ),
        "spec/LOWERING_AND_RUNTIME_CONTRACTS.md": "fallback-only-wording",
        "tests/tooling/test_runtime_dispatch_contract.py": (
            "deterministic-runtime-arithmetic"
        ),
    }


def assert_negative_absence_assertions_allowed(tmp_path: Path) -> None:
    write_fixture(
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


def assert_source_hygiene_violation_fixtures_excluded(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "tests/tooling/source_hygiene/fixtures/retired_msgsend_violation.txt",
        "objc3_msgsend_i32 compatibility_runtime_dispatch_symbol\n",
    )

    report = build_report(root=tmp_path, scan_roots=("tests",))

    assert report["ok"] is True
    assert report["stats"]["active_finding_count"] == 0


def assert_retired_tmp_allowlist_ignored(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "native/objc3c/src/driver/options.cpp",
        "const char *flag = \"--objc3-compat-mode\";\n",
    )
    write_fixture(
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


def assert_legacy_language_profile_enum_values_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "native/objc3c/src/pipeline/options.h",
        "enum class Objc3FrontendLanguageProfile { kCanonical = 0u, kLegacy = 1u };\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "legacy-language-profile-enum"


def assert_plain_compatibility_mode_wording_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "native/objc3c/src/driver/options.cpp",
        "const char *claim = \"compatibility mode is supported\";\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "public-compatibility-mode"


def assert_implementation_compatibility_bridge_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "int compatibility_bridge = 1;\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "compatibility-wrapper-or-bridge-surface"
    )


def assert_implementation_fallback_handler_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "int fallback_handler = 1;\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "fallback-implementation-surface"


def assert_implementation_migration_lane_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "int migration_lane = 1;\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "migration-implementation-surface"
    )


def assert_implementation_legacy_support_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "native/objc3c/src/runtime/dispatch.cpp",
        "int legacy_support = 1;\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "legacy-compatibility-support-surface"
    )


def assert_legacy_literal_diagnostics_switch_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "native/objc3c/src/pipeline/options.h",
        "bool legacy_literal_diagnostics = false;\n",
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "canonical-rejection-diagnostics-surface"
    )


def assert_fallback_behavior_wording_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "docs/runbooks/runtime.md",
        "Allowed fallback behaviors:\n",
    )

    report = build_report(root=tmp_path, scan_roots=("docs",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "deterministic-fallback-wording"
    )


def assert_fallback_only_wording_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "spec/LOWERING_AND_RUNTIME_CONTRACTS.md",
        "The runtime dispatch path is fallback-only.\n",
    )

    report = build_report(root=tmp_path, scan_roots=("spec",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == "fallback-only-wording"


def assert_deterministic_arithmetic_wording_rejected(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "tests/tooling/test_runtime_dispatch_contract.py",
        'CLAIM = "deterministic arithmetic runtime dispatch formula"\n',
    )

    report = build_report(root=tmp_path, scan_roots=("tests",), excludes=())

    assert report["ok"] is False
    assert report["active_findings"][0]["pattern_id"] == (
        "deterministic-runtime-arithmetic"
    )


def assert_canonical_config_registry_excluded(tmp_path: Path) -> None:
    write_fixture(
        tmp_path / "native/objc3c/src/config/objc3_language_profile.h",
        'const char *removed = "--objc3-compat-mode";\n',
    )

    report = build_report(root=tmp_path, scan_roots=("native/objc3c",))

    assert report["ok"] is True
    assert report["stats"]["active_finding_count"] == 0
