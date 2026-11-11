"""Native controls for bounded fork/exec coverage without losing child branches."""
import json
import os
from pathlib import Path
import subprocess
import pytest
from scripts.objc3c_toolchain.source_coverage import suite_profile_environment

@pytest.mark.parametrize('shared', [False, True])
@pytest.mark.parametrize('per_process', [False, True])
def test_real_fork_exec_profile_merging_retains_every_branch(tmp_path, per_process, shared):
    selected=os.environ.get('OBJC3C_BOOTSTRAP_LLVM_ROOT')
    if not selected:pytest.skip('requires the pinned LLVM bootstrap and profile runtime')
    tools={name:Path(selected)/'bin'/name for name in ('clang','llvm-profdata','llvm-cov')}
    if not all(path.is_file() for path in tools.values()):pytest.skip('requires pinned native coverage tools')
    source=Path(__file__).parent/'fixtures/source_coverage/fork_exec.c';binary=tmp_path/'control'
    flags=['-O1','-fprofile-instr-generate','-fcoverage-mapping','-fprofile-update=atomic',
           '-fuse-ld=lld','-Wl,--build-id=sha1']
    library=tmp_path/'libprofile.so'
    if shared:
        subprocess.run([str(tools['clang']),*flags,'-shared','-fPIC','-DPROFILE_SHARED_LIBRARY',
                        str(source),'-o',str(library)],check=True,capture_output=True,text=True,timeout=30)
    link=['-DPROFILE_SHARED_CLIENT',str(library),'-Wl,-rpath,$ORIGIN'] if shared else []
    subprocess.run([str(tools['clang']),*flags,*link,str(source),'-o',str(binary)],
                   check=True,capture_output=True,text=True,timeout=30)
    raw=tmp_path/'raw';raw.mkdir()
    environment=suite_profile_environment(dict(os.environ),raw)
    if per_process:environment['LLVM_PROFILE_FILE']=str(raw/'%p-%m.profraw')
    subprocess.run([str(binary)],env=environment,check=True,capture_output=True,text=True,timeout=10)
    profiles=sorted(raw.glob('*.profraw'))
    assert len(profiles)>=(3 if shared else 2) if per_process else len(profiles)==(2 if shared else 1)
    indexed=tmp_path/'merged.profdata'
    subprocess.run([str(tools['llvm-profdata']),'merge','--sparse',*map(str,profiles),'-o',str(indexed)],
                   check=True,capture_output=True,text=True,timeout=30)
    objects=['--object='+str(library)] if shared else []
    result=subprocess.run([str(tools['llvm-cov']),'export',str(binary),*objects,'--instr-profile='+str(indexed)],
                          check=True,capture_output=True,text=True,timeout=30)
    functions=json.loads(result.stdout)['data'][0]['functions']
    marker=next(row for row in functions if row['name']=='profile_branch')
    assert len(marker['branches'])==4 and all(row[4]>0 and row[5]>0 for row in marker['branches'])
