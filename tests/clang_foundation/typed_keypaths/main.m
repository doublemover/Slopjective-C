#ifndef TYPED_KEYPATH_RUNTIME_PCH
#include "runtime.h"
#endif
#include <assert.h>
#include <string.h>

static int path_evaluations, root_evaluations, value_evaluations;
static int operand_sequence;
static WritableKeyPath<RuntimePathNode *, int> once_path(void) {
  ++path_evaluations;
  operand_sequence = operand_sequence * 10 + 1;
  return runtime_nested_path();
}
static RuntimePathNode *once_root(RuntimePathNode *root) {
  ++root_evaluations;
  operand_sequence = operand_sequence * 10 + 2;
  return root;
}
static int once_value(void) {
  ++value_evaluations;
  operand_sequence = operand_sequence * 10 + 3;
  return 42;
}
static WritableKeyPath<RuntimePathNode *, RuntimePathNode * _Nullable> once_optional_path(void) {
  ++path_evaluations; operand_sequence = operand_sequence * 10 + 1;
  return @keypath(RuntimePathNode, optionalNext.optionalValue);
}
static RuntimePathNode *once_optional_value(RuntimePathNode *value) {
  ++value_evaluations; operand_sequence = operand_sequence * 10 + 3;
  return value;
}
static KeyPath<RuntimePathNode *, int> once_readonly_return(void) {
  return once_path();
}
static int consume_readonly(KeyPath<RuntimePathNode *, int> path,
                            RuntimePathNode * _Nonnull root) {
  return KeyPathGet(path, root);
}

static void check_subscripts(void) {
  RuntimePathNode *root __attribute__((objc_precise_lifetime))=[RuntimePathNode newWithCount:11];
  RuntimePathNode *child __attribute__((objc_precise_lifetime))=[RuntimePathNode newWithCount:17];
  root.child=child;
  WritableKeyPath<RuntimePathNode*,int> path=runtime_nested_path();
  path_evaluations=root_evaluations=value_evaluations=operand_sequence=0;
  int reads=path_count_reads,sets=path_count_sets;
  assert(once_root(root)[once_path()]==17);
  assert(operand_sequence==21 && path_evaluations==1 && root_evaluations==1);
  assert(path_count_reads==reads+1 && path_count_sets==sets);
  operand_sequence=0;
  assert((once_root(root)[once_path()]=once_value())==42);
  assert(operand_sequence==213 && path_evaluations==2 && root_evaluations==2 && value_evaluations==1);
  assert(path_count_reads==reads+1 && path_count_sets==sets+1);
  reads=path_count_reads;sets=path_count_sets;
  assert((root[path]+=8)==50);
  assert(path_count_reads==reads+1 && path_count_sets==sets+1);
  assert(root[path]++==50);assert(++root[path]==52);
  assert(--root[path]==51);assert(root[path]--==51);assert(root[path]==50);
  long wide=44;assert((root[path]=wide)==44);
  WritableKeyPath<RuntimePathNode*,int> selected=path;
  root[selected]=(selected=@keypath(RuntimePathNode,count),39);
  assert(root.count==11 && child.count==39);
  selected=path;
  assert((root[selected]+=(selected=@keypath(RuntimePathNode,count),3))==42);
  assert(root.count==11 && child.count==42);
  KeyPath<RuntimePathNode*,int> read=path;assert(root[read]==42);
  PathPair pair={31,32};
  WritableKeyPath<RuntimePathNode*,PathPair> pair_path=runtime_pair_path();
  PathPair assigned=(root[pair_path]=pair);
  assert(assigned.first==31 && assigned.second==32);
  assert(root[pair_path].first==31 && root[pair_path].second==32);
  WritableKeyPath<RuntimePathNode*,long> first=@keypath(RuntimePathNode,pair.first);
  assert((root[first]+=11)==42 && root.pair.second==32);
  int before=path_destroyed;
  RuntimePathNode *snapshot_owner=[RuntimePathNode newWithCount:5];
#if __has_feature(objc_arc)
  __weak RuntimePathNode *witness=snapshot_owner;
  snapshot_owner[(snapshot_owner=nil,@keypath(RuntimePathNode,count))]=6;
  assert(witness==nil);
#else
  snapshot_owner[(objc_release(snapshot_owner),snapshot_owner=nil,@keypath(RuntimePathNode,count))]=6;
#endif
  assert(path_destroyed==before+1);
#if !__has_feature(objc_arc)
  objc_release(child);objc_release(root);
#endif
}

