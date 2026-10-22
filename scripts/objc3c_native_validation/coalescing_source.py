"""Execute the pinned compiler's scalar/C++ coalescing regression sources."""
from pathlib import Path

ROOT = Path(__file__).resolve().parents[2]
FIXTURES = ROOT / 'out/toolchains/objc3-development/src/clang/test/ObjC3/CodeGen'
INPUTS = (Path(__file__).resolve(), FIXTURES / 'optional-coalescing-runtime.m',
          FIXTURES / 'optional-coalescing-cxx-runtime.mm')


def qualify(run, directory: Path, compiler: Path, sdk: Path, report: dict) -> None:
    result = {'status': 'running', 'profile_conformance_claimed': False,
              'configurations': []}
    report['coalescing_source'] = result
    started = len(report['commands'])
    for standard in ('c17', 'c23', 'c++20', 'c++23'):
        cpp = standard.startswith('c++')
        source = INPUTS[2 if cpp else 1]
        for optimization in ('O0', 'O2'):
            name = f'coalescing-{standard}-{optimization}'
            executable = directory / 'coalescing-application'
            run(name + '-compile', [compiler, '--driver-mode=g++',
                '--no-default-config', '-x', 'objective-c++' if cpp else 'objective-c',
                '-std=' + standard, '-' + optimization, '-fobjc-version=3',
                '-fobjc-runtime=gnustep-2.2', '-fno-exceptions',
                '-fno-objc-exceptions', '-fuse-ld=lld', source,
                '-L' + str(sdk / 'lib'), '-lobjc',
                '-Wl,-rpath,' + str(sdk / 'lib'), '-o', executable])
            run(name + '-execute', [executable])
            result['configurations'].append({'standard': standard,
                'optimization': optimization, 'source': source.name})
    result.update(status='PASS', commands_passed=len(report['commands']) - started)
