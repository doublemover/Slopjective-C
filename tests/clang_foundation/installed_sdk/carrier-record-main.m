#include "carrier-record-call.h"
#include <stdio.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"record failure %d: %s\n",__LINE__,#x); return __LINE__; } } while(0)


static int weak_nil(RecordWeak value) {
  match(value) {
    case .Some(let object): { return object == (id)0; }
    case .None: { return -1; }
  }
  return -2;
}
static int ordinary(void) {
  {
    id object = record_object(0);
    struct RecordPair source = {42, record_value(1), Some(id __weak, object), {record_value(2),record_value(3)}};
    struct RecordPair copy = source;
    CHECK(copy.marker == 42 && !record_destroyed(1));
    source.first = None(id);
    source.values[0] = None(id);
    CHECK(!record_destroyed(1) && !record_destroyed(2));
    object = (id)0; CHECK(record_destroyed(0)==1 && weak_nil(copy.weak)==1);
    copy.first = None(id); CHECK(record_destroyed(1)==1);
    copy.values[0] = None(id); CHECK(record_destroyed(2)==1);
    copy = copy;
  }
  CHECK(record_destroyed(3)==1);
  {
    struct RecordPair source = {7,record_value(4),None(id __weak),{record_value(5)}};
    struct RecordPair target = {8,record_value(6),None(id __weak),{record_value(7)}};
    target = source;
    CHECK(target.marker==7 && record_destroyed(6)==1 && record_destroyed(7)==1);
    source.first=None(id);source.values[0]=None(id);
    CHECK(!record_destroyed(4) && !record_destroyed(5));
  }
  CHECK(record_destroyed(4)==1 && record_destroyed(5)==1);
  {
    struct RecordNested value = {11,{42,record_value(8),None(id __weak),{record_value(9)}},Err(id,id __weak,(id)0)};
    struct RecordNested copied=value;
    value.child.first=None(id);value.child.values[0]=None(id);
    CHECK(copied.child.marker==42 && !record_destroyed(8));
  }
  CHECK(record_destroyed(8)==1 && record_destroyed(9)==1);
  {
    struct RecordPair values[2] = {{0,record_value(10),None(id __weak),{record_value(11)}}, {0,record_value(12),None(id __weak),{}}};
    CHECK(hasValue(values[1].first));
  }
  CHECK(record_destroyed(10)==1 && record_destroyed(11)==1 && record_destroyed(12)==1);
  return 0;
}
static RecordStrong fail_native(void) { @throw record_object(16); }
static int failures(void) {
  @autoreleasepool {
    @try {
      struct RecordPair value={1,record_value(13),None(id __weak),{record_value(14),fail_native()}};
      CHECK(0);
    } @catch(id error) { CHECK(error && record_destroyed(13)==1 && record_destroyed(14)==1); }
  }
  CHECK(record_destroyed(16)==1);
  return 0;
}
#ifdef __cplusplus
struct FailCleanup { ~FailCleanup() noexcept(false) { throw 42; } };
struct BodyFails {
  RecordStrong first;
  RecordStrong values[2];
  BodyFails() : first(record_value(20)),values{record_value(21)} { throw 42; }
};
struct ArrayMember {
  RecordStrong values[2];
  ArrayMember(int key) : values{record_value(key),record_value(key+1)} {}
};
struct RecordBase { RecordStrong value; };
struct RecordDerived : RecordBase { RecordStrong tail; };
template<class T> struct RecordTemplate { T value; };
static bool record_copy_fails;
struct RecordBomb {
  RecordBomb() = default;
  RecordBomb(const RecordBomb &) { if (record_copy_fails) throw 73; }
};
static int block_failure(void) {
  RecordBomb bomb;
  RecordAction __unsafe_unretained raw = ^{ (void)bomb; };
  record_copy_fails=true;
  try {
    struct WithBlock { RecordStrong first; RecordBlock action; };
    WithBlock values[2] = {{record_value(60),None(RecordAction)},
      {record_value(61),Some(RecordAction,raw)}};
    CHECK(0);
  } catch(int error) {
    CHECK(error==73 && record_destroyed(60)==1 && record_destroyed(61)==1);
  }
  try {
    struct WithNativeBlock { RecordStrong first; RecordAction action; };
    WithNativeBlock value={record_value(62),raw}; CHECK(0);
  } catch(int error) { CHECK(error==73 && record_destroyed(62)==1); }
  record_copy_fails=false;
  return 0;
}
static RecordCall return_cleanup(int key) {
  FailCleanup fail;
  return record_make(key);
}
static RecordCall return_nrvo_outer(int key) {
  FailCleanup fail;
  RecordCall result=record_make(key);
  return result;
}
static RecordCall return_nrvo_inner(int key) {
  RecordCall result=record_make(key);
  FailCleanup fail;
  return result;
}
static RecordCall return_nrvo_body(int key) {
  RecordCall result=record_make(key);
  throw 42;
  return result;
}
struct ReturnParameter {
  RecordStrong value;
  ~ReturnParameter() noexcept(false) { throw 42; }
};
static RecordCall return_parameter(ReturnParameter parameter) { return record_make(85); }
static int return_failures(void) {
  RecordCall (*functions[])(int)={return_cleanup,return_nrvo_outer,return_nrvo_inner,return_nrvo_body};
  for(int i=0;i!=4;++i) {
    int key=80+i;
    try { functions[i](key); CHECK(0); }
    catch(int error) { CHECK(error==42 && record_destroyed(key)==1); }
  }
  try { return_parameter(ReturnParameter{record_value(84)}); CHECK(0); }
  catch(int error) { CHECK(error==42 && record_destroyed(84)==1 && record_destroyed(85)==1); }
  return 0;
}
static int cpp(void) {
  static_assert(!__is_trivially_copyable(RecordPair));
  static_assert(!__is_trivially_destructible(RecordPair));
  {
    RecordPair empty;
    CHECK(!hasValue(empty.first) && !hasValue(empty.values[0]));
  }
  {
    RecordPair original={0,record_value(17),None(id __weak),{record_value(18)}};
    RecordPair moved=static_cast<RecordPair&&>(original);
    moved.first=None(id);moved.values[0]=None(id);
    CHECK(record_destroyed(17)==1 && record_destroyed(18)==1);
    CHECK(hasValue(original.first));
  }
  try { BodyFails value; CHECK(0); }
  catch(int error) { CHECK(error==42 && record_destroyed(20)==1 && record_destroyed(21)==1); }
  {
    ArrayMember original(22), copy=original;
    original.values[0]=None(id);
    CHECK(!record_destroyed(22));
    ArrayMember moved=static_cast<ArrayMember&&>(copy);
    moved.values[0]=None(id); CHECK(record_destroyed(22)==1);
  }
  CHECK(record_destroyed(23)==1);
  try {
    RecordPair value={0,Some(id,(FailCleanup{},record_object(24))),None(id __weak),{record_value(25)}};
    CHECK(0);
  } catch(int error) { CHECK(error==42 && record_destroyed(24)==1 && record_destroyed(25)==1); }
  {
    RecordDerived source={{record_value(56)},record_value(57)}, copy=source;
    source.value=None(id); source.tail=None(id);
    CHECK(!record_destroyed(56) && !record_destroyed(57));
    auto closure=[copy] { return hasValue(copy.value) && hasValue(copy.tail); };
    copy.value=None(id); copy.tail=None(id);
    CHECK(closure() && !record_destroyed(56));
  }
  CHECK(record_destroyed(56)==1 && record_destroyed(57)==1);
  {
    RecordTemplate<RecordStrong> source{record_value(58)}, copy=source;
    source.value=None(id); CHECK(!record_destroyed(58));
    copy=static_cast<decltype(copy)&&>(copy);
    CHECK(hasValue(copy.value) && !record_destroyed(58));
  }
  CHECK(record_destroyed(58)==1);
  CHECK(block_failure()==0);
  CHECK(return_failures()==0);
  return 0;
}
#endif
static int calls(void) {
  {
    struct RecordCall source=record_make(32), copy=record_echo(source);
    CHECK(copy.marker==42 && !record_destroyed(32));
    record_replace(source,33); CHECK(record_destroyed(33)==1 && !record_destroyed(32));
    source.value=None(id); CHECK(!record_destroyed(32));
    copy.value=None(id); CHECK(record_destroyed(32)==1);
  }
  record_echo(record_make(34)); CHECK(record_destroyed(34)==1);
  @autoreleasepool {
    @try { record_native(record_make(38)); CHECK(0); }
    @catch(id error) { CHECK(error && record_destroyed(38)==1); }
  }
  CHECK(record_destroyed(39)==1);
  return 0;
}
static int typed_fail(void) __objc3_throws(id<Error>) {
  __objc3_throw((id<Error>)record_object(43));
}
static int typed_partial(void) __objc3_throws(id<Error>) {
  struct RecordPair value={__objc3_try(typed_fail()),record_value(40),None(id __weak),{record_value(41)}};
  return 0;
}
static int typed_after_prefix(void) __objc3_throws(id<Error>) {
  struct RecordPair value={0,record_value(44),None(id __weak),
    {record_value(45), (__objc3_try(typed_fail()),record_value(46))}};
  return 0;
}
static int typed_exits(void) {
  @autoreleasepool {
  __objc3_do {
    (void)__objc3_try(typed_partial()); CHECK(0);
  } __objc3_catch(id<Error> error) { CHECK(error && !record_destroyed(40) && !record_destroyed(41)); }
  __objc3_do {
    (void)__objc3_try(typed_after_prefix()); CHECK(0);
  } __objc3_catch(id<Error> error) { CHECK(error && record_destroyed(44)==1 && record_destroyed(45)==1); }
  }
  CHECK(record_destroyed(43)==2);
  return 0;
}
struct HybridRecord { RecordStrong carrier; id strong; id __weak weak; RecordAction action; };
static int hybrid(void) {
  id __weak observed;
  @autoreleasepool {
  {
    id object=record_object(54); observed=object;
    struct HybridRecord source={record_value(53),record_object(55),object,^{ if (!object) __builtin_trap(); }};
    struct HybridRecord copy=source;
    source.carrier=None(id); source.strong=(id)0; source.action=(RecordAction)0;
    CHECK(!record_destroyed(53) && !record_destroyed(55));
    copy=copy;
    copy.carrier=None(id); copy.strong=(id)0;
    CHECK(record_destroyed(53)==1 && record_destroyed(55)==1);
    object=(id)0; CHECK(!record_destroyed(54)); copy.action=(RecordAction)0;

  }
  }
  CHECK(record_destroyed(54)==1 && observed==(id)0);
  return 0;
}
static void fail_record_cleanup(int *key) { @throw record_object(*key+1); }
static struct RecordCall return_c_cleanup(int key) {
  int cleanup __attribute__((cleanup(fail_record_cleanup)))=key;
  return record_make(key);
}
static struct RecordCall return_c_named(int key) {
  int cleanup __attribute__((cleanup(fail_record_cleanup)))=key;
  struct RecordCall result=record_make(key);
  return result;
}
static RecordWeak fail_record_weak(void) { @throw record_object(89); }
static struct RecordCall return_partial_record(void) {
  return (struct RecordCall){42,record_value(88),fail_record_weak()};
}
static int native_return_failures(void) {
  @autoreleasepool {
    @try { return_c_cleanup(90); CHECK(0); }
    @catch(id error) { CHECK(error && record_destroyed(90)==1); }
    @try { return_c_named(92); CHECK(0); }
    @catch(id error) { CHECK(error && record_destroyed(92)==1); }
    @try { return_partial_record(); CHECK(0); }
    @catch(id error) { CHECK(error && record_destroyed(88)==1); }
  }
  CHECK(record_destroyed(89)==1 && record_destroyed(91)==1 && record_destroyed(93)==1);
  return 0;
}
int main(void) {
  CHECK(ordinary()==0);CHECK(failures()==0);CHECK(calls()==0);CHECK(typed_exits()==0);CHECK(hybrid()==0);CHECK(native_return_failures()==0);
#ifdef __cplusplus
  CHECK(cpp()==0);
#endif
  return 0;
}
