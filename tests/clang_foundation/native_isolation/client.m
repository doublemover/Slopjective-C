#include "provider.h"
#include "cf-contract.h"
#include <assert.h>
#include <sched.h>
#include <string.h>
#ifdef __cplusplus
#include "objc3_task.hpp"
#endif
extern void release_object(void *) __asm__("objc_release");
static objc3_executor_t *caller, *main_destination, *global, *named;
static objc3_service_status copy_int(void *to, const void *from) {
  memcpy(to, from, sizeof(int));
  return OBJC3_SERVICE_OK;
}
static objc3_service_status move_int(void *to, void *from) { return copy_int(to, from); }
static void destroy_int(void *p) { (void)p; }
static const objc3_value_ops int_ops = {sizeof(objc3_value_ops),
                                        OBJC3_VALUE_OPS_VERSION,
                                        sizeof(int),
                                        _Alignof(int),
                                        copy_int,
                                        move_int,
                                        destroy_int};
static objc3_task_t *start(objc3_async_frame *frame) {
  objc3_task_t *task = 0;
  assert(objc3_async_frame_start(frame, caller, 0, &task) == OBJC3_SERVICE_OK);
  return task;
}
static objc3_completion finish(objc3_task_t *task) {
  objc3_completion completion = {sizeof(completion), OBJC3_SERVICE_ABI_VERSION};
  for (unsigned step = 0; step != 1000000; ++step) {
    uint32_t count;
    assert(objc3_executor_pump(main_destination, 32, &count) ==
           OBJC3_SERVICE_OK);
    assert(objc3_executor_pump(named, 32, &count) == OBJC3_SERVICE_OK);
    assert(objc3_executor_pump(caller, 32, &count) == OBJC3_SERVICE_OK);
    objc3_service_status status = objc3_task_get_completion(task, &completion);
    if (status == OBJC3_SERVICE_OK)
      return completion;
    assert(status == OBJC3_SERVICE_NOT_READY);
    sched_yield();
  }
  assert(!"native isolation task did not progress");
  return completion;
}
#ifdef __cplusplus
@interface HopSuperChild (CxxTaskSuper)
- (int)taskSuper:(objc3_executor_t *)destination __objc3_async;
@end
@implementation HopSuperChild (CxxTaskSuper)
- (int)taskSuper:(objc3_executor_t *)destination __objc3_async {
  int value=__objc3_await [super superValue:1];
  assert(objc3_executor_is_current(destination));
  return value;
}
@end
static int task_actor_entry(HopCounter *counter,
                            objc3_executor_t *destination) __objc3_async {
  int value = __objc3_await [counter add:1];
  assert(objc3_executor_is_current(destination));
  return value;
}
static int task_packet_entry(HopCounter *counter,
                             objc3_executor_t *destination) __objc3_async {
  HopPacket source={40,{10,20,30}};
  HopPacket packet=__objc3_await hop_packet_on_main(source);
  assert(objc3_executor_is_current(destination));
  packet=__objc3_await [counter transform:packet];
  assert(objc3_executor_is_current(destination));
  assert(source.value==40 && source.weights[2]==30 && packet.weights[2]==32);
  return packet.value;
}
#endif
static int class_roundtrip(int kind, objc3_executor_t *destination)
    __objc3_async_frame {
  int result=0;
  switch (kind) {
  case 0:
    result=__objc3_await [HopObject classScalar:hop_class_argument(destination)];
    break;
  case 1: {
    HopPacket source={hop_class_argument(destination)+40,{10,20,30}};
    HopPacket value=__objc3_await [HopCounter classPacket:source];
    assert(source.value==41 && value.weights[2]==30);
    result=value.value;
    break;
  }
  case 2:
    result=__objc3_await [HopCounter classFrame:hop_class_argument(destination)];
    break;
  case 3:
    result=[(__objc3_await [HopObject newOnMain]) number];
    break;
  case 4:
    __objc3_await [HopObject classVoid];
    result=42;
    break;
  default: assert(!"unknown class entry case");
  }
  assert(objc3_executor_is_current(destination));
  return result;
}
int main(void) {
  hop_automatic_association_checks();
  @autoreleasepool {
    assert(objc3_executor_create(1, &caller) == OBJC3_SERVICE_OK);
    assert(objc3_executor_create(1, &named) == OBJC3_SERVICE_OK);
    assert(objc3_executor_get_main(&main_destination) == OBJC3_SERVICE_OK);
    assert(objc3_executor_get_global(&global) == OBJC3_SERVICE_OK);
    assert(objc3_executor_register_named("native.isolation", named) ==
           OBJC3_SERVICE_OK);
    objc3_actor_t *actor = 0;
    assert(objc3_actor_create(named, &actor) == OBJC3_SERVICE_OK);
    void *object = hop_new_counter();
    assert(objc3_actor_bind_object(object, actor) == OBJC3_SERVICE_OK);
    release_object(object); // The explicit association pins the receiver.
    __unsafe_unretained HopCounter *counter = (__bridge HopCounter *)object;
    hop_configure(caller, main_destination, global, named,
                  objc3_actor_identity(actor));
    objc3_async_frame frame = hop_roundtrip(counter, caller);
    const objc3_async_result_type *type = frame.result_type;
    objc3_task_t *task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    int result = 0;
    assert(objc3_task_copy_result(task, type, &int_ops, &result) ==
           OBJC3_SERVICE_OK);
    assert(result == 42 && hop_arguments() == 1 && hop_entries() == 3 &&
           hop_cleanups() == 1);
    objc3_task_release(task);

    frame = hop_packet_roundtrip(counter, caller);
    type = frame.result_type;
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    result = 0;
    assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
    assert(result == 42 && hop_packet_arguments() == 1 && hop_packet_entries() == 2);
    objc3_task_release(task);

    frame = hop_nil_roundtrip();
    type = frame.result_type;
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    result = -1;
    assert(objc3_task_copy_result(task, type, &int_ops, &result) ==
           OBJC3_SERVICE_OK);
    assert(result == 0);
    objc3_task_release(task);

    // Cancel after caller-side argument evaluation but before the child enters
    // its target executor. Neither target invocation nor an argument replay is
    // allowed; the parent's lexical cleanup executes once.
    hop_configure(caller, main_destination, global, named,
                  objc3_actor_identity(actor));
    frame = hop_roundtrip(counter, caller);
    task = start(&frame);
    uint32_t count;
    assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK &&
           count == 1);
    assert(hop_arguments() == 1);
    objc3_task_request_cancel(task);
    assert(finish(task).kind == OBJC3_COMPLETION_CANCELLED);
    assert(hop_arguments() == 1 && hop_entries() == 0 && hop_cleanups() == 1);
    objc3_task_release(task);

    hop_configure(caller, main_destination, global, named,
                  objc3_actor_identity(actor));
    frame = hop_packet_roundtrip(counter, caller);
    task = start(&frame);
    assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK && count == 1);
    assert(hop_packet_arguments() == 1);
    objc3_task_request_cancel(task);
    assert(finish(task).kind == OBJC3_COMPLETION_CANCELLED);
    assert(hop_packet_arguments() == 1 && hop_packet_entries() == 0);
    objc3_task_release(task);

