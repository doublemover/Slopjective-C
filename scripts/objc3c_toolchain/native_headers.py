"""Content-bound native C++ and platform header inputs for the Linux SDK."""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import re
import shlex
import shutil
import subprocess
import tempfile

from .sdk_example import dependencies

BUNDLE = 'objc3-native-headers'
SCHEMA = 'objc3c.native-standard-headers.v1'


def digest(path: Path) -> str:
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def identity(manifest: dict) -> str:
    return hashlib.sha256(json.dumps(manifest, sort_keys=True,
                                     separators=(',', ':')).encode()).hexdigest()


def complete_root_files(root: Path) -> list[Path]:
    """Enumerate every regular header input below one driver search root.

    Depfiles only expose the branch selected by the discovery preprocessor
    invocation. Platform headers can include different ``bits/`` and ``sys/``
    leaves under optimization, language-mode, feature-test, and sanitizer
    macros, so an installed SDK must retain the complete checked search root.
    """
    return sorted(path for path in root.rglob('*') if path.is_file())


def _run(compiler: Path, arguments: list[str], *, source='') -> subprocess.CompletedProcess:
    environment = {**os.environ, 'LC_ALL': 'C'}
    companion = str(compiler.parent.parent / 'lib')
    environment['LD_LIBRARY_PATH'] = companion + (os.pathsep + environment['LD_LIBRARY_PATH']
                                                 if environment.get('LD_LIBRARY_PATH') else '')
    result = subprocess.run([str(compiler), '--no-default-config', *arguments],
                            input=source, text=True, capture_output=True, timeout=60,
                            env=environment)
    if result.returncode:
        raise ValueError(f'native header discovery failed: {result.stderr[-6000:]}')
    return result


def discover(compiler: Path) -> dict:
    """Read the actual driver search order; copy C++ trees and reached C files."""
    compiler = compiler.absolute()
    resource = Path(_run(compiler, ['-print-resource-dir']).stdout.strip()).resolve(strict=True)
    if not re.fullmatch(r'[0-9][0-9A-Za-z.+_-]{0,47}', resource.name):
        raise ValueError('unsupported compiler resource directory version')
    arguments = ['-x', 'c++', '-std=c++23', '-E', '-']
    printed = _run(compiler, ['-v', *arguments]).stderr
    roots = []
    active = False
    for line in printed.splitlines():
        if line.strip() == '#include <...> search starts here:':
            active = True
        elif line.strip() == 'End of search list.':
            active = False
        elif active:
            if line.strip().endswith('(framework directory)'):
                raise ValueError('Linux native header bundles do not admit framework search roots')
            root = Path(line.strip()).resolve(strict=True)
            if root not in roots:
                roots.append(root)
    cpp = [root for root in roots if (root / 'string').is_file()
           and (root / 'type_traits').is_file()]
    if not cpp:
        raise ValueError('the compiler has no native C++ standard-library header root')
    # The verbose list has the effective order; cc1 supplies each root's actual
    # system/extern-C classification. Preserve both instead of guessing from
    # distribution-specific path spelling.
    roles = {}
    for line in _run(compiler, ['-###', *arguments]).stderr.splitlines():
        argv = shlex.split(line)
        if '-cc1' not in argv:
            continue
        for index, flag in enumerate(argv[:-1]):
            if flag in ('-internal-isystem', '-isystem', '-cxx-isystem',
                        '-internal-externc-isystem', '-I', '-iquote', '-idirafter'):
                role = 'externc' if flag == '-internal-externc-isystem' else 'system'
                if flag in ('-I', '-iquote'):
                    role = 'user'
                roles.setdefault(Path(argv[index + 1]).resolve(), role)
    public = sorted({path.name for root in cpp for path in root.iterdir()
                     if path.is_file() and re.fullmatch(r'[a-z][a-z0-9_]*', path.name)})
    probe = ''.join(f'#if __has_include(<{name}>)\n#include <{name}>\n#endif\n'
                    for name in public)
    with tempfile.TemporaryDirectory(prefix='objc3-native-headers-') as temporary:
        work = Path(temporary)
        source, depfile = work / 'headers.cpp', work / 'headers.d'
        source.write_text(probe)
        _run(compiler, ['-x', 'c++', '-std=c++23', '-M', '-MT', 'headers',
                        '-MF', str(depfile), str(source)])
        reached = [path for path in dependencies(depfile, work, preserve_spelling=True) if path != source]
    records, sources, description = {}, {}, []

    def add(path: Path, name: str) -> None:
        if name in records:
            return
        stat = path.stat()
        records[name] = {'sha256': digest(path), 'bytes': stat.st_size}
        sources[name] = str(path)
        if len(records) > 100_000:
            raise ValueError('native header bundle exceeds its file budget')

    for index, root in enumerate(roots):
        builtin = root == resource / 'include'
        cxx = any(root.is_relative_to(base) for base in cpp) or (root / 'bits/c++config.h').is_file()
        role = 'resource' if builtin else 'cxx' if cxx else roles.get(root)
        if role is None:
            raise ValueError(f'compiler search root has no cc1 classification: {root}')
        description.append({'index': index, 'role': role})
        if not builtin and role in ('cxx', 'system', 'externc'):
            for path in complete_root_files(root):
                add(path, f'roots/{index:02d}/' + path.relative_to(root).as_posix())
    for path in reached:
        found = False
        for index, root in enumerate(roots):
            if not path.is_relative_to(root):
                continue
            found = True
            if root != resource / 'include':
                add(path, f'roots/{index:02d}/' + path.relative_to(root).as_posix())
            break
        if not found:
            raise ValueError(f'native header dependency escapes the compiler search roots: {path}')
    # Retain the distribution notices for the copied development headers.
    notices = [Path('/usr/share/doc/libc6-dev/copyright'),
               Path('/usr/share/doc/linux-libc-dev/copyright')]
    for root in cpp:
        if root.name.isdecimal():
            notices.append(Path(f'/usr/share/doc/libstdc++-{root.name}-dev/copyright'))
    for path in notices:
        if path.is_file():
            add(path, 'licenses/' + path.parent.name + '-copyright')
    if sum(row['bytes'] for row in records.values()) > 128 * 1024 * 1024:
        raise ValueError('native header bundle exceeds its byte budget')
    manifest = {'schema': SCHEMA, 'resource_version': resource.name,
                'roots': description, 'files': records}
    return {'sha256': identity(manifest), 'manifest': manifest, 'sources': sources}


