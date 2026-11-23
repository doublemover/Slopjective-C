"""Record Apple Silicon bootstrap prerequisites for the Objective-C 3 port."""
from __future__ import annotations

import argparse
import hashlib
import json
import os
from pathlib import Path
import platform
import re
import struct
import subprocess


ROOT = Path(__file__).resolve().parents[2]
DEPLOYMENT_TARGET = '15.0'
TOOLS = ('clang', 'llc', 'llvm-ar', 'llvm-config', 'llvm-dwarfdump')
MACHO_TYPES = {'object': 1, 'executable': 2, 'dynamic_library': 6, 'debug_info': 10}


def require_apple_silicon() -> None:
    if platform.system() != 'Darwin' or platform.machine() != 'arm64':
        raise ValueError('the Darwin bootstrap probe requires an Apple Silicon Mac running native arm64 Python')


def verify_macho(path: Path, kind: str) -> dict:
    with path.open('rb') as stream:
        header = stream.read(32)
        if len(header) != 32:
            raise ValueError(f'missing Mach-O header at {path}')
        magic, cpu, _, file_type, commands, command_size, _, _ = struct.unpack('<8I', header)
        if (magic != 0xfeedfacf or cpu != 0x0100000c or file_type != MACHO_TYPES[kind] or
                commands == 0 or command_size < commands * 8 or path.stat().st_size < 32 + command_size):
            raise ValueError(f'expected an arm64 Mach-O {kind} at {path}')
        stream.seek(0)
        return {'path': str(path), 'format': 'Mach-O arm64', 'kind': kind,
                'sha256': hashlib.file_digest(stream, 'sha256').hexdigest()}


