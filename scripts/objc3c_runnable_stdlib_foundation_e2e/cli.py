from __future__ import annotations

from datetime import datetime

from .compilation import compile_stdlib_modules
from .constants import REPORT_PATH, ROOT
from .manifest import load_stdlib_package_surface
from .packaging import run_package_command
from .summary import build_summary_payload, write_summary


def main() -> int:
    run_id = datetime.now().strftime("%Y%m%d_%H%M%S_%f")
    package_root = ROOT / "tmp" / "pkg" / "objc3c-stdlib-foundation-e2e" / run_id
    manifest_path = (
        package_root / "artifacts" / "package" / "objc3c-runnable-toolchain-package.json"
    )
    artifacts_root = package_root / "tmp" / "artifacts" / "stdlib" / "runnable-e2e"

    package_result = run_package_command(package_root=package_root)
    surface = load_stdlib_package_surface(
        package_root=package_root,
        manifest_path=manifest_path,
    )
    compile_results = compile_stdlib_modules(
        package_root=package_root,
        artifacts_root=artifacts_root,
        surface=surface,
    )
    payload = build_summary_payload(
        package_result=package_result,
        package_root=package_root,
        manifest_path=manifest_path,
        surface=surface,
        compile_results=compile_results,
    )
    write_summary(REPORT_PATH, payload)
    return 0
