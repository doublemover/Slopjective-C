// Register only the reviewed implementation defect checks. The upstream
// clang-tidy engine registers its Static Analyzer checks independently.
#include "ClangTidyModule.h"
#include "ClangTidyModuleRegistry.h"
#include "bugprone/DanglingHandleCheck.h"
#include "bugprone/SizeofExpressionCheck.h"
#include "bugprone/UseAfterMoveCheck.h"

namespace clang::tidy {
namespace {
class ImplementationChecks : public ClangTidyModule {
public:
  void addCheckFactories(ClangTidyCheckFactories &factories) override {
    factories.registerCheck<bugprone::DanglingHandleCheck>(
        "bugprone-dangling-handle");
    factories.registerCheck<bugprone::SizeofExpressionCheck>(
        "bugprone-sizeof-expression");
    factories.registerCheck<bugprone::UseAfterMoveCheck>(
        "bugprone-use-after-move");
  }
};
ClangTidyModuleRegistry::Add<ImplementationChecks> registration(
    "objc3-implementation", "Pinned implementation defect checks");
} // namespace
} // namespace clang::tidy