#ifdef __cplusplus
    // Exercise a C++ task caller entering the C provider's actor through a
    // generated native frame, then returning to the original destination.
    {
      auto parent = task_actor_entry(counter, caller).start(caller);
      assert(finish(parent.native_handle()).kind == OBJC3_COMPLETION_SUCCESS);
      assert(parent.result() == 43);
    }
    {
      auto parent = task_packet_entry(counter, caller).start(caller);
      assert(finish(parent.native_handle()).kind == OBJC3_COMPLETION_SUCCESS);
      assert(parent.result() == 42);
    }
#endif
    assert(hop_cxx_packet_live() == 0);
    frame = hop_cxx_packet_roundtrip(caller);
    type = frame.result_type;
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    result = 0;
    assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
    assert(result == 42 && hop_cxx_packet_entries() == 1);
    objc3_task_release(task);
    for (unsigned step = 0; step != 1000000 && hop_cxx_packet_live() != 0; ++step)
      sched_yield();
    assert(hop_cxx_packet_live() == 0);
    frame = hop_cxx_packet_roundtrip(caller);
    task = start(&frame);
    assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK && count == 1);
    objc3_task_request_cancel(task);
    assert(finish(task).kind == OBJC3_COMPLETION_CANCELLED);
    objc3_task_release(task);
    for (unsigned step = 0; step != 1000000 && hop_cxx_packet_live() != 0; ++step)
      sched_yield();
    assert(hop_cxx_packet_entries() == 1 && hop_cxx_packet_live() == 0);
    for (int failed_move = 1; failed_move <= 3; ++failed_move) {
      hop_cxx_packet_fail_move(failed_move);
      frame = hop_cxx_packet_roundtrip(caller);
      task = start(&frame);
      assert(finish(task).kind == OBJC3_COMPLETION_CANCELLED);
      objc3_task_release(task);
      for (unsigned step = 0; step != 1000000 && hop_cxx_packet_live() != 0; ++step)
        sched_yield();
      assert(hop_cxx_packet_entries() == 1 && hop_cxx_packet_live() == 0);
    }
    hop_cxx_packet_fail_move(0);

    assert(hop_cxx_value_live() == 0);
    frame = hop_cxx_value_roundtrip(caller);
    type = frame.result_type;
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    result = 0;
    assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
    assert(result == 42);
    objc3_task_release(task);
    for (unsigned step = 0; step != 1000000 && hop_cxx_value_live() != 0; ++step)
      sched_yield();
    assert(hop_cxx_value_live() == 0);

    for (int kind=0; kind!=5; ++kind) {
      int entries_before=hop_class_entries();
      int arguments_before=hop_class_arguments();
      int destroyed_before=hop_destroyed();
      frame=class_roundtrip(kind, caller);
      type=frame.result_type;
      task=start(&frame);
      assert(finish(task).kind==OBJC3_COMPLETION_SUCCESS);
      result=0;
      assert(objc3_task_copy_result(task,type,&int_ops,&result)==OBJC3_SERVICE_OK);
      assert(result==42 && hop_class_entries()==entries_before+1);
      assert(hop_class_arguments()==arguments_before+(kind<3));
      objc3_task_release(task);
      int expected_destroyed=destroyed_before+(kind==3);
      for (unsigned step=0; step!=1000000 && hop_destroyed()!=expected_destroyed; ++step)
        sched_yield();
      assert(hop_destroyed()==expected_destroyed);

      entries_before=hop_class_entries();
      arguments_before=hop_class_arguments();
      frame=class_roundtrip(kind, caller);
      task=start(&frame);
      assert(objc3_executor_pump(caller,1,&count)==OBJC3_SERVICE_OK && count==1);
      objc3_task_request_cancel(task);
      assert(finish(task).kind==OBJC3_COMPLETION_CANCELLED);
      assert(hop_class_entries()==entries_before);
      assert(hop_class_arguments()==arguments_before+(kind<3));
      objc3_task_release(task);
      assert(hop_destroyed()==expected_destroyed);
    }

    for (int kind=0; kind!=7; ++kind) {
      for (int outcome=0; outcome!=3; ++outcome) {
        int arguments_before=hop_super_arguments();
        int entries_before=hop_super_entries();
        int destroyed_before=hop_destroyed();
        hop_super_fail_factory(outcome==2);
        frame=kind==5 ? hop_super_actor_frame(counter,caller) :
              kind==6 ? hop_super_consumed_frame(caller) : hop_super_frame(kind,caller);
        type=frame.result_type;
        task=start(&frame);
        if (outcome==1) {
          assert(objc3_executor_pump(caller,1,&count)==OBJC3_SERVICE_OK && count==1);
          objc3_task_request_cancel(task);
        }
        objc3_completion completion=finish(task);
        if (outcome==0) {
          assert(completion.kind==OBJC3_COMPLETION_SUCCESS);
          result=0;
          assert(objc3_task_copy_result(task,type,&int_ops,&result)==OBJC3_SERVICE_OK);
          assert(result==42);
        } else if (outcome==1) {
          assert(completion.kind==OBJC3_COMPLETION_CANCELLED);
        } else {
          assert(completion.kind==OBJC3_COMPLETION_ERROR);
          objc3_service_status service=OBJC3_SERVICE_OK;
          assert(objc3_async_error_get_service(completion.error,&service)==OBJC3_SERVICE_OK &&
                 service==OBJC3_SERVICE_OUT_OF_MEMORY);
        }
        assert(hop_super_arguments()==arguments_before+1);
        assert(hop_super_entries()==entries_before+(outcome==0));
        objc3_task_release(task);
        int expected_destroyed=destroyed_before+(kind!=1 && kind!=5)+(kind==4 && outcome==0);
        for (unsigned step=0; step!=1000000 && hop_destroyed()!=expected_destroyed; ++step)
          sched_yield();
        assert(hop_destroyed()==expected_destroyed);
      }
    }

