from __future__ import annotations

from pathlib import Path
import re
from typing import Any

from objc3c_tooling.json_io import load_json_any as load_json

from check_stdlib_surface_model import CanonicalModuleSurface, PackageImportSurface


_DECLARATION_NAME_RE = re.compile(
    r"^\s*(?:extern\s+fn|async\s+fn|fn|let)\s+(?P<name>[A-Za-z_][A-Za-z0-9_]*)"
)
_SOURCE_EXTERN_RE = re.compile(
    r"^extern fn (?P<name>[A-Za-z_][A-Za-z0-9_]*)\((?P<params>.*)\) -> i32$"
)
_C_RUNTIME_ABI_RE = re.compile(
    r"\bint\s+(?P<name>objc3_runtime_[A-Za-z0-9_]+)\s*\((?P<params>[^)]*)\)\s*;"
)
_STDLIB_RUNTIME_PREFIX_RE = re.compile(
    r"^objc3_runtime_stdlib_(?P<family>[a-z0-9]+)_"
)


def _normalize_signature_line(line: str) -> str | None:
    stripped = line.strip()
    if not stripped:
        return None
    match = _DECLARATION_NAME_RE.match(stripped)
    if match is None:
        return None
    if "{" in stripped:
        stripped = stripped.split("{", 1)[0].strip()
    if stripped.endswith(";"):
        stripped = stripped[:-1].strip()
    return " ".join(stripped.split())


def extract_stdlib_abi_signatures(source_text: str) -> dict[str, str]:
    signatures: dict[str, str] = {}
    for line in source_text.splitlines():
        signature = _normalize_signature_line(line)
        if signature is None:
            continue
        match = _DECLARATION_NAME_RE.match(signature)
        if match is None:
            continue
        signatures[match.group("name")] = signature
    return signatures


def _source_extern_signature_to_c(signature: str) -> str | None:
    match = _SOURCE_EXTERN_RE.match(signature)
    if match is None:
        return None
    raw_params = match.group("params").strip()
    if not raw_params:
        return f"int {match.group('name')}(void)"
    c_params: list[str] = []
    for raw_param in raw_params.split(","):
        pieces = raw_param.strip().split(":")
        if len(pieces) != 2:
            return None
        param_name = pieces[0].strip()
        param_type = pieces[1].strip()
        if not param_name or param_type != "i32":
            return None
        c_params.append(f"int {param_name}")
    return f"int {match.group('name')}({', '.join(c_params)})"


def _extract_runtime_c_abi_signatures(header_text: str) -> dict[str, str]:
    normalized_text = " ".join(header_text.split())
    signatures: dict[str, str] = {}
    for match in _C_RUNTIME_ABI_RE.finditer(normalized_text):
        raw_params = " ".join(match.group("params").split())
        params = "void" if raw_params == "void" else raw_params
        signatures[match.group("name")] = f"int {match.group('name')}({params})"
    return signatures


def _stdlib_runtime_contract_header_for_symbol(root: Path, symbol_name: str) -> Path | None:
    match = _STDLIB_RUNTIME_PREFIX_RE.match(symbol_name)
    if match is None:
        return None
    return (
        root
        / "native"
        / "objc3c"
        / "src"
        / "runtime"
        / "stdlib"
        / f"{match.group('family')}_runtime_contract.h"
    )


def _validate_runtime_contract_headers(
    *,
    root: Path,
    module_name: str,
    runtime_abi: list[str],
    source_signatures: dict[str, str],
) -> str | None:
    contract_headers = {
        header_path
        for runtime_symbol in runtime_abi
        if (header_path := _stdlib_runtime_contract_header_for_symbol(root, runtime_symbol))
        is not None
    }
    if not contract_headers:
        return None

    public_umbrella = (
        root / "native" / "objc3c" / "src" / "runtime" / "public" / "objc3_runtime_api.h"
    )
    if public_umbrella.is_file():
        public_umbrella_text = public_umbrella.read_text(encoding="utf-8")
    else:
        public_umbrella_text = ""

    header_signatures: dict[str, str] = {}
    for header_path in sorted(contract_headers):
        if not header_path.is_file():
            return (
                "module runtime ABI contract header missing for "
                f"{module_name}: {header_path.relative_to(root).as_posix()}"
            )
        include_path = header_path.relative_to(root / "native" / "objc3c" / "src").as_posix()
        if public_umbrella_text and f'#include "{include_path}"' not in public_umbrella_text:
            return (
                "module runtime ABI contract header is not public for "
                f"{module_name}: {include_path}"
            )
        header_signatures.update(
            _extract_runtime_c_abi_signatures(header_path.read_text(encoding="utf-8"))
        )

    for runtime_symbol in runtime_abi:
        if _stdlib_runtime_contract_header_for_symbol(root, runtime_symbol) is None:
            continue
        source_signature = source_signatures.get(runtime_symbol)
        expected_c_signature = (
            _source_extern_signature_to_c(source_signature)
            if source_signature is not None
            else None
        )
        if expected_c_signature is None:
            return (
                "module runtime ABI source signature is not C-contract comparable "
                f"for {module_name}.{runtime_symbol}"
            )
        observed_c_signature = header_signatures.get(runtime_symbol)
        if observed_c_signature != expected_c_signature:
            return (
                "module runtime contract header signature drifted for "
                f"{module_name}.{runtime_symbol}: expected {expected_c_signature!r}, "
                f"observed {observed_c_signature!r}"
            )
    return None


