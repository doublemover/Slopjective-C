"""Render the independent typed model as ordinary native ObjC3 witnesses."""
from __future__ import annotations

from .semantic_model import Operation, transform, validate

HEADER = '''#pragma once
typedef Result(int, int) OracleResult;
id oracle_make(int key) __attribute__((ns_returns_retained));
int oracle_value(id value);
int oracle_alive(int key);
int oracle_destroyed(int key);
OracleResult oracle_step(OracleResult value, int delta);
'''

PROVIDER = '''#include "oracle.h"
#include <objc/runtime.h>
extern "C" id class_createInstance(Class, __SIZE_TYPE__) __attribute__((ns_returns_retained));
extern "C" id object_dispose(id);
static int Dead[33];
static __weak id Watchers[33];
__attribute__((objc_root_class))
@interface OracleObject { Class isa; @public int key; }
+ (id)newWithKey:(int)value;
@end
@implementation OracleObject
+ (id)newWithKey:(int)value {
  OracleObject *object = class_createInstance(self, 0);
  object->key = value;
  return object;
}
- (void)dealloc { ++Dead[key]; object_dispose(self); }
@end
id oracle_make(int key) {
  id object = [OracleObject newWithKey:key];
  Watchers[key] = object;
  return object;
}
int oracle_value(id object) { return ((OracleObject *)object)->key; }
int oracle_alive(int key) { return Watchers[key] != (id)0; }
int oracle_destroyed(int key) { return Dead[key]; }
static int private_bias() { return ORACLE_PRIVATE_BIAS; }
OracleResult oracle_step(OracleResult value, int delta) {
  match(value) {
  case .Ok(let number): {
    return Ok(int, int, number + delta + private_bias() + ORACLE_BAD_PROVIDER);
  }
  case .Err(let error): {
    return Err(int, int, error + ORACLE_BAD_ERROR);
  }
  }
}
'''


def provider(*, bias=0, mutant=''):
    if bias not in (0, 1) or mutant not in ('', 'provider', 'error'):
        raise ValueError('unsupported provider control')
    return ('#define ORACLE_PRIVATE_BIAS '+str(bias)+'\n'
            '#define ORACLE_BAD_PROVIDER '+str(int(mutant == 'provider'))+'\n'
            '#define ORACLE_BAD_ERROR '+str(int(mutant == 'error'))+'\n'+PROVIDER)


