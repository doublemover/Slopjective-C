"""Actual product execution, replay and same-path runtime substitution controls."""
from pathlib import Path
import json
import os
import shutil
import subprocess
import sys

import pytest

from scripts.objc3c_toolchain.product_evidence import ProductCommands, blob_bytes, phase_of, replay
from scripts.objc3c_toolchain.sanitizer_policy import SanitizerQualification


@pytest.mark.parametrize('entry', ['run_linux_feature_batch.py', 'qualify_async_c_abi.py',
                                  'product_evidence.py'])
def test_installed_helpers_import_without_a_source_checkout(tmp_path, entry):
    sources = Path(__file__).resolve().parents[2] / 'scripts/objc3c_toolchain'
    installed = tmp_path / 'installed'
    installed.mkdir()
    for original, name in [('source_graph.py', 'build.py'), *[(name, name) for name in (
            'sdk_example.py', 'binary_packages.py', 'sanitizer_policy.py',
            'llvm_ir_execution.py', 'product_evidence.py', 'run_linux_feature_batch.py',
            'qualify_async_c_abi.py')]]:
        shutil.copyfile(sources / original, installed / name)
    environment = dict(os.environ)
    environment.pop('PYTHONPATH', None)
    result = subprocess.run([sys.executable, '-B', '-S', str(installed / entry), '--help'],
        cwd=tmp_path, env=environment, capture_output=True, text=True, timeout=30)
    assert result.returncode == 0, result.stderr
    assert not list(installed.rglob('__pycache__'))


@pytest.mark.parametrize('name,argv,phase', [
    ('case-execute', ['program'], 'execute'), ('case-execution', ['program'], 'execute'),
    ('symbols', ['nm', 'object.o'], 'inspect'), ('compile', ['clang', '-c', 'input.cpp'], 'build')])
def test_product_stages_distinguish_native_execution_from_inspection(name, argv, phase):
    assert phase_of(name, argv) == phase


def test_retained_input_budget_and_fresh_output_are_explicit(tmp_path):
    source = tmp_path / 'fixture.h'
    source.write_bytes(b'x'*64)
    with pytest.raises(ValueError, match='budget'):
        ProductCommands(tmp_path / 'bounded', sources=(source,), budget=10)
    with pytest.raises(FileExistsError):
        ProductCommands(tmp_path / 'bounded')


@pytest.fixture
def native_product(tmp_path):
    selected = os.environ.get('OBJC3C_BOOTSTRAP_LLVM_ROOT')
    if not selected:
        pytest.skip('native product evidence requires the selected bootstrap LLVM')
    root = Path(selected)
    compiler, resource = root / 'bin/clang++', root / 'lib/clang/23'
    canonical, work, runtime_root = [tmp_path / name for name in ('sources', 'work', 'selected-runtime')]
    for path in (canonical, work, runtime_root):
        path.mkdir()
    header = canonical / 'contract.h'
    header.write_text('#define EXPECTED_VALUE 7\n')
    source = canonical / 'main.cpp'
    source.write_text('#include "contract.h"\nextern "C" int runtime_value();\nint main(){return runtime_value()==EXPECTED_VALUE ? 0 : 1;}\n')
    library_source = canonical / 'runtime.cpp'
    library_source.write_text('extern "C" int runtime_value(){return 7;}\n')
    library = runtime_root / 'libselected.so'
    flags = ['--no-default-config', '-g', '-O0', '-fsanitize=address,undefined',
             '-fno-sanitize-recover=undefined', '-resource-dir', str(resource)]
    subprocess.run([str(compiler), *flags, '-shared', '-fPIC', str(library_source),
                    '-Wl,-soname,libselected.so', '-o', str(library)], check=True, capture_output=True)
    gate = SanitizerQualification(compiler, resource, tmp_path / 'detectors', 'address,undefined', libraries=(library,))
    gate.ensure_controls('O0')
    products = ProductCommands(tmp_path / 'evidence', sources=(canonical,))
    obj, binary = work / 'main.o', work / 'program'
    def build():
        products.run(gate, 'client-compile', [compiler, *flags, '-I'+str(canonical),
            '-c', source, '-o', obj], cwd=work)
        products.run(gate, 'client-link', [compiler, *flags, '-fuse-ld=lld', obj,
            library, '-Wl,-rpath,'+str(runtime_root), '-o', binary], cwd=work)
    return products, gate, source, header, library_source, library, work, binary, flags, build


