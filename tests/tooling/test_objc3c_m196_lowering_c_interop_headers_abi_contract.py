from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]

ARTIFACTS_DOC_FRAGMENT = ROOT / "docs" / "objc3c-native" / "src" / "50-artifacts.md"
C_API_HEADER = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "c_api.h"
C_API_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "c_api.cpp"
FRONTEND_API_HEADER = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "api.h"
FRONTEND_VERSION_HEADER = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "version.h"
FRONTEND_ANCHOR_SOURCE = ROOT / "native" / "objc3c" / "src" / "libobjc3c_frontend" / "frontend_anchor.cpp"
C_API_RUNNER_SOURCE = ROOT / "native" / "objc3c" / "src" / "tools" / "objc3c_frontend_c_api_runner.cpp"
FRONTEND_PIPELINE_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "frontend_pipeline_contract.h"
LOWERING_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "lower" / "objc3_lowering_contract.cpp"
IR_EMITTER_SOURCE = ROOT / "native" / "objc3c" / "src" / "ir" / "objc3_ir_emitter.cpp"
FRONTEND_ARTIFACTS_SOURCE = ROOT / "native" / "objc3c" / "src" / "pipeline" / "objc3_frontend_artifacts.cpp"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_m196_lowering_runtime_c_interop_headers_abi_alignment_section_present() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    for text in (
        "## M196 lowering/runtime C interop headers and ABI alignment",
        "tmp/artifacts/compilation/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/",
        "tmp/reports/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/",
        "tmp/artifacts/compilation/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/module.ll",
        "tmp/artifacts/compilation/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/module.manifest.json",
        "tmp/artifacts/compilation/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/module.diagnostics.json",
        "tmp/reports/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/abi-ir-anchors.txt",
        "tmp/reports/objc3c-native/m196/lowering-runtime-c-interop-headers-abi-alignment/c-interop-header-abi-source-anchors.txt",
        "C interop headers + ABI alignment architecture/isolation anchors",
        "Optional C ABI shim for non-C++ embedding environments.",
        "#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u",
        "Public embedding ABI contract:",
        "#define OBJC3C_FRONTEND_ABI_VERSION 1u",
        "kRuntimeDispatchDefaultSymbol = \"objc3_msgsend_i32\";",
        "Objc3LoweringIRBoundaryReplayKey(...)",
        '"lowering":{"runtime_dispatch_symbol":"<symbol>","runtime_dispatch_arg_slots":<N>,"selector_global_ordering":"lexicographic"}',
        "python -m pytest tests/tooling/test_objc3c_m196_lowering_c_interop_headers_abi_contract.py -q",
    ):
        assert text in fragment


def test_m196_lowering_runtime_c_interop_headers_abi_alignment_source_anchor_mapping() -> None:
    fragment = _read(ARTIFACTS_DOC_FRAGMENT)
    c_api_header = _read(C_API_HEADER)
    c_api_source = _read(C_API_SOURCE)
    frontend_api_header = _read(FRONTEND_API_HEADER)
    frontend_version_header = _read(FRONTEND_VERSION_HEADER)
    frontend_anchor_source = _read(FRONTEND_ANCHOR_SOURCE)
    c_api_runner_source = _read(C_API_RUNNER_SOURCE)
    frontend_pipeline_contract_source = _read(FRONTEND_PIPELINE_CONTRACT_SOURCE)
    lowering_contract_source = _read(LOWERING_CONTRACT_SOURCE)
    ir_emitter_source = _read(IR_EMITTER_SOURCE)
    frontend_artifacts_source = _read(FRONTEND_ARTIFACTS_SOURCE)

    mapped_source_anchors = (
        (
            "Optional C ABI shim for non-C++ embedding environments.",
            c_api_header,
            "* Optional C ABI shim for non-C++ embedding environments.",
        ),
        (
            "#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u",
            c_api_header,
            "#define OBJC3C_FRONTEND_C_API_ABI_VERSION 1u",
        ),
        (
            'static_assert(OBJC3C_FRONTEND_C_API_ABI_VERSION == 1u, "unexpected c api wrapper abi version");',
            c_api_source,
            'static_assert(OBJC3C_FRONTEND_C_API_ABI_VERSION == 1u, "unexpected c api wrapper abi version");',
        ),
        (
            "static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,",
            c_api_source,
            "static_assert(std::is_same_v<objc3c_frontend_c_compile_options_t, objc3c_frontend_compile_options_t>,",
        ),
        (
            "return objc3c_frontend_is_abi_compatible(requested_abi_version);",
            c_api_source,
            "return objc3c_frontend_is_abi_compatible(requested_abi_version);",
        ),
        (
            "Public embedding ABI contract:",
            frontend_api_header,
            "* Public embedding ABI contract:",
        ),
        (
            "Reserved struct fields are for forward ABI growth and should be zero-initialized by callers.",
            frontend_api_header,
            "* - Reserved struct fields are for forward ABI growth and should be zero-initialized by callers.",
        ),
        (
            "ABI evolution policy for exposed structs/enums is additive; existing fields and values remain stable.",
            frontend_api_header,
            "* - ABI evolution policy for exposed structs/enums is additive; existing fields and values remain stable.",
        ),
        (
            "#define OBJC3C_FRONTEND_ABI_VERSION 1u",
            frontend_version_header,
            "#define OBJC3C_FRONTEND_ABI_VERSION 1u",
        ),
        (
            "#define OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION OBJC3C_FRONTEND_ABI_VERSION",
            frontend_version_header,
            "#define OBJC3C_FRONTEND_MAX_COMPATIBILITY_ABI_VERSION OBJC3C_FRONTEND_ABI_VERSION",
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
            'kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32";',
            frontend_pipeline_contract_source,
            'kRuntimeDispatchDefaultSymbol = "objc3_msgsend_i32";',
        ),
        (
            "Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary)",
            lowering_contract_source,
            "Objc3LoweringIRBoundaryReplayKey(const Objc3LoweringIRBoundary &boundary)",
        ),
        (
            'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +',
            lowering_contract_source,
            'return "runtime_dispatch_symbol=" + boundary.runtime_dispatch_symbol +',
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
    )

    for doc_anchor, source_text, source_anchor in mapped_source_anchors:
        assert doc_anchor in fragment
        assert source_anchor in source_text
