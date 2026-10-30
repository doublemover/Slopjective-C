"""Fail-closed Linux sanitizer policy and paired, symbolized detector controls.

This proves detection for one immutable execution route. It does not prove that
all compiler, service or generated-client objects were instrumented. No ambient
suppression/option file is admitted; an exploratory run is not qualification.
"""
from __future__ import annotations

import hashlib
import json
import os
from pathlib import Path
import subprocess
import time

from .binary_packages import digest, write_json

# Only these developer values may enter promoting sanitizer subprocesses. Tool
# search paths, library loading, symbolizers and all sanitizer flags are owned.
ENVIRONMENT_ALLOWLIST = ('HOME', 'TMPDIR', 'TMP', 'TEMP', 'LANG', 'LC_CTYPE', 'TZ')
OPTIONS = {
    'ASAN_OPTIONS': 'detect_leaks=1:halt_on_error=1:abort_on_error=0:exitcode=86:symbolize=1',
    'LSAN_OPTIONS': 'detect_leaks=1:exitcode=86:symbolize=1',
    'UBSAN_OPTIONS': 'halt_on_error=1:exitcode=86:print_stacktrace=1:symbolize=1',
    'TSAN_OPTIONS': 'halt_on_error=1:exitcode=86:symbolize=1',
}
DETECTORS = {
    'asan': ('AddressSanitizer: heap-buffer-overflow', 'objc3_asan_detector', 86, ['16']),
    'ubsan': ('runtime error: signed integer overflow', 'objc3_ubsan_detector', 86, ['2147483647']),
    'lsan': ('LeakSanitizer: detected memory leaks', 'objc3_lsan_detector', 86, []),
    'tsan': ('ThreadSanitizer: data race', 'objc3_tsan_detector', 86, []),
}
FINDINGS = ('AddressSanitizer:', 'LeakSanitizer:', 'ThreadSanitizer:',
            'UndefinedBehaviorSanitizer:', 'runtime error:')


def strict_environment(compiler: Path, symbolizer: Path, *, library_dirs=(), base=None) -> dict:
    """Replace ambient policy, rather than appending flags or loading suppressions."""
    base = os.environ if base is None else base
    result = {key: base[key] for key in ENVIRONMENT_ALLOWLIST if key in base}
    result.update(OPTIONS)
    result.update(LC_ALL='C', SOURCE_DATE_EPOCH='0',
                  PATH=os.pathsep.join(dict.fromkeys((str(compiler.parent),
                      str(symbolizer.parent), *os.defpath.split(os.pathsep)))),
                  LLVM_SYMBOLIZER_PATH=str(symbolizer), ASAN_SYMBOLIZER_PATH=str(symbolizer))
    if library_dirs:
        result['LD_LIBRARY_PATH'] = os.pathsep.join(dict.fromkeys(str(path) for path in library_dirs))
    return result


def validate_detection(detector: str, source: Path, code: int, diagnostic: str, *, bad: bool):
    """Signals, loader failures, unrelated findings and zero-exit reports fail."""
    if not bad:
        if code != 0 or any(marker in diagnostic for marker in FINDINGS):
            raise ValueError(f'{detector}: corrected detector control was not clean')
        return
    marker, site, expected, _ = DETECTORS[detector]
    if (code != expected or marker not in diagnostic or site not in diagnostic or
            str(source) not in diagnostic):
        raise ValueError(f'{detector}: expected its symbolized source site and exit {expected}, got {code}')
    if any(text in diagnostic for text in ('unrecognized flag', 'failed to parse suppressions',
                                          'failed to open suppressions', 'Invalid path to external symbolizer')):
        raise ValueError(f'{detector}: invalid sanitizer configuration')


def require_clean_diagnostic(diagnostic: str):
    if any(marker in diagnostic for marker in FINDINGS):
        raise ValueError('sanitizer diagnostic in a supposedly successful project command')


