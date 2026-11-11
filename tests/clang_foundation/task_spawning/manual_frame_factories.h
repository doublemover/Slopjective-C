#ifndef OBJC3_MANUAL_FRAME_FACTORIES_H
#define OBJC3_MANUAL_FRAME_FACTORIES_H
#include <cstdio>

// Manual factories deliberately return the physical carrier without source
// async effects. Their frame borrows a field in the callable environment, so
// copying a Block after invocation or dropping a callable at publication fails.
namespace ManualFrameFactories {
struct State {
  std::atomic<unsigned> pins{0}, factories{0}, resumes{0}, destroyed{0};
};
struct __attribute__((objc_unsafe_sendable)) Token {
  State *state;
  int value=42;
  explicit Token(State *state):state(state){++state->pins;}
  Token(const Token &other):state(other.state),value(other.value){++state->pins;}
  Token(Token &&other) noexcept:Token(other) {}
  ~Token(){--state->pins;}
};
struct Frame {
  State *state;
  const int *borrowed;
  static void resume(objc3_task_t *owner,void *opaque) {
    auto *self=static_cast<Frame *>(opaque);
    assert(self->borrowed && *self->borrowed==42 && self->state->pins>0);
    ++self->state->resumes;
    objc3_completion completion{};
    completion.struct_size=sizeof(completion);
    completion.version=OBJC3_SERVICE_ABI_VERSION;
    completion.kind=objc3_task_is_cancel_requested(owner)
        ? OBJC3_COMPLETION_CANCELLED : OBJC3_COMPLETION_SUCCESS;
    if(completion.kind==OBJC3_COMPLETION_SUCCESS) {
      completion.has_payload=1;
      completion.payload=self->borrowed;
      completion.payload_ops=objc3::task_detail::result_ops<int>();
      completion.result_type=objc3::task_detail::result_type<int>();
    }
    assert(objc3_task_complete(owner,&completion)==OBJC3_SERVICE_OK);
  }
  static void destroy(void *opaque) noexcept {
    auto *self=static_cast<Frame *>(opaque);
    assert(*self->borrowed==42 && self->state->pins>0);
    ++self->state->destroyed;
    delete self;
  }
};
inline objc3_async_frame make(State *state,const int *value) {
  ++state->factories;
  return {sizeof(objc3_async_frame),OBJC3_ASYNC_ABI_VERSION,
      new Frame{state,value},Frame::resume,Frame::destroy,
      objc3::task_detail::result_type<int>()};
}
struct __attribute__((objc_unsafe_sendable)) Functor {
  Token token;
  explicit Functor(State *state):token(state) {}
  Functor(const Functor &)=delete;
  Functor(Functor &&)=default;
  objc3_async_frame operator()(){return make(token.state,&token.value);}
};
inline State *function_state=nullptr;
inline const int *function_value=nullptr;
inline objc3_async_frame function_factory(){return make(function_state,function_value);}
inline objc3::task<int> task_factory(){co_await objc3::task_yield;co_return 42;}
inline int spawn_parent(State *state,unsigned shape,unsigned route) __objc3_async
    __attribute__((objc_task_group)) {
  auto group=objc3::task_group_handle::current_scope();
  objc3::task_handle<int> child;
#define OBJC3_START_MANUAL(work) \
  do { \
    if(route==0) child=objc3_task_spawn<int>(work); \
    else if(route==1) child=objc3_task_detached<int>(work); \
    else if(route==2) child=objc3_task_spawn_in_group<int>(group,work); \
    else child=group.spawn<int>(work); \
  } while(false)
  if(shape==0) {
    OBJC3_START_MANUAL(Functor{state});
  } else if(shape==1) {
    OBJC3_START_MANUAL(([token=Token{state}]() mutable {
      return make(token.state,&token.value);
    }));
  } else if(shape==3) {
    // A function pointer owns no environment. Its producer-provided token is
    // held by the test caller until detached frame cleanup has also finished.
    OBJC3_START_MANUAL(function_factory);
  } else {
    Token token{state};
    OBJC3_START_MANUAL((^objc3_async_frame {
      return make(token.state,&token.value);
    }));
  }
#undef OBJC3_START_MANUAL
  int value=__objc3_await child;
  assert(value==42);
  return value;
}
inline objc3::task<int> block_group(State *state) {
  Token token{state};
  // Return before the lazy group operation invokes this stack Block.
  return objc3::with_task_group<int>(^objc3_async_frame(objc3::task_group_handle) {
    return make(token.state,&token.value);
  });
}
inline int task_function_parent(unsigned route) __objc3_async __attribute__((objc_task_group)) {
  auto group=objc3::task_group_handle::current_scope();
  objc3::task_handle<int> child;
  if(route==0)child=objc3_task_spawn(task_factory);
  else if(route==1)child=objc3_task_detached(task_factory);
  else if(route==2)child=objc3_task_spawn_in_group(group,task_factory);
  else child=group.spawn(task_factory);
  return __objc3_await child;
}
inline void finish(objc3_executor_t *source,const objc3::task_handle<int> &task) {
  for(unsigned step=0;step!=1000000;++step) {
    uint32_t count=0;
    assert(objc3_executor_pump(source,UINT32_MAX,&count)==OBJC3_SERVICE_OK);
    objc3_completion completion{};
    auto status=objc3_task_get_completion(task.native_handle(),&completion);
    if(status==OBJC3_SERVICE_OK)return;
    assert(status==OBJC3_SERVICE_NOT_READY);
    std::this_thread::yield();
  }
  assert(!"manual frame factory failed to quiesce");
}
inline void finish_retirement(State &state,unsigned expected_pins) {
  for(unsigned step=0;step!=1000000;++step) {
    if(state.destroyed==1 && state.pins==expected_pins)return;
    std::this_thread::yield();
  }
  assert(!"detached manual frame did not release its callable after completion");
}
inline void contract(objc3_executor_t *source) {
  for(unsigned route=0;route!=4;++route) {
    auto parent=task_function_parent(route).start(source);
    finish(source,parent);assert(parent.result()==42);
  }
  for(unsigned shape=0;shape!=4;++shape)
    for(unsigned route=0;route!=4;++route) {
      State state;
      std::optional<Token> function_owner;
      if(shape==3) {
        function_owner.emplace(&state);
        function_state=&state;function_value=&function_owner->value;
      }
      auto parent=spawn_parent(&state,shape,route).start(source);
      finish(source,parent);
      assert(parent.result()==42);
      parent={};
      // Detached completion can publish before its worker retires the frame.
      // Preserve producer storage through that cleanup; structured routes are
      // already joined by the parent's compiler-owned scope.
      if(route==1)finish_retirement(state,shape==3?1u:0u);
      function_owner.reset();
      if(shape==3){function_state=nullptr;function_value=nullptr;}
      if(state.factories!=1 || state.resumes!=1 || state.destroyed!=1 || state.pins!=0)
        std::fprintf(stderr,"manual frame shape=%u route=%u factories=%u resumes=%u destroyed=%u pins=%u\n",shape,route,state.factories.load(),state.resumes.load(),state.destroyed.load(),state.pins.load());
      assert(state.factories==1 && state.resumes==1 && state.destroyed==1 && state.pins==0);
    }
  for(unsigned phase=0;phase!=4;++phase) {
    State state;
    auto lazy=block_group(&state);
    assert(state.pins==1 && state.factories==0 && state.resumes==0);
    if(phase==0) { // Unstarted frames still own and then release the callback.
      lazy={};assert(state.pins==0);continue;
    }
    auto parent=std::move(lazy).start(source);
    if(phase==3) {
      uint32_t count=0;
      assert(objc3_executor_pump(source,1,&count)==OBJC3_SERVICE_OK);
      assert(state.factories==1 && state.destroyed==0);
    }
    if(phase==1 || phase==3)parent.cancel();
    finish(source,parent);
    if(phase==1 || phase==3) {
      bool cancelled=false;
      try{(void)parent.result();}catch(const objc3::task_cancelled &){cancelled=true;}
      assert(cancelled && state.factories==(phase==3 ? 1u : 0u) && state.destroyed==state.factories);
    } else assert(parent.result()==42 && state.factories==1 && state.destroyed==1);
    parent={};assert(state.pins==0);
  }
  {
    State state;
    Token token{&state};
    bool rejected=false;
    try {
      (void)objc3_task_spawn<double>(^objc3_async_frame {return make(token.state,&token.value);});
    } catch(const objc3::service_failure &error) {
      rejected=error.status()==OBJC3_SERVICE_TYPE_MISMATCH;
    }
    assert(rejected && state.factories==1 && state.resumes==0 && state.destroyed==1 && state.pins==1);
  }
  {
    State state;
    Token token{&state};
    bool rejected=false;
    try {
      (void)objc3_task_spawn<int>(^objc3_async_frame {return make(token.state,&token.value);});
    } catch(const objc3::service_failure &error) {
      rejected=error.status()==OBJC3_SERVICE_NOT_READY;
    }
    assert(rejected && state.factories==1 && state.resumes==0 && state.destroyed==1 && state.pins==1);
  }
  {
    State state;
    Token token{&state};
    bool threw=false;
    try {
      (void)objc3_task_spawn<int>(^objc3_async_frame {(void)token;throw 73;});
    } catch(int value){threw=value==73;}
    assert(threw && state.factories==0 && state.pins==1);
    bool malformed=false;
    try {
      (void)objc3_task_spawn<int>(^objc3_async_frame {
        (void)token;
        return {sizeof(objc3_async_frame),OBJC3_ASYNC_ABI_VERSION+1,
            nullptr,Frame::resume,Frame::destroy,objc3::task_detail::result_type<int>()};
      });
    } catch(const objc3::service_failure &error){malformed=error.status()==OBJC3_SERVICE_UNSUPPORTED_VERSION;}
    assert(malformed && state.pins==1 && state.destroyed==0);
  }
  {
    bool block=false,function=false;
    try{(void)objc3_task_spawn<int>((objc3_async_frame (^)(void))nullptr);}
    catch(const objc3::service_failure &error){block=error.status()==OBJC3_SERVICE_INVALID_ARGUMENT;}
    try{(void)objc3_task_spawn<int>((objc3_async_frame (*)(void))nullptr);}
    catch(const objc3::service_failure &error){function=error.status()==OBJC3_SERVICE_INVALID_ARGUMENT;}
    assert(block && function);
  }
}
} // namespace ManualFrameFactories
#endif