def configuration(manifest: dict) -> str:
    version = manifest['resource_version']
    if not isinstance(version, str) or not re.fullmatch(r'[0-9][0-9A-Za-z.+_-]{0,47}', version):
        raise ValueError('unsupported compiler resource directory version')
    base = f'<CFGDIR>/../lib/clang/{version}'
    lines = ['# Native standard headers shipped with this Objective-C 3 SDK.', '-nostdinc++']
    for root in manifest['roots']:
        role = root['role']
        if (role not in ('cxx', 'system', 'externc', 'user', 'resource') or
                type(root['index']) is not int or not 0 <= root['index'] < 128):
            raise ValueError('invalid native header search root')
        name = f'{base}/include' if role == 'resource' else f'{base}/{BUNDLE}/roots/{root["index"]:02d}'
        if role == 'externc':
            lines.extend(['-Xclang -internal-externc-isystem', f'-Xclang "{name}"'])
        else:
            option = '-cxx-isystem' if role == 'cxx' else '-I' if role == 'user' else '-isystem'
            lines.append(f'{option} "{name}"')
    return '\n'.join(lines) + '\n'


def materialize(prefix: Path, plan: dict) -> dict:
    """Install one checked header bundle without retaining an intermediate copy."""
    manifest = plan['manifest']
    if manifest['schema'] != SCHEMA or identity(manifest) != plan['sha256']:
        raise ValueError('native header plan identity differs')
    config = configuration(manifest)
    resource = prefix / 'lib/clang' / manifest['resource_version']
    destination = resource / BUNDLE
    if destination.exists() or any((prefix / 'bin' / name).exists() for name in ('clang.cfg', 'clang++.cfg')):
        raise ValueError('native header destination/configuration already exists')
    resource.mkdir(parents=True, exist_ok=True)
    with tempfile.TemporaryDirectory(prefix='.native-headers-', dir=resource) as temporary:
        stage = Path(temporary) / BUNDLE
        stage.mkdir()
        for root in manifest['roots']:
            if root['role'] != 'resource':
                (stage / f'roots/{root["index"]:02d}').mkdir(parents=True)
        for name, record in manifest['files'].items():
            target = stage / name
            if not target.resolve().is_relative_to(stage.resolve()):
                raise ValueError('native header plan path escapes its bundle')
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(plan['sources'][name], target)
            if digest(target) != record['sha256'] or target.stat().st_size != record['bytes']:
                raise ValueError(f'native header input changed during installation: {name}')
        (stage / 'manifest.json').write_text(json.dumps(manifest, indent=2, sort_keys=True) + '\n')
        for name in ('clang.cfg', 'clang++.cfg'):
            (stage / name).write_text(config)
        os.replace(stage, destination)
    (prefix / 'bin').mkdir(exist_ok=True)
    for name in ('clang.cfg', 'clang++.cfg'):
        (prefix / 'bin' / name).write_text(config)
    return {'sha256': plan['sha256'], 'files': len(manifest['files']),
            'bytes': sum(row['bytes'] for row in manifest['files'].values()),
            'path': str(destination)}
