"""Command line interface for pinned LLVM source preparation."""

from __future__ import annotations

import argparse
import json
import sys
from pathlib import Path

from .core import ToolchainError, build_plan, prepare, seal_development, verify_development


def main(argv: list[str] | None = None) -> int:
    parser = argparse.ArgumentParser(prog="python -m scripts.objc3c_toolchain")
    commands = parser.add_subparsers(dest="command", required=True)
    sdk = commands.add_parser("build-sdk", help="build, qualify, package and install the native Linux SDK")
    sdk.add_argument("--repo", type=Path, default=Path.cwd())
    sdk.add_argument("--fetch", action="store_true")
    sdk.add_argument("--install-root", type=Path)
    sdk.add_argument("--foundation-sdk", type=Path, help="reuse a fully verified sealed SDK with the same LLVM source/build identity")
    compile_command = commands.add_parser("compile", help="invoke the installed public compiler")
    compile_command.add_argument("arguments", nargs=argparse.REMAINDER)
    package = commands.add_parser("package-sdk", help="seal and cache an installed, fully qualified Linux SDK")
    package.add_argument("--repo", type=Path, default=Path.cwd())
    package.add_argument("--prefix", type=Path, required=True)
    package.add_argument("--validation-report", type=Path, required=True)
    package.add_argument("--cache-dir", type=Path, required=True)
    facade = commands.add_parser("facade-identity", help="identify native frontend source and CMake inputs")
    facade.add_argument("--repo", type=Path, default=Path.cwd())
    tools = commands.add_parser("check-host-tools", help="check development version references and optional live tools")
    tools.add_argument("--repo", type=Path, default=Path.cwd())
    tools.add_argument("--runtime", action="store_true", help="also check installed development tool versions")
    build = commands.add_parser("build", help="execute sealed foundation configure/build/test plans and retain timings")
    build.add_argument("--repo", type=Path, default=Path.cwd())
    build.add_argument("--host", choices=("windows-x86_64", "linux-x86_64"))
    build.add_argument("--fetch", action="store_true", help="acquire pinned source when the development checkout is absent")
    build.add_argument("--stage", action="store_true", help="also install the SDK distribution")
    for name in ("prepare", "verify"):
        command = commands.add_parser(name, help=f"{'materialize' if name == 'prepare' else 'verify'} pinned LLVM source")
        command.add_argument("--repo", type=Path, default=Path.cwd(), help="repository root")
        command.add_argument("--manifest", type=Path, help="manifest path (defaults to repository manifest)")
        command.add_argument("--source-dir", type=Path, help="prepared source path (must be under repo/out/toolchains)")
        if name == "prepare":
            command.add_argument("--fetch", action="store_true", help="fetch the pinned upstream commit from the manifest URL")
        command.add_argument("--pristine-baseline", action="store_true",
                             help="explicitly prepare upstream-only baseline; never labels it as the patched fork")
    for name in ("seal-development", "verify-development"):
        command = commands.add_parser(name, help="compare development source with the canonical locked patch stack")
        command.add_argument("--repo", type=Path, default=Path.cwd(), help="repository root")
        command.add_argument("--manifest", type=Path, help="manifest path (defaults to repository manifest)")
        command.add_argument("--source-dir", type=Path, help="development source (defaults to out/toolchains/objc3-development/src)")
    plan = commands.add_parser("build-plan", help="print an identified configure/build/test/stage command; never executes it")
    plan.add_argument("phase", choices=("configure", "build", "test", "stage"))
    plan.add_argument("--repo", type=Path, default=Path.cwd(), help="repository root")
    plan.add_argument("--manifest", type=Path, help="manifest path (defaults to repository manifest)")
    plan.add_argument("--source-dir", type=Path, help="sealed development source directory")
    plan.add_argument("--host", choices=("windows-x86_64", "linux-x86_64"),
                      help="native build host (defaults to the current host)")
    args = parser.parse_args(argv)
    try:
        if args.command == "build-sdk":
            from .linux_sdk import build_sdk
            result = build_sdk(args.repo, fetch=args.fetch, install_root=args.install_root, foundation_sdk=args.foundation_sdk)
        elif args.command == "compile":
            import os
            import subprocess
            root = Path(os.environ.get('OBJC3C_SDK_ROOT', str(Path.cwd() / 'out/toolchains/linux-sdk/current')))
            arguments = args.arguments[1:] if args.arguments[:1] == ['--'] else args.arguments
            return subprocess.run([str(root / 'bin/objc3c'), *arguments]).returncode
        elif args.command == "package-sdk":
            from .package import package_sdk
            result = package_sdk(args.repo, args.prefix, args.validation_report, args.cache_dir)
        elif args.command == "facade-identity":
            from .facade import facade_identity
            result = facade_identity(args.repo)
        elif args.command == "build":
            from .build import build_foundation
            result = build_foundation(args.repo, host=args.host, fetch=args.fetch, stage=args.stage)
        elif args.command == "check-host-tools":
            from .versions import check_host_tools
            result = check_host_tools(args.repo, runtime=args.runtime)
        elif args.command in {"prepare", "verify"}:
            result = prepare(
                args.repo,
                manifest_path=args.manifest,
                fetch=getattr(args, "fetch", False),
                pristine=args.pristine_baseline,
                source_dir=args.source_dir,
                verify_only=args.command == "verify",
            )
        elif args.command == "seal-development":
            result = seal_development(args.repo, manifest_path=args.manifest, source_dir=args.source_dir)
        elif args.command == "verify-development":
            result = verify_development(args.repo, manifest_path=args.manifest, source_dir=args.source_dir)
        else:
            result = build_plan(args.repo, phase=args.phase, manifest_path=args.manifest,
                                source_dir=args.source_dir, host=args.host)
    except (ToolchainError, ValueError, OSError, KeyError) as exc:
        print(f"objc3c-toolchain: {exc}", file=sys.stderr)
        return 2
    print(json.dumps(result, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