def test_actual_command_header_loader_and_binary_evidence_survives_workspace_removal(native_product, tmp_path):
    products, gate, source, header, _, library, work, binary, _, build = native_product
    build()
    row = products.run(gate, 'client-execute', [binary], cwd=work)
    receipt = products.finalize()
    assert row['exit_code'] == 0 and row['signal'] is None and row['seconds'] >= 0
    assert row['environment'] == gate.environment and row['log_sha256']
    assert str(library) in row['loaded_graph']['libraries']
    compile_row = products.report['commands'][0]
    assert str(header) in compile_row['headers'] and str(source) in compile_row['headers']
    assert blob_bytes(products.output, compile_row['headers'][str(header)]) == header.read_bytes()
    assert '-MD' in compile_row['argv'] and '-MF' in compile_row['argv']
    shutil.rmtree(work)
    assert products.finalize() == receipt
    assert replay(products.receipt, 'client-execute', tmp_path / 'replay')['status'] == 'PASS'


def test_same_path_wrong_runtime_is_rejected_before_product_execution(native_product):
    products, gate, _, _, library_source, library, work, binary, flags, build = native_product
    build()
    products.run(gate, 'original-execute', [binary], cwd=work)
    stamp = library.stat()
    library_source.write_text('extern "C" int runtime_value(){return 8;}\n')
    subprocess.run([str(gate.compiler), *flags, '-shared', '-fPIC', str(library_source),
                    '-Wl,-soname,libselected.so', '-o', str(library)], check=True, capture_output=True)
    os.utime(library, ns=(stamp.st_atime_ns, stamp.st_mtime_ns))
    with pytest.raises(ValueError, match='input changed|inputs changed|route changed'):
        products.run(gate, 'substituted-execute', [binary], cwd=work)
    failed = products.report['commands'][-1]
    assert failed['status'] == 'FAIL' and 'exit_code' not in failed
    assert json.loads(products.receipt.read_text())['status'] == 'FAIL'


@pytest.mark.parametrize('mutation', ['flags', 'resource'])
def test_weakened_product_compilation_cannot_reuse_successful_detector_controls(native_product, mutation):
    products, gate, source, _, _, _, work, _, flags, _ = native_product
    flags = list(flags)
    if mutation == 'flags':
        flags.remove('-fsanitize=address,undefined')
    else:
        flags[flags.index('-resource-dir')+1] = str(work)
    with pytest.raises(ValueError, match='sanitizer flags|resource directory'):
        products.run(gate, 'weakened-compile', [gate.compiler, *flags, '-c', source, '-o', work / 'bad.o'], cwd=work)
    assert not (work / 'bad.o').exists()


def test_actual_product_cannot_pass_with_a_missing_runtime_route(native_product):
    products, gate, _, _, _, _, work, _, flags, _ = native_product
    source, binary = work / 'independent.cpp', work / 'independent'
    source.write_text('int main(){return 0;}\n')
    products.run(gate, 'independent-build', [gate.compiler, *flags, source, '-o', binary], cwd=work)
    with pytest.raises(ValueError, match='required runtime route'):
        products.run(gate, 'missing-execute', [binary], cwd=work)
    assert 'exit_code' not in products.report['commands'][-1]


@pytest.mark.parametrize('mutation', ['log', 'blob'])
def test_changed_retained_product_evidence_cannot_be_promoted(native_product, mutation):
    products, gate, _, _, _, _, work, binary, _, build = native_product
    build()
    row = products.run(gate, 'client-execute', [binary], cwd=work)
    products.finalize()
    target = Path(row['log']) if mutation == 'log' else products.output / row['executable']['blob']
    target.write_bytes(target.read_bytes()+b'changed evidence')
    with pytest.raises(ValueError, match='artifact changed|diagnostic artifact changed'):
        products.finalize()