int main(void) {
  @autoreleasepool {
    RuntimePathNode *root = [RuntimePathNode newWithCount:1];
    RuntimePathNode *child = [RuntimePathNode newWithCount:17];
    root.child = child;
    WritableKeyPath<RuntimePathNode *, int> path = runtime_nested_path();
    const objc3_keypath_descriptor_v2 *metadata = KeyPathMetadata(path);
    assert(path_child_reads == 0 && path_count_reads == 0);
    assert(objc3_keypath_validate_v2(metadata) == OBJC3_KEYPATH_OK);
    assert(metadata->component_count == 2 &&
           !strcmp(metadata->path, "child.count"));
    const objc3_keypath_component_v2 *part = 0;
    assert(objc3_keypath_component_v2_at(metadata, 0, &part) == 0);
    assert(!strcmp(part->getter_selector, "readChild"));
    assert(part->flags & OBJC3_KEYPATH_COMPONENT_STRONG);
    assert(part->flags & OBJC3_KEYPATH_COMPONENT_NONNULL);
    assert(objc3_keypath_component_v2_at(metadata, 1, &part) == 0);
    assert(!strcmp(part->setter_selector, "writeCount:"));

    assert(KeyPathGet(once_path(), once_root(root)) == 17);
    assert(path_evaluations == 1 && root_evaluations == 1);
    assert(operand_sequence == 12);
    operand_sequence = 0;
    assert(path_child_reads == 1 && path_count_reads == 1);
    KeyPathSet(once_path(), once_root(root), once_value());
    assert(path_evaluations == 2 && root_evaluations == 2 &&
           value_evaluations == 1);
    assert(operand_sequence == 123);
    assert(path_child_reads == 2 && path_count_sets == 1);
    assert(KeyPathGet(path, root) == 42);
    assert(KeyPathGet(runtime_identity_path(), child) == 142);

    // A provider-owned record and a consumer literal share structural metadata
    // identity while their descriptor/global helper addresses may differ.
    int equal = 0;
    assert(objc3_keypath_equal_v2(metadata,
        KeyPathMetadata(@keypath(RuntimePathNode, child.count)), &equal) == 0);
    assert(equal);
    KeyPath<RuntimePathNode *, int> read = KeyPathAsReadOnly(path);
    assert(KeyPathGet(read, root) == 42);
    assert(KeyPathMetadata(read) == metadata);
    int evaluations_before_narrowing = path_evaluations;
    read = once_path();
    assert(path_evaluations == evaluations_before_narrowing + 1);
    assert(KeyPathGet(once_readonly_return(), root) == 42);
    assert(path_evaluations == evaluations_before_narrowing + 2);
    assert(consume_readonly(once_path(), root) == 42);
    assert(path_evaluations == evaluations_before_narrowing + 3);

    // The selected path is a value snapshot before later operands execute.
    // Changing the source record while evaluating the root cannot redirect it.
    WritableKeyPath<RuntimePathNode *, int> selected = runtime_nested_path();
    assert(KeyPathGet(selected,
        (selected = @keypath(RuntimePathNode, count), root)) == 42);
    selected = runtime_nested_path();
    KeyPathSet(selected, (selected = @keypath(RuntimePathNode, count), root), 43);
    assert(root.count == 1 && child.count == 43);
    KeyPathSet(runtime_nested_path(), root, 42);

    // A root snapshot remains alive while a later setter operand releases
    // the source owner, then dies after the enclosing full expression.
    int destroyed_before_snapshot = path_destroyed;
    RuntimePathNode *snapshot_owner = [RuntimePathNode newWithCount:5];
#if __has_feature(objc_arc)
    __weak RuntimePathNode *snapshot_witness = snapshot_owner;
    KeyPathSet(@keypath(RuntimePathNode, count), snapshot_owner,
        (snapshot_owner = nil,
         assert(snapshot_witness != nil), 6));
    assert(snapshot_witness == nil);
#else
    KeyPathSet(@keypath(RuntimePathNode, count), snapshot_owner,
        (objc_release(snapshot_owner),
         assert(path_destroyed == destroyed_before_snapshot), 6));
#endif
    assert(path_destroyed == destroyed_before_snapshot + 1);
    snapshot_owner = [RuntimePathNode newWithCount:7];
#if __has_feature(objc_arc)
    snapshot_witness = snapshot_owner;
#endif
    destroyed_before_snapshot = path_destroyed;
    @try {
      KeyPathSet(@keypath(RuntimePathNode, count), snapshot_owner, ({
#if __has_feature(objc_arc)
        snapshot_owner = nil;
        assert(snapshot_witness != nil);
#else
        objc_release(snapshot_owner);
        assert(path_destroyed == destroyed_before_snapshot);
#endif
        @throw root;
        8;
      }));
      assert(0);
    } @catch (RuntimePathNode *failure) {
      assert(failure == root);
    }
    assert(path_destroyed == destroyed_before_snapshot + 1);
#if __has_feature(objc_arc)
    assert(snapshot_witness == nil);
#endif

    // Nullable chains preserve their final nullable type and skip missing writes.
    WritableKeyPath<RuntimePathNode *, RuntimePathNode * _Nullable> optional_path =
        @keypath(RuntimePathNode, optionalNext.optionalValue);
    path_optional_reads = path_value_reads = 0;
    assert(KeyPathGet(optional_path, root) == nil);
    assert(path_optional_reads == 1 && path_value_reads == 0);
    path_evaluations = root_evaluations = value_evaluations = operand_sequence = 0;
    path_value_sets = path_optional_reads = 0;
    KeyPathSet(once_optional_path(), once_root(root), once_optional_value(root));
    assert(operand_sequence == 123 && path_evaluations == 1 && root_evaluations == 1 && value_evaluations == 1);
    assert(path_optional_reads == 1 && path_value_sets == 0);
    operand_sequence = 0;
    assert((once_root(root)[once_optional_path()] = once_optional_value(root)) == root);
    assert(operand_sequence == 213 && path_value_sets == 0);
    root.optionalNext = child;
    path_optional_reads = path_value_reads = 0;
    assert(KeyPathGet(@keypath(RuntimePathNode,
        optionalNext.optionalNext.optionalValue), root) == nil);
    assert(path_optional_reads == 2 && path_value_reads == 0);
    RuntimePathNode *leaf = [RuntimePathNode newWithCount:9];
    child.optionalValue = leaf;
    path_optional_reads = path_value_reads = 0;
    assert(KeyPathGet(optional_path, root) == leaf);
    assert(path_optional_reads == 1 && path_value_reads == 1);
    const objc3_keypath_descriptor_v2 *optional_metadata =
        KeyPathMetadata(optional_path);
    assert(objc3_keypath_validate_v2(optional_metadata) == 0);
    assert((optional_metadata->flags & OBJC3_KEYPATH_NULLABLE_TRAVERSAL) &&
           (optional_metadata->flags & OBJC3_KEYPATH_WRITABLE) &&
           (optional_metadata->flags & OBJC3_KEYPATH_NULLABLE_WRITES));
    path_value_sets = path_optional_reads = path_value_reads = 0;
    KeyPathSet(optional_path, root, root);
    assert(child.optionalValue == root);
    assert(path_optional_reads == 1 && path_value_sets == 1 && path_value_reads == 1);
    assert((root[optional_path] = leaf) == leaf);
    assert(child.optionalValue == leaf && path_value_sets == 2);
    path_optional_reads = path_value_sets = path_value_reads = 0;
    KeyPathSet(@keypath(RuntimePathNode, optionalNext.optionalNext.optionalValue), root, root);
    assert(path_optional_reads == 2 && path_value_sets == 0 && path_value_reads == 0);
    child.optionalNext = leaf;
    KeyPathSet(@keypath(RuntimePathNode, optionalNext.optionalNext.optionalValue), root, child);
    assert(leaf.optionalValue == child && path_value_sets == 1);
    leaf.optionalValue = nil;
    child.optionalNext = nil;
    path_throw_optional_setter = 1;
    @try {
      KeyPathSet(optional_path, root, root);
      assert(0);
    } @catch (RuntimePathNode *failure) {
      assert(failure == child);
    }
    path_throw_optional_setter = 0;
    assert(child.optionalValue == leaf);
    // A record routing prefix is read once and is never written back when
    // the mutation targets the object reached through its nullable field.
    PathRoute route = {child}; root.route = route;
    path_route_reads = path_route_sets = 0;
    KeyPathSet(@keypath(RuntimePathNode, route.next.optionalValue), root, leaf);
    assert(path_route_reads == 1 && path_route_sets == 0 && child.optionalValue == leaf);
    child.route = route;
    path_route_reads = path_route_sets = 0;
    KeyPathSet(@keypath(RuntimePathNode, optionalNext.route.next), root, leaf);
    assert(path_route_reads == 1 && path_route_sets == 1 && child.route.next == leaf);
    route.next = nil; root.route = route;
    path_route_reads = path_route_sets = path_value_sets = 0;
    KeyPathSet(@keypath(RuntimePathNode, route.next.optionalValue), root, root);
    assert(path_route_reads == 1 && path_route_sets == 0 && path_value_sets == 0);
    // The setter may remove the last external owner of its own receiver.
    int destroyed_before_nullable_set = path_destroyed;
    RuntimePathNode *transient = [RuntimePathNode newWithCount:5];
    root.optionalNext = transient;
#if __has_feature(objc_arc)
    __weak RuntimePathNode *transient_witness = transient;
    transient = nil;
#else
    objc_release(transient);
#endif
    path_clear_optional_parent = root;
    KeyPathSet(optional_path, root, leaf);
    assert(path_destroyed == destroyed_before_nullable_set + 1);
#if __has_feature(objc_arc)
    assert(transient_witness == nil);
#endif
    // The same pin must unwind after a throwing setter releases the owner.
    transient = [RuntimePathNode newWithCount:6];
    root.optionalNext = transient;
#if __has_feature(objc_arc)
    transient_witness = transient; transient = nil;
#else
    objc_release(transient);
#endif
    path_clear_optional_parent = root;
    path_throw_optional_setter = 1;
    @try {
      KeyPathSet(optional_path, root, leaf); assert(0);
    } @catch (RuntimePathNode *failure) { assert(failure != nil); }
    path_throw_optional_setter = 0;
    // Exception storage can retain the thrown receiver to the pool boundary.
    root.optionalNext = child;
    path_throw_optional_value = 1;
    @try {
      (void)KeyPathGet(optional_path, root);
      assert(0);
    } @catch (RuntimePathNode *failure) {
      assert(failure == child);
    }
    path_throw_optional_value = 0;
    int callback_value = 73;
    child.callback = ^{ return callback_value; };
    callback_value = 0;
    PathCallback callback = KeyPathGet(
        @keypath(RuntimePathNode, optionalNext.callback), root);
    assert(callback && callback() == 73);
    path_callback_sets = 0;
    KeyPathSet(@keypath(RuntimePathNode, optionalNext.callback), root, ^{ return 91; });
    callback = root[@keypath(RuntimePathNode, optionalNext.callback)];
    assert(callback && callback() == 91 && path_callback_sets == 1);
    root.optionalNext = nil;
    KeyPathSet(@keypath(RuntimePathNode, optionalNext.callback), root, ^{ return 17; });
    assert(path_callback_sets == 1);
    path_callback_reads = 0;
    assert(KeyPathGet(@keypath(RuntimePathNode, optionalNext.callback), root) == nil);
    assert(path_callback_reads == 0);
    child.callback = nil;
    child.optionalValue = nil;
#if !__has_feature(objc_arc)
    objc_release(leaf);
#endif

    // Native +1 getter results normalize to the ordinary +0 KeyPathGet
    // convention under MRC; the inner pool witnesses the balancing release.
    int destroyed_before_owned_getter = path_destroyed;
    @autoreleasepool {
      RuntimePathNode *produced =
          KeyPathGet(@keypath(RuntimePathNode, produced), root);
      assert(produced && produced.count == root.count);
      assert(objc3_keypath_component_v2_at(
          KeyPathMetadata(@keypath(RuntimePathNode, produced)), 0, &part) == 0);
      assert(part->flags & OBJC3_KEYPATH_COMPONENT_GETTER_RETURNS_RETAINED);
    }
    assert(path_destroyed == destroyed_before_owned_getter + 1);

    PathPair pair = {21, 22};
    KeyPathSet(runtime_pair_path(), root, pair);
    PathPair actual = KeyPathGet(runtime_pair_path(), root);
    assert(actual.first == 21 && actual.second == 22);
    path_pair_reads = path_pair_sets = 0;
    WritableKeyPath<RuntimePathNode *, long> first =
        @keypath(RuntimePathNode, pair.first);
    KeyPathSet(first, root, 41L);
    assert(path_pair_reads == 1 && path_pair_sets == 1);
    actual = KeyPathGet(runtime_pair_path(), root);
    assert(actual.first == 41 && actual.second == 22);
    assert(KeyPathGet(first, root) == 41);
    const objc3_keypath_descriptor_v2 *field_metadata = KeyPathMetadata(first);
    assert(!(field_metadata->flags & OBJC3_KEYPATH_KVC_COMPATIBLE));
    assert(objc3_keypath_component_v2_at(field_metadata, 1, &part) == 0);
    assert(part->flags & OBJC3_KEYPATH_COMPONENT_RECORD_FIELD);
    assert(!*part->getter_selector && !*part->setter_selector);
    path_throw_pair_getter = 1;
    int sets_before_failure = path_pair_sets;
    @try {
      KeyPathSet(first, root, 99L);
      assert(0);
    } @catch (RuntimePathNode *failure) {
      assert(failure == root);
    }
    path_throw_pair_getter = 0;
    assert(path_pair_sets == sets_before_failure);
    path_throw_pair_setter = 1;
    @try {
      KeyPathSet(first, root, 99L);
      assert(0);
    } @catch (RuntimePathNode *failure) {
      assert(failure == root);
    }
    path_throw_pair_setter = 0;
    actual = root.pair;
    assert(actual.first == 41L && actual.second == 22L);

    PathBits bits={1,-3,91};
    root.bits=bits;
    path_bits_reads=path_bits_sets=0;
    WritableKeyPath<RuntimePathNode *, unsigned> bits_path=runtime_bits_path();
    KeyPathSet(bits_path,root,10U);
    assert(path_bits_reads==1 && path_bits_sets==1);
    assert(KeyPathGet(bits_path,root)==2U);
    bits=root.bits;
    assert(bits.value==2U && bits.small==-3 && bits.sibling==91);
    WritableKeyPath<RuntimePathNode *, int> signed_bits=@keypath(RuntimePathNode,bits.small);
    KeyPathSet(signed_bits,root,-9);
    assert(KeyPathGet(signed_bits,root)==-9);
    bits=root.bits;
    assert(bits.value==2U && bits.small==-9 && bits.sibling==91);
    int bits_equal=0;
    assert(objc3_keypath_equal_v2(KeyPathMetadata(bits_path),
        KeyPathMetadata(@keypath(RuntimePathNode,bits.value)),&bits_equal)==0 && bits_equal);
    assert(objc3_keypath_component_v2_at(KeyPathMetadata(bits_path),1,&part)==0);
    assert(part->flags & OBJC3_KEYPATH_COMPONENT_RECORD_FIELD);
#ifdef __cplusplus
    PathInherited inherited;
    inherited.left=17; inherited.right=21; inherited.tail=91;
    root.inherited=inherited;
    path_inherited_reads=path_inherited_sets=0;
    auto inherited_path=runtime_inherited_path();
    KeyPathSet(inherited_path,root,42L);
    assert(path_inherited_reads==1 && path_inherited_sets==1);
    assert(KeyPathGet(inherited_path,root)==42L);
    inherited=root.inherited;
    assert(inherited.left==17 && inherited.right==42L && inherited.tail==91);
    assert(objc3_keypath_component_v2_at(KeyPathMetadata(inherited_path),1,&part)==0);
    assert(strstr(part->owner_type,"PathInherited") &&
           strstr(part->declaration_owner,"PathRight"));
#endif

    // Property copying remains the native setter's job. The key-path record
    // stores no captured object and does not bypass copy/strong semantics.
    KeyPathSet(@keypath(RuntimePathNode, copied), root, child);
    assert(path_copies == 1);
    RuntimePathNode *copied = KeyPathGet(@keypath(RuntimePathNode, copied), root);
    assert(copied != child && copied.count == child.count);
#if __has_feature(objc_arc)
    RuntimePathNode *ephemeral = [RuntimePathNode newWithCount:7];
    __weak RuntimePathNode *weak = ephemeral;
    WritableKeyPath<RuntimePathNode *, int> no_capture = [ephemeral selfPath];
    ephemeral = nil;
    assert(weak == nil);
    assert(KeyPathGet(no_capture, child) == 42);
    @autoreleasepool {
      RuntimePathNode *observed = [RuntimePathNode newWithCount:9];
      KeyPathSet(@keypath(RuntimePathNode, observer), root, observed);
      assert(KeyPathGet(@keypath(RuntimePathNode, observer), root) == observed);
      observed = nil;
    }
    assert(KeyPathGet(@keypath(RuntimePathNode, observer), root) == nil);
    const objc3_keypath_component_v2 *weak_part = 0;
    assert(objc3_keypath_component_v2_at(
        KeyPathMetadata(@keypath(RuntimePathNode, observer)), 0, &weak_part) == 0);
    assert((weak_part->flags & (OBJC3_KEYPATH_COMPONENT_WEAK |
                                OBJC3_KEYPATH_COMPONENT_NULLABLE)) ==
           (OBJC3_KEYPATH_COMPONENT_WEAK | OBJC3_KEYPATH_COMPONENT_NULLABLE));
#else
    objc_release(child);
    objc_release(root);
#endif
  }
  assert(path_destroyed >= 3);
  check_subscripts();
}
