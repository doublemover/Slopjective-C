#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_template_demo.h"

#include <ostream>

void WriteFrontendCApiRunnerBonusExperiencesTemplateDemoJsonRows(
    std::ostream &out,
    const std::string &child_indent,
    const std::string &grandchild_indent,
    bool showcase_surface_ready,
    bool tutorial_surface_ready) {
  out << child_indent << "\"template_and_demo_harness\": {\n";
  out << grandchild_indent << "\"available\": "
      << ((showcase_surface_ready && tutorial_surface_ready) ? "true"
                                                             : "false")
      << ",\n";
  out << grandchild_indent << "\"source_roots\": [\n";
  out << grandchild_indent << "  \"showcase/portfolio.json\",\n";
  out << grandchild_indent << "  \"showcase/tutorial_walkthrough.json\",\n";
  out << grandchild_indent << "  \"docs/tutorials/build_run_verify.md\",\n";
  out << grandchild_indent << "  \"docs/tutorials/guided_walkthrough.md\"\n";
  out << grandchild_indent << "],\n";
  out << grandchild_indent << "\"public_actions\": [\n";
  out << grandchild_indent << "  \"validate-showcase\",\n";
  out << grandchild_indent << "  \"validate-runnable-showcase\",\n";
  out << grandchild_indent << "  \"validate-getting-started\"\n";
  out << grandchild_indent << "]\n";
  out << child_indent << "}\n";
}
