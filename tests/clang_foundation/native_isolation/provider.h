#ifndef NATIVE_ISOLATION_PROVIDER_H
#define NATIVE_ISOLATION_PROVIDER_H
#include "objc3_runtime_services.h"
@protocol Sendable @end
typedef struct HopPacket { int value; double weights[3]; } HopPacket;
__attribute__((objc_root_class))
@interface HopObject <Sendable> { Class isa; }
+ (id)new __attribute__((nothrow));
- (int)number __attribute__((nothrow));
@end
actor class HopCounter : HopObject { int value; }
- (int)add:(int)amount
    __attribute__((nothrow, objc_executor(named("native.isolation"))));
- (int)read __attribute__((nothrow));
- (HopPacket)transform:(HopPacket)packet __attribute__((nothrow));
@end
@interface HopObject (ClassEntries)
+ (int)classScalar:(int)value __attribute__((nothrow, objc_executor(main)));
+ (HopPacket)classPacket:(HopPacket)value __attribute__((nothrow, objc_executor(global)));
+ (int)classFrame:(int)value __objc3_async_frame
    __attribute__((nothrow, objc_executor(named("native.isolation"))));
+ (HopObject *)newOnMain __attribute__((nothrow, ns_returns_retained, objc_executor(main)));
+ (void)classVoid __attribute__((nothrow, objc_executor(main)));
@end
@interface HopCounter (ActorSuper)
- (int)actorTouch:(int)value __attribute__((nothrow));
@end
actor class HopSuperActor : HopCounter
- (int)actorSuperRoundtrip:(objc3_executor_t *)caller __objc3_async_frame
    __attribute__((objc_nonisolated));
@end
@interface HopSuperBase : HopObject
- (int)superValue:(int)value __attribute__((nothrow, objc_executor(main)));
+ (int)superClassValue:(int)value
    __attribute__((nothrow, objc_executor(named("native.isolation"))));
- (HopPacket)superPacket:(HopPacket)value __attribute__((nothrow, objc_executor(main)));
- (HopObject *)newSuperObject:(int)value
    __attribute__((nothrow, ns_returns_retained, objc_executor(main)));
