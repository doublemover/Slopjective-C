#ifndef OBJC3_KEYPATH_CXX_VALUES_H
#define OBJC3_KEYPATH_CXX_VALUES_H
struct PathOwnedValue {
  static inline int live=0,reads=0,sets=0;
  static inline bool fail_copy=false,fail_set=false;
  int value=0;
  PathOwnedValue(){++live;}
  explicit PathOwnedValue(int value):value(value){++live;}
  PathOwnedValue(const PathOwnedValue &other):value(other.value){if(fail_copy)throw 17;++live;}
  PathOwnedValue &operator=(const PathOwnedValue &other){value=other.value;return *this;}
  ~PathOwnedValue(){--live;}
};
@interface RuntimeCppPathNode : RuntimePathNode {PathOwnedValue _owned;}
@property(nonatomic) PathOwnedValue owned;
@end
@implementation RuntimeCppPathNode
- (PathOwnedValue)owned {++PathOwnedValue::reads;return _owned;}
- (void)setOwned:(PathOwnedValue)value {++PathOwnedValue::sets;if(PathOwnedValue::fail_set)throw 29;_owned=value;}
@end
static void keypath_cxx_value_contract() {
  assert(PathOwnedValue::live==0);
  @autoreleasepool {
    RuntimeCppPathNode *node=[RuntimeCppPathNode newWithCount:0];
    const int baseline=PathOwnedValue::live;assert(baseline==1);
    {
      auto value=KeyPathGet(@keypath(RuntimeCppPathNode,owned),node);
      assert(value.value==0 && PathOwnedValue::live==baseline+1);
    }
    assert(PathOwnedValue::live==baseline);
    auto path=@keypath(RuntimeCppPathNode,owned.value);
    const int reads=PathOwnedValue::reads,sets=PathOwnedValue::sets;
    KeyPathSet(path,node,42);
    assert(PathOwnedValue::reads==reads+1 && PathOwnedValue::sets==sets+1);
    assert(KeyPathGet(path,node)==42 && PathOwnedValue::live==baseline);
    {
      PathOwnedValue value(43);KeyPathSet(@keypath(RuntimeCppPathNode,owned),node,value);
      assert(PathOwnedValue::live==baseline+1 && KeyPathGet(path,node)==43);
    }
    {
      auto whole=@keypath(RuntimeCppPathNode,owned);
      PathOwnedValue value(47);node[whole]=value;
      assert(node[whole].value==47);
      assert(PathOwnedValue::live==baseline+1);
      node[whole]=PathOwnedValue(43);
      assert(KeyPathGet(path,node)==43 && PathOwnedValue::live==baseline+1);
    }
    PathOwnedValue::fail_set=true;bool failed=false;
    try {KeyPathSet(path,node,44);}catch(int value){assert(value==29);failed=true;}
    PathOwnedValue::fail_set=false;assert(failed && PathOwnedValue::live==baseline && KeyPathGet(path,node)==43);
    PathOwnedValue::fail_copy=true;failed=false;
    try {(void)KeyPathGet(@keypath(RuntimeCppPathNode,owned),node);}catch(int value){assert(value==17);failed=true;}
    PathOwnedValue::fail_copy=false;assert(failed && PathOwnedValue::live==baseline);
#if !__has_feature(objc_arc)
    objc_release(node);
#endif
    node=nil;
  }
  assert(PathOwnedValue::live==0);
}
#endif
