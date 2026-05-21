from __future__ import annotations

import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
SCRIPTS_ROOT = ROOT / "scripts"
if str(SCRIPTS_ROOT) not in sys.path:
    sys.path.insert(0, str(SCRIPTS_ROOT))

from check_stdlib_surface_model import CanonicalModuleSurface, PackageImportSurface
from stdlib_surface.artifacts import (
    extract_stdlib_abi_signatures,
    validate_module_artifacts,
)


def test_stdlib_abi_signature_extraction_covers_exports_and_externs() -> None:
    source_text = """
module objc3_core;

extern fn objc3_runtime_stdlib_core_count_i32(count: i32) -> i32;
let Objc3CoreLanguageRevision = 1;

fn objc3_core_array_count(count: i32) {
  return objc3_runtime_stdlib_core_count_i32(count);
}
"""

    assert extract_stdlib_abi_signatures(source_text) == {
        "objc3_runtime_stdlib_core_count_i32": (
            "extern fn objc3_runtime_stdlib_core_count_i32(count: i32) -> i32"
        ),
        "Objc3CoreLanguageRevision": "let Objc3CoreLanguageRevision = 1",
        "objc3_core_array_count": "fn objc3_core_array_count(count: i32)",
    }


def test_stdlib_abi_signature_gate_rejects_public_export_drift(tmp_path: Path) -> None:
    module_root = tmp_path / "stdlib" / "modules" / "objc3.core"
    module_root.mkdir(parents=True)
    (module_root / "smoke.objc3").write_text("module smoke;\n", encoding="utf-8")
    (module_root / "module.objc3").write_text(
        "module objc3_core;\n"
        "fn objc3_core_array_count(count: i32, unexpected: i32) {\n"
        "  return count + unexpected;\n"
        "}\n",
        encoding="utf-8",
    )
    (module_root / "module.json").write_text(
        """
{
  "contract_id": "objc3c.stdlib.module.surface.v1",
  "canonical_module": "objc3.core",
  "implementation_module": "objc3_core",
  "capability_id": "objc3.cap.core",
  "module_semver": {"major": 1, "minor": 0, "patch": 0},
  "workspace_root": "stdlib/modules/objc3.core",
  "source": "stdlib/modules/objc3.core/module.objc3",
  "smoke_source": "stdlib/modules/objc3.core/smoke.objc3",
  "api_families": ["array-slice"],
  "exports": ["objc3_core_array_count"],
  "abi_signatures": {
    "objc3_core_array_count": "fn objc3_core_array_count(count: i32)"
  }
}
""".strip(),
        encoding="utf-8",
    )

    error = validate_module_artifacts(
        root=tmp_path,
        module_surfaces=[
            CanonicalModuleSurface(
                module="objc3.core",
                implementation_module="objc3_core",
                capability_id="objc3.cap.core",
                required_profile="Core and above",
                workspace_root="stdlib/modules/objc3.core",
                source="stdlib/modules/objc3.core/module.objc3",
                smoke_source="stdlib/modules/objc3.core/smoke.objc3",
                manifest="stdlib/modules/objc3.core/module.json",
            )
        ],
        package_imports_by_module={
            "objc3.core": PackageImportSurface(
                canonical_module="objc3.core",
                implementation_module="objc3_core",
                source_declaration="module objc3_core;",
            )
        },
        semantic_policy={
            "module_semver": {
                "objc3.core": {"major": 1, "minor": 0, "patch": 0}
            }
        },
    )

    assert error is not None
    assert "module source ABI signature drifted for objc3.core.objc3_core_array_count" in error


def test_stdlib_abi_signature_gate_rejects_runtime_extern_drift(tmp_path: Path) -> None:
    module_root = tmp_path / "stdlib" / "modules" / "objc3.core"
    module_root.mkdir(parents=True)
    (module_root / "smoke.objc3").write_text("module smoke;\n", encoding="utf-8")
    (module_root / "module.objc3").write_text(
        "module objc3_core;\n"
        "extern fn objc3_runtime_stdlib_core_count_i32(count: i32, extra: i32) -> i32;\n"
        "fn objc3_core_array_count(count: i32) {\n"
        "  return objc3_runtime_stdlib_core_count_i32(count, 0);\n"
        "}\n",
        encoding="utf-8",
    )
    (module_root / "module.json").write_text(
        """
{
  "contract_id": "objc3c.stdlib.module.surface.v1",
  "canonical_module": "objc3.core",
  "implementation_module": "objc3_core",
  "capability_id": "objc3.cap.core",
  "module_semver": {"major": 1, "minor": 0, "patch": 0},
  "workspace_root": "stdlib/modules/objc3.core",
  "source": "stdlib/modules/objc3.core/module.objc3",
  "smoke_source": "stdlib/modules/objc3.core/smoke.objc3",
  "api_families": ["array-slice"],
  "runtime_abi": ["objc3_runtime_stdlib_core_count_i32"],
  "runtime_abi_signatures": {
    "objc3_runtime_stdlib_core_count_i32": "extern fn objc3_runtime_stdlib_core_count_i32(count: i32) -> i32"
  },
  "exports": ["objc3_core_array_count"],
  "abi_signatures": {
    "objc3_core_array_count": "fn objc3_core_array_count(count: i32)"
  }
}
""".strip(),
        encoding="utf-8",
    )

    error = validate_module_artifacts(
        root=tmp_path,
        module_surfaces=[
            CanonicalModuleSurface(
                module="objc3.core",
                implementation_module="objc3_core",
                capability_id="objc3.cap.core",
                required_profile="Core and above",
                workspace_root="stdlib/modules/objc3.core",
                source="stdlib/modules/objc3.core/module.objc3",
                smoke_source="stdlib/modules/objc3.core/smoke.objc3",
                manifest="stdlib/modules/objc3.core/module.json",
            )
        ],
        package_imports_by_module={
            "objc3.core": PackageImportSurface(
                canonical_module="objc3.core",
                implementation_module="objc3_core",
                source_declaration="module objc3_core;",
            )
        },
        semantic_policy={
            "module_semver": {
                "objc3.core": {"major": 1, "minor": 0, "patch": 0}
            }
        },
    )

    assert error is not None
    assert (
        "module source ABI signature drifted for "
        "objc3.core.objc3_runtime_stdlib_core_count_i32"
    ) in error


