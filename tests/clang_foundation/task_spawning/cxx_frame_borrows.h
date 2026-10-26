#ifndef OBJC3_CXX_FRAME_BORROWS_H
#define OBJC3_CXX_FRAME_BORROWS_H

struct FrameBorrowCounts {
  int live = 0, entered = 0, exited = 0, children = 0;
  objc3::task_handle<void> child;
};
struct FrameBorrowValue {
  FrameBorrowCounts *counts;
  int number;
  FrameBorrowValue(FrameBorrowCounts &counts, int number)
      : counts(&counts), number(number) { ++counts.live; }
  FrameBorrowValue(const FrameBorrowValue &) = delete;
  ~FrameBorrowValue() { --counts->live; }
  int read() const & __objc3_async_frame {
    ++counts->entered;
    defer { ++counts->exited; }
    __objc3_await frame_borrow_leaf();
    return number;
  }
  int scoped_read() const & __objc3_async_frame __attribute__((objc_task_group));
  int operator()(int &extra) const __objc3_async_frame {
    __objc3_await frame_borrow_leaf();
    return number + extra;
  }
  static int frame_borrow_leaf() __objc3_async_frame { return 42; }
};
static void frame_borrow_scope_child(const FrameBorrowValue &value)
    __objc3_async_frame {
  __objc3_await FrameBorrowValue::frame_borrow_leaf();
  assert(value.number == 42 && value.counts->live == 1);
  ++value.counts->children;
}
int FrameBorrowValue::scoped_read() const & __objc3_async_frame
    __attribute__((objc_task_group)) {
  ++counts->entered;
  defer { ++counts->exited; }
  auto group = objc3::task_group_handle::current_scope();
  counts->child = __objc3_async_from_frame<void>(frame_borrow_scope_child(*this))
                     .start_child(group);
  return number;
}
static int frame_borrow_reference(FrameBorrowValue &value) __objc3_async_frame {
  ++value.counts->entered;
  defer { ++value.counts->exited; }
  __objc3_await FrameBorrowValue::frame_borrow_leaf();
  return value.number;
}
static int frame_borrow_rvalue(FrameBorrowValue &&value) __objc3_async_frame {
  return __objc3_await frame_borrow_reference(value);
}
static int frame_borrow_cv(const volatile int &value) __objc3_async_frame {
  __objc3_await FrameBorrowValue::frame_borrow_leaf();
  return value;
}
static int frame_borrow_array(int (&values)[3]) __objc3_async_frame {
  __objc3_await FrameBorrowValue::frame_borrow_leaf();
  values[1] += values[0];
  return values[1] + values[2];
}
static int frame_borrow_function(int (&function)(int), int *&pointer)
    __objc3_async_frame {
  __objc3_await FrameBorrowValue::frame_borrow_leaf();
  return function(*pointer);
}
static int frame_borrow_double(int value) { return value * 2; }

struct FrameBorrowLeft {
  virtual ~FrameBorrowLeft() = default;
  long padding[4] = {1,2,3,4};
};
struct FrameBorrowRight {
  int number = 21;
  virtual ~FrameBorrowRight() = default;
  virtual int read() const __objc3_async_frame {
    __objc3_await FrameBorrowValue::frame_borrow_leaf();
    return number;
  }
};
struct FrameBorrowDerived : FrameBorrowLeft, FrameBorrowRight {
  int other = 21;
  int read() const __objc3_async_frame override {
    __objc3_await FrameBorrowValue::frame_borrow_leaf();
    return number + other;
  }
};

#if __cplusplus > 202002L
struct FrameOwnedReceiver {
  std::unique_ptr<int> value;
  int read(this FrameOwnedReceiver self) __objc3_async_frame {
    __objc3_await FrameBorrowValue::frame_borrow_leaf();
    return *self.value;
  }
};
#endif

static void cxx_frame_borrow_contract(objc3_executor_t *source) {
  auto pump = [&] {
    uint32_t executed = 0;
    assert(objc3_executor_pump(source, UINT32_MAX, &executed) == OBJC3_SERVICE_OK);
  };
  for (int mode = 0; mode != 5; ++mode) {
    FrameBorrowCounts counts;
    {
      FrameBorrowValue value(counts, 1);
      auto frame = mode == 0 ? frame_borrow_reference(value)
                   : mode == 1 ? frame_borrow_rvalue(std::move(value))
                               : value.read();
      assert(counts.live == 1 && counts.entered == 0);
      value.number = 41;
      if (mode == 3) {
        assert(objc3_async_frame_dispose(&frame) == OBJC3_SERVICE_OK);
        assert(counts.entered == 0 && counts.exited == 0 && counts.live == 1);
        continue;
      }
      auto task = __objc3_async_from_frame<int>(frame).start(source);
      if (mode == 4) task.cancel();
      else {
        uint32_t executed = 0;
        while (!counts.entered) {
          assert(objc3_executor_pump(source, 1, &executed) == OBJC3_SERVICE_OK);
          assert(executed != 0);
        }
        assert(counts.entered == 1 && counts.exited == 0 && counts.live == 1);
        value.number = 42;
        if (mode == 2) task.cancel();
      }
      pump();
      if (mode == 2 || mode == 4) {
        bool cancelled = false;
        try { (void)task.result(); }
        catch (const objc3::task_cancelled &) { cancelled = true; }
        assert(cancelled);
      } else assert(task.result() == 42);
      assert(counts.live == 1);
      assert(counts.entered == (mode == 4 ? 0 : 1));
      assert(counts.exited == counts.entered);
    }
    assert(counts.live == 0);
  }
  {
    volatile int cv = 3;
    int values[] = {10,20,12};
    int first = 1, second = 21;
    int *pointer = &first;
    auto a = __objc3_async_from_frame<int>(frame_borrow_cv(cv)).start(source);
    auto b = __objc3_async_from_frame<int>(frame_borrow_array(values)).start(source);
    auto c = __objc3_async_from_frame<int>(frame_borrow_function(frame_borrow_double, pointer)).start(source);
    cv = 42;
    pointer = &second;
    pump();
    assert(a.result() == 42 && b.result() == 42 && c.result() == 42);
    assert(values[1] == 30);
  }
  {
    FrameBorrowCounts counts;
    FrameBorrowValue value(counts, 41);
    auto task = __objc3_async_from_frame<int>(value.scoped_read()).start(source);
    uint32_t executed = 0;
    assert(objc3_executor_pump(source, 1, &executed) == OBJC3_SERVICE_OK);
    assert(counts.entered == 1 && counts.exited == 1 && counts.children == 0);
    objc3_completion completion{};
    assert(objc3_task_get_completion(task.native_handle(), &completion) ==
           OBJC3_SERVICE_NOT_READY);
    value.number = 42;
    pump();
    assert(task.result() == 41 && counts.children == 1 && counts.live == 1);
    int extra = 1;
    auto callable = __objc3_async_from_frame<int>(value(extra)).start(source);
    value.number = 40; extra = 2;
    pump();
    assert(callable.result() == 42);
  }
  {
    FrameBorrowDerived value;
    FrameBorrowRight *right = &value;
    assert(static_cast<void *>(right) != static_cast<void *>(&value));
    auto member = &FrameBorrowRight::read;
    auto task = __objc3_async_from_frame<int>((right->*member)()).start(source);
    value.number = 20; value.other = 22;
    pump();
    assert(task.result() == 42);
  }
#if __cplusplus > 202002L
  {
    auto task = __objc3_async_from_frame<int>(
        FrameOwnedReceiver{std::make_unique<int>(42)}.read()).start(source);
    pump();
    assert(task.result() == 42);
  }
#endif
}
#endif
