#ifndef OBJC3_KEYPATH_CXX_RESOURCE_VALUES_H
#define OBJC3_KEYPATH_CXX_RESOURCE_VALUES_H
static int path_resource_live,path_resource_opened,path_resource_closed,path_resource_next;
static int path_resource_copies,path_resource_copy_fault,path_resource_gets,path_resource_sets;
static int path_resource_stored=31;
static bool path_resource_active[4096],path_resource_set_fault;
static bool path_resource_union_owned;
static int path_resource_union_stored;
static void path_resource_close(int handle) noexcept {
  if(handle==-1)return;
  assert(handle>0 && handle<4096 && path_resource_active[handle]);
  path_resource_active[handle]=false;--path_resource_live;++path_resource_closed;
}
typedef int PathResourceHandle __attribute__((objc_requires_cleanup(path_resource_close),objc_resource_family(PathResourceHandles),objc_invalid(-1)));
static int path_resource_open() {
  int handle=++path_resource_next;assert(handle<4096);
  path_resource_active[handle]=true;++path_resource_live;++path_resource_opened;return handle;
}
struct PathResourceValue {
  PathResourceHandle handle;int value;
  explicit PathResourceValue(int value):handle(path_resource_open()),value(value) {}
  PathResourceValue(const PathResourceValue &other):handle(-1),value(other.value) {
    if(++path_resource_copies==path_resource_copy_fault)throw 711;
    handle=path_resource_open();
  }
  PathResourceValue(PathResourceValue &&other) noexcept:handle(other.handle),value(other.value) {other.handle=-1;}
  ~PathResourceValue() noexcept {path_resource_close(handle);}
};
struct PathResourcePair {PathResourceValue first,second;};
union PathResourceUnion {
  struct ResourceArm {int tag;PathResourceHandle handle;int value;} owned;
  struct ScalarArm {int tag,value;} plain;
  explicit PathResourceUnion(int value,bool resource) {
    if(resource)owned={1,path_resource_open(),value};else plain={0,value};
  }
  PathResourceUnion(const PathResourceUnion &other) {
    if(++path_resource_copies==path_resource_copy_fault)throw 711;
    if(other.owned.tag)owned={1,path_resource_open(),other.owned.value};else plain=other.plain;
  }
  PathResourceUnion(PathResourceUnion &&other) noexcept {
    if(other.owned.tag){owned=other.owned;other.owned.handle=-1;}else plain=other.plain;
  }
  ~PathResourceUnion() noexcept {if(owned.tag)path_resource_close(owned.handle);}
  bool owns_resource() const {return owned.tag!=0;}
  int value() const {return owns_resource()?owned.value:plain.value;}
};
@interface RuntimeResourcePathNode : RuntimePathNode {
  __unsafe_unretained RuntimeResourcePathNode *_resourceNext;
}
@property(nonatomic) PathResourceValue resource;
@property(nonatomic) PathResourcePair resources;
@property(nonatomic) PathResourceUnion activeResource;
@property(nonatomic,assign) RuntimeResourcePathNode * _Nullable resourceNext;
@end
@implementation RuntimeResourcePathNode
@synthesize resourceNext=_resourceNext;
- (PathResourceUnion)activeResource {++path_resource_gets;return PathResourceUnion(path_resource_union_stored,path_resource_union_owned);}
- (void)setActiveResource:(PathResourceUnion)value {
  ++path_resource_sets;if(path_resource_set_fault)throw 712;path_resource_union_owned=value.owns_resource();path_resource_union_stored=value.value();
}
- (PathResourceValue)resource {++path_resource_gets;return PathResourceValue(path_resource_stored);}
- (void)setResource:(PathResourceValue)value {
  ++path_resource_sets;if(path_resource_set_fault)throw 712;path_resource_stored=value.value;
}
- (PathResourcePair)resources {++path_resource_gets;return {PathResourceValue(path_resource_stored),PathResourceValue(32)};}
- (void)setResources:(PathResourcePair)value {
  ++path_resource_sets;if(path_resource_set_fault)throw 712;path_resource_stored=value.first.value;
}
@end
static void keypath_cxx_resource_contract() {
  assert(!path_resource_live);
  @autoreleasepool {
    RuntimeResourcePathNode *node=[RuntimeResourcePathNode newWithCount:0];
    auto whole=@keypath(RuntimeResourcePathNode,resource);
    auto field=@keypath(RuntimeResourcePathNode,resource.value);
    auto pair=@keypath(RuntimeResourcePathNode,resources);
    auto nested=@keypath(RuntimeResourcePathNode,resources.first.value);
    assert(objc3_keypath_validate_v2(KeyPathMetadata(whole))==0);
    for(int route=0;route!=6;++route)for(int fault=0;fault!=7;++fault) {
      path_resource_copies=0;path_resource_copy_fault=fault;path_resource_set_fault=false;
      bool failed=false;
      try {
        if(route==0){auto value=KeyPathGet(whole,node);assert(value.value==path_resource_stored);}
        if(route==1){PathResourceValue value(47);KeyPathSet(whole,node,value);}
        if(route==2){KeyPathSet(field,node,48);assert(KeyPathGet(field,node)==48);}
        if(route==3){auto value=KeyPathGet(pair,node);assert(value.second.value==32);}
        if(route==4){PathResourcePair value={PathResourceValue(49),PathResourceValue(50)};node[pair]=static_cast<PathResourcePair &&>(value);}
        if(route==5){node[nested]=51;assert(node[nested]==51);}
      } catch(int error) {assert(error==711);failed=true;}
      assert(failed==(fault && path_resource_copies>=fault));
      assert(!path_resource_live && path_resource_opened==path_resource_closed);
    }
    for(int resource=0;resource!=2;++resource)for(int fault=0;fault!=7;++fault) {
      path_resource_union_owned=resource;path_resource_union_stored=73;path_resource_copies=0;path_resource_copy_fault=fault;
      bool failed=false;
      try {
        auto path=@keypath(RuntimeResourcePathNode,activeResource);
        PathResourceUnion value(73,resource);KeyPathSet(path,node,value);
        auto result=KeyPathGet(path,node);assert(result.owns_resource()==bool(resource) && result.value()==73);
      }catch(int error){assert(error==711);failed=true;}
      assert(failed==(fault && path_resource_copies>=fault));
      assert(!path_resource_live && path_resource_opened==path_resource_closed);
    }
    for(int resource=0;resource!=2;++resource)for(int fault=0;fault!=7;++fault) {
      path_resource_union_owned=resource;path_resource_union_stored=85;
      path_resource_copies=0;path_resource_copy_fault=fault;path_resource_set_fault=false;
      bool failed=false;
      try {
        if(resource) {
          KeyPathSet(@keypath(RuntimeResourcePathNode,activeResource.owned.value),node,86);
          assert(KeyPathGet(@keypath(RuntimeResourcePathNode,activeResource.owned.value),node)==86);
        }else {
          node[@keypath(RuntimeResourcePathNode,activeResource.plain.value)]=87;
          assert(node[@keypath(RuntimeResourcePathNode,activeResource.plain.value)]==87);
        }
      }catch(int error){assert(error==711);failed=true;}
      assert(failed==(fault && path_resource_copies>=fault));
      assert(path_resource_union_owned==bool(resource));
      assert(!path_resource_live && path_resource_opened==path_resource_closed);
    }
    path_resource_copy_fault=0;
    for(int resource=0;resource!=2;++resource) {
      path_resource_set_fault=true;bool failed=false;
      bool before_owned=path_resource_union_owned;int before_value=path_resource_union_stored;
      try {PathResourceUnion value(81,resource);KeyPathSet(@keypath(RuntimeResourcePathNode,activeResource),node,value);}
      catch(int error){assert(error==712);failed=true;}
      assert(failed && path_resource_union_owned==before_owned && path_resource_union_stored==before_value);
      assert(!path_resource_live && path_resource_opened==path_resource_closed);
      failed=false;
      try {
        if(before_owned)KeyPathSet(@keypath(RuntimeResourcePathNode,activeResource.owned.value),node,88);
        else KeyPathSet(@keypath(RuntimeResourcePathNode,activeResource.plain.value),node,89);
      }catch(int error){assert(error==712);failed=true;}
      assert(failed && path_resource_union_owned==before_owned && path_resource_union_stored==before_value);
      assert(!path_resource_live && path_resource_opened==path_resource_closed);
    }
    for(int route=0;route!=3;++route) {
      path_resource_set_fault=true;bool failed=false;int before=path_resource_stored;
      try {
        if(route==0){PathResourceValue value(61);KeyPathSet(whole,node,value);}
        if(route==1)KeyPathSet(field,node,62);
        if(route==2)KeyPathSet(nested,node,63);
      } catch(int error) {assert(error==712);failed=true;}
      assert(failed && path_resource_stored==before && !path_resource_live && path_resource_opened==path_resource_closed);
    }
    path_resource_set_fault=false;
    {
      auto missing=@keypath(RuntimeResourcePathNode,resourceNext.resource);
      PathResourceValue value(71);int gets=path_resource_gets,sets=path_resource_sets;
      KeyPathSet(missing,node,value);
      assert(path_resource_gets==gets && path_resource_sets==sets && path_resource_live==1);
      node.resourceNext=node;KeyPathSet(missing,node,value);node.resourceNext=nil;
      assert(path_resource_stored==71 && path_resource_live==1);
    }
    assert(!path_resource_live && path_resource_opened==path_resource_closed);
#if !__has_feature(objc_arc)
    objc_release(node);
#endif
    node=nil;
  }
  assert(!path_resource_live && path_resource_opened==path_resource_closed);
}
#endif