def _text_module_surface() -> CanonicalModuleSurface:
    return CanonicalModuleSurface(
        module="objc3.text",
        implementation_module="objc3_text",
        capability_id="objc3.cap.text",
        required_profile="Core and above",
        workspace_root="stdlib/modules/objc3.text",
        source="stdlib/modules/objc3.text/module.objc3",
        smoke_source="stdlib/modules/objc3.text/smoke.objc3",
        manifest="stdlib/modules/objc3.text/module.json",
    )


def _write_text_runtime_module(tmp_path: Path) -> None:
    module_root = tmp_path / "stdlib" / "modules" / "objc3.text"
    module_root.mkdir(parents=True)
    (module_root / "smoke.objc3").write_text("module smoke;\n", encoding="utf-8")
    (module_root / "module.objc3").write_text(
        "module objc3_text;\n"
        "extern fn objc3_runtime_stdlib_text_byte_count_i32(handle: i32) -> i32;\n"
        "fn objc3_text_byte_count(handle: i32) {\n"
        "  return objc3_runtime_stdlib_text_byte_count_i32(handle);\n"
        "}\n",
        encoding="utf-8",
    )
    (module_root / "module.json").write_text(
        """
{
  "contract_id": "objc3c.stdlib.module.surface.v1",
  "canonical_module": "objc3.text",
  "implementation_module": "objc3_text",
  "capability_id": "objc3.cap.text",
  "module_semver": {"major": 1, "minor": 0, "patch": 0},
  "workspace_root": "stdlib/modules/objc3.text",
  "source": "stdlib/modules/objc3.text/module.objc3",
  "smoke_source": "stdlib/modules/objc3.text/smoke.objc3",
  "api_families": ["text-length"],
  "runtime_abi": ["objc3_runtime_stdlib_text_byte_count_i32"],
  "runtime_abi_signatures": {
    "objc3_runtime_stdlib_text_byte_count_i32": "extern fn objc3_runtime_stdlib_text_byte_count_i32(handle: i32) -> i32"
  },
  "exports": ["objc3_text_byte_count"],
  "abi_signatures": {
    "objc3_text_byte_count": "fn objc3_text_byte_count(handle: i32)"
  }
}
""".strip(),
        encoding="utf-8",
    )


def _validate_text_module(tmp_path: Path) -> str | None:
    return validate_module_artifacts(
        root=tmp_path,
        module_surfaces=[_text_module_surface()],
        package_imports_by_module={
            "objc3.text": PackageImportSurface(
                canonical_module="objc3.text",
                implementation_module="objc3_text",
                source_declaration="module objc3_text;",
            )
        },
        semantic_policy={
            "module_semver": {
                "objc3.text": {"major": 1, "minor": 0, "patch": 0}
            }
        },
    )


def _write_public_text_contract(tmp_path: Path, declaration: str) -> None:
    src_root = tmp_path / "native" / "objc3c" / "src"
    runtime_stdlib_root = src_root / "runtime" / "stdlib"
    runtime_public_root = src_root / "runtime" / "public"
    runtime_stdlib_root.mkdir(parents=True)
    runtime_public_root.mkdir(parents=True)
    (runtime_stdlib_root / "text_runtime_contract.h").write_text(
        "#pragma once\n"
        "#ifdef __cplusplus\n"
        'extern "C" {\n'
        "#endif\n"
        f"{declaration}\n"
        "#ifdef __cplusplus\n"
        "}\n"
        "#endif\n",
        encoding="utf-8",
    )
    (runtime_public_root / "objc3_runtime_api.h").write_text(
        '#include "runtime/stdlib/text_runtime_contract.h"\n',
        encoding="utf-8",
    )


def test_stdlib_runtime_abi_gate_accepts_matching_public_contract_header(
    tmp_path: Path,
) -> None:
    _write_text_runtime_module(tmp_path)
    _write_public_text_contract(
        tmp_path,
        "int objc3_runtime_stdlib_text_byte_count_i32(int handle);",
    )

    assert _validate_text_module(tmp_path) is None


def test_stdlib_runtime_abi_gate_rejects_contract_header_signature_drift(
    tmp_path: Path,
) -> None:
    _write_text_runtime_module(tmp_path)
    _write_public_text_contract(
        tmp_path,
        "int objc3_runtime_stdlib_text_byte_count_i32(int handle, int extra);",
    )

    error = _validate_text_module(tmp_path)

    assert error is not None
    assert (
        "module runtime contract header signature drifted for "
        "objc3.text.objc3_runtime_stdlib_text_byte_count_i32"
    ) in error