#ifdef __cplusplus
    {
      HopSuperChild *super_receiver=[HopSuperChild new];
      auto super_task=[super_receiver taskSuper:caller].start(caller);
      assert(finish(super_task.native_handle()).kind==OBJC3_COMPLETION_SUCCESS);
      assert(super_task.result()==42);
#if !__has_feature(objc_arc)
      release_object((void *)super_receiver);
#endif
    }
#endif
    frame = hop_task_abi_roundtrip(counter, caller);
    type = frame.result_type;
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    result = 0;
    assert(objc3_task_copy_result(task,type,&int_ops,&result)==OBJC3_SERVICE_OK);
    assert(result==42 && hop_task_abi_entries()==1);
    objc3_task_release(task);
    for (unsigned step=0; step!=1000000 && hop_cxx_value_live(); ++step)
      sched_yield();
    assert(hop_cxx_value_live()==0);

    for (int outcome=0; outcome!=4; ++outcome) {
      int entries_before=hop_task_abi_entries();
      frame=hop_task_abi_failure_frame(outcome==2,caller);
      type=frame.result_type;
      task=start(&frame);
      if (outcome==1) {
        assert(objc3_executor_pump(caller,1,&count)==OBJC3_SERVICE_OK && count==1);
        objc3_task_request_cancel(task);
      } else if (outcome==3) {
        for (unsigned step=0; step!=1000 && hop_task_abi_entries()==entries_before; ++step) {
          assert(objc3_executor_pump(caller,1,&count)==OBJC3_SERVICE_OK);
          assert(objc3_executor_pump(main_destination,1,&count)==OBJC3_SERVICE_OK);
        }
        assert(hop_task_abi_entries()==entries_before+1);
        objc3_task_request_cancel(task);
      }
      objc3_completion completion=finish(task);
      if (outcome==0) {
        assert(completion.kind==OBJC3_COMPLETION_SUCCESS);
        result=0;
        assert(objc3_task_copy_result(task,type,&int_ops,&result)==OBJC3_SERVICE_OK);
        assert(result==42 && hop_task_abi_entries()==entries_before+1);
      } else if (outcome==2) {
        assert(completion.kind==OBJC3_COMPLETION_ERROR);
        objc3_service_status service=OBJC3_SERVICE_OK;
        assert(objc3_async_error_get_service(completion.error,&service)==OBJC3_SERVICE_OK);
        assert(service==OBJC3_SERVICE_OUT_OF_MEMORY && hop_task_abi_entries()==entries_before);
      } else {
        assert(completion.kind==OBJC3_COMPLETION_CANCELLED);
        assert(hop_task_abi_entries()==entries_before+(outcome==3));
      }
      objc3_task_release(task);
      for (unsigned step=0; step!=1000000 && hop_cxx_value_live(); ++step)
        sched_yield();
      assert(hop_cxx_value_live()==0);
    }

    for (int kind=0; kind!=3; ++kind) {
      frame=kind==2 ? hop_task_abi_class_roundtrip(caller) :
                     hop_task_abi_super_frame(kind,caller);
      type=frame.result_type;
      task=start(&frame);
      assert(finish(task).kind==OBJC3_COMPLETION_SUCCESS);
      result=0;
      assert(objc3_task_copy_result(task,type,&int_ops,&result)==OBJC3_SERVICE_OK);
      assert(result==42);
      objc3_task_release(task);
    }

    frame = hop_cxx_actor_roundtrip(counter, caller);
    type = frame.result_type;
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    result = 0;
    assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
    assert(result == 42);
    objc3_task_release(task);
    for (unsigned step = 0; step != 1000000 &&
         (hop_cxx_value_live() || hop_cxx_packet_live()); ++step)
      sched_yield();
    assert(hop_cxx_value_live() == 0 && hop_cxx_packet_live() == 0);

    for (int kind=0;kind!=8;++kind) {
      for(int mode=0;mode!=(kind<4 ? 3 : 1);++mode) {
        int before=hop_destroyed();hop_snapshot_reset(mode==2);
        frame=hop_snapshot_roundtrip(kind,caller);type=frame.result_type;task=start(&frame);
        if(mode==1) {
          assert(objc3_executor_pump(caller,1,&count)==OBJC3_SERVICE_OK && count==1);
          assert(hop_snapshot_arguments()==1);objc3_task_request_cancel(task);
        }
        objc3_completion completion=finish(task);
        if(kind>=4 || mode==1)assert(completion.kind==OBJC3_COMPLETION_CANCELLED);
        else if(mode==2) {
          objc3_service_status service=OBJC3_SERVICE_OK;
          assert(completion.kind==OBJC3_COMPLETION_ERROR);
          assert(objc3_async_error_get_service(completion.error,&service)==OBJC3_SERVICE_OK && service==OBJC3_SERVICE_OUT_OF_MEMORY);
        } else {
          assert(completion.kind==OBJC3_COMPLETION_SUCCESS);result=0;
          assert(objc3_task_copy_result(task,type,&int_ops,&result)==OBJC3_SERVICE_OK && result==42);
        }
        assert(hop_snapshot_arguments()==1 && hop_snapshot_entries()==(kind<4 && mode==0));
        objc3_task_release(task);
        for(unsigned step=0;step!=1000000 && hop_destroyed()!=before+1;++step)sched_yield();
        assert(hop_destroyed()==before+1);
      }
    }

    for (int kind = 0; kind != 13; ++kind) {
      for (int outcome = 0; outcome != (kind >= 8 ? 5 : 3); ++outcome) {
        int before = hop_destroyed();
        int expected_arguments = kind == 7 || kind == 12 ? 2 : 1;
        int expected_destroyed = expected_arguments + (kind == 6 && outcome == 0);
        hop_mrc_reset(outcome == 3 ? 2 : outcome == 2);
        frame = hop_mrc_roundtrip(kind, caller);
        type = frame.result_type;
        task = start(&frame);
        if (outcome == 1 || outcome == 4) {
          assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK && count == 1);
          assert(hop_mrc_arguments() == expected_arguments);
          if (outcome == 4) {
            // The first caller callback creates the entry child; its own
            // first resume performs the hop before main constructs the target.
            assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK && count == 1);
            assert(objc3_executor_pump(main_destination, 1, &count) == OBJC3_SERVICE_OK && count == 1);
            assert(hop_mrc_entries() == 0);
          }
          objc3_task_request_cancel(task);
        }
        objc3_completion completion = finish(task);
        if (outcome == 0) {
          assert(completion.kind == OBJC3_COMPLETION_SUCCESS);
          result = -1;
          assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
          assert(result == (kind == 5 ? 0 : 42));
        } else if (outcome == 1 || outcome == 4) {
          assert(completion.kind == OBJC3_COMPLETION_CANCELLED);
        } else {
          objc3_service_status service = OBJC3_SERVICE_OK;
          assert(completion.kind == OBJC3_COMPLETION_ERROR);
          assert(objc3_async_error_get_service(completion.error, &service) == OBJC3_SERVICE_OK &&
                 service == OBJC3_SERVICE_OUT_OF_MEMORY);
        }
        assert(hop_mrc_arguments() == expected_arguments);
        assert(hop_mrc_entries() == (outcome == 0));
        objc3_task_release(task);
        for (unsigned step = 0; step != 1000000 &&
             hop_destroyed() != before + expected_destroyed; ++step) sched_yield();
        assert(hop_destroyed() == before + expected_destroyed);
      }
    }

    for (int ownership = 0; ownership != 2; ++ownership) {
      for (int kind = 0; kind != 14; ++kind) {
        if (!ownership && (kind == 8 || kind == 9)) continue;
        int before = hop_cf_destroyed(), objects_before = hop_destroyed();
        int object_count = (kind >= 6) + (kind == 8 || kind == 9);
        int entered = kind >= 4 && kind != 10 && kind != 11;
        int cancelled = entered && kind % 2 == 1;
        hop_mrc_reset(0);
        hop_cf_reset(0);
        frame = ownership ? hop_cf_arc_aux_roundtrip(kind, caller) : hop_cf_aux_roundtrip(kind, caller);
        task = start(&frame);
        if (cancelled) {
          for (unsigned step = 0; step != 1000 && hop_cf_entries() == 0; ++step)
            assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK);
          assert(hop_cf_entries() == 1);
          objc3_task_request_cancel(task);
        }
        assert(finish(task).kind == (cancelled ? OBJC3_COMPLETION_CANCELLED : OBJC3_COMPLETION_SUCCESS));
        assert(hop_cf_arguments() == 1 && hop_cf_entries() == entered);
        objc3_task_release(task);
        for (unsigned step = 0; step != 1000000 &&
             (hop_cf_live() || hop_destroyed() != objects_before + object_count); ++step) sched_yield();
        assert(hop_cf_live() == 0 && hop_cf_destroyed() == before + 1);
        assert(hop_destroyed() == objects_before + object_count);
      }
      for (int kind = 0; kind != 13; ++kind) {
        int representation = kind >= 10 ? kind - 10 : kind % 3;
        int outcomes = kind == 9 ? 2 : representation == 0 ? 3 : representation == 1 ? 6 : 5;
        for (int outcome = 0; outcome != outcomes; ++outcome) {
          int foreign_before = hop_cf_destroyed(), object_before = hop_destroyed();
          int foreign_arguments = kind == 9 ? 0 : 1;
          int object_arguments = kind >= 6 && kind <= 8;
          hop_mrc_reset(0);
          hop_cf_reset(outcome == 5 ? 2 : outcome == 2);
          frame = ownership ? hop_cf_arc_roundtrip(kind, caller) : hop_cf_roundtrip(kind, caller);
          type = frame.result_type;
          task = start(&frame);
          if (outcome == 1 || outcome == 3) {
            assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK && count == 1);
            assert(hop_cf_arguments() == foreign_arguments);
            if (outcome == 3) {
              assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK && count == 1);
              assert(objc3_executor_pump(main_destination, 1, &count) == OBJC3_SERVICE_OK && count == 1);
              assert(hop_cf_entries() == 0);
            }
            objc3_task_request_cancel(task);
          } else if (outcome == 4) {
            for (unsigned step = 0; step != 1000 && hop_cf_entries() == 0; ++step) {
              assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK);
              assert(objc3_executor_pump(main_destination, 1, &count) == OBJC3_SERVICE_OK);
            }
            assert(hop_cf_entries() == 1);
            objc3_task_request_cancel(task);
          }
          objc3_completion completion = finish(task);
          if (outcome == 0) {
            assert(completion.kind == OBJC3_COMPLETION_SUCCESS);
            result = -1;
            assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
            assert(result == (kind == 9 ? 0 : 42));
          } else if (outcome == 2 || outcome == 5) {
            objc3_service_status service = OBJC3_SERVICE_OK;
            assert(completion.kind == OBJC3_COMPLETION_ERROR);
            assert(objc3_async_error_get_service(completion.error, &service) == OBJC3_SERVICE_OK &&
                   service == OBJC3_SERVICE_OUT_OF_MEMORY);
          } else {
            assert(completion.kind == OBJC3_COMPLETION_CANCELLED);
          }
          assert(hop_cf_arguments() == foreign_arguments && hop_mrc_arguments() == object_arguments);
          assert(hop_cf_entries() == (outcome == 0 || outcome == 4));
          objc3_task_release(task);
          for (unsigned step = 0; step != 1000000 &&
               (hop_cf_live() || hop_destroyed() != object_before + object_arguments); ++step)
            sched_yield();
          assert(hop_cf_live() == 0 && hop_cf_destroyed() == foreign_before + foreign_arguments);
          assert(hop_destroyed() == object_before + object_arguments);
        }
      }
    }

    for (int ownership = 0; ownership != 2; ++ownership) {
      for (int kind = 0; kind != 8; ++kind) {
        int before = hop_destroyed();
        hop_mrc_reset(0);
        frame = ownership ? hop_arc_ordinary_roundtrip(kind, caller) :
                            hop_mrc_ordinary_roundtrip(kind, caller);
        task = start(&frame);
        if (kind % 4 == 3) {
          for (unsigned step = 0; step != 1000 && hop_mrc_entries() == 0; ++step)
            assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK);
          assert(hop_mrc_entries() == 1);
          objc3_task_request_cancel(task);
        }
        assert(finish(task).kind == (kind % 4 == 3 ? OBJC3_COMPLETION_CANCELLED : OBJC3_COMPLETION_SUCCESS));
        assert(hop_mrc_arguments() == 1 && hop_mrc_entries() == (kind % 4 >= 2));
        objc3_task_release(task);
        for (unsigned step = 0; step != 1000000 && hop_destroyed() != before + 1; ++step)
          sched_yield();
        assert(hop_destroyed() == before + 1);
      }
      for (int kind = 0; kind != (ownership ? 6 : 5); ++kind) {
        for (int outcome = 0; outcome != 5; ++outcome) {
          int before = hop_destroyed();
          int expected_arguments = kind == 4 ? 2 : 1;
          int expected_destroyed = expected_arguments + (kind == 5 && (outcome == 0 || outcome == 4));
          hop_mrc_reset(outcome == 2);
          frame = ownership ? hop_arc_task_roundtrip(kind, caller) :
                              hop_mrc_task_roundtrip(kind, caller);
          type = frame.result_type;
          task = start(&frame);
          if (outcome == 1 || outcome == 3) {
            assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK && count == 1);
            assert(hop_mrc_arguments() == expected_arguments);
            if (outcome == 3) {
              assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK && count == 1);
              assert(objc3_executor_pump(main_destination, 1, &count) == OBJC3_SERVICE_OK && count == 1);
              assert(hop_mrc_entries() == 0);
            }
            objc3_task_request_cancel(task);
          } else if (outcome == 4) {
            for (unsigned step = 0; step != 1000 && hop_mrc_entries() == 0; ++step) {
              assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK);
              assert(objc3_executor_pump(main_destination, 1, &count) == OBJC3_SERVICE_OK);
            }
            assert(hop_mrc_entries() == 1);
            objc3_task_request_cancel(task);
          }
          objc3_completion completion = finish(task);
          if (outcome == 0) {
            assert(completion.kind == OBJC3_COMPLETION_SUCCESS);
            result = 0;
            assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
            assert(result == 42);
          } else if (outcome == 2) {
            objc3_service_status service = OBJC3_SERVICE_OK;
            assert(completion.kind == OBJC3_COMPLETION_ERROR);
            assert(objc3_async_error_get_service(completion.error, &service) == OBJC3_SERVICE_OK &&
                   service == OBJC3_SERVICE_OUT_OF_MEMORY);
          } else {
            assert(completion.kind == OBJC3_COMPLETION_CANCELLED);
          }
          assert(hop_mrc_arguments() == expected_arguments);
          assert(hop_mrc_entries() == (outcome == 0 || outcome == 4));
          objc3_task_release(task);
          for (unsigned step = 0; step != 1000000 &&
               hop_destroyed() != before + expected_destroyed; ++step) sched_yield();
          assert(hop_destroyed() == before + expected_destroyed);
        }
      }
      {
        int before = hop_destroyed();
        hop_mrc_reset(0);
        frame = ownership ? hop_arc_unstarted_roundtrip(caller) :
                            hop_mrc_unstarted_roundtrip(caller);
        task = start(&frame);
        assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
        assert(hop_mrc_arguments() == 1 && hop_mrc_entries() == 0);
        objc3_task_release(task);
        for (unsigned step = 0; step != 1000000 && hop_destroyed() != before + 1; ++step)
          sched_yield();
        assert(hop_destroyed() == before + 1);
      }
      {
        int before = hop_destroyed();
        hop_mrc_reset(0);
        frame = ownership ? hop_arc_failed_copy_roundtrip(caller) :
                            hop_mrc_failed_copy_roundtrip(caller);
        task = start(&frame);
        assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
        assert(hop_mrc_arguments() == 1 && hop_mrc_entries() == 0);
        objc3_task_release(task);
        for (unsigned step = 0; step != 1000000 && hop_destroyed() != before + 1; ++step)
          sched_yield();
        assert(hop_destroyed() == before + 1);
      }
    }

    for (int kind = 0; kind != 16; ++kind) {
      if (kind == 12) continue; // Separate throwing-argument control below.
      int expected_objects = (kind == 4 || kind == 8 || kind == 14) ? 2 : 1;
      int successful_objects = expected_objects + (kind == 15);
      int destroyed_before = hop_destroyed();
      hop_consumed_reset(0);
      frame = hop_consumed_roundtrip(kind, caller);
      type = frame.result_type;
      task = start(&frame);
      assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
      result = 0;
      assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
      assert(result == 42 && hop_consumed_arguments() == expected_objects &&
             hop_consumed_entries() == 1);
      objc3_task_release(task);
      for (unsigned step = 0; step != 1000000 &&
           hop_destroyed() != destroyed_before + successful_objects; ++step)
        sched_yield();
      assert(hop_destroyed() == destroyed_before + successful_objects);

      destroyed_before = hop_destroyed();
      hop_consumed_reset(0);
      frame = hop_consumed_roundtrip(kind, caller);
      task = start(&frame);
      assert(objc3_executor_pump(caller, 1, &count) == OBJC3_SERVICE_OK && count == 1);
      assert(hop_consumed_arguments() == expected_objects);
      objc3_task_request_cancel(task);
      assert(finish(task).kind == OBJC3_COMPLETION_CANCELLED);
      assert(hop_consumed_entries() == 0);
      objc3_task_release(task);
      for (unsigned step = 0; step != 1000000 &&
           hop_destroyed() != destroyed_before + expected_objects; ++step)
        sched_yield();
      assert(hop_destroyed() == destroyed_before + expected_objects);

      destroyed_before = hop_destroyed();
      hop_consumed_reset(1);
      frame = hop_consumed_roundtrip(kind, caller);
      task = start(&frame);
      objc3_completion failure = finish(task);
      assert(failure.kind == OBJC3_COMPLETION_ERROR);
      objc3_service_status service = OBJC3_SERVICE_OK;
      assert(objc3_async_error_get_service(failure.error, &service) == OBJC3_SERVICE_OK &&
             service == OBJC3_SERVICE_OUT_OF_MEMORY);
      assert(hop_consumed_entries() == 0 && hop_consumed_arguments() == expected_objects);
      objc3_task_release(task);
      for (unsigned step = 0; step != 1000000 &&
           hop_destroyed() != destroyed_before + expected_objects; ++step)
        sched_yield();
      assert(hop_destroyed() == destroyed_before + expected_objects);
    }
    int snapshot_destroyed_before = hop_destroyed();
    hop_consumed_reset(0);
    frame = hop_consumed_roundtrip(12, caller);
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_CANCELLED);
    assert(hop_consumed_arguments() == 1 && hop_consumed_entries() == 0);
    objc3_task_release(task);
    for (unsigned step = 0; step != 1000000 &&
         hop_destroyed() != snapshot_destroyed_before + 1; ++step)
      sched_yield();
    assert(hop_destroyed() == snapshot_destroyed_before + 1);
    hop_consumed_reset(0);

    int before = hop_destroyed();
    frame = hop_owned_packet_roundtrip(caller);
    type = frame.result_type;
    task = start(&frame);
    assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
    result = 0;
    assert(objc3_task_copy_result(task, type, &int_ops, &result) == OBJC3_SERVICE_OK);
    assert(result == 42);
    objc3_task_release(task);
    for (unsigned step = 0; step != 1000000 && hop_destroyed() == before; ++step)
      sched_yield();
    assert(hop_destroyed() == before + 1);
    before = hop_destroyed();
    @autoreleasepool {
      frame = hop_owned_roundtrip();
      type = frame.result_type;
      task = start(&frame);
      assert(finish(task).kind == OBJC3_COMPLETION_SUCCESS);
      void *owned = 0;
      const objc3_value_ops *ops =
          objc3_async_reference_ops(OBJC3_ASYNC_OBJECT);
      assert(objc3_task_copy_result(task, type, ops, &owned) ==
             OBJC3_SERVICE_OK);
      objc3_task_release(task);
      assert([(__bridge HopObject *)owned number] == 42);
      ops->destroy(&owned);
    }
    // Completion publication can precede the global worker's frame/pool
    // cleanup. Observe final result-owner destruction before checking its count.
    for (unsigned step = 0; step != 1000000 && hop_destroyed() == before;
         ++step)
      sched_yield();
    assert(hop_destroyed() == before + 1);
    frame=hop_automatic_transfer_roundtrip();type=frame.result_type;task=start(&frame);
    assert(finish(task).kind==OBJC3_COMPLETION_SUCCESS);
    result=0;assert(objc3_task_copy_result(task,type,&int_ops,&result)==OBJC3_SERVICE_OK && result==126);
    objc3_task_release(task);
    assert(objc3_actor_close(actor) == OBJC3_SERVICE_OK);
    assert(objc3_actor_join(actor) == OBJC3_SERVICE_OK);
    assert(objc3_actor_unbind_object(object) == OBJC3_SERVICE_OK);
    objc3_actor_release(actor);
    assert(objc3_executor_unregister_named("native.isolation") ==
           OBJC3_SERVICE_OK);
    objc3_executor_release(named);
    objc3_executor_release(global);
    objc3_executor_release(main_destination);
    objc3_executor_release(caller);
  }
}
