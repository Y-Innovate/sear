#ifndef __RACFU_PARAMETER_VALIDATOR_H_
#define __RACFU_PARAMETER_VALIDATOR_H_

#include <nlohmann/json.hpp>
#include <string>

#include "errors.hpp"

#define PARAMETER_USAGE_REQUIRED 0
#define PARAMETER_USAGE_OPTIONAL 1
#define PARAMETER_USAGE_ONE_OF_REQUIRED 2

#define PARAMETER_TYPE_STRING 0
#define PARAMETER_TYPE_JSON 1
#define PARAMETER_TYPE_BOOLEAN 2

class ParameterType {
 public:
  std::string name;
  char type;

  ParameterType(std::string name, char type) {
    this->name = name;
    this->type = type;
  }
};

class ParameterRule {
 public:
  std::string name;
  std::string alternate;
  char usage;

  ParameterRule(std::string name, char usage) {
    this->name  = name;
    this->usage = usage;
  }

  ParameterRule(std::string name, char usage, std::string alternate) {
    this->name      = name;
    this->usage     = usage;
    this->alternate = alternate;
  }
};

class OperationSpecificParameterRules {
 public:
  std::string operation;
  std::vector<ParameterRule> parameter_rules;

  OperationSpecificParameterRules(std::string operation,
                                  std::vector<ParameterRule> parameter_rules) {
    this->operation       = operation;
    this->parameter_rules = parameter_rules;
  }
};

class AdminTypeParameterRules {
 public:
  std::string admin_type;
  std::vector<ParameterRule> profile_identification_parameter_rules;
  std::vector<OperationSpecificParameterRules>
      operation_specific_parameter_rules;

  AdminTypeParameterRules(
      std::string admin_type,
      std::vector<ParameterRule> profile_identification_parameter_rules,
      std::vector<OperationSpecificParameterRules>
          operation_specific_parameter_rules) {
    this->admin_type = admin_type;
    this->profile_identification_parameter_rules =
        profile_identification_parameter_rules;
    this->operation_specific_parameter_rules =
        operation_specific_parameter_rules;
  }
};

static const std::vector<std::string> VALID_OPERATIONS  = {"add", "alter",
                                                           "delete", "extract"};
static const std::vector<std::string> VALID_ADMIN_TYPES = {
    "user",     "group",        "group-connection", "resource",
    "data-set", "racf-options", "permission"};

static const std::vector<ParameterType> PARAMETER_TYPES = {
    ParameterType("userid", PARAMETER_TYPE_STRING),
    ParameterType("group", PARAMETER_TYPE_STRING),
    ParameterType("resource", PARAMETER_TYPE_STRING),
    ParameterType("class", PARAMETER_TYPE_STRING),
    ParameterType("data_set", PARAMETER_TYPE_STRING),
    ParameterType("traits", PARAMETER_TYPE_JSON),
    ParameterType("volume", PARAMETER_TYPE_STRING),
    ParameterType("generic", PARAMETER_TYPE_BOOLEAN),
    ParameterType("run_as_userid", PARAMETER_TYPE_STRING)};

static const std::vector<std::string> ALL_ADMMIN_TYPE_PARAMETERS = {
    "userid", "group", "resource", "class", "data_set"};

static const std::vector<std::string> ALL_OPERATION_SPECIFIC_PARAMETERS = {
    "traits", "volume", "generic", "run_as_userid"};

