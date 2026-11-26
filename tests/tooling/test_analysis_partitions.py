"""Complete analysis is a disjoint union of bounded, independently verified actions."""
import json
import shutil

import pytest

from scripts.objc3c_toolchain.analysis_partition import FULL_PARTITIONS, partition_rows, partition_spec
from scripts.objc3c_implementation_assurance import evidence
from test_implementation_assurance import passing_bundle, REV


def test_native_variant_partitions_cover_uneven_catalogue_once():
    rows=[{'identity':f'{value:064x}'} for value in range(235)]
    shards=[partition_rows(rows,partition_spec(index,FULL_PARTITIONS))
            for index in range(FULL_PARTITIONS)]
    assert sorted(row['identity'] for shard in shards for row in shard)==[row['identity'] for row in rows]
    assert {len(shard) for shard in shards}=={29,30}
    assert partition_rows(rows)==rows
    for index,count in ((True,4),(-1,4),(4,4),(0,0),(0,16)):
        with pytest.raises(ValueError):partition_spec(index,count)


def test_partition_places_expensive_actions_first_and_balances_them():
    rows = [{'identity': f'{index:064x}', 'file': name}
            for index, name in enumerate(('AST.cpp', 'Parser.cpp', 'Fast.cpp', 'Short.cpp', 'Tiny.cpp'))]
    costs = {'default': 120, 'AST.cpp': 4200, 'Parser.cpp': 4200}
    shards = [partition_rows(rows, partition_spec(index, 2), seconds=costs) for index in range(2)]
    assert [[row['file'] for row in shard] for shard in shards] == [
        ['AST.cpp', 'Fast.cpp', 'Tiny.cpp'], ['Parser.cpp', 'Short.cpp']]
    assert partition_rows(list(reversed(rows)), partition_spec(0, 2), seconds=costs) == shards[0]


@pytest.mark.parametrize('fault',[None,'missing','duplicate','source','inventory','header','case','count','failed'])
def test_relocated_partition_gate_requires_all_current_inputs_and_every_action(tmp_path,fault):
    plan,_=passing_bundle(tmp_path,periodic=True)
    part=tmp_path/('part-0' if fault=='case' else 'part-1')
    outer_path=part/'result.json';inner_path=part/'analysis/qualification.json'
    outer=json.loads(outer_path.read_text());inner=json.loads(inner_path.read_text())
    if fault=='missing':shutil.rmtree(tmp_path/'part-0')
    elif fault=='duplicate':outer['partition']['index']=2
    elif fault=='count':outer['partition']['count']=2
    elif fault=='source':inner['source_build_id']=outer['source_build_id']='other-fork'
    elif fault=='inventory':inner['inventory'][-1]['file']='/producer/other.cpp'
    elif fault=='header':
        key=inner['inventory'][-1]['dependencies'][0];inner['inputs'][key]='d'*64
    elif fault=='case':inner['results'].clear();inner['analyzed_variants']=0;inner['fresh_native_executions']=0
    elif fault=='failed':outer['status']='FAIL'
    inner_path.write_text(json.dumps(inner))
    outer['artifacts']=[evidence.artifact(part,p) for p in evidence.evidence_files(part)]
    outer_path.write_text(json.dumps(outer))
    call=lambda:evidence.aggregate(plan,tmp_path,outcomes={'plan':'success','analysis':'success'},tested=REV['tested'])
    if fault:
        with pytest.raises(ValueError):call()
    else:
        assert (call()['status']=='PASS' and call()['analyzed_variants']==3 and
                call()['partitions']==FULL_PARTITIONS)
