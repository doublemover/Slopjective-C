#pragma once

#include <cerrno>
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>
#include <system_error>
#include <type_traits>
#include <sys/wait.h>
#include <unistd.h>

namespace objc3::clang_foundation::test {
// Compiler fixtures use assertions to guard AST prerequisites. A failed
// prerequisite must stop that case while independent cases still execute.
// The enclosing CTest timeout and sanitizer settings apply to every child.
class IsolatedCases {
  unsigned passed_ = 0, failed_ = 0;
public:
  template<class Work> void Run(const std::string &name, Work work) {
    std::cout.flush();std::cerr.flush();std::fflush(nullptr);
    const auto child = fork();
    if(child < 0)throw std::system_error(errno,std::generic_category(),"fork test case");
    if(child == 0) {
      std::cerr << "CASE " << name << '\n';
      try {
        if constexpr(std::is_same_v<std::invoke_result_t<Work>,bool>)
          std::exit(work() ? 0 : 1);
        else {work();std::exit(0);}
      } catch(const std::exception &error) {
        std::cerr << "CASE " << name << ": " << error.what() << '\n';
      } catch(...) {std::cerr << "CASE " << name << ": uncaught exception\n";}
      std::exit(1);
    }
    int status = 0;
    while(waitpid(child,&status,0) < 0) {
      if(errno == EINTR)continue;
      throw std::system_error(errno,std::generic_category(),"wait for test case");
    }
    if(WIFEXITED(status) && WEXITSTATUS(status) == 0) {
      ++passed_;std::cerr << "PASS " << name << '\n';
    } else {
      ++failed_;std::cerr << "FAIL " << name;
      if(WIFSIGNALED(status))std::cerr << " (signal " << WTERMSIG(status) << ')';
      else if(WIFEXITED(status))std::cerr << " (exit " << WEXITSTATUS(status) << ')';
      std::cerr << '\n';
    }
  }
  int Finish() const {
    std::cerr << passed_ << " independent cases passed; " << failed_ << " failed\n";
    return failed_ ? 1 : 0;
  }
};
} // namespace objc3::clang_foundation::test
