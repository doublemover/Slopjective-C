#ifndef OBJC3_GROUP_RESULT_DEDUCTION_H
#define OBJC3_GROUP_RESULT_DEDUCTION_H
namespace group_result_deduction {
objc3::task<int> value(objc3::task_group_handle);
objc3::task<void> empty(objc3::task_group_handle);
objc3_async_frame erased(objc3::task_group_handle);
objc3::task<int> &borrowed_task(objc3::task_group_handle);
template<class Work> concept CanDeduce=requires(Work work){objc3::with_task_group(std::move(work));};
static_assert(std::is_same_v<decltype(objc3::with_task_group(&value)),objc3::task<int>>);
static_assert(std::is_same_v<decltype(objc3::with_task_group(&empty)),objc3::task<void>>);
static_assert(std::is_same_v<decltype(objc3::with_task_group<int>(&erased)),objc3::task<int>>);
static_assert(!CanDeduce<decltype(&erased)>);
static_assert(!CanDeduce<decltype(&borrowed_task)>);
}
#endif
