"""Deliver the pinned bootstrap's native compiler-rt, including sanitizers/FMV."""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import re
import shutil
import subprocess

from .linux_host import host_identity, native_target, require_elf


REQUIRED = ('builtins', 'asan', 'asan_static', 'asan_cxx', 'tsan', 'tsan_cxx',
            'ubsan_standalone', 'ubsan_standalone_cxx', 'profile', 'fuzzer',
            'fuzzer_no_main')


def digest(path):
    with path.open('rb') as stream:
        return hashlib.file_digest(stream, 'sha256').hexdigest()


def archive_members(path: Path, machine: int) -> list[str]:
    """Admit every real ELF object without executing a tool from the archive.

    Thin archives, bitcode, foreign/32-bit/big-endian objects, truncated records
    and non-object payloads are rejected. GNU and BSD name tables are metadata.
    """
    members, strings = [], b''
    size = path.stat().st_size
    if not 8 < size <= 64 * 1024**2:
        raise ValueError('native compiler runtime archive exceeds its bounded size')
    with path.open('rb') as stream:
        if stream.read(8) != b'!<arch>\n':
            raise ValueError('native compiler runtimes require a regular archive')
        while stream.tell() < size:
            header = stream.read(60)
            if len(header) != 60 or header[58:60] != b'`\n':
                raise ValueError('native compiler runtimes have a truncated archive header')
            try:
                name, count = header[:16].decode('ascii').strip(), int(header[48:58])
            except (UnicodeDecodeError, ValueError) as error:
                raise ValueError('native compiler runtime archive header is invalid') from error
            end = stream.tell() + count
            if count < 0 or end > size:
                raise ValueError('native compiler runtimes have a truncated archive member')
            if name == '//':
                strings = stream.read(count)
            elif name not in ('/', '/SYM64/'):
                if name.startswith('#1/'):
                    length = int(name[3:])
                    if not 0 < length <= count:
                        raise ValueError('native compiler runtime BSD member name is invalid')
                    name = stream.read(length).rstrip(b'\0').decode('utf-8')
                elif name.startswith('/'):
                    offset = int(name[1:]);terminator = strings.find(b'/\n', offset)
                    if offset < 0 or terminator < offset:
                        raise ValueError('native compiler runtime GNU member name is invalid')
                    name = strings[offset:terminator].decode('utf-8')
                else:
                    name = name.removesuffix('/')
                if not name.startswith('__.SYMDEF'):
                    if not name or name in ('.', '..') or Path(name).name != name:
                        raise ValueError('native compiler runtime member name escaped its archive')
                    available = end - stream.tell()
                    elf = stream.read(min(64, available))
                    if (len(elf) != 64 or elf[:6] != b'\x7fELF\x02\x01' or
                            int.from_bytes(elf[16:18], 'little') != 1 or
                            int.from_bytes(elf[18:20], 'little') != machine):
                        raise ValueError('native compiler runtimes contain a foreign or non-relocatable ELF member: ' + name)
                    members.append(name)
            stream.seek(end)
            if count % 2 and stream.read(1) != b'\n':
                raise ValueError('native compiler runtime archive padding is invalid')
    if not members:
        raise ValueError('native compiler runtimes contain no native objects')
    return members


def discover(repo: Path, bootstrap: Path) -> dict:
    compiler = bootstrap / 'bin/clang'
    require_elf(compiler)
    expected = json.loads((repo / 'toolchains/llvm/manifest.json').read_text())['upstream']['tag'].removeprefix('llvmorg-')
    environment = {key: value for key, value in os.environ.items()
                   if key not in ('LD_LIBRARY_PATH', 'LD_PRELOAD', 'LD_AUDIT')}
    environment['LD_LIBRARY_PATH'] = str(bootstrap / 'lib')
    def query(*arguments):
        return subprocess.check_output([str(compiler), '--no-default-config', *arguments],
                                       text=True, timeout=30, env=environment).strip()
    if not re.search(r'(?<![\d.])' + re.escape(expected) + r'(?![\d.])', query('--version')):
        raise ValueError('native compiler runtimes require the pinned bootstrap compiler')
    resource = Path(query('-print-resource-dir')).resolve(strict=True)
    if not resource.is_relative_to(bootstrap.resolve(strict=True)):
        raise ValueError('native compiler runtimes escaped the pinned bootstrap')
    source = Path(query('--target=' + native_target(), '--rtlib=compiler-rt', '-print-libgcc-file-name')).resolve(strict=True)
    if (not source.is_relative_to(resource) or not re.fullmatch(
            r'libclang_rt\.builtins(?:-(?:x86_64|aarch64))?\.a', source.name)):
        raise ValueError('native compiler runtime escaped the explicit bootstrap resource directory')
    host = host_identity()
    return catalogue(resource, source.parent, host, digest(compiler))