def probe(llvm_root: Path, output: Path) -> dict:
    require_apple_silicon()
    pin = json.loads((ROOT / 'toolchains/llvm/manifest.json').read_text())['upstream']
    version = pin['tag'].removeprefix('llvmorg-')
    llvm_root = llvm_root.resolve(strict=True)
    output = output.resolve()
    env = {key: value for key, value in os.environ.items()
           if not key.startswith('DYLD_') and key not in ('SDKROOT', 'MACOSX_DEPLOYMENT_TARGET')}
    env['LC_ALL'] = 'C'
    commands = []

    def run(*argv) -> str:
        command = list(map(str, argv))
        result = subprocess.run(command, env=env, capture_output=True, text=True, timeout=30)
        stdout, stderr = result.stdout.strip(), result.stderr.strip()
        commands.append({'argv': command, 'exit_status': result.returncode, 'stdout': stdout, 'stderr': stderr})
        if result.returncode:
            raise ValueError(f'{" ".join(command)} exited {result.returncode}: {stderr or stdout}')
        return stdout

    def find_apple(name: str) -> str:
        return run('/usr/bin/xcrun', '--sdk', 'macosx', '--find', name)

    macos = run('/usr/bin/sw_vers', '-productVersion')
    sdk_version = run('/usr/bin/xcrun', '--sdk', 'macosx', '--show-sdk-version')
    for name, detected in (('macOS', macos), ('macOS SDK', sdk_version)):
        if int(detected.split('.', 1)[0]) < int(DEPLOYMENT_TARGET.split('.', 1)[0]):
            raise ValueError(f'the Darwin bootstrap probe requires {name} >= {DEPLOYMENT_TARGET}; found {detected}')
    sdk = run('/usr/bin/xcrun', '--sdk', 'macosx', '--show-sdk-path')
    developer_dir = run('/usr/bin/xcode-select', '-p')
    if Path(developer_dir).name == 'CommandLineTools':
        developer_tools = run('/usr/sbin/pkgutil', '--pkg-info', 'com.apple.pkg.CLTools_Executables')
    else:
        developer_tools = run('/usr/bin/xcodebuild', '-version')
    apple_tools = {name: find_apple(name) for name in ('clang', 'ld', 'lipo', 'otool', 'dsymutil')}
    apple_clang = run(apple_tools['clang'], '--version')
    if not apple_clang.startswith('Apple clang version '):
        raise ValueError(f'expected Apple Clang from the active Xcode or Command Line Tools: {apple_tools["clang"]}')

    llvm = {}
    for name in TOOLS:
        tool = llvm_root / 'bin' / name
        if not tool.is_file() or not tool.resolve().is_relative_to(llvm_root):
            raise ValueError(f'expected LLVM {version} tool inside {llvm_root}: {tool}')
        if 'arm64' not in run(apple_tools['lipo'], '-archs', tool).split():
            raise ValueError(f'expected an arm64 slice in {tool}')
        actual = run(tool, '--version')
        if not re.search(r'(?m)^(?:[ \t]*(?:LLVM|clang) version )?' + re.escape(version) + r'(?=\s|$)', actual):
            raise ValueError(f'expected LLVM {version} at {tool}; found {actual}')
        if name == 'clang' and pin['commit'] not in actual:
            raise ValueError(f'expected Clang from upstream revision {pin["commit"]} at {tool}')
        llvm[name] = {'path': str(tool), 'version': actual}

    output.mkdir(parents=True)
    case_file = output / 'ProbeCase'
    case_file.touch()
    case_sensitive = not case_file.with_name('probecase').exists()
    case_file.unlink()
    staging = output / 'Staging'
    (staging / 'bin').mkdir(parents=True)
    (staging / 'lib').mkdir()
    implementation, consumer = output / 'answer.c', output / 'main.c'
    implementation.write_text('int answer(void) { return 42; }\n')
    consumer.write_text('int answer(void);\nint main(void) { return answer() == 42 ? 0 : 1; }\n')
    ir, obj = output / 'answer.ll', output / 'answer.o'
    archive, dylib = (staging / 'lib' / name for name in ('libanswer.a', 'libanswer.dylib'))
    defaults = ('-arch', 'arm64', '-isysroot', sdk, '-mmacosx-version-min=' + DEPLOYMENT_TARGET)
    clang = llvm['clang']['path']
    linker = '--ld-path=' + apple_tools['ld']
    run(clang, *defaults, '-O0', '-gdwarf-4', '-S', '-emit-llvm', implementation, '-o', ir)
    run(llvm['llc']['path'], '--filetype=obj', ir, '-o', obj)
    run(llvm['llvm-ar']['path'], '--format=darwin', 'rcs', archive, obj)
    if run(llvm['llvm-ar']['path'], 't', archive) != obj.name:
        raise ValueError(f'archive member inventory differs from {obj.name}: {archive}')
    run(clang, *defaults, linker, '-dynamiclib', obj, '-Wl,-install_name,@rpath/libanswer.dylib', '-o', dylib)
    run(clang, *defaults, linker, '-gdwarf-4', consumer, archive, '-o', staging / 'bin/static')
    run(clang, *defaults, linker, '-gdwarf-4', consumer, dylib, '-Wl,-rpath,@loader_path/../lib',
        '-o', staging / 'bin/dynamic')
    debug = output / 'static.dSYM'
    run(apple_tools['dsymutil'], staging / 'bin/static', '-o', debug)
    debug_file = debug / 'Contents/Resources/DWARF/static'
    dwarfdump = llvm['llvm-dwarfdump']['path']
    for path in (obj, debug_file):
        run(dwarfdump, '--verify', path)
        if 'DW_TAG_compile_unit' not in run(dwarfdump, '--debug-info', path):
            raise ValueError(f'missing DWARF compile unit in {path}')

    installed = staging.rename(output / 'Relocated install')
    artifacts = {'object': verify_macho(obj, 'object'), 'debug_info': verify_macho(debug_file, 'debug_info')}
    for relative, kind in (('bin/static', 'executable'), ('bin/dynamic', 'executable'),
                           ('lib/libanswer.dylib', 'dynamic_library')):
        path = installed / relative
        artifacts[relative] = verify_macho(path, kind)
        libraries = [line.strip().split(' (', 1)[0] for line in
                     run(apple_tools['otool'], '-L', path).splitlines()[1:]]
        expected = ['/usr/lib/libSystem.B.dylib']
        if relative != 'bin/static':
            expected.append('@rpath/libanswer.dylib')
        if sorted(libraries) != sorted(expected):
            raise ValueError(f'unexpected Darwin libraries recorded by {path}: {libraries}')
        artifacts[relative]['load_paths'] = libraries
    for name in ('static', 'dynamic'):
        run(installed / 'bin' / name)
    archive = installed / 'lib' / archive.name
    artifacts['archive'] = {'path': str(archive), 'format': 'ar', 'members': [obj.name],
                            'sha256': hashlib.sha256(archive.read_bytes()).hexdigest()}
    report = {'schema': 'objc3c.darwin-bootstrap-probe.v1', 'status': 'bootstrap-verified', 'support_truth': False,
              'checkout_revision': run('/usr/bin/git', '-C', ROOT, 'rev-parse', 'HEAD'),
              'probe_sha256': hashlib.sha256(Path(__file__).read_bytes()).hexdigest(),
              'platform_identity': {'os': 'macOS', 'version': macos, 'arch': 'arm64', 'developer_dir': developer_dir,
                                    'sdk_path': sdk, 'sdk_version': sdk_version, 'case_sensitive': case_sensitive},
              'apple_toolchain': {'paths': apple_tools, 'developer_tools_version': developer_tools,
                                  'clang_version': apple_clang},
              'llvm_identity': {'root': str(llvm_root), 'version': version, 'revision': pin['commit'], 'tools': llvm},
              'deployment_target': DEPLOYMENT_TARGET, 'installation_root': str(installed),
              'artifacts': artifacts, 'commands': commands}
    (output / 'report.json').write_text(json.dumps(report, indent=2) + '\n')
    return report


def main() -> int:
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--llvm-root', required=True, type=Path)
    parser.add_argument('--output-dir', required=True, type=Path, help='new directory for fixtures, artifacts and receipt')
    args = parser.parse_args()
    try:
        probe(args.llvm_root, args.output_dir)
    except (OSError, ValueError, subprocess.SubprocessError) as error:
        parser.exit(2, f'objc3c Darwin bootstrap: {error}\n')
    print(args.output_dir.resolve() / 'report.json')
    return 0


if __name__ == '__main__':
    raise SystemExit(main())
