"""Seal, verify and cache a qualified relocatable SDK using Python 3.15 zstd."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import shutil
import stat
import tarfile
import tempfile

MANIFEST = 'objc3-sdk.json'
SCHEMA = 'objc3.sdk.v2'


def _digest(value) -> str:
    return hashlib.sha256(json.dumps(value, sort_keys=True, separators=(',', ':')).encode()).hexdigest()


def _file_hash(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def inventory(prefix: Path) -> dict:
    prefix = prefix.resolve()
    files = {'.': {'kind': 'directory', 'mode': stat.S_IMODE(prefix.stat().st_mode)}}
    for path in sorted(prefix.rglob('*')):
        name = path.relative_to(prefix).as_posix()
        if name == MANIFEST:
            continue
        if path.is_symlink():
            if Path(os.readlink(path)).is_absolute() or not path.resolve(strict=True).is_relative_to(prefix):
                raise ValueError(f'SDK symbolic link escapes its prefix: {name}')
            files[name] = {'kind': 'symlink', 'target': os.readlink(path)}
        elif path.is_file():
            files[name] = {'kind': 'file', 'sha256': _file_hash(path),
                           'mode': stat.S_IMODE(path.stat().st_mode)}
        elif path.is_dir():
            files[name] = {'kind': 'directory', 'mode': stat.S_IMODE(path.stat().st_mode)}
    return files


def normalize_permissions(prefix: Path) -> None:
    for path in (prefix, *prefix.rglob('*')):
        if path.is_symlink():
            continue
        if path.is_dir():
            path.chmod(0o755)
        elif path.is_file():
            path.chmod(0o755 if path.stat().st_mode & 0o111 else 0o644)


def verify_sdk(prefix: Path, expected: str | None = None) -> dict:
    if stat.S_IMODE((prefix / MANIFEST).stat().st_mode) != 0o644:
        raise ValueError('SDK manifest permissions differ from the distribution contract')
    data = json.loads((prefix / MANIFEST).read_text())
    seal = data.pop('seal_sha256')
    if data.get('schema') != SCHEMA or _digest(data) != seal or (expected and seal != expected):
        raise ValueError('SDK seal identity mismatch')
    if inventory(prefix) != data['files']:
        raise ValueError('SDK files, permissions or symbolic links differ from the seal')
    return {**data, 'seal_sha256': seal}


def package_sdk(repo: Path, prefix: Path, validation_report: Path, cache_dir: Path) -> dict:
    from compression import zstd

    # Imports stay inside this producer so the installed verifier needs only
    # Python's standard library and never needs a source checkout.
    from .core import verify_development
    from .facade import facade_identity
    from scripts.objc3c_native_validation.identity import verify_toolchain_snapshot
    from scripts.objc3c_native_validation.plan import build_plan
    from scripts.objc3c_native_validation.artifacts import read_node_result
    from scripts.objc3c_native_validation.cli import inventory_identity
    from scripts.objc3c_cross_lane_execution import compute_node_identities

    repo, prefix = repo.resolve(), prefix.resolve()
    report = json.loads(validation_report.read_text())
    plan = build_plan(repo, drivers=('clang', 'objc3c-clang'))
    expected_nodes = plan['selected_node_ids']
    if (report.get('validation_complete') is not True or report.get('status') != 'PASS' or
            set(report.get('selected_node_ids', [])) != set(expected_nodes)):
        raise ValueError('SDK packaging requires a complete successful Linux native validation report')
    context = json.loads(Path(report['context_path']).read_text())
    toolchain = context['toolchain']
    if toolchain['platform'] != 'linux-x86_64' or Path(toolchain['tools']['facade']).parent != prefix / 'bin':
        raise ValueError('qualify the installed SDK frontend before packaging')
    verify_toolchain_snapshot(toolchain)
    identities = compute_node_identities(plan, input_root=repo,
        build_identity=toolchain['build_identity'], inventory_identity=inventory_identity(plan))
    records = {record['node_id']: record for record in report['node_records']
               if record['status'] in ('completed', 'reused')}
    if (report['build_identity'] != toolchain['build_identity'] or
            context['node_identities'] != identities or set(records) != set(expected_nodes)):
        raise ValueError('SDK validation source or node identities are stale')
    for node in expected_nodes:
        saved = read_node_result(Path(records[node]['result_path']), node_id=node,
            node_identity=identities[node], build_identity=toolchain['build_identity'], allowed_roots=[repo])
        if saved['run_id'] != report['run_id']:
            raise ValueError('SDK validation node belongs to a different run')
    source = verify_development(repo)
    if source['build_id'] != report['sdk_build_id']:
        raise ValueError('SDK report refers to another source build')
    source_root = repo / 'out/toolchains/objc3-development/src'
    for project in ('llvm', 'clang', 'lld', 'lldb', 'clang-tools-extra'):
        destination = prefix / 'share/licenses' / project
        destination.mkdir(parents=True, exist_ok=True)
        shutil.copyfile(source_root / project / 'LICENSE.TXT', destination / 'LICENSE.TXT')
    example = prefix / 'share/objc3c/examples/managed'
    example.mkdir(parents=True, exist_ok=True)
    sources = example / 'src'
    sources.mkdir()
    for name in ('provider.m', 'managed-value-support.h'):
        shutil.copyfile(repo / 'tests/clang_foundation/native_package' / name, sources / name)
    shutil.copyfile(repo / 'tests/clang_foundation/native_package/consumer-source.m', sources / 'main.m')
    shutil.copyfile(Path(__file__).with_name('sdk_example.py'), example / 'build.py')
    shutil.copyfile(Path(__file__).with_name('qualify_source_modules.py'), example / 'qualify.py')
    builder = prefix / 'share/objc3c/source-build'
    builder.mkdir(parents=True, exist_ok=True)
    for original, installed in (('source_graph.py', 'build.py'), ('sdk_example.py', 'sdk_example.py'),
                                ('qualify_source_graph.py', 'qualify.py')):
        shutil.copyfile(Path(__file__).with_name(original), builder / installed)
    shutil.copytree(repo / 'tests/clang_foundation/source_graph',
                    prefix / 'share/objc3c/examples/packages')
    launcher = prefix / 'bin/objc3c-build'
    launcher.write_text('#!/bin/sh\nset -eu\n'
        'prefix=$(CDPATH= cd -- "$(dirname -- "$0")/.." && pwd)\n'
        'exec python3 "$prefix/share/objc3c/source-build/build.py" --sdk-root "$prefix" "$@"\n')
    launcher.chmod(0o755)
    import subprocess
    with tempfile.TemporaryDirectory(prefix='objc3-source-sample-') as temporary:
        subprocess.run([os.sys.executable, '-B', str(example / 'build.py'), '--output-dir', temporary,
                        '--installed-layout', '--target', toolchain['target_triple']], check=True)
        shutil.copy2(Path(temporary) / 'objc3-managed-sample', prefix / 'bin/objc3-managed-sample')
    shutil.copytree(repo / 'tests/clang_foundation/installed_sdk',
                    prefix / 'share/objc3c/examples/native', dirs_exist_ok=True)
    verifier = prefix / 'share/objc3c/verify-sdk.py'
    verifier.parent.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(Path(__file__), verifier)
    shutil.copyfile(Path(__file__).with_name('install_sdk.py'), prefix / 'share/objc3c/install-sdk.py')
    shutil.copyfile(repo / 'scripts/install_objc3c_linux.sh', prefix / 'share/objc3c/install-linux.sh')
    runtime_profile = json.loads((repo / 'scripts/objc3c_toolchain_runtime/runtime-profile.json').read_text())['profiles']['linux-x86_64']
    robin_license = repo / f"out/toolchains/robin-map-{runtime_profile['robin_map_release']}/src/LICENSE"
    license_dir = prefix / 'share/licenses/robin-map'
    license_dir.mkdir(parents=True, exist_ok=True)
    shutil.copyfile(robin_license, license_dir / 'LICENSE')
    normalize_permissions(prefix)
    data = {'schema': SCHEMA, 'source_build_id': source['build_id'],
            'upstream_commit': source['upstream_commit'], 'patch_digest': source['patch_digest'],
            'target': toolchain['target_triple'],
            'linux_abi': {name: json.loads((repo / 'scripts/objc3c_toolchain_runtime/runtime-profile.json').read_text())['profiles']['linux-x86_64'][name]
                          for name in ('glibc_minimum', 'libstdcxx_symbol_minimum')},
            'host_tools': json.loads((repo / 'toolchains/llvm/manifest.json').read_text())['host_tools'],
            'frontend_inputs': facade_identity(repo)['sha256'],
            'qualification': {'build_identity': report['build_identity'], 'nodes': expected_nodes,
                              'runtime_count': report['runtime_count']}, 'files': inventory(prefix)}
    data['seal_sha256'] = _digest(data)
    (prefix / MANIFEST).write_text(json.dumps(data, indent=2) + '\n')
    (prefix / MANIFEST).chmod(0o644)
    from .qualify_source_modules import qualify
    module_evidence = qualify(prefix, validation_report.parent / 'installed-source-modules')
    from .qualify_source_graph import qualify as qualify_graph
    graph_evidence = qualify_graph(prefix, validation_report.parent / 'installed-source-graph')
    cache_dir.mkdir(parents=True, exist_ok=True)
    archive = cache_dir / (source['build_id'] + '-linux-x86_64-' + data['seal_sha256'][:16] + '.tar.zst')
    if archive.exists():
        receipt = json.loads(archive.with_suffix('.json').read_text())
        if receipt['archive_sha256'] != _file_hash(archive):
            raise ValueError('cached SDK archive changed; preserving it for inspection')
        verify_sdk(prefix, receipt['seal_sha256'])
        return {**receipt, 'cache_hit': True}
    descriptor, temporary = tempfile.mkstemp(prefix='.sdk-', dir=cache_dir)
    os.close(descriptor)
    try:
        def normalize(info):
            info.uid = info.gid = 0
            info.uname = info.gname = ''
            info.mtime = 0
            return info
        with zstd.ZstdFile(temporary, 'wb', level=3) as compressed:
            with tarfile.open(fileobj=compressed, mode='w|', dereference=False) as tar:
                tar.add(prefix, arcname='objc3-sdk', filter=normalize)
        verify_sdk(prefix, data['seal_sha256'])
        os.replace(temporary, archive)
    finally:
        Path(temporary).unlink(missing_ok=True)
    receipt = {'archive': str(archive), 'archive_sha256': _file_hash(archive),
               'archive_bytes': archive.stat().st_size, 'seal_sha256': data['seal_sha256'],
               'source_build_id': source['build_id'],
               'installed_source_modules': module_evidence, 'installed_source_graph': graph_evidence}
    archive.with_suffix('.json').write_text(json.dumps(receipt, indent=2) + '\n')
    return {**receipt, 'cache_hit': False}


def main(argv=None):
    parser = argparse.ArgumentParser(description='Verify an installed SDK without a checkout')
    parser.add_argument('prefix', type=Path)
    parser.add_argument('--expected-seal')
    args = parser.parse_args(argv)
    try:
        result = verify_sdk(args.prefix, args.expected_seal)
    except (OSError, ValueError, KeyError) as error:
        parser.exit(2, str(error) + '\n')
    print(json.dumps({'status': 'PASS', 'source_build_id': result['source_build_id'],
                      'seal_sha256': result['seal_sha256'], 'entries': len(result['files']),
                      'files': sum(item['kind'] != 'directory' for item in result['files'].values())}, indent=2))


if __name__ == '__main__':
    main()
