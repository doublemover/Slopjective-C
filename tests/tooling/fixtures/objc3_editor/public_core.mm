#define throws(value) (value)
typedef Optional( int ) MaybeInt;
@protocol Probe
- (int)async:(int)async throws:(int)state;
@end
int async(int value){return throws(value);}
void exercise(void){Some( int,0); None( int );hasValue(None( int ));}
typedef Result( int,long ) IntOrLong;
int scalar_match(int input){MaybeInt maybe=Some(int,input);IntOrLong outcome=Ok(int,long,input);int selected=0;
match(maybe){case .Some(let payload):{selected=throws(payload);}case .None:{selected=0;}}
match(outcome){case .Ok(let success):{selected+=success;}case .Err(let error):{selected+=(int)error;}}
return isOk(outcome)?selected:async(input);}

#undef throws
@protocol Error @end
@protocol MessageProbe
- (int)checked:(int)value throws(id<Error>);
@end
int message_probe(id<MessageProbe> receiver){return tryForce([receiver checked:0]);}

void defer_probe(int *value) { defer { ++*value; } }

id managed_probe(id value) throws(id<Error>);
typedef void (^Action)(void);
Action managed_action(void) throws(id<Error>);
void managed_optional_probe(id input) {
  Optional(id) object = tryOptional(managed_probe(input));
  Optional(Action) action = tryOptional(managed_action());
  (void)hasValue(object);
  (void)hasValue(action);
}

#ifdef __cplusplus
void managed_transfer_probe(id input) {
  Optional(id) source = Some(id, input);
  Optional(id) copied = copyValue(source);
  Optional(id) moved = moveValue(copied);
  (void)hasValue(copied);
  (void)hasValue(moved);
}
#endif

void managed_assignment_probe(id input) {
  Optional(id) original = Some(id, input);
  Optional(id) implicit = original;
  implicit = None(id);
  implicit = original;
  (void)hasValue(implicit);
}

@protocol ManagedRuntimeProbe
- (Optional(id))managedEcho:(Optional(id))value;
@end
void managed_runtime_call_probe(id<ManagedRuntimeProbe> receiver,
    Optional(id) (^callback)(Optional(id))) {
  Optional(id) message_result = [receiver managedEcho:None(id)];
  Optional(id) block_result = callback(message_result);
  (void)hasValue(block_result);
}

Optional(id) managed_throwing_value(Optional(id) input) throws(id<Error>);
void managed_throwing_call_probe(id input) {
  Optional(id) throwing_result = tryForce(managed_throwing_value(Some(id, input)));
  (void)hasValue(throwing_result);
}

__attribute__((objc_root_class))
@interface ManagedDirectProbe
- (Optional(id))directEcho:(Optional(id))input throws(id<Error>) __attribute__((objc_direct));
@end
void managed_direct_probe(ManagedDirectProbe *receiver, id input) {
  Optional(id) direct_result = tryForce([receiver directEcho:Some(id, input)]);
  (void)hasValue(direct_result);
}

typedef Optional(id) ManagedInner;
void managed_nested_probe(id input) {
  Optional(ManagedInner) nested = Some(ManagedInner, Some(id, input));
  (void)hasValue(nested);
  match(nested) {
    case .Some(let borrowed): { (void)hasValue(borrowed); }
    case .None: { }
  }
}

int managed_capture_probe(id input) {
  Optional(id) captured = Some(id, input);
  int (^snapshot)(void) = ^int { return hasValue(captured); };
  return snapshot();
}

void managed_array_probe(id input, Action action) {
  Optional(id) managed_values[2] = {Some(id, input)};
  Optional(Action __weak) weak_action = Some(Action __weak, action);
  (void)hasValue(managed_values[0]);
  (void)hasValue(weak_action);
}

struct ManagedRecord { int marker; Optional(id) values[2]; };
struct ManagedRecord managed_record_echo(struct ManagedRecord value);
void managed_record_probe(id input) {
  struct ManagedRecord source = {42, {Some(id, input)}};
  struct ManagedRecord managed_record = managed_record_echo(source);
  managed_record = source;
  (void)hasValue(managed_record.values[0]);
}

@protocol ManagedHeapProbe
@property(nonatomic) Optional(id) stored;
@property(nonatomic) struct ManagedRecord record;
@end
int managed_heap_probe(id<ManagedHeapProbe> receiver, id input) {
  receiver.stored = Some(id, input);
  Optional(id) heap_value = receiver.stored;
  __block struct ManagedRecord shared_record = receiver.record;
  int (^shared_state)(void) = ^int { return shared_record.marker; };
  struct ManagedRecord (^snapshot)(void) = ^{ return shared_record; };
  (void)hasValue(heap_value);
  return shared_state() + snapshot().marker;
}

#ifdef __cplusplus
struct CxxInterfaceRecord { int marker; Optional(id) value; };
extern "C" CxxInterfaceRecord c_linked_record(CxxInterfaceRecord value);
CxxInterfaceRecord cpp_linked_record(CxxInterfaceRecord value);
void cxx_interface_probe(CxxInterfaceRecord source) {
  CxxInterfaceRecord cxx_record = cpp_linked_record(c_linked_record(source));
  (void)hasValue(cxx_record.value);
}
#endif
