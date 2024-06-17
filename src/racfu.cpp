#include "racfu.hpp"

#include "irrsmo00-conn.hpp"

#include <nlohmann/json.hpp>

void racfu(racfu_result_t *result, char *request_json) {
  nlohmann::json request;
  request = nlohmann::json::parse(request_json);
  // {
  //     "operation": "add",
  //     "admin_type": "user",
  //     "profile_name": "SQUIDWRD",
  //     "request_data": {
  //        ...
  //     }
  // }
  // Extract
  if (request["operation"].get<std::string>().compare("extract") == 0) {
    // todo extract call
  // Add/Alter/Delete
  } else {
    // todo just pass the json object.
    call_irrsmo00_with_json(request_json, result);
  }
}