class SanitizerQualification:
    """Run controls once per optimization within an immutable qualification job."""
    def __init__(self, compiler: Path, resource: Path, output: Path, sanitizer: str,
                 *, libraries=(), additional_inputs=(), symbolizer: Path | None = None, base_environment=None):
        if sanitizer not in ('address,undefined', 'thread', 'address'):
            raise ValueError('unsupported promoting sanitizer configuration')
        self.compiler, self.resource = compiler.resolve(strict=True), resource.resolve(strict=True)
        self.symbolizer = (symbolizer or self.resource.parents[2] / 'bin/llvm-symbolizer').resolve(strict=True)
        if not self.symbolizer.is_file() or not os.access(self.symbolizer, os.X_OK):
            raise ValueError('an explicit executable LLVM symbolizer is required')
        self.libraries = tuple(path.resolve(strict=True) for path in libraries)
        self.linker = next((path.resolve(strict=True) for path in
            (self.compiler.parent / 'ld.lld', self.symbolizer.parent / 'ld.lld') if path.is_file()), None)
        if self.linker is None:
            raise ValueError('the selected compiler route requires an owned LLD linker')
        self.loader = Path('/lib64/ld-linux-x86-64.so.2').resolve(strict=True)
        self.output, self.sanitizer = output.resolve(), sanitizer
        self.fixtures = Path(__file__).resolve().parents[2] / 'tests/tooling/fixtures/native_sanitizer_controls'
        self.environment = strict_environment(self.compiler, self.symbolizer,
            library_dirs=[path.parent for path in self.libraries if '.so' in path.name], base=base_environment)
        if any(self.environment.get(name) != value for name, value in OPTIONS.items()):
            raise ValueError('strict sanitizer options were weakened or replaced')
        permitted = {*ENVIRONMENT_ALLOWLIST, *OPTIONS, 'LC_ALL', 'SOURCE_DATE_EPOCH',
                     'PATH', 'LLVM_SYMBOLIZER_PATH', 'ASAN_SYMBOLIZER_PATH', 'LD_LIBRARY_PATH'}
        if set(self.environment) - permitted:
            raise ValueError('unapproved sanitizer subprocess environment')
        names = ('tsan',) if sanitizer == 'thread' else ('asan', 'ubsan', 'lsan')
        runtimes = [path for path in self.resource.rglob('libclang_rt.*')
                    if path.is_file() and any(name in path.name for name in names)]
        if not runtimes or not (self.resource / 'include/stddef.h').is_file():
            raise ValueError('explicit Linux sanitizer resources are required')
        target = subprocess.check_output([self.compiler, '--no-default-config', '-dumpmachine'],
            env=self.environment, text=True, timeout=30).strip()
        if target not in ('x86_64-unknown-linux-gnu', 'x86_64-pc-linux-gnu'):
            raise ValueError('detector controls require the native Linux x86-64 lane')
        self.target = target
        sources = list(self.fixtures.glob('*.cpp'))
        if len(sources) != 4:
            raise ValueError('the four source-owned detector controls are required')
        self.inputs = {str(path): digest(path) for path in
                       [self.compiler, self.symbolizer, self.linker, self.loader, Path(__file__), Path(digest.__code__.co_filename),
                        *runtimes, *self.libraries, *additional_inputs, *sources]}
        self._controls = {}
        self._expected_environment = dict(self.environment)
        self._expected_configuration = (self.compiler, self.resource, self.symbolizer,
                                        self.libraries, self.sanitizer)

    def _check_immutable(self):
        if (self.environment != self._expected_environment or
                (self.compiler, self.resource, self.symbolizer, self.libraries, self.sanitizer) !=
                self._expected_configuration):
            raise ValueError('sanitizer qualification configuration changed')

    def receipt(self):
        self._check_immutable()
        return {'schema': 'objc3c.strict-sanitizer-policy.v1', 'promoting': True,
                'environment_allowlist': list(ENVIRONMENT_ALLOWLIST),
                'environment': dict(self.environment), 'target': self.target,
                'link_driver': str(self.linker), 'elf_loader': str(self.loader),
                'inputs': dict(self.inputs), 'controls': {key: dict(value) for key, value in self._controls.items()},
                'suppression_policy': 'No suppression files or ambient sanitizer options are admitted.',
                'instrumentation_scope': 'Detector controls establish detection, not whole-program instrumentation.'}

    def finalize(self):
        self._check_immutable()
        if not self._controls:
            raise ValueError('no detector controls were executed')
        if any(digest(Path(path)) != value for path, value in self.inputs.items()):
            raise ValueError('sanitizer qualification inputs changed')
        for record in self._controls.values():
            if digest(Path(record['path'])) != record['sha256']:
                raise ValueError('detector-control receipt changed')
        return self.receipt()

    def ensure_controls(self, optimization: str) -> dict:
        self._check_immutable()
        if optimization not in ('O0', 'O1', 'O2', 'O3', 'Os', 'Oz'):
            raise ValueError('unsupported detector optimization')
        if any(digest(Path(path)) != value for path, value in self.inputs.items()):
            raise ValueError('sanitizer qualification inputs changed')
        if optimization in self._controls:
            record = self._controls[optimization]
            if digest(Path(record['path'])) != record['sha256']:
                raise ValueError('cached detector-control receipt changed')
            return dict(record)
        output = self.output / optimization
        output.mkdir(parents=True, exist_ok=False)
        receipt = output / 'qualification.json'
        detectors = ('tsan',) if self.sanitizer == 'thread' else (
            ('asan', 'lsan') if self.sanitizer == 'address' else ('asan', 'ubsan', 'lsan'))
        shared = [path for path in self.libraries if '.so' in path.name]
        flags = ['--no-default-config', '--driver-mode=g++', '--target='+self.target,
                 '-std=c++20', '-'+optimization, '-g', '-fno-omit-frame-pointer',
                 '-fno-sanitize-recover=undefined', '-fsanitize='+self.sanitizer,
                 '-resource-dir', str(self.resource), '-fuse-ld=lld', '-pthread']
        configuration = {'inputs': self.inputs, 'target': self.target, 'flags': flags,
                         'environment': self.environment, 'shared_libraries': list(map(str, shared))}
        identity = hashlib.sha256(json.dumps(configuration, sort_keys=True).encode()).hexdigest()
        report = {'schema': 'objc3c.sanitizer-detector-controls.v1', 'status': 'running',
                  'loaded_libraries': {},
                  'configuration_identity': identity, **configuration, 'commands': [], 'cases': []}

        def run(name, argv):
            log = output / (name+'.log')
            started = time.monotonic()
            with log.open('w') as stream:
                try:
                    result = subprocess.run(list(map(str, argv)), env=self.environment,
                        stdout=stream, stderr=subprocess.STDOUT, timeout=60)
                    code = result.returncode
                except subprocess.TimeoutExpired:
                    stream.write('\nDetector control timed out.\n')
                    code = 124
            report['commands'].append({'name': name, 'argv': list(map(str, argv)),
                'exit_code': code, 'signal': -code if code < 0 else None,
                'duration_seconds': round(time.monotonic()-started, 6),
                'log': str(log), 'log_sha256': digest(log)})
            write_json(receipt, report)
            if log.stat().st_size > 4 * 1024 * 1024:
                raise ValueError('detector diagnostic exceeds the bounded control output')
            return code, log.read_text(errors='replace')

        try:
            for detector in detectors:
                source = self.fixtures / (detector+'.cpp')
                for bad in (False, True):
                    name = detector+('-bad' if bad else '-good')
                    binary = output / name
                    command = [self.compiler, *flags, *(['-DOBJC3_DETECTOR_BAD=1'] if bad else []),
                               source, '-Wl,--no-as-needed', *shared, '-Wl,--as-needed', '-o', binary]
                    code, _ = run(name+'-build', command)
                    if code != 0:
                        raise ValueError('detector control compilation failed: '+name)
                    code, loaded = run(name+'-loader', [self.loader, '--list', binary])
                    if code or 'not found' in loaded:
                        raise ValueError('detector control has an invalid runtime loading route')
                    loaded_paths = set()
                    for line in loaded.splitlines():
                        value = line.split(' => ', 1)[-1].strip().rsplit(' (0x', 1)[0]
                        if not value.startswith('/'):
                            continue # Linux's virtual vdso has no file identity.
                        path = Path(value).resolve(strict=True)
                        loaded_paths.add(path)
                        signature = digest(path)
                        previous = self.inputs.get(str(path))
                        if previous is not None and previous != signature:
                            raise ValueError('loaded sanitizer dependency changed')
                        self.inputs[str(path)] = signature
                        report['loaded_libraries'][str(path)] = signature
                    if not set(shared).issubset(loaded_paths):
                        raise ValueError('selected shared runtime was not loaded by the detector control')
                    code, diagnostic = run(name+'-execute', [binary, *DETECTORS[detector][3]])
                    validate_detection(detector, source, code, diagnostic, bad=bad)
                    report['cases'].append({'detector': detector, 'classification': 'expected-detection' if bad else 'clean-control',
                        'binary_sha256': digest(binary), 'source_sha256': digest(source)})
            if any(digest(Path(path)) != value for path, value in self.inputs.items()):
                raise ValueError('sanitizer qualification inputs changed')
            identity = hashlib.sha256(json.dumps(configuration, sort_keys=True).encode()).hexdigest()
            report['configuration_identity'] = identity
            report['status'] = 'PASS'
        except BaseException as error:
            report.update(status='FAIL', error=str(error))
            raise
        finally:
            write_json(receipt, report)
        record = {'path': str(receipt), 'sha256': digest(receipt),
                  'configuration_identity': identity, 'status': 'PASS'}
        self._controls[optimization] = record
        return dict(record)