static const std::vector<AdminTypeParameterRules> PARAMETER_RULES = {
    AdminTypeParameterRules(
        "user",
        {
            ParameterRule("userid", PARAMETER_USAGE_REQUIRED),
        },
        {OperationSpecificParameterRules(
             "add", {ParameterRule("traits", PARAMETER_USAGE_REQUIRED),
                     ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules(
             "alter",
             {ParameterRule("traits", PARAMETER_USAGE_REQUIRED),
              ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules(
             "delete",
             {ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules("extract", {})}),
    AdminTypeParameterRules(
        "group",
        {
            ParameterRule("group", PARAMETER_USAGE_REQUIRED),
        },
        {OperationSpecificParameterRules(
             "add", {ParameterRule("traits", PARAMETER_USAGE_REQUIRED),
                     ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules(
             "alter",
             {ParameterRule("traits", PARAMETER_USAGE_REQUIRED),
              ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules(
             "delete",
             {ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules("extract", {})}),
    AdminTypeParameterRules(
        "group-connection",
        {ParameterRule("userid", PARAMETER_USAGE_REQUIRED),
         ParameterRule("group", PARAMETER_USAGE_REQUIRED)},
        {OperationSpecificParameterRules(
             "alter",
             {ParameterRule("traits", PARAMETER_USAGE_REQUIRED),
              ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules("extract", {})}),
    AdminTypeParameterRules(
        "resource",
        {
            ParameterRule("resource", PARAMETER_USAGE_REQUIRED),
            ParameterRule("class", PARAMETER_USAGE_REQUIRED),
        },
        {OperationSpecificParameterRules(
             "add", {ParameterRule("traits", PARAMETER_USAGE_REQUIRED),
                     ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules(
             "alter",
             {ParameterRule("traits", PARAMETER_USAGE_REQUIRED),
              ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules(
             "delete",
             {ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules("extract", {})}),
    AdminTypeParameterRules(
        "data-set",
        {
            ParameterRule("data_set", PARAMETER_USAGE_REQUIRED),
        },
        {OperationSpecificParameterRules(
             "add", {ParameterRule("traits", PARAMETER_USAGE_REQUIRED),
                     ParameterRule("volume", PARAMETER_USAGE_OPTIONAL),
                     ParameterRule("generic", PARAMETER_USAGE_OPTIONAL),
                     ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules(
             "alter",
             {ParameterRule("traits", PARAMETER_USAGE_REQUIRED),
              ParameterRule("volume", PARAMETER_USAGE_OPTIONAL),
              ParameterRule("generic", PARAMETER_USAGE_OPTIONAL),
              ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules(
             "delete",
             {ParameterRule("volume", PARAMETER_USAGE_OPTIONAL),
              ParameterRule("generic", PARAMETER_USAGE_OPTIONAL),
              ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules("extract", {})}),
    AdminTypeParameterRules(
        "racf-options", {},
        {OperationSpecificParameterRules(
             "alter",
             {ParameterRule("traits", PARAMETER_USAGE_REQUIRED),
              ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules("extract", {})}),
    AdminTypeParameterRules(
        "permission",
        {ParameterRule("userid", PARAMETER_USAGE_ONE_OF_REQUIRED, "group"),
         ParameterRule("group", PARAMETER_USAGE_ONE_OF_REQUIRED, "userid"),
         ParameterRule("resource", PARAMETER_USAGE_REQUIRED),
         ParameterRule("class", PARAMETER_USAGE_REQUIRED)},
        {OperationSpecificParameterRules(
             "add", {ParameterRule("traits", PARAMETER_USAGE_REQUIRED),
                     ParameterRule("volume", PARAMETER_USAGE_OPTIONAL),
                     ParameterRule("generic", PARAMETER_USAGE_OPTIONAL),
                     ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules(
             "alter",
             {ParameterRule("traits", PARAMETER_USAGE_REQUIRED),
              ParameterRule("volume", PARAMETER_USAGE_OPTIONAL),
              ParameterRule("generic", PARAMETER_USAGE_OPTIONAL),
              ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)}),
         OperationSpecificParameterRules(
             "delete",
             {ParameterRule("volume", PARAMETER_USAGE_OPTIONAL),
              ParameterRule("generic", PARAMETER_USAGE_OPTIONAL),
              ParameterRule("run_as_userid", PARAMETER_USAGE_OPTIONAL)})})};

class ParameterValidator {
 public:
  nlohmann::json* request;
  nlohmann::json* errors;
  std::string admin_type;
  std::string operation;

  ParameterValidator(nlohmann::json* request, nlohmann::json* errors) {
    this->request = request;
    this->errors  = errors;
  }

  void validate_parameters() {
    // "operation" and "admin_type" are always required
    check_main_parameter(std::string("operation"), VALID_OPERATIONS);
    check_main_parameter(std::string("admin_type"), VALID_ADMIN_TYPES);
    if (!(this->errors)->empty()) {
      return;
    }
    this->admin_type = (*this->request)["admin_type"].get<std::string>();
    this->operation  = (*this->request)["operation"].get<std::string>();
    // Loop over the parameter rules for each admin type
    for (AdminTypeParameterRules admin_type_parameter_rules : PARAMETER_RULES) {
      if (this->admin_type == admin_type_parameter_rules.admin_type) {
        // Check profile identification parameters
        check_parameter_rules(
            ALL_ADMMIN_TYPE_PARAMETERS,
            admin_type_parameter_rules.profile_identification_parameter_rules);
        // Check operation specific parameters
        for (OperationSpecificParameterRules
                 operation_specific_parameter_rules :
             admin_type_parameter_rules.operation_specific_parameter_rules) {
          if (this->operation == operation_specific_parameter_rules.operation) {
            check_parameter_rules(
                ALL_OPERATION_SPECIFIC_PARAMETERS,
                operation_specific_parameter_rules.parameter_rules);
          }
        }
      }
    }
    // Check for extraneous parameters
    bool found;
    for (const auto& parameter : request->items()) {
      if (parameter.key() == "admin_type" || parameter.key() == "operation") {
        // "admin_type" and "operation" can be assumed to already be validated.
        continue;
      }
      found = false;
      for (ParameterType parameter_type : PARAMETER_TYPES) {
        if (parameter.key() == parameter_type.name) {
          found = true;
        }
      }
      if (found == false) {
        update_error_json(errors, BAD_PARAMETER_NAME,
                          nlohmann::json{
                              { "parameter",  parameter.key()},
                              {"admin_type", this->admin_type}
        });
      }
    }
    if (!(this->errors)->empty()) {
      return;
    }
  }

 private:
  void check_main_parameter(const std::string parameter_name,
                            const std::vector<std::string> valid_values) {
    if (!(*this->request).contains(parameter_name)) {
      update_error_json(this->errors, REQUIRED_PARAMETER,
                        nlohmann::json{
                            {"parameter", parameter_name}
      });
    } else if (!(*this->request)[parameter_name].is_string()) {
      update_error_json(
          this->errors, BAD_PARAMETER_DATA_TYPE,
          nlohmann::json{
              {"parameter", parameter_name},
              {"data_type",       "string"}
      });
    } else {
      bool found = false;
      std::string parameter_value =
          (*this->request)[parameter_name].get<std::string>();
      for (std::string value : valid_values) {
        if (value == parameter_value) {
          found = true;
        }
      }
      if (found == false) {
        update_error_json(this->errors, BAD_PARAMETER_VALUE,
                          nlohmann::json{
                              {      "parameter",  parameter_name},
                              {"parameter_value", parameter_value}
        });
      }
    }
  }

  void check_parameter_rules(std::vector<std::string> all_possible_parameters,
                             std::vector<ParameterRule> parameter_rules) {
    std::vector<std::string> allowed_parameters;
    for (ParameterRule parameter_rule : parameter_rules) {
      check_parameter_usage(parameter_rule);
      allowed_parameters.push_back(parameter_rule.name);
    }
    // Check if the parameter is valid, but just not in this context.
    for (std::string parameter : all_possible_parameters) {
      int occurances = std::count(allowed_parameters.begin(),
                                  allowed_parameters.end(), parameter);
      if (occurances == 0 && (*this->request).contains(parameter)) {
        update_error_json(this->errors,
                          BAD_PARAMETER_FOR_ADMIN_TYPE_OPERATION_COMBO,
                          nlohmann::json{
                              { "parameter",        parameter},
                              {"admin_type", this->admin_type},
                              { "operation",  this->operation}
        });
      }
    }
  }

  void check_parameter_usage(ParameterRule parameter_rule) {
    switch (parameter_rule.usage) {
      case PARAMETER_USAGE_OPTIONAL:
        if ((*this->request).contains(parameter_rule.name)) {
          check_parameter_type(parameter_rule,
                               &(*this->request)[parameter_rule.name]);
        }
        break;
      case PARAMETER_USAGE_REQUIRED:
        if (!(*this->request).contains(parameter_rule.name)) {
          update_error_json(this->errors, MISSING_PARAMETER,
                            nlohmann::json{
                                { "parameter", parameter_rule.name},
                                {"admin_type",    this->admin_type},
                                { "operation",     this->operation}
          });
          break;
        }
        check_parameter_type(parameter_rule,
                             &(*this->request)[parameter_rule.name]);
        break;
      case PARAMETER_USAGE_ONE_OF_REQUIRED:
        if ((!(*this->request).contains(parameter_rule.name) &&
             !(*this->request).contains(parameter_rule.alternate)) ||
            ((*this->request).contains(parameter_rule.name) &&
             (*this->request).contains(parameter_rule.alternate))) {
          update_error_json(this->errors, ONE_OF_THESE_PARAMETERS_REQUIRED,
                            nlohmann::json{
                                { "parameter",      parameter_rule.name},
                                { "alternate", parameter_rule.alternate},
                                {"admin_type",         this->admin_type},
                                { "operation",          this->operation}
          });
        } else if ((*this->request).contains(parameter_rule.name)) {
          check_parameter_type(parameter_rule,
                               &(*this->request)[parameter_rule.name]);
        }
        break;
      default:
        break;
    }
  }

  char get_parameter_type(std::string parameter_name) {
    for (ParameterType parameter_type : PARAMETER_TYPES) {
      if (parameter_name == parameter_type.name) {
        return parameter_type.type;
      }
    }
    return -1;
  }

  void check_parameter_type(ParameterRule parameter_rule,
                            const nlohmann::json* parameter_p) {
    switch (get_parameter_type(parameter_rule.name)) {
      case PARAMETER_TYPE_STRING:
        if (!(*parameter_p).is_string()) {
          update_error_json(
              this->errors, BAD_PARAMETER_DATA_TYPE,
              nlohmann::json{
                  {"parameter", parameter_rule.name},
                  {"data_type",            "string"}
          });
        }
        break;
      case PARAMETER_TYPE_JSON:
        if (!(*parameter_p).is_structured()) {
          update_error_json(
              this->errors, BAD_PARAMETER_DATA_TYPE,
              nlohmann::json{
                  {"parameter", parameter_rule.name},
                  {"data_type",              "json"}
          });
        }
        break;
      case PARAMETER_TYPE_BOOLEAN:
        if (!(*parameter_p).is_boolean()) {
          update_error_json(this->errors, BAD_PARAMETER_DATA_TYPE,
                            nlohmann::json{
                                {"parameter", parameter_rule.name},
                                {"data_type",           "boolean"}
          });
        }
        break;
      default:
        break;
    }
  }
};

#endif
