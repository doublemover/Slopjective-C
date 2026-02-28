from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

CLI_OPTIONS_HEADER = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.h"
CLI_OPTIONS_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.cpp"
FRONTEND_OPTIONS_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_frontend_options.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
CLI_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "10-cli.md"
CLI_DOC_AGGREGATE = ROOT / "docs" / "objc3c-native.md"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def _assert_in_order(text: str, snippets: list[str]) -> None:
    cursor = -1
    for snippet in snippets:
        index = text.find(snippet)
        assert index != -1, f"missing snippet: {snippet}"
        assert index > cursor, f"snippet out of order: {snippet}"
        cursor = index


def test_m222_driver_cli_supports_language_version_and_migration_controls() -> None:
    header = _read(CLI_OPTIONS_HEADER)
    source = _read(CLI_OPTIONS_SOURCE)

    assert "enum class Objc3CompatMode" in header
    assert "std::uint32_t language_version = 3;" in header
    assert "Objc3CompatMode compat_mode = Objc3CompatMode::kCanonical;" in header
    assert "bool migration_assist = false;" in header

    assert "[-fobjc-version=<N>] [--objc3-language-version <N>]" in source
    assert "[--objc3-compat-mode <canonical|legacy>] [--objc3-migration-assist]" in source
    assert "if (flag.rfind(\"-fobjc-version=\", 0) == 0)" in source
    assert "flag == \"-fobjc-version\" || flag == \"--objc3-language-version\"" in source
    assert "flag == \"--objc3-compat-mode\"" in source
    assert "flag == \"--objc3-migration-assist\"" in source
    assert "unsupported Objective-C language version for native frontend (expected 3): " in source

    frontend_options_source = _read(FRONTEND_OPTIONS_SOURCE)
    assert "options.language_version = static_cast<std::uint8_t>(cli_options.language_version);" in frontend_options_source
    assert "options.compatibility_mode = cli_options.compat_mode == Objc3CompatMode::kLegacy" in frontend_options_source
    assert "? Objc3FrontendCompatibilityMode::kLegacy" in frontend_options_source
    assert ": Objc3FrontendCompatibilityMode::kCanonical;" in frontend_options_source
    assert "options.migration_assist = cli_options.migration_assist;" in frontend_options_source
    _assert_in_order(
        frontend_options_source,
        [
            "options.language_version = static_cast<std::uint8_t>(cli_options.language_version);",
            "options.compatibility_mode = cli_options.compat_mode == Objc3CompatMode::kLegacy",
            "? Objc3FrontendCompatibilityMode::kLegacy",
            ": Objc3FrontendCompatibilityMode::kCanonical;",
            "options.migration_assist = cli_options.migration_assist;",
            "options.lowering.max_message_send_args = cli_options.max_message_send_args;",
        ],
    )

    frontend_artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    assert "CompatibilityModeName(Objc3FrontendCompatibilityMode mode)" in frontend_artifacts_source
    assert "case Objc3FrontendCompatibilityMode::kLegacy:" in frontend_artifacts_source
    assert 'return "legacy";' in frontend_artifacts_source
    assert 'return "canonical";' in frontend_artifacts_source
    assert "\\\"compatibility_mode\\\":\\\"" in frontend_artifacts_source
    assert "\\\"migration_assist\\\":" in frontend_artifacts_source
    _assert_in_order(
        frontend_artifacts_source,
        [
            'manifest << "  \\"frontend\\": {\\n";',
            'manifest << "    \\"language_version\\":"',
            'manifest << "    \\"compatibility_mode\\":\\""',
            'manifest << "    \\"migration_assist\\":"',
            'manifest << "    \\"max_message_send_args\\":"',
        ],
    )


def test_m222_docs_publish_new_compatibility_controls() -> None:
    cli_fragment = _read(CLI_DOC_FRAGMENT)
    aggregate_doc = _read(CLI_DOC_AGGREGATE)

    for text in (
        "[-fobjc-version=<N>] [--objc3-language-version <N>]",
        "[--objc3-compat-mode <canonical|legacy>] [--objc3-migration-assist]",
        "Default language version: `3`",
        "Default `--objc3-compat-mode`: `canonical`",
        "Default `--objc3-migration-assist`: `off`",
    ):
        assert text in cli_fragment
        assert text in aggregate_doc
