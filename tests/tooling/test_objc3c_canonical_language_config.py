from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CONFIG_HEADER = ROOT / "native" / "objc3c" / "src" / "config" / "objc3_language_profile.h"
LANGUAGE_VERSION_HEADER = ROOT / "native" / "objc3c" / "src" / "config" / "objc3_language_version.h"
FEATURE_STATE_HEADER = ROOT / "native" / "objc3c" / "src" / "config" / "objc3_feature_state_catalog.h"
REMOVED_OPTIONS_HEADER = ROOT / "native" / "objc3c" / "src" / "config" / "objc3_command_options.h"
LANGUAGE_PROFILE_SCHEMA = ROOT / "schemas" / "language" / "objc3-language-profile-v1.schema.json"
REMOVED_OPTION_CONTRACT_HEADER = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "config"
    / "objc3_removed_command_option_contract.h"
)
REMOVED_LANGUAGE_OPTIONS = ROOT / "native" / "objc3c" / "src" / "config" / "objc3_rejected_command_option_language_data.cpp"
REMOVED_REPORTING_OPTIONS = ROOT / "native" / "objc3c" / "src" / "config" / "objc3_rejected_command_option_reporting_data.cpp"
CLI_HEADER = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.h"
CLI_APPLICATION = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_option_application.cpp"
CLI_VALIDATION = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_option_validation.cpp"
FRONTEND_OPTIONS = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_frontend_options.cpp"
LEXER_HEADER = ROOT / "native" / "objc3c" / "src" / "lex" / "objc3_lexer.h"
PIPELINE_COMPILE_OPTIONS = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "pipeline"
    / "results"
    / "compile_options.h"
)
REMOVED_MODES = (
    ROOT
    / "native"
    / "objc3c"
    / "src"
    / "diagnostics"
    / "modes"
    / "canonical_rejections.cpp"
)
CONFIG_CMAKE_FILE = ROOT / "native" / "objc3c" / "src" / "config" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_canonical_language_config_surface_exists() -> None:
    profile = _read(CONFIG_HEADER)
    version = _read(LANGUAGE_VERSION_HEADER)
    feature_state = _read(FEATURE_STATE_HEADER)
    removed_options = _read(REMOVED_OPTIONS_HEADER)
    removed_option_contract = _read(REMOVED_OPTION_CONTRACT_HEADER)
    removed_language_options = _read(REMOVED_LANGUAGE_OPTIONS)
    removed_reporting_options = _read(REMOVED_REPORTING_OPTIONS)

    assert '#include "config/objc3_language_version.h"' in profile
    assert '#include "config/objc3_command_options.h"' in profile
    assert "inline constexpr std::uint8_t kCanonicalLanguageVersion = 3u;" in version
    assert 'inline constexpr const char *kCanonicalLanguageProfileName = "canonical";' in version
    assert "enum class FeatureState" in feature_state
    assert "CanonicalFeatureStates()" in feature_state
    assert "RemovedCommandOptions()" in removed_options
    assert "enum class RemovedCommandOptionOwner" in removed_options
    assert "RemovedCommandOptionOwnerName(" in removed_options
    assert "BuildRemovedCommandOptionValidationContractSummary()" in removed_option_contract
    assert "fail_closed_removed_option_table" in removed_option_contract
    assert ('"--objc3-' + 'com' + 'pat-mode"') in removed_language_options
    assert '"--objc3-migration-assist"' in removed_language_options
    assert '"--objc3-canonical-rejection-diagnostics"' in removed_reporting_options
    assert "IsCanonicalLanguageVersion(std::uint32_t version)" in version
    assert "UnsupportedLanguageVersionDiagnostic(std::uint32_t version)" in version
    assert '"schema_version": {' in _read(LANGUAGE_PROFILE_SCHEMA)
    assert '"const": "objc3-language-profile-v1"' in _read(LANGUAGE_PROFILE_SCHEMA)


def test_frontend_surfaces_use_canonical_config_defaults() -> None:
    assert '#include "config/objc3_language_profile.h"' in _read(CLI_HEADER)
    assert "objc3c::config::kCanonicalLanguageVersion" in _read(CLI_HEADER)
    assert "objc3c::config::IsCanonicalLanguageVersion(options.language_version)" in _read(CLI_VALIDATION)
    assert "objc3c::config::UnsupportedLanguageVersionDiagnostic(" in _read(CLI_VALIDATION)
    assert "BuildCanonicalModeRejectionDiagnostic(" in _read(CLI_APPLICATION)
    assert "objc3c::config::kCanonicalLanguageVersion" in _read(FRONTEND_OPTIONS)
    assert "objc3c::config::kCanonicalLanguageVersion" in _read(LEXER_HEADER)
    assert "objc3c::config::kCanonicalLanguageVersion" in _read(PIPELINE_COMPILE_OPTIONS)
    assert "objc3c::config::ValidateRemovedCommandOption(flag)" in _read(REMOVED_MODES)


def test_cmake_registers_config_target() -> None:
    cmake = _read(CONFIG_CMAKE_FILE)
    assert "add_library(objc3c_config STATIC" in cmake
    assert "objc3_language_profile_data.cpp" in cmake
    assert "objc3_removed_command_option_contract.cpp" in cmake
    assert "objc3_command_options.cpp" in cmake
    assert "objc3_rejected_command_option_language_data.cpp" in cmake
    assert "objc3c_config" in cmake