def _validate_signature_manifest(
    *,
    module_name: str,
    signature_label: str,
    expected_names: list[str],
    manifest_signatures: object,
    source_signatures: dict[str, str],
) -> str | None:
    if not isinstance(manifest_signatures, dict):
        return f"module manifest {signature_label} missing or malformed for {module_name}"
    if set(manifest_signatures) != set(expected_names):
        return f"module manifest {signature_label} keys drifted for {module_name}"
    for symbol_name in expected_names:
        expected_signature = manifest_signatures.get(symbol_name)
        if not isinstance(expected_signature, str) or not expected_signature:
            return (
                f"module manifest {signature_label} entry malformed for "
                f"{module_name}.{symbol_name}"
            )
        observed_signature = source_signatures.get(symbol_name)
        if observed_signature != expected_signature:
            return (
                f"module source ABI signature drifted for {module_name}.{symbol_name}: "
                f"expected {expected_signature!r}, observed {observed_signature!r}"
            )
    return None


def validate_module_artifacts(
    *,
    root: Path,
    module_surfaces: list[CanonicalModuleSurface],
    package_imports_by_module: dict[str, PackageImportSurface],
    semantic_policy: dict[str, Any],
) -> str | None:
    for module_surface in module_surfaces:
        for path_key in ("workspace_root", "source", "smoke_source", "manifest"):
            raw_path = module_surface.path_for(path_key)
            path = root / raw_path
            if path_key == "workspace_root":
                if not path.is_dir():
                    return f"missing module workspace root: {raw_path}"
            else:
                if not path.is_file():
                    return f"missing module artifact: {raw_path}"
        manifest_payload = load_json(root / module_surface.manifest)
        if manifest_payload.get("contract_id") != "objc3c.stdlib.module.surface.v1":
            return f"module manifest contract_id drifted for {module_surface.module}"
        if manifest_payload.get("canonical_module") != module_surface.module:
            return f"module manifest canonical_module drifted for {module_surface.module}"
        if manifest_payload.get("implementation_module") != module_surface.implementation_module:
            return f"module manifest implementation_module drifted for {module_surface.module}"
        if manifest_payload.get("capability_id") != module_surface.capability_id:
            return f"module manifest capability_id drifted for {module_surface.module}"
        if manifest_payload.get("workspace_root") != module_surface.workspace_root:
            return f"module manifest workspace_root drifted for {module_surface.module}"
        if (
            manifest_payload.get("module_semver")
            != semantic_policy.get("module_semver", {}).get(module_surface.module)
        ):
            return f"module manifest module_semver drifted for {module_surface.module}"
        if manifest_payload.get("source") != module_surface.source:
            return f"module manifest source drifted for {module_surface.module}"
        if manifest_payload.get("smoke_source") != module_surface.smoke_source:
            return f"module manifest smoke_source drifted for {module_surface.module}"
        source_text = (root / module_surface.source).read_text(encoding="utf-8")
        source_signatures = extract_stdlib_abi_signatures(source_text)
        expected_decl = module_surface.expected_source_declaration()
        if expected_decl not in source_text:
            return f"module source declaration drifted for {module_surface.module}"
        package_import = package_imports_by_module.get(module_surface.module)
        if package_import is None:
            return f"package surface missing canonical module {module_surface.module}"
        if package_import.implementation_module != module_surface.implementation_module:
            return f"package surface implementation_module drifted for {module_surface.module}"
        if package_import.source_declaration != expected_decl:
            return f"package surface source_declaration drifted for {module_surface.module}"
        manifest_api_families = manifest_payload.get("api_families")
        if not isinstance(manifest_api_families, list) or not all(
            isinstance(value, str) and value for value in manifest_api_families
        ):
            return f"module manifest api_families malformed for {module_surface.module}"
        manifest_exports = manifest_payload.get("exports")
        if not isinstance(manifest_exports, list) or not all(
            isinstance(value, str) and value for value in manifest_exports
        ):
            return f"module manifest exports malformed for {module_surface.module}"
        for export_name in manifest_exports:
            if export_name not in source_text:
                return f"module source missing exported symbol spelling {export_name} for {module_surface.module}"
        signature_error = _validate_signature_manifest(
            module_name=module_surface.module,
            signature_label="abi_signatures",
            expected_names=manifest_exports,
            manifest_signatures=manifest_payload.get("abi_signatures"),
            source_signatures=source_signatures,
        )
        if signature_error is not None:
            return signature_error
        runtime_abi = manifest_payload.get("runtime_abi", [])
        if runtime_abi:
            if not isinstance(runtime_abi, list) or not all(
                isinstance(value, str) and value for value in runtime_abi
            ):
                return f"module manifest runtime_abi malformed for {module_surface.module}"
            for runtime_symbol in runtime_abi:
                if f"extern fn {runtime_symbol}" not in source_text:
                    return (
                        f"module source missing runtime ABI extern {runtime_symbol} "
                        f"for {module_surface.module}"
                    )
            runtime_signature_error = _validate_signature_manifest(
                module_name=module_surface.module,
                signature_label="runtime_abi_signatures",
                expected_names=runtime_abi,
                manifest_signatures=manifest_payload.get("runtime_abi_signatures"),
                source_signatures=source_signatures,
            )
            if runtime_signature_error is not None:
                return runtime_signature_error
            runtime_contract_error = _validate_runtime_contract_headers(
                root=root,
                module_name=module_surface.module,
                runtime_abi=runtime_abi,
                source_signatures=source_signatures,
            )
            if runtime_contract_error is not None:
                return runtime_contract_error
    return None
