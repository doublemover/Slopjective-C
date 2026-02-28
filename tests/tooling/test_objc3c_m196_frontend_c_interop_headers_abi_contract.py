from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

GRAMMAR_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "20-grammar.md"
C_API_HEADER = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "c_api.h"
C_API_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "c_api.cpp"
FRONTEND_ANCHOR_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
PIPELINE_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_contract.h"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m196_frontend_c_interop_headers_abi_section_exists() -> None:
    fragment = _read(GRAMMAR_DOC_FRAGMENT)
    for text in (
        "## M196 frontend C interop headers and ABI alignment",
        "#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u",
        "typedef objc3c_frontend_compile_options_t objc3c_frontend_c_compile_options_t;",
        "typedef objc3c_frontend_compile_result_t objc3c_frontend_c_compile_result_t;",
        "static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,",
        "BuildFrontendOptions(const objc3c_frontend_compile_options_t &options)",
        "frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;",
        "kRuntimeDispatchDefaultArgs = 4",
        "kRuntimeDispatchMaxArgs = 16",
        "kRuntimeDispatchDefaultSymbol = \"objc3_msgsend_i32\"",
        "python -m pytest tests/tooling/test_objc3c_m196_frontend_c_interop_headers_abi_contract.py -q",
    ):
        assert text in fragment


def test_m196_frontend_c_interop_headers_abi_markers_map_to_sources() -> None:
    c_api_header = _read(C_API_HEADER)
    c_api_source = _read(C_API_SOURCE)
    frontend_anchor_source = _read(FRONTEND_ANCHOR_SOURCE)
    pipeline_contract_source = _read(PIPELINE_CONTRACT_SOURCE)

    for marker in (
        "#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u",
        "typedef objc3c_frontend_compile_options_t objc3c_frontend_c_compile_options_t;",
        "typedef objc3c_frontend_compile_result_t objc3c_frontend_c_compile_result_t;",
    ):
        assert marker in c_api_header

    for marker in (
        "static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,",
        "static_assert(std::is_same_v<objc3c_frontend_c_compile_result_t, objc3c_frontend_compile_result_t>,",
        "static_assert(std::is_same_v<objc3c_frontend_c_context_t, objc3c_frontend_context_t>,",
    ):
        assert marker in c_api_source

    for marker in (
        "Objc3FrontendOptions BuildFrontendOptions(const objc3c_frontend_compile_options_t &options) {",
        "frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;",
    ):
        assert marker in frontend_anchor_source

    for marker in (
        "inline constexpr std::size_t kRuntimeDispatchDefaultArgs = 4;",
        "inline constexpr std::size_t kRuntimeDispatchMaxArgs = 16;",
        "inline constexpr const char *kRuntimeDispatchDefaultSymbol = \"objc3_msgsend_i32\";",
    ):
        assert marker in pipeline_contract_source