@end
@interface HopSuperChild : HopSuperBase
- (int)superRoundtrip:(objc3_executor_t *)caller __objc3_async_frame;
+ (int)superClassRoundtrip:(objc3_executor_t *)caller __objc3_async_frame;
- (int)superPacketRoundtrip:(objc3_executor_t *)caller __objc3_async_frame;
- (int)superObjectRoundtrip:(objc3_executor_t *)caller __objc3_async_frame;
@end
@interface HopSuperChild (SuperCategory)
- (int)superCategoryRoundtrip:(objc3_executor_t *)caller __objc3_async_frame;
@end
#ifdef __cplusplus
extern "C" {
#endif
void hop_configure(objc3_executor_t *caller, objc3_executor_t *main,
                   objc3_executor_t *global, objc3_executor_t *named,
                   uint64_t actor_identity);
void *hop_new_counter(void);
int hop_arguments(void);
int hop_entries(void);
int hop_cleanups(void);
int hop_destroyed(void);
int hop_roundtrip(HopCounter *, objc3_executor_t *) __objc3_async_frame;
int hop_nil_roundtrip(void) __objc3_async_frame;
HopObject *hop_owned_roundtrip(void) __objc3_async_frame;
int hop_packet_roundtrip(HopCounter *, objc3_executor_t *) __objc3_async_frame;
int hop_packet_arguments(void);
int hop_packet_entries(void);
HopPacket hop_packet_on_main(HopPacket)
    __attribute__((nothrow, objc_executor(main)));
int hop_owned_packet_roundtrip(objc3_executor_t *) __objc3_async_frame;
int hop_cxx_packet_roundtrip(objc3_executor_t *) __objc3_async_frame;
int hop_cxx_packet_live(void);
int hop_cxx_packet_entries(void);
void hop_cxx_packet_fail_move(int);
int hop_cxx_value_roundtrip(objc3_executor_t *) __objc3_async_frame;
int hop_cxx_value_live(void);
int hop_task_abi_roundtrip(HopCounter *, objc3_executor_t *) __objc3_async_frame;
int hop_task_abi_entries(void);
int hop_task_abi_failure_frame(int, objc3_executor_t *) __objc3_async_frame;
objc3_async_frame hop_task_abi_super_frame(int, objc3_executor_t *);
int hop_task_abi_class_roundtrip(objc3_executor_t *) __objc3_async_frame;
int hop_cxx_actor_roundtrip(HopCounter *, objc3_executor_t *) __objc3_async_frame;
int hop_consumed_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
void hop_integrated_cf_reset(void);
void hop_integrated_cf_release_owner(void);
int hop_integrated_cf_entries(void);
int hop_integrated_cf_completed(void);
int hop_integrated_cf_created(void);
int hop_integrated_cf_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_integrated_cf_task_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
objc3_async_frame hop_integrated_cf_unstarted(int);
void hop_arc_integrated_cf_reset(void);
void hop_arc_integrated_cf_release_owner(void);
int hop_arc_integrated_cf_entries(void);
int hop_arc_integrated_cf_completed(void);
int hop_arc_integrated_cf_created(void);
int hop_arc_integrated_cf_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_arc_integrated_cf_task_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
objc3_async_frame hop_arc_integrated_cf_unstarted(int);
void hop_consumed_reset(int);
int hop_consumed_arguments(void);
int hop_consumed_entries(void);
int hop_initializing_nil_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_arc_initializing_nil_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_initializing_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_arc_initializing_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
void hop_initializing_reset(int);
void hop_arc_initializing_reset(int);
int hop_initializing_entries(void);
int hop_arc_initializing_entries(void);
int hop_initializing_completed(void);
int hop_arc_initializing_completed(void);
int hop_initializing_created(void);
int hop_arc_initializing_created(void);
int hop_mrc_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
void hop_mrc_reset(int);
int hop_mrc_arguments(void);
int hop_mrc_entries(void);
HopObject *hop_mrc_source(objc3_executor_t *) __attribute__((nothrow, ns_returns_retained));
void hop_mrc_enter(void);
void hop_mrc_enter_on(objc3_executor_t *);
int hop_mrc_task_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_mrc_unstarted_roundtrip(objc3_executor_t *) __objc3_async_frame;
int hop_arc_task_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_arc_unstarted_roundtrip(objc3_executor_t *) __objc3_async_frame;
int hop_mrc_failed_copy_roundtrip(objc3_executor_t *) __objc3_async_frame;
int hop_arc_failed_copy_roundtrip(objc3_executor_t *) __objc3_async_frame;
int hop_mrc_initializer_task_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_arc_initializer_task_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_mrc_ordinary_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_arc_ordinary_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_task_result_roundtrip(int, int, objc3_executor_t *) __objc3_async_frame;
int hop_task_result_nil_roundtrip(int, int, objc3_executor_t *) __objc3_async_frame;
void hop_task_result_reset(void);
int hop_task_result_entries(void);
int hop_task_result_completed(void);
int hop_task_result_created(void);
void hop_task_result_release_owner(void);
int hop_arc_task_result_roundtrip(int, int, objc3_executor_t *) __objc3_async_frame;
int hop_arc_task_result_nil_roundtrip(int, int, objc3_executor_t *) __objc3_async_frame;
void hop_arc_task_result_reset(void);
int hop_arc_task_result_entries(void);
int hop_arc_task_result_completed(void);
int hop_arc_task_result_created(void);
void hop_arc_task_result_release_owner(void);
int hop_cf_result_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_cf_arc_result_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
void hop_cf_result_reset(int);
void hop_cf_arc_result_reset(int);
int hop_cf_result_completed(void);
int hop_cf_arc_result_completed(void);
void hop_cf_result_release_owner(void);
void hop_cf_arc_result_release_owner(void);
int hop_cf_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_cf_arc_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_cf_aux_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_cf_arc_aux_roundtrip(int, objc3_executor_t *) __objc3_async_frame;
int hop_throwing_argument(void);
int hop_class_argument(objc3_executor_t *);
int hop_class_arguments(void);
int hop_class_entries(void);
objc3_async_frame hop_super_frame(int, objc3_executor_t *);
int hop_super_arguments(void);
int hop_super_entries(void);
void hop_super_fail_factory(int);
int hop_super_argument(objc3_executor_t *);
void hop_super_enter_main(void);
objc3_async_frame hop_super_actor_frame(HopCounter *, objc3_executor_t *);
objc3_async_frame hop_super_consumed_frame(objc3_executor_t *);
void hop_fail_next_frame_allocation(void);
void hop_fail_second_frame_allocation(void);
int hop_snapshot_roundtrip(int,objc3_executor_t *) __objc3_async_frame;
void hop_snapshot_reset(int);
int hop_snapshot_entries(void);
int hop_snapshot_arguments(void);
#ifdef __cplusplus
}
#endif
#endif

void hop_automatic_association_checks(void);

int hop_automatic_transfer_roundtrip(void) __objc3_async_frame;
