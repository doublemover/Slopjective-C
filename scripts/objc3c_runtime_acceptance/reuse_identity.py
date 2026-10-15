"""Content identities that make whole-run runtime acceptance reuse verifiable."""

from __future__ import annotations

import hashlib
import json
import os
import re
import shutil
import sys
from pathlib import Path
from typing import Any, Iterable

from .paths import NATIVE_EXE, ROOT, RUNTIME_LIB

REUSE_IDENTITY_CONTRACT_ID = "objc3c.runtime.acceptance.reuse.identity.v1"
_MODULE_DECLARATION = re.compile(r"(?m)^\s*module\s+([A-Za-z_][A-Za-z0-9_]*)\s*;")
_MODULE_IMPORT = re.compile(r"(?m)^\s*@import\s+([A-Za-z_][A-Za-z0-9_.]*)\s*;")
_SOURCE_EXTENSIONS = {".objc3", ".h", ".hpp", ".c", ".cc", ".cpp", ".m", ".mm", ".swift"}
_MODULE_ROOTS = ("tests/tooling/fixtures/native", "tests/native/runtime")


def _sha256(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as stream:
        for block in iter(lambda: stream.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()


def _display_path(path: Path, root: Path) -> str:
    resolved = path.resolve()
    try:
        return resolved.relative_to(root.resolve()).as_posix()
    except ValueError:
        return str(resolved)


def _file_identity(path: Path, root: Path) -> dict[str, str]:
    resolved = path.resolve()
    return {"path": _display_path(resolved, root), "sha256": _sha256(resolved)}


def _canonical_digest(value: Any) -> str:
    encoded = json.dumps(value, sort_keys=True, separators=(",", ":"), ensure_ascii=False).encode("utf-8")
    return hashlib.sha256(encoded).hexdigest()


def runtime_acceptance_options(args: Any) -> dict[str, Any]:
    return {
        "suite": str(getattr(args, "suite", "full")),
        "cases": sorted(str(value) for value in getattr(args, "cases", [])),
        "compile_backend": os.environ.get("OBJC3C_RUNTIME_ACCEPTANCE_COMPILE_BACKEND", "direct-native").strip().lower() or "direct-native",
        "probe_retries": os.environ.get("OBJC3C_RUNTIME_ACCEPTANCE_PROBE_RETRIES", "1").strip() or "1",
        "platform": sys.platform,
        "python_version": list(sys.version_info[:3]),
    }


def _walk_strings(value: Any, key: str = "") -> Iterable[tuple[str, str]]:
    if isinstance(value, dict):
        for child_key, child in value.items():
            yield from _walk_strings(child, str(child_key))
    elif isinstance(value, (list, tuple)):
        for child in value:
            yield from _walk_strings(child, key)
    elif isinstance(value, str):
        yield key, value


def _source_candidates(results: Iterable[Any], run_dir: Path, root: Path) -> tuple[set[Path], list[str]]:
    candidates: set[Path] = set()
    failures: list[str] = []
    for result in results:
        direct = getattr(result, "fixture", None)
        if direct:
            path = Path(str(direct))
            path = path if path.is_absolute() else root / path
            if path.is_file():
                candidates.add(path.resolve())
            else:
                failures.append(f"fixture missing: {direct}")
        for key, raw in _walk_strings(getattr(result, "summary", {})):
            if not (key == "fixture" or key.endswith("_fixture") or key.endswith("_source")):
                continue
            if Path(raw).suffix.lower() not in _SOURCE_EXTENSIONS:
                continue
            path = Path(raw)
            path = path if path.is_absolute() else root / path
            if path.is_file():
                candidates.add(path.resolve())
            else:
                failures.append(f"referenced source missing: {raw}")
    if run_dir.is_dir():
        candidates.update(path.resolve() for path in run_dir.rglob("*.objc3") if path.is_file())
    return candidates, failures


def _module_inventory(root: Path, run_dir: Path) -> tuple[dict[str, list[Path]], str]:
    roots = [root / relative for relative in _MODULE_ROOTS]
    if run_dir.is_dir():
        roots.append(run_dir)
    index: dict[str, list[Path]] = {}
    rows: list[dict[str, str]] = []
    visited: set[Path] = set()
    for directory in roots:
        if not directory.is_dir():
            continue
        for path in directory.rglob("*.objc3"):
            resolved = path.resolve()
            if resolved in visited or not resolved.is_file():
                continue
            visited.add(resolved)
            try:
                text = resolved.read_text(encoding="utf-8")
            except (OSError, UnicodeError):
                continue
            match = _MODULE_DECLARATION.search(text)
            if not match:
                continue
            module = match.group(1)
            index.setdefault(module, []).append(resolved)
            rows.append({"module": module, "path": _display_path(resolved, root)})
    rows.sort(key=lambda row: (row["module"], row["path"]))
    return index, _canonical_digest(rows)


def _source_closure(
    roots: set[Path],
    module_index: dict[str, list[Path]],
    root: Path,
) -> tuple[list[dict[str, Any]], list[str]]:
    pending = list(sorted(roots))
    visited: set[Path] = set()
    failures: list[str] = []
    identities: dict[Path, dict[str, Any]] = {}
    while pending:
        path = pending.pop()
        if path in visited:
            continue
        visited.add(path)
        try:
            text = path.read_text(encoding="utf-8")
        except (OSError, UnicodeError) as exc:
            failures.append(f"source unreadable: {_display_path(path, root)} ({exc})")
            continue
        imports = sorted(set(_MODULE_IMPORT.findall(text)))
        resolved_imports: list[dict[str, str]] = []
        for module in imports:
            matches = module_index.get(module, [])
            if len(matches) != 1:
                failures.append(f"import closure unresolved or ambiguous: {module} from {_display_path(path, root)}")
                continue
            imported = matches[0]
            resolved_imports.append({"module": module, "path": _display_path(imported, root)})
            pending.append(imported)
        try:
            identities[path] = {
                **_file_identity(path, root),
                "module": (_MODULE_DECLARATION.search(text).group(1) if _MODULE_DECLARATION.search(text) else ""),
                "imports": resolved_imports,
            }
        except OSError as exc:
            failures.append(f"source hash unavailable: {_display_path(path, root)} ({exc})")
    return [identities[path] for path in sorted(identities)], failures


def _interface_artifacts(results: Iterable[Any], run_dir: Path, root: Path) -> tuple[list[dict[str, str]], list[str]]:
    candidates: set[Path] = set()
    failures: list[str] = []
    suffixes = ("runtime-import-surface.json", "runtime-registration-manifest.json", "cross-module-runtime-link-plan.json", "module-interface.json", "module.interface.json", ".modulemap")
    if run_dir.is_dir():
        for path in run_dir.rglob("*"):
            if path.is_file() and any(path.name.endswith(suffix) for suffix in suffixes):
                candidates.add(path.resolve())
    for result in results:
        for key, raw in _walk_strings(getattr(result, "summary", {})):
            normalized_key = key.lower()
            if not any(marker in normalized_key for marker in ("import_surface", "module_interface", "link_plan")):
                continue
            path = Path(raw)
            path = path if path.is_absolute() else root / path
            if path.is_file() and any(path.name.endswith(suffix) for suffix in suffixes):
                candidates.add(path.resolve())
    identities: list[dict[str, str]] = []
    for path in sorted(candidates):
        try:
            identities.append(_file_identity(path, root))
        except OSError as exc:
            failures.append(f"interface artifact hash unavailable: {_display_path(path, root)} ({exc})")
    return identities, failures


def _tooling_paths(root: Path, clangxx: Path, powershell: Path | None) -> tuple[list[dict[str, str]], list[str]]:
    files: set[Path] = set()
    failures: list[str] = []
    explicit = [
        root / "scripts/check_objc3c_runtime_acceptance.py",
        root / "scripts/build_objc3c_native.ps1",
        root / "scripts/objc3c_runtime_acceptance",
        root / "scripts/objc3c_tooling",
        root / "scripts/objc3c_workflow/command_powershell_policy.py",
        root / "scripts/objc3c_native_compile.ps1",
        root / "scripts/objc3c_native_compile_command.psm1",
        root / "scripts/objc3c_native_compile_io.psm1",
        root / "scripts/objc3c_native_compile_wrapper.psm1",
        root / "scripts/objc3c_native_compile_toolchain.psm1",
        root / "scripts/objc3c_native_compile_frontend_guards.psm1",
    ]
    for path in explicit:
        if path.is_dir():
            files.update(item.resolve() for item in path.rglob("*.py") if item.is_file())
        elif path.is_file():
            files.add(path.resolve())
        else:
            failures.append(f"tool input missing: {_display_path(path, root)}")
    for directory_name in (
        "objc3c_native_compile_io",
        "objc3c_native_compile_wrapper",
        "objc3c_native_compile_toolchain",
        "objc3c_native_compile_frontend_guards",
        "objc3c_native_compile_provenance",
    ):
        directory = root / "scripts" / directory_name
        if directory.is_dir():
            files.update(item.resolve() for item in directory.rglob("*") if item.is_file() and item.suffix.lower() in {".ps1", ".psm1", ".py"})
        else:
            failures.append(f"tool input directory missing: {_display_path(directory, root)}")
    runtime_headers = root / "native/objc3c/src/runtime/public"
    if runtime_headers.is_dir():
        files.update(item.resolve() for item in runtime_headers.rglob("*") if item.is_file() and item.suffix.lower() in {".h", ".hpp"})
    else:
        failures.append(f"runtime headers missing: {_display_path(runtime_headers, root)}")
    runtime_test_sources = root / "tests/tooling/runtime"
    if runtime_test_sources.is_dir():
        files.update(item.resolve() for item in runtime_test_sources.rglob("*") if item.is_file())
    else:
        failures.append(f"runtime test inputs missing: {_display_path(runtime_test_sources, root)}")
    if powershell is not None and powershell.is_file():
        files.add(powershell.resolve())
    else:
        failures.append("PowerShell executable identity unavailable")
    if clangxx.is_file():
        files.add(clangxx.resolve())
    else:
        failures.append("clang++ executable identity unavailable")
    identities: list[dict[str, str]] = []
    for path in sorted(files):
        try:
            identities.append(_file_identity(path, root))
        except OSError as exc:
            failures.append(f"tool input hash unavailable: {_display_path(path, root)} ({exc})")
    return identities, failures


def build_runtime_acceptance_reuse_identity(
    *,
    args: Any,
    run_dir: Path,
    clangxx: str,
    results: Iterable[Any],
    case_timings: Iterable[dict[str, Any]] = (),
    root: Path = ROOT,
    native_executable: Path = NATIVE_EXE,
    runtime_library: Path = RUNTIME_LIB,
    powershell_executable: str | None = None,
) -> dict[str, Any]:
    root = root.resolve()
    run_dir = run_dir.resolve()
    results = list(results)
    failures: list[str] = []
    artifact_fields: dict[str, dict[str, str]] = {}
    for name, path in (
        ("native_executable", native_executable),
        ("runtime_library", runtime_library),
    ):
        try:
            if not path.is_file():
                raise OSError("file missing")
            artifact_fields[name] = _file_identity(path, root)
        except OSError as exc:
            failures.append(f"{name} identity unavailable: {exc}")
    clang_path = Path(clangxx)
    if not clang_path.is_file():
        failures.append("clang++ identity unavailable")
    resolved_powershell = powershell_executable or shutil.which("pwsh") or shutil.which("powershell")
    powershell_path = Path(resolved_powershell) if resolved_powershell else None
    source_roots, source_failures = _source_candidates(results, run_dir, root)
    failures.extend(source_failures)
    module_index, module_inventory_sha256 = _module_inventory(root, run_dir)
    source_closure, closure_failures = _source_closure(source_roots, module_index, root)
    failures.extend(closure_failures)
    interface_artifacts, interface_failures = _interface_artifacts(results, run_dir, root)
    failures.extend(interface_failures)
    tooling_files, tooling_failures = _tooling_paths(root, clang_path, powershell_path)
    failures.extend(tooling_failures)
    options = runtime_acceptance_options(args)
    fixture_ids = sorted(str(getattr(result, "case_id", "")) for result in results)
    fixture_ids = [value for value in fixture_ids if value]
    case_label_pairs = [
        {"label": str(entry.get("label", "")), "case_id": str(entry.get("case_id", ""))}
        for entry in case_timings
        if isinstance(entry, dict)
    ]
    if (
        not case_label_pairs
        or any(not item["label"] or not item["case_id"] for item in case_label_pairs)
        or len({item["label"] for item in case_label_pairs}) != len(case_label_pairs)
        or sorted(item["case_id"] for item in case_label_pairs) != fixture_ids
    ):
        failures.append("case label to fixture identity unavailable or inconsistent")
    payload: dict[str, Any] = {
        "contract_id": REUSE_IDENTITY_CONTRACT_ID,
        "complete": not failures,
        "failures": sorted(set(failures)),
        "artifacts": {
            **artifact_fields,
            "clangxx": _file_identity(clang_path, root) if clang_path.is_file() else {"path": str(clang_path), "sha256": ""},
        },
        "source_closure": source_closure,
        "interface_artifacts": interface_artifacts,
        "module_inventory_roots": list(_MODULE_ROOTS),
        "module_inventory_sha256": module_inventory_sha256,
        "module_inventory_run_dir": _display_path(run_dir, root),
        "tooling_files": tooling_files,
        "options": options,
        "options_digest": _canonical_digest(options),
        "fixture_ids": fixture_ids,
        "case_labels": [item["label"] for item in case_label_pairs],
        "fixture_label_pairs": case_label_pairs,
    }
    payload["identity_digest"] = _canonical_digest({key: value for key, value in payload.items() if key != "identity_digest"})
    return payload


def validate_runtime_acceptance_reuse_identity(identity: Any, *, root: Path = ROOT) -> tuple[bool, str]:
    if not isinstance(identity, dict) or identity.get("contract_id") != REUSE_IDENTITY_CONTRACT_ID:
        return False, "reuse identity contract missing"
    if identity.get("complete") is not True:
        return False, "reuse identity incomplete"
    if identity.get("identity_digest") != _canonical_digest({key: value for key, value in identity.items() if key != "identity_digest"}):
        return False, "reuse identity digest mismatch"
    artifacts = identity.get("artifacts")
    if not isinstance(artifacts, dict):
        return False, "artifact identity missing"
    for name in ("native_executable", "runtime_library", "clangxx"):
        item = artifacts.get(name)
        if not isinstance(item, dict) or not item.get("path") or not item.get("sha256"):
            return False, f"{name} identity missing"
        path_text = str(item["path"])
        path = Path(path_text)
        if not path.is_absolute():
            path = root / path
        try:
            if not path.is_file() or _sha256(path) != item["sha256"]:
                return False, f"{name} identity changed"
        except OSError:
            return False, f"{name} identity unavailable"
    for field in ("source_closure", "interface_artifacts", "tooling_files"):
        records = identity.get(field)
        if not isinstance(records, list) or (field == "tooling_files" and not records):
            return False, f"{field} missing"
        for record in records:
            if not isinstance(record, dict) or not record.get("path") or not record.get("sha256"):
                return False, f"{field} identity malformed"
            path = Path(str(record["path"]))
            if not path.is_absolute():
                path = root / path
            try:
                if not path.is_file() or _sha256(path) != record["sha256"]:
                    return False, f"{field} identity changed"
            except OSError:
                return False, f"{field} identity unavailable"
    roots = identity.get("module_inventory_roots")
    if roots != list(_MODULE_ROOTS):
        return False, "module inventory roots missing or unexpected"
    run_dir_text = identity.get("module_inventory_run_dir")
    if not isinstance(run_dir_text, str) or not run_dir_text:
        return False, "module inventory run directory missing"
    run_dir = Path(run_dir_text)
    if not run_dir.is_absolute():
        run_dir = root / run_dir
    _, inventory_digest = _module_inventory(root, run_dir)
    if inventory_digest != identity.get("module_inventory_sha256"):
        return False, "module inventory changed"
    options = identity.get("options")
    if not isinstance(options, dict) or identity.get("options_digest") != _canonical_digest(options):
        return False, "options identity malformed"
    if not isinstance(identity.get("fixture_ids"), list) or not identity["fixture_ids"]:
        return False, "fixture identity missing"
    return True, "verified"


__all__ = [
    "REUSE_IDENTITY_CONTRACT_ID",
    "build_runtime_acceptance_reuse_identity",
    "runtime_acceptance_options",
    "validate_runtime_acceptance_reuse_identity",
]