def consumer(program: list[Operation], *, transport='source', relation='', mutant='') -> str:
    validate(program)
    if relation:
        program = transform(program, relation)
    if transport not in ('source', 'pch', 'interface') or mutant not in ('', 'tag', 'defer', 'destructor'):
        raise ValueError('unsupported consumer/control configuration')
    prefix = 'renamed_' if relation == 'rename' else ''
    function = lambda name: '@oracle.Provider.'+name if transport == 'interface' else name
    names = {key: prefix+key for key in ('optional', 'result', 'object')}
    count = sum(op.kind == 'new' for op in program)
    begin = '@import oracle.Provider;\n' if transport == 'interface' else '#include "oracle.h"\n'
    begin += '''#include <cstdio>
#include <vector>
typedef Optional(int) Number;
typedef Result(int, int) Outcome;
typedef Optional(id) Object;
static std::vector<std::vector<int>> States;
static std::vector<int> Events;
struct Guard {
  int event;
  Guard(int begin, int end, bool fail = false) : event(end) {
    Events.push_back(begin);
    if (fail) throw 99;
  }
  ~Guard() { Events.push_back(event); ORACLE_DUPLICATE }
};
static void add_number(std::vector<int> &row, Number value) {
  if let number = value { row.push_back(1); row.push_back(number); }
  else { row.push_back(0); row.push_back(0); }
}
static void add_result(std::vector<int> &row, Outcome value) {
  match(value) {
  case .Ok(let number): { row.push_back(1); row.push_back(number); }
  case .Err(let error): { row.push_back(0); row.push_back(error); }
  }
}
static void print_values(const std::vector<int> &row) {
  std::printf("[");
  for (unsigned index = 0; index < row.size(); ++index)
    std::printf("%s%d", index ? "," : "", row[index]);
  std::printf("]");
}
int main() {
  Number OPTIONAL[2] = {None(int), None(int)};
  Outcome RESULT[2] = {Err(int, int, 0), Err(int, int, 0)};
  Object OBJECT[2] = {None(id), None(id)};
  auto snapshot = [&] {
    std::vector<int> row;
    for (int index = 0; index != 2; ++index) add_number(row, OPTIONAL[index]);
    for (int index = 0; index != 2; ++index) add_result(row, RESULT[index]);
    for (int index = 0; index != 2; ++index) {
      if let object = OBJECT[index] { row.push_back(ORACLE_VALUE(object)); }
      else { row.push_back(0); }
    }
    for (int key = 1; key <= ORACLE_COUNT; ++key) {
      row.push_back(ORACLE_ALIVE(key)); row.push_back(ORACLE_DESTROYED(key));
    }
    States.push_back(row);
  };
'''
    begin = (begin.replace('ORACLE_DUPLICATE', 'Events.push_back(event);' if mutant == 'destructor' else '')
        .replace('OPTIONAL', names['optional']).replace('RESULT', names['result']).replace('OBJECT', names['object'])
        .replace('ORACLE_VALUE', function('oracle_value')).replace('ORACLE_ALIVE', function('oracle_alive'))
        .replace('ORACLE_DESTROYED', function('oracle_destroyed')).replace('ORACLE_COUNT', str(count)))
    lines, next_id = [begin], 0
    for index, op in enumerate(program):
        o, r, m = (names[key]+'['+str(op.slot)+']' for key in ('optional', 'result', 'object'))
        if op.kind == 'some': statement = f'{o} = Some(int, {op.value});'
        elif op.kind == 'none': statement = f'{o} = None(int);'
        elif op.kind == 'copy_optional': statement = f'{o} = {names["optional"]}[{op.other}];'
        elif op.kind == 'alias_optional':
            value = f'({op.value}+0)+0' if relation == 'pure-grouping' else f'{op.value}+(0+0)'
            statement = f'{{ Number *alias = &{o}; *alias = Some(int, {value}); }}'
        elif op.kind == 'self_optional': statement = f'{o} = {o};'
        elif op.kind in ('ok', 'err'):
            ok = (op.kind == 'ok') != (mutant == 'tag')
            statement = f'{r} = {"Ok" if ok else "Err"}(int, int, {op.value});'
        elif op.kind == 'copy_result': statement = f'{r} = {names["result"]}[{op.other}];'
        elif op.kind == 'step': statement = f'{r} = {function("oracle_step")}({r}, {op.value});'
        elif op.kind == 'new':
            next_id += 1
            statement = f'{m} = Some(id, {function("oracle_make")}({next_id}));'
        elif op.kind == 'copy_object': statement = f'{m} = {names["object"]}[{op.other}];'
        elif op.kind == 'clear': statement = f'{m} = None(id);'
        elif op.kind == 'scope':
            event = '' if mutant == 'defer' else f'Events.push_back({3000+index});'
            statement = f'{{ Guard first({1000+index}, {4000+index}); defer {{ {event} }} Events.push_back({2000+index}); }}'
        elif op.kind == 'partial_throw':
            event = '' if mutant == 'defer' else f'Events.push_back({3000+index});'
            statement = (f'try {{ Guard first({1000+index}, {4000+index}); defer {{ {event} }} '
                f'Guard failed({2000+index}, {6000+index}, true); }} '
                f'catch (int code) {{ if (code != 99) return 3; Events.push_back({5000+index}); }}')
        lines += [f'  // operation {index}: {op.kind}\n' if relation == 'comments' else '', '  '+statement+'\n  snapshot();\n']
    lines += [f'  {names["object"]}[1] = None(id); {names["object"]}[0] = None(id); snapshot();\n',
        '''  std::printf("{\\"states\\":[");
  for (unsigned index = 0; index < States.size(); ++index) {
    if (index) std::printf(","); print_values(States[index]);
  }
  std::printf("],\\"events\\":"); print_values(Events); std::printf("}\\n");
  return 0;
}
''']
    return ''.join(lines)
