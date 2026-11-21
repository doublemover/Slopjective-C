#ifndef OBJC3_CXX_FRAME_VALUES_H
#define OBJC3_CXX_FRAME_VALUES_H

inline int frame_value_case = -1;

// Kept in the maintained source/PCH spawn matrix: native frame parameters and
// results must construct independent C++ owners, never copy their bytes.
struct FrameValueCounts {
  int live = 0;
  int bodies = 0;
  int moves = 0;
  int copies = 0;
  int fail_move = -1;
  bool fail_copy = false;
};

struct alignas(128) FrameValue {
  FrameValueCounts *counts;
  int value;
  FrameValue(FrameValueCounts *counts, int value) : counts(counts), value(value) {
    assert(reinterpret_cast<uintptr_t>(this) % alignof(FrameValue) == 0);
    ++counts->live;
  }
  FrameValue(const FrameValue &other) : counts(other.counts), value(other.value) {
    assert(reinterpret_cast<uintptr_t>(this) % alignof(FrameValue) == 0);
    if (counts->fail_copy) throw std::bad_alloc();
    ++counts->live;
    ++counts->copies;
  }
  FrameValue(FrameValue &&other) : counts(other.counts), value(other.value) {
    assert(reinterpret_cast<uintptr_t>(this) % alignof(FrameValue) == 0);
    if (value == counts->fail_move) throw 74;
    other.value = -1;
    ++counts->live;
    ++counts->moves;
  }
  ~FrameValue() noexcept { --counts->live; }
};

FrameValue frame_value_leaf(FrameValue value) __objc3_async_frame {
  ++value.counts->bodies;
  return value;
}

FrameValue frame_value_parent(FrameValue value) __objc3_async_frame {
  FrameValue child = __objc3_await frame_value_leaf(std::move(value));
  return child;
}

int frame_move_only(std::unique_ptr<int> value) __objc3_async_frame {
  return *value;
}

const FrameValue frame_const_value(const FrameValue value) __objc3_async_frame {
  ++value.counts->bodies;
  return value;
}

int frame_partial_capture(FrameValue first, FrameValue second)
    __objc3_async_frame {
  ++first.counts->bodies;
  return first.value + second.value;
}

objc3::task<void> frame_scope_child() {
  co_await objc3::task_yield;
  co_return;
}

FrameValue frame_value_scope(FrameValue value) __objc3_async_frame
    __attribute__((objc_task_group)) {
  auto group = objc3::task_group_handle::current_scope();
  auto child = frame_scope_child().start_child(group);
  ++value.counts->bodies;
  return value;
}

namespace {
struct PrivateFrameValue : FrameValue {
  using FrameValue::FrameValue;
};
enum class PrivateFrameEnum { answer = 42 };
PrivateFrameValue private_frame_value(PrivateFrameValue value) __objc3_async_frame {
  return value;
}
PrivateFrameEnum private_frame_enum() __objc3_async_frame {
  return PrivateFrameEnum::answer;
}
}

inline void frame_cxx_value_contract(objc3_executor_t *source) {
  {
    FrameValueCounts counts;
    {
      auto task = __objc3_async_from_frame<PrivateFrameValue>(
          private_frame_value(PrivateFrameValue(&counts, 42))).start(source);
      auto enumeration = __objc3_async_from_frame<PrivateFrameEnum>(
          private_frame_enum()).start(source);
      uint32_t executed = 0;
      assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
      assert(task.result().value == 42);
      assert(enumeration.result() == PrivateFrameEnum::answer);
    }
    assert(counts.live == 0);
  }
  {
    FrameValueCounts counts;
    auto frame = frame_value_leaf(FrameValue(&counts, 42));
    assert(counts.bodies == 0 && counts.live == 1 && counts.moves == 1);
    assert(objc3_async_frame_dispose(&frame) == OBJC3_SERVICE_OK);
    assert(counts.live == 0 && counts.bodies == 0);
  }
  for (int mode : {0, 1, 2, 3, 4, 5}) {
    frame_value_case = mode;
    FrameValueCounts counts;
    {
      auto frame = mode == 1 ? frame_value_parent(FrameValue(&counts, 42))
                   : mode == 4 ? frame_value_scope(FrameValue(&counts, 42))
                   : mode == 5 ? frame_const_value(FrameValue(&counts, 42))
                               : frame_value_leaf(FrameValue(&counts, 42));
      auto pending = __objc3_async_from_frame<FrameValue>(frame);
      auto task = std::move(pending).start(source);
      if (mode == 2) task.cancel();
      if (mode == 3) counts.fail_copy = true;
      uint32_t executed = 0;
      if (mode == 4) {
        assert(objc3_executor_pump(source, 1, &executed) == OBJC3_SERVICE_OK);
        assert(counts.bodies == 1 && counts.live >= 1);
        objc3_completion completion{};
        assert(objc3_task_get_completion(task.native_handle(), &completion) ==
               OBJC3_SERVICE_NOT_READY);
      }
      assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
      if (mode == 2) {
        bool cancelled = false;
        try { (void)task.result(); }
        catch (const objc3::task_cancelled &) { cancelled = true; }
        assert(cancelled && counts.bodies == 0);
      } else if (mode == 3) {
        bool failed = false;
        try { (void)task.result(); }
        catch (const objc3::service_failure &error) {
          failed = error.status() == OBJC3_SERVICE_OUT_OF_MEMORY;
        }
        assert(failed && counts.bodies == 1);
      } else {
        auto result = task.result();
        assert(result.value == 42 && counts.bodies == 1 && counts.copies > 0);
      }
    }
    assert(counts.live == 0);
  }
  {
    auto task = __objc3_async_from_frame<int>(frame_move_only(std::make_unique<int>(42))).start(source);
    uint32_t executed = 0;
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
    assert(task.result() == 42);
  }
  {
    FrameValueCounts counts;
    counts.fail_move = 2;
    bool failed = false;
    try {
      auto frame = frame_partial_capture(FrameValue(&counts, 1), FrameValue(&counts, 2));
      (void)objc3_async_frame_dispose(&frame);
    } catch (int error) { failed = error == 74; }
    assert(failed && counts.live == 0 && counts.bodies == 0 && counts.moves == 1);
  }
}

#endif
