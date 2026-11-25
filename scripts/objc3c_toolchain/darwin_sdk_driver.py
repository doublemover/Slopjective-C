#!/usr/bin/env python3
"""Compile and link using the installed experimental Objective-C 3 Darwin SDK."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import subprocess
import sys


def digest(path: Path) -> str:
    hashed = hashlib.sha256()
    with path.open('rb') as stream:
        while chunk := stream.read(1024 * 1024):
            hashed.update(chunk)
    return hashed.hexdigest()


def host_toolchain(manifest, run):
    requirements = manifest['host_requirements']
    xcrun = ['/usr/bin/xcrun', '--sdk', 'macosx']
    if (run('/usr/bin/xcodebuild', '-version') != requirements['xcode'] or
            run(*xcrun, '--show-sdk-version') != requirements['sdk_version']):
        raise ValueError('active Xcode and macOS SDK differ from the installed host lock')
    developer = Path(run('/usr/bin/xcode-select', '-p')).resolve(strict=True)
    sdk = Path(run(*xcrun, '--show-sdk-path')).resolve(strict=True)
    linker = Path(run(*xcrun, '--find', 'ld')).resolve(strict=True)
    if (not sdk.is_dir() or not sdk.is_relative_to(developer) or not linker.is_relative_to(developer) or
            digest(linker) != requirements['tools']['ld'] or
            digest(sdk / 'SDKSettings.json') != requirements['sdk_settings_sha256']):
        raise ValueError('active macOS SDK or linker differs from the installed host fingerprint')
    cxx = requirements['libcxx']
    config = (developer / cxx['config_relative']).resolve(strict=True)
    if (not config.is_relative_to(developer) or digest(config) != cxx['config_sha256'] or
            digest(sdk / 'usr/lib/libc++.tbd') != cxx['stub_sha256']):
        raise ValueError('active Xcode libc++ differs from the installed host fingerprint')
    return sdk, linker


def driver_command(prefix: Path, manifest: dict, sdk: Path, linker: Path, args: list[str], runtime: str):
    resource = manifest['resource_dir']
    if (manifest['profile'] != 'macos-arm64-registration-experimental' or
            manifest['target'] != 'arm64-apple-macos15.0' or
            manifest['compiler_runtime_flag'] != '-fobjc-runtime=gnustep-2.99' or
            not re.fullmatch(r'lib/clang/[1-9][0-9]*', resource)):
        raise ValueError('incompatible installed Objective-C 3 SDK')
    fixed = ('-fobjc-runtime', '-fobjc-version', '-fnext-runtime', '-fgnu-runtime', '-target', '--target',
             '-arch', '-isysroot', '--sysroot', '-resource-dir', '--ld-path', '-fuse-ld', '-mmacosx-version-min')
    for arg in args:
        if (arg.split('=', 1)[0] in fixed or arg.startswith(('@', '-isysroot', '-Wl,-syslibroot')) or
                arg in ('-lobjc', '-lobjc.A', '-Xclang', '-Xarch_arm64', '--config')):
            raise ValueError(f'option overrides the installed Objective-C 3 SDK or runtime: {arg}')
    argv = [str(prefix / 'bin/clang'), '--no-default-config']
    if args == ['--version']:
        return [*argv, *args]
    argv += ['--target=' + manifest['target'], '-isysroot', str(sdk), '-resource-dir', str(prefix / resource)]
    if (any(arg.endswith(('.m', '.mm', '.objc3')) or arg.startswith('-xobjective-c') for arg in args) or
            any(arg == '-x' and language.startswith('objective-c') for arg, language in zip(args, args[1:]))):
        argv += ['-fobjc-version=3', manifest['compiler_runtime_flag']]
    if any(arg in ('-c', '-S', '-E', '-fsyntax-only', '-M', '-MM') for arg in args):
        return [*argv, *args]
    library = prefix / 'lib'
    argv += ['--ld-path=' + str(linker), *args]
    if runtime == 'static':
        return [*argv, '-x', 'none', str(library / 'libobjc3darwin.a')]
    return [*argv, '-x', 'none', str(library / 'libobjc3darwin.dylib'), '-Xlinker', '-rpath', '-Xlinker', str(library)]


def main():
    parser = argparse.ArgumentParser(description=__doc__, allow_abbrev=False)
    parser.add_argument('--runtime-link', choices=('static', 'dynamic'), default='dynamic')
    options, args = parser.parse_known_args()
    if not args:
        parser.error('supply Clang arguments')
    try:
        if (platform.system(), platform.machine()) != ('Darwin', 'arm64'):
            raise ValueError('the installed compiler requires native Apple Silicon')
        prefix = Path(__file__).resolve().parents[1]
        manifest = json.loads((prefix / 'objc3-sdk.json').read_bytes())
        seal = manifest.pop('seal_sha256')
        if hashlib.sha256(json.dumps(manifest, sort_keys=True, separators=(',', ':')).encode()).hexdigest() != seal:
            raise ValueError('installed SDK manifest differs from its seal')
        env = {'PATH': '/usr/bin:/bin:/usr/sbin:/sbin', 'LC_ALL': 'C'}
        if 'DEVELOPER_DIR' in os.environ:
            env['DEVELOPER_DIR'] = os.environ['DEVELOPER_DIR']

        def run(*command):
            return subprocess.check_output(command, text=True, env=env).strip()

        sdk, linker = host_toolchain(manifest, run)
        argv = driver_command(prefix, manifest, sdk, linker, args, options.runtime_link)
        os.execve(argv[0], argv, env)
    except (OSError, ValueError, KeyError, subprocess.SubprocessError) as error:
        parser.exit(2, f'objc3c: {error}\n')


if __name__ == '__main__':
    main()
