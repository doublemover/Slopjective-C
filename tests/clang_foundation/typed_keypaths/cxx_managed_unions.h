#ifndef OBJC3_KEYPATH_CXX_MANAGED_UNIONS_H
#define OBJC3_KEYPATH_CXX_MANAGED_UNIONS_H
#if __has_feature(objc_arc)
#include <new>
static int path_union_created,path_union_destroyed,path_union_payload_live;
static int path_union_copies,path_union_copy_fault,path_union_gets,path_union_sets;
static bool path_union_set_fault;
@interface RuntimeUnionOwner : RuntimePathNode
@end
@implementation RuntimeUnionOwner
+ (instancetype)newWithCount:(int)value {
  ++path_union_created;return [super newWithCount:value];
}
- (void)dealloc {++path_union_destroyed;}
@end
struct PathUnionPayload {
  int value;
  explicit PathUnionPayload(int value):value(value){++path_union_payload_live;}
  PathUnionPayload(const PathUnionPayload &other):value(other.value) {
    if(++path_union_copies==path_union_copy_fault)throw 756;
    ++path_union_payload_live;
  }
  PathUnionPayload(PathUnionPayload &&other) noexcept:value(other.value){++path_union_payload_live;}
  ~PathUnionPayload() noexcept {--path_union_payload_live;}
};
union PathManagedUnion {
  struct Owned {
    int tag;RuntimeUnionOwner *owner;__weak RuntimeUnionOwner *observer;
    PathCallback callback;PathUnionPayload payload;
  } owned;
  struct Plain {int tag,value;} plain;
  PathManagedUnion():plain{0,0} {}
  explicit PathManagedUnion(int value,bool resource) {
    if(resource) {
      RuntimeUnionOwner *owner=[RuntimeUnionOwner newWithCount:value];
      ::new((void *)&owned) Owned{1,owner,owner,^{return value;},PathUnionPayload(value)};
    }else ::new((void *)&plain) Plain{0,value};
  }
  PathManagedUnion(const PathManagedUnion &other) {
    if(other.plain.tag)::new((void *)&owned) Owned(other.owned);
    else ::new((void *)&plain) Plain(other.plain);
  }
  PathManagedUnion(PathManagedUnion &&other) noexcept {
    if(other.plain.tag)::new((void *)&owned) Owned(static_cast<Owned &&>(other.owned));
    else ::new((void *)&plain) Plain(other.plain);
  }
  PathManagedUnion &operator=(PathManagedUnion other) noexcept {
    this->~PathManagedUnion();::new((void *)this) PathManagedUnion(static_cast<PathManagedUnion &&>(other));return *this;
  }
  // The implicit exception specification must be resolved before admission.
  ~PathManagedUnion(){if(plain.tag)owned.~Owned();}
  int value() const {return plain.tag?owned.payload.value:plain.value;}
};
struct PathUnionEnvelope {int prefix;PathManagedUnion active;};
@interface RuntimeManagedUnionPathNode : RuntimePathNode {
  PathManagedUnion _active;PathUnionEnvelope _envelope;
  __unsafe_unretained RuntimeManagedUnionPathNode *_unionNext;
}
@property(nonatomic) PathManagedUnion active;
@property(nonatomic) PathUnionEnvelope envelope;
@property(nonatomic,assign) RuntimeManagedUnionPathNode *_Nullable unionNext;
@end
@implementation RuntimeManagedUnionPathNode
@synthesize unionNext=_unionNext;
- (PathManagedUnion)active {++path_union_gets;return _active;}
- (void)setActive:(PathManagedUnion)value {
  ++path_union_sets;if(path_union_set_fault)throw 757;_active=static_cast<PathManagedUnion &&>(value);
}
- (PathUnionEnvelope)envelope {++path_union_gets;return _envelope;}
- (void)setEnvelope:(PathUnionEnvelope)value {
  ++path_union_sets;if(path_union_set_fault)throw 757;_envelope.active=static_cast<PathManagedUnion &&>(value.active);_envelope.prefix=value.prefix;
}
@end
static void keypath_cxx_managed_union_contract() {
  assert(!path_union_payload_live);
  @autoreleasepool {
    RuntimeManagedUnionPathNode *node=[RuntimeManagedUnionPathNode newWithCount:0];
    auto whole=@keypath(RuntimeManagedUnionPathNode,active);
    auto field=@keypath(RuntimeManagedUnionPathNode,active.owned.payload.value);
    auto nested=@keypath(RuntimeManagedUnionPathNode,envelope.active.owned.payload.value);
    const bool arms[]={false,true};
    for(bool resource:arms) {
      PathManagedUnion value(81,resource);KeyPathSet(whole,node,value);
      auto result=KeyPathGet(whole,node);assert(result.value()==81 && bool(result.plain.tag)==resource);
      if(resource) {
        assert(result.owned.owner==result.owned.observer && result.owned.callback()==81);
        KeyPathSet(field,node,82);assert(KeyPathGet(field,node)==82);
        PathUnionEnvelope envelope{19,PathManagedUnion(83,true)};node.envelope=envelope;
        node[nested]=84;assert(node[nested]==84 && node.envelope.prefix==19);
        auto weak=@keypath(RuntimeManagedUnionPathNode,active.owned.observer);
        assert(KeyPathGet(weak,node)==result.owned.owner);
        auto callback=@keypath(RuntimeManagedUnionPathNode,active.owned.callback);
        assert(KeyPathGet(callback,node)()==81);
      }
    }
    for(int route=0;route!=4;++route)for(int fault=1;fault!=8;++fault) {
      path_union_copy_fault=0;node.active=PathManagedUnion(90,true);
      PathUnionEnvelope envelope{20,PathManagedUnion(91,true)};node.envelope=envelope;
      const int baseline=path_union_payload_live;path_union_copies=0;path_union_copy_fault=fault;
      bool failed=false;
      try {
        if(route==0){auto copy=KeyPathGet(whole,node);assert(copy.value()==90);}
        if(route==1)KeyPathSet(field,node,92);
        if(route==2)node[nested]=93;
        if(route==3){PathManagedUnion value(94,true);KeyPathSet(whole,node,value);}
      }catch(int error){assert(error==756);failed=true;}
      assert(failed==(path_union_copies>=fault));
      assert(path_union_payload_live==baseline);
    }
    path_union_copy_fault=0;
    for(int route=0;route!=3;++route) {
      node.active=PathManagedUnion(100,true);path_union_set_fault=true;bool failed=false;
      try {
        if(route==0){PathManagedUnion value(101,true);KeyPathSet(whole,node,value);}
        if(route==1)KeyPathSet(field,node,102);
        if(route==2)node[nested]=103;
      }catch(int error){assert(error==757);failed=true;}
      path_union_set_fault=false;assert(failed && KeyPathGet(field,node)==100);
    }
    {
      const int gets=path_union_gets,sets=path_union_sets;
      PathManagedUnion value(105,true);
      KeyPathSet(@keypath(RuntimeManagedUnionPathNode,unionNext.active),node,value);
      assert(path_union_gets==gets && path_union_sets==sets);
      node.unionNext=node;KeyPathSet(@keypath(RuntimeManagedUnionPathNode,unionNext.active.owned.payload.value),node,106);
      node.unionNext=nil;assert(KeyPathGet(field,node)==106);
    }
    node=nil;
  }
  assert(!path_union_payload_live && path_union_created==path_union_destroyed);
}
#else
static void keypath_cxx_managed_union_contract() {}
#endif
#endif
