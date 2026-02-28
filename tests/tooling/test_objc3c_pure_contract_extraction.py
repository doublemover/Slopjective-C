from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SEMA_PASSES = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_semantic_passes.cpp"
PURE_CONTRACT_SOURCE = ROOT / "native" / "objc3c" / "src" / "sema" / "objc3_pure_contract.cpp"
CMAKE_FILE = ROOT / "native" / "objc3c" / "CMakeLists.txt"


def _read(path: Path) -> str:
    return path.read_text(encoding="utf-8")


def test_pure_contract_module_exists_and_sema_passes_no_longer_owns_it() -> None:
    assert PURE_CONTRACT_SOURCE.exists()

    sema_passes = _read(SEMA_PASSES)
    assert "void ValidatePureContractSemanticDiagnostics(" not in sema_passes

    pure_contract = _read(PURE_CONTRACT_SOURCE)
    assert "void ValidatePureContractSemanticDiagnostics(" in pure_contract


def test_cmake_registers_pure_contract_source() -> None:
    cmake = _read(CMAKE_FILE)
    assert "add_library(objc3c_sema STATIC" in cmake
    assert "src/sema/objc3_pure_contract.cpp" in cmake
