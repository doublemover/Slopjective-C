from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
ROUTING_SOURCE = (
    ROOT / "native" / "objc3c" / "src" / "driver" / "objc3_llvm_capability_routing.cpp"
)


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_driver_llvm_capability_routing_is_fail_closed_and_mode_pinned() -> None:
    source = _read(ROUTING_SOURCE)

    assert "objc3c-llvm-capabilities-v2" in source
    assert "capability routing fail-closed: --objc3-route-backend-from-capabilities requires --llvm-capabilities-summary" in source
    assert "capability routing fail-closed: sema/type-system parity capability unavailable:" in source
    assert "clang backend selected but capability summary reports clang unavailable" in source
    assert "llvm-direct backend selected but llc --filetype=obj capability is unavailable" in source
    assert "summary.llc_supports_filetype_obj ? Objc3IrObjectBackend::kLLVMDirect : Objc3IrObjectBackend::kClang"
    assert "options.clang_path = summary.clang_path;" in source
    assert "options.llc_path = summary.llc_path;" in source
