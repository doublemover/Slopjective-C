#include "tools/objc3c_frontend_c_api_runner_bonus_experiences_json_contract.h"

#include <ostream>

void WriteFrontendCApiRunnerBonusExperiencesContractJsonRows(
    std::ostream &out,
    const std::string &child_indent) {
  out << child_indent << "\"contract_id\": "
      << "\"objc3c.bonus.experiences.boundary.v1\",\n";
  out << child_indent << "\"product_boundary_model\": "
      << "\"public-runner compile inspect trace showcase and tutorial flows "
         "define the current bonus experience product boundary\",\n";
  out << child_indent << "\"runtime_boundary_model\": "
      << "\"frontend-c-api summary artifacts and runtime inspection ABI "
         "snapshots define the live runtime boundary for bonus experiences\",\n";
  out << child_indent << "\"fail_closed_model\": "
      << "\"no sidecar playground service visual shell or template catalog is "
         "authoritative until it is implemented on the live public runner and "
         "checked-in example roots\",\n";
}
