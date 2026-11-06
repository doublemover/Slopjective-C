#ifndef OBJC3_CONDITIONAL_BLOCK_VALUES_H
#define OBJC3_CONDITIONAL_BLOCK_VALUES_H

// The same source exercises C/C++, ARC/MRC, source/PCH and optimized builds.
// The manual executor makes pre-entry cancellation deterministic. Each factory
// leaves only the selected capture environment alive before the first resume.
__attribute__((objc_root_class))
@interface ConditionalBlockOwner <Sendable> {
  Class isa;
@public
  unsigned *destroyed;
  unsigned number;
}
+ (id)new;
@end
@implementation ConditionalBlockOwner
+ (id)new { return class_createInstance(self, 0); }
- (void)dealloc {
  __atomic_fetch_add(destroyed, 1u, __ATOMIC_RELEASE);
  object_dispose(self);
}
#if !__has_feature(objc_arc)
- (void)_ARCCompliantRetainRelease {}
#endif
@end

typedef void (^ConditionalBlockWork)(void) __objc3_async_frame;
static void conditional_block_pause(void) __objc3_async_frame {}
static objc3_task_t *conditional_block_start(ConditionalBlockWork work,
    objc3_executor_t *executor) __attribute__((objc_task_detached)) {
  objc3_async_frame frame = work();
  objc3_task_t *task = 0;
  if (objc3_async_frame_start(&frame, executor, 0, &task) != OBJC3_SERVICE_OK)
    (void)objc3_async_frame_dispose(&frame);
  return task;
}
static objc3_task_t *conditional_block_factory(objc3_executor_t *executor,
    unsigned shape, unsigned choose, unsigned *destroyed, unsigned *ran,
    unsigned *deferred, unsigned *selected) {
  ConditionalBlockOwner *one = [ConditionalBlockOwner new];
  ConditionalBlockOwner *two = [ConditionalBlockOwner new];
  one->destroyed = destroyed; one->number = 1;
  two->destroyed = destroyed; two->number = 2;
  ConditionalBlockWork const first = ^void(void) __objc3_async_frame {
    defer {++*deferred;}
    assert(one->number == 1); *ran += one->number;
    __objc3_await conditional_block_pause();
    assert(one->number == 1);
  };
  ConditionalBlockWork const second = ^void(void) __objc3_async_frame {
    defer {++*deferred;}
    assert(two->number == 2); *ran += two->number;
    __objc3_await conditional_block_pause();
    assert(two->number == 2);
  };
  ConditionalBlockWork const choice = choose ? first : second;
  objc3_task_t *task = 0;
  switch (shape) {
  case 0: task = conditional_block_start(choose ? first : second, executor); break;
  case 1: {
    ConditionalBlockWork const alias = choose ? choice : choice;
    task = conditional_block_start(alias, executor); break;
  }
  case 2: {
    void (^const left)(void) = ^{assert(one->number == 1); *ran += one->number;};
    void (^const right)(void) = ^{assert(two->number == 2); *ran += two->number;};
    void (^const callback)(void) = choose ? left : right;
    task = conditional_block_start(^void(void) __objc3_async_frame {
      defer {++*deferred;}
      __objc3_await conditional_block_pause(); callback();
    }, executor);
    break;
  }
  case 3: task = conditional_block_start((++*selected, choice), executor); break;
  case 4: {
    ConditionalBlockWork mutable_work = first;
    task = conditional_block_start(mutable_work = choice, executor); break;
  }
  case 5:
    task = conditional_block_start((choose ? first : (ConditionalBlockWork)0) ?: second, executor);
    break;
  case 6: case 7: {
    __unsafe_unretained ConditionalBlockOwner *unsafe = choose ? one : two;
    ConditionalBlockWork const explicit_mode = ^[strong unsafe] void(void) __objc3_async_frame {
      defer {++*deferred;}
      __objc3_await conditional_block_pause(); *ran += unsafe->number;
    };
    ConditionalBlockWork const default_mode = ^[unsafe] void(void) __objc3_async_frame {
      defer {++*deferred;}
      __objc3_await conditional_block_pause(); *ran += unsafe->number;
    };
    task = conditional_block_start(shape == 6 ? explicit_mode : default_mode, executor);
    break;
  }
  default: __builtin_trap();
  }
#if !__has_feature(objc_arc)
  release_source((__bridge void *)one);
  release_source((__bridge void *)two);
#endif
  return task;
}
static void conditional_block_values_contract(objc3_executor_t *executor) {
  for (unsigned shape = 0; shape != 8; ++shape)
    for (unsigned choose = 0; choose != 2; ++choose)
      for (unsigned cancel = 0; cancel != 2; ++cancel) {
        unsigned destroyed = 0, ran = 0, deferred = 0, selected = 0;
        objc3_task_t *task = conditional_block_factory(executor, shape, choose,
            &destroyed, &ran, &deferred, &selected);
        assert(task && ran == 0 && deferred == 0);
        assert(__atomic_load_n(&destroyed, __ATOMIC_ACQUIRE) == 1);
        assert(selected == (shape == 3));
        if (cancel) objc3_task_request_cancel(task);
        uint32_t executed = 0;
        assert(objc3_executor_pump(executor, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
        objc3_task_group_t *group = 0;
        assert(objc3_task_group_create(0, &group) == OBJC3_SERVICE_OK);
        assert(objc3_task_group_add(group, task) == OBJC3_SERVICE_OK);
        assert(objc3_task_group_close(group) == OBJC3_SERVICE_OK);
        assert(objc3_task_group_join(group) == OBJC3_SERVICE_OK);
        objc3_completion completion = {0};
        assert(objc3_task_get_completion(task, &completion) == OBJC3_SERVICE_OK);
        assert(completion.kind == (cancel ? OBJC3_COMPLETION_CANCELLED : OBJC3_COMPLETION_SUCCESS));
        assert(ran == (cancel ? 0u : choose ? 1u : 2u));
        assert(deferred == (cancel ? 0u : 1u));
        assert(__atomic_load_n(&destroyed, __ATOMIC_ACQUIRE) == 2);
        objc3_task_release(task);
        objc3_task_group_release(group);
      }
}
#endif