def catalogue(resource: Path, directory: Path, host: dict, compiler_sha256: str) -> dict:
    """Admit the native compiler-rt directory without importing another target."""
    resource, directory = resource.resolve(strict=True), directory.resolve(strict=True)
    if not directory.is_relative_to(resource / 'lib'):
        raise ValueError('native compiler runtimes escaped their resource lib directory')
    files = []
    for path in sorted(directory.iterdir()):
        # LLVM's distribution also places Flang libraries here. This product
        # owns compiler-rt, not every language runtime in the bootstrap.
        if not path.name.startswith(('libclang_rt.', 'clang_rt.')):
            continue
        if path.is_symlink() or not path.is_file():
            raise ValueError('native compiler runtimes require regular owned files')
        record = {'source': str(path), 'resource_relative': path.relative_to(resource).as_posix(),
                  'sha256': digest(path)}
        if re.fullmatch(r'libclang_rt\.[A-Za-z0-9_.+-]+\.a', path.name):
            record['members'] = archive_members(path, host['elf_machine'])
        elif re.fullmatch(r'libclang_rt\.[A-Za-z0-9_.+-]+\.so', path.name):
            require_elf(path, host['host'])
            if int.from_bytes(path.read_bytes()[16:18], 'little') != 3:
                raise ValueError('native compiler runtime shared library is not ELF ET_DYN')
        elif path.name in ('clang_rt.crtbegin.o', 'clang_rt.crtend.o'):
            require_elf(path, host['host'])
            if int.from_bytes(path.read_bytes()[16:18], 'little') != 1:
                raise ValueError('native compiler runtime startup object is not ELF ET_REL')
        elif re.fullmatch(r'libclang_rt\.[A-Za-z0-9_.+-]+\.a\.syms', path.name):
            if path.stat().st_size > 4 * 1024**2 or '\0' in path.read_text():
                raise ValueError('native compiler runtime symbol list is invalid')
            if not path.with_suffix('').is_file():
                raise ValueError('native compiler runtime symbol list has no archive')
        else:
            raise ValueError('unrecognized native compiler runtime file: ' + path.name)
        files.append(record)
    names = {Path(item['source']).name for item in files}
    missing = [name for name in REQUIRED if not any(
        re.fullmatch(r'libclang_rt\.' + re.escape(name) + r'(?:-(?:x86_64|aarch64))?\.a', candidate)
        for candidate in names)]
    if missing:
        raise ValueError('native compiler runtime inventory is incomplete: ' + ', '.join(missing))
    payload = {'schema': 'objc3c.native-compiler-runtimes.v1', **host,
               'resource_version': resource.name, 'files': files,
               'bootstrap_compiler_sha256': compiler_sha256}
    return {**payload, 'sha256': hashlib.sha256(json.dumps(payload, sort_keys=True).encode()).hexdigest()}


def materialize(prefix: Path, record: dict) -> dict:
    payload = {key: value for key, value in record.items() if key != 'sha256'}
    if (record['schema'] != 'objc3c.native-compiler-runtimes.v1' or
            record['host'] != host_identity()['host'] or
            hashlib.sha256(json.dumps(payload, sort_keys=True).encode()).hexdigest() != record['sha256']):
        raise ValueError('native compiler runtime inventory changed before SDK assembly')
    if not re.fullmatch('[0-9]+', record['resource_version']):
        raise ValueError('native compiler runtimes destination escaped its resource directory')
    root = prefix / 'lib/clang' / record['resource_version']
    installed = []
    for item in record['files']:
        relative, source = Path(item['resource_relative']), Path(item['source'])
        if (relative.is_absolute() or '..' in relative.parts or
                len(relative.parts) < 3 or relative.parts[0] != 'lib'):
            raise ValueError('native compiler runtimes destination escaped its resource directory')
        if source.is_symlink() or digest(source) != item['sha256']:
            raise ValueError('native compiler runtimes changed before SDK assembly')
        destination = root / relative
        if destination.exists() or destination.is_symlink():
            raise ValueError('native compiler runtime destination is already populated')
        destination.parent.mkdir(parents=True, exist_ok=True)
        if not destination.parent.resolve().is_relative_to(prefix.resolve()):
            raise ValueError('native compiler runtime destination escaped its SDK')
        shutil.copyfile(source, destination)
        if digest(destination) != item['sha256']:
            raise ValueError('staged native compiler runtimes differ from their pinned input')
        installed.append(destination.relative_to(prefix).as_posix())
    return {**record, 'installed': installed}
