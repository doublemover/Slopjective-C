from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
C_API_HEADER = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "c_api.h"
C_API_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "c_api.cpp"
FRONTEND_ANCHOR_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
C_API_RUNNER_SOURCE = ROOT / "native" / "objc3c" / "src" / "tools" / "objc3c_frontend_c_api_runner.cpp"
FRONTEND_PIPELINE_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"
RUNTIME_SHIM_SOURCE = ROOT / "tests" / "tooling" / "runtime" / "objc3_msgsend_i32_shim.c"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m197_lowering_cpp_interop_shim_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M197 lowering/runtime C++ interop shim strategy",
        "tmp/artifacts/compilation/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/",
        "tmp/reports/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/",
        "tmp/artifacts/compilation/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/module.manifest.json",
        "tmp/artifacts/compilation/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/module.diagnostics.json",
        "tmp/reports/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m197/lowering-runtime-cpp-interop-shim-strategy/cpp-interop-shim-source-anchors.txt",
        "Optional C ABI shim for non-C++ embedding environments.",
        "OBJC3C_FRONTEND_C_API_ABI_VERSION == 1u",
        "Objc3LoweringIRBoundaryReplayKey(...)",
        "runtime_dispatch_symbol=",
        "kRuntimeDispatchDefaultSymbol = \"objc3_msgsend_i32\";",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "python -m pytest tests/tooling/test_objc3c_m197_lowering_cpp_interop_shim_contract.py -q",
    ):
        assert text in fragment


def test_m197_lowering_cpp_interop_shim_source_anchor_mapping() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    c_api_header = _read(C_API_HEADER)
    c_api_source = _read(C_API_SOURCE)
    frontend_anchor_source = _read(FRONTEND_ANCHOR_SOURCE)
    c_api_runner_source = _read(C_API_RUNNER_SOURCE)
    frontend_pipeline_contract_source = _read(FRONTEND_PIPELINE_CONTRACT_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    ir_emitter_source = _read(IR_EMITTER_SOURCE)
    frontend_artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)
    runtime_shim_source = _read(RUNTIME_SHIM_SOURCE)

    mapped_source_anchors = (
        (
            "Optional C ABI shim for non-C++ embedding environments.",
            c_api_header,
            "* Optional C ABI shim for non-C++ embedding environments.",
        ),
        (
            "OBJC3C_FRONTEND_C_API_ABI_VERSION == 1u",
            c_api_source,
            "OBJC3C_FRONTEND_C_API_ABI_VERSION == 1u",
        ),
        (
            "return objc3c_frontend_compile_file(context, options, result);",
            c_api_source,
            "return objc3c_frontend_compile_file(context, options, result);",
        ),
        (
            "frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;",
            frontend_anchor_source,
            "frontend_options.lowering.runtime_dispatch_symbol = options.runtime_dispatch_symbol;",
        ),
        (
            "compile_options.runtime_dispatch_symbol = runtime_symbol;",
            c_api_runner_source,
            "compile_options.runtime_dispatch_symbol = runtime_symbol;",
        ),
        (
            "kRuntimeDispatchDefaultSymbol = \"objc3_msgsend_i32\";",
            frontend_pipeline_contract_source,
            "kRuntimeDispatchDefaultSymbol = \"objc3_msgsend_i32\";",
        ),
        (
            "Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary)",
            lowering_contract_source,
            "Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary)",
        ),
        (
            "return \"runtime_dispatch_symbol=\" + boundary.runtime_dispatch_symbol +",
            lowering_contract_source,
            "return \"runtime_dispatch_symbol=\" + boundary.runtime_dispatch_symbol +",
        ),
        (
            'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";',
            ir_emitter_source,
            'out << "declare i32 @" << lowering_ir_boundary_.runtime_dispatch_symbol << "(i32, ptr";',
        ),
        (
            'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
            frontend_artifacts_source,
            'manifest << "  \\"lowering\\": {\\"runtime_dispatch_symbol\\":\\"" << options.lowering.runtime_dispatch_symbol',
        ),
        (
            "int objc3_msgsend_i32(int receiver, const char *selector, int a0, int a1, int a2, int a3) {",
            runtime_shim_source,
            "int objc3_msgsend_i32(int receiver, const char *selector, int a0, int a1, int a2, int a3) {",
        ),
        (
            "static const int64_t kModulus = 2147483629LL;",
            runtime_shim_source,
            "static const int64_t kModulus = 2147483629LL;",
        ),
    )

    for doc_anchor, source_text, source_anchor in mapped_source_anchors:
        assert doc_anchor in fragment
        assert source_anchor in source_text
