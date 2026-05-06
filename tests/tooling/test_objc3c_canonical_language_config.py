from pathlib import Path


ROOT = Path(__file__).resolve().parents[2]
CONFIG_HEADER = ROOT / "native" / "objc3c" / "src" / "config" / "objc3_language_profile.h"
CLI_HEADER = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.h"
CLI_SOURCE = ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_cli_options.cpp"
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
    / "objc3_removed_mode_options.cpp"
)
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_canonical_language_config_surface_exists() -> None:
    config = _read(CONFIG_HEADER)
    assert "inline constexpr std::uint8_t kCanonicalLanguageVersion = 3u;" in config
    assert 'inline constexpr const char *kCanonicalLanguageProfileName = "canonical";' in config
    assert "enum class FeatureState" in config
    assert "kCanonicalFeatureStates" in config
    assert "kRemovedCommandOptions" in config
    assert '"--objc3-compat-mode"' in config
    assert '"--objc3-migration-assist"' in config
    assert '"--objc3-canonical-rejection-diagnostics"' in config
    assert "IsCanonicalLanguageVersion(std::uint32_t version)" in config
    assert "UnsupportedLanguageVersionDiagnostic(std::uint32_t version)" in config


def test_frontend_surfaces_use_canonical_config_defaults() -> None:
    assert '#include "config/objc3_language_profile.h"' in _read(CLI_HEADER)
    assert "objc3c::config::kCanonicalLanguageVersion" in _read(CLI_HEADER)
    assert "objc3c::config::IsCanonicalLanguageVersion(options.language_version)" in _read(CLI_SOURCE)
    assert "objc3c::config::UnsupportedLanguageVersionDiagnostic(" in _read(CLI_SOURCE)
    assert "objc3c::config::kCanonicalLanguageVersion" in _read(FRONTEND_OPTIONS)
    assert "objc3c::config::kCanonicalLanguageVersion" in _read(LEXER_HEADER)
    assert "objc3c::config::kCanonicalLanguageVersion" in _read(PIPELINE_COMPILE_OPTIONS)
    assert "objc3c::config::FindRemovedCommandOption(flag)" in _read(REMOVED_MODES)


def test_cmake_registers_config_target() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_config INTERFACE)" in cmake
    assert "target_link_libraries(objc3c_config INTERFACE" in cmake
    assert "objc3c_config" in cmake
