"""Reproduce installed package graphs with sealed development tools and no archive."""
from __future__ import annotations

import argparse
import json
import os
from pathlib import Path
import runpy
import shutil

from .binary_packages import digest, write_json
from .llvm_tools import foundation
from .package import SCHEMA, _digest, inventory, verify_sdk
from .qualify_source_graph import qualify, qualify_case
from .qualify_source_modules import qualify as qualify_managed


def prepare(repo: Path, prefix: Path, *, facade: Path, services: Path, objc_runtime: Path) -> dict:
    plan, _ = foundation(repo)
    stage = Path(plan['paths']['stage'])
    if prefix.exists() or not prefix.resolve().is_relative_to(repo / 'out'):
        raise ValueError('development graph needs a fresh repository out prefix')
    prefix.mkdir(parents=True)

    def link(source, destination):
        destination.parent.mkdir(parents=True, exist_ok=True)
        os.link(source.resolve(strict=True), destination)

    for name in ('clang', 'clang++', 'ld.lld', 'llvm-dwarfdump'):
        link(stage / 'bin' / name, prefix / 'bin' / name)
    link(facade, prefix / 'bin/objc3c')
    for name in ('libLLVM.so', 'libclang-cpp.so'):
        actual = (stage / 'lib' / name).resolve(strict=True)
        link(actual, prefix / 'lib' / actual.name)
        (prefix / 'lib' / name).symlink_to(actual.name)
    actual = objc_runtime.resolve(strict=True)
    link(actual, prefix / 'lib' / actual.name)
    if actual.name != 'libobjc.so':
        (prefix / 'lib/libobjc.so').symlink_to(actual.name)
    link(services, prefix / 'lib/libobjc3_runtime_services.a')
    for source, target in ((stage / 'lib/clang/23/include', 'lib/clang/23/include'),
                           (repo / 'stdlib/include', 'include'),
                           (repo / 'native/objc3c/src/runtime/public', 'include/objc3c/runtime'),
                           (objc_runtime.parent.parent / 'include/objc', 'include/objc')):
        shutil.copytree(source, prefix / target, copy_function=os.link, dirs_exist_ok=True)
    builder = prefix / 'share/objc3c/source-build'
    builder.mkdir(parents=True)
    for source, name in (('source_graph.py', 'build.py'), ('sdk_example.py', 'sdk_example.py')):
        shutil.copyfile(Path(__file__).with_name(source), builder / name)
    shutil.copyfile(Path(__file__).with_name('package.py'), prefix / 'share/objc3c/verify-sdk.py')
    shutil.copytree(repo / 'tests/clang_foundation/source_graph', prefix / 'share/objc3c/examples/packages')
    shutil.copytree(repo / 'tests/clang_foundation/installed_sdk', prefix / 'share/objc3c/examples/native')
    shutil.copytree(repo / 'tests/clang_foundation/ownership_projection', prefix / 'share/objc3c/examples/ownership-projection')
    example = prefix / 'share/objc3c/examples/managed'
    sources = example / 'src'
    sources.mkdir(parents=True)
    for source, name in (('provider.m', 'provider.m'), ('managed-value-support.h', 'managed-value-support.h'),
                         ('consumer-source.m', 'main.m')):
        shutil.copyfile(repo / 'tests/clang_foundation/native_package' / source, sources / name)
    shutil.copyfile(Path(__file__).with_name('sdk_example.py'), example / 'build.py')
    shutil.copyfile(Path(__file__).with_name('qualify_source_modules.py'), example / 'qualify.py')
    launcher = prefix / 'bin/objc3c-build'
    launcher.write_text('#!/bin/sh\nset -eu\n'
        'prefix=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)\n'
        'exec python3 -B "$prefix/share/objc3c/source-build/build.py" --sdk-root "$prefix" "$@"\n')
    launcher.chmod(0o755)
    record = {'schema': SCHEMA, 'source_build_id': plan['source_build_id'],
              'target': plan['host_environment']['target_triple'],
              'purpose': 'Development-only source graph reproduction',
              'tool_inputs': {str(path): digest(path) for path in (facade, services, objc_runtime)},
              'files': inventory(prefix)}
    record['seal_sha256'] = _digest(record)
    write_json(prefix / 'objc3-sdk.json', record)
    (prefix / 'objc3-sdk.json').chmod(0o644)
    return verify_sdk(prefix)


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--repo', type=Path, default=Path.cwd())
    parser.add_argument('--output', type=Path, required=True)
    parser.add_argument('--facade', type=Path, required=True)
    parser.add_argument('--services', type=Path, required=True)
    parser.add_argument('--objc-runtime', type=Path, required=True)
    parser.add_argument('--language', default='objective-c')
    parser.add_argument('--standard', default='c17')
    parser.add_argument('--ownership', default='arc')
    parser.add_argument('--optimization', default='O0')
    selection = parser.add_mutually_exclusive_group()
    selection.add_argument('--matrix', action='store_true', help='run the complete 16-mode source graph matrix')
    selection.add_argument('--managed-example', action='store_true', help='qualify installed managed-example freshness')
    selection.add_argument('--cxx-interfaces', action='store_true', help='qualify the current C++ interface boundary')
    parser.add_argument('--jobs', type=int, choices=range(1, 5), default=2)
    args = parser.parse_args(argv)
    root = args.output.resolve()
    prepare(args.repo.resolve(strict=True), root / 'prefix', facade=args.facade,
            services=args.services, objc_runtime=args.objc_runtime)
    try:
        if args.cxx_interfaces:
            owner = runpy.run_path(str(root / 'prefix/share/objc3c/examples/native/qualify-cpp-interfaces.py'))
            report = owner['qualify'](root / 'prefix', root / 'evidence')
        elif args.managed_example:
            report = qualify_managed(root / 'prefix', root / 'evidence')
        elif args.matrix:
            report = qualify(root / 'prefix', root / 'evidence', jobs=args.jobs)
        else:
            builds, rejections = qualify_case(root / 'prefix', root / 'evidence', args.language,
                                             args.standard, args.ownership, args.optimization)
            report = {'status': 'PASS', 'builds': builds, 'rejections': rejections}
    except Exception as error:
        write_json(root / 'qualification.json', {'status': 'FAIL', 'error': str(error)})
        raise
    write_json(root / 'qualification.json', report)
    print(json.dumps({'status': report['status'], 'builds': len(report.get('builds', report.get('executions', []))),
                      'rejections': len(report.get('rejections', report.get('rejected_imports', [])))}))


if __name__ == '__main__':
    main()
