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
    args = parser.parse_args(argv)
    try:
        if args.command in {"prepare", "verify"}:
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
            result = build_plan(args.repo, phase=args.phase, manifest_path=args.manifest, source_dir=args.source_dir)
    except ToolchainError as exc:
        print(f"objc3c-toolchain: {exc}", file=sys.stderr)
        return 2
    print(json.dumps(result, indent=2))
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
