#ifndef __RACFU_PARAMETER_VALIDATOR_H_
#define __RACFU_PARAMETER_VALIDATOR_H_

#include <nlohmann/json.hpp>
#include <string>

#include "errors.hpp"

namespace {

enum ParameterUsage { Required, Optional, OneOfRequired };

enum DataType { String, JSON, Boolean, Unknown };

class ParameterType {
 public:
  std::string name;
  DataType type;

  ParameterType(std::string name, DataType type);
};

class ParameterRule {
 public:
  std::string name;
  std::string alternate;
  ParameterUsage usage;

  ParameterRule(std::string name, ParameterUsage usage);
  ParameterRule(std::string name, ParameterUsage usage, std::string alternate);
};

class OperationSpecificParameterRules {
 public:
  std::string operation;
  std::vector<ParameterRule> parameter_rules;

  OperationSpecificParameterRules(std::string operation,
                                  std::vector<ParameterRule> parameter_rules);
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
          operation_specific_parameter_rules);
};
}  // namespace

namespace RACFu {
class ParameterValidator {
 public:
  ParameterValidator(nlohmann::json* request, Errors* errors);
  void validate_parameters();

 private:
  const nlohmann::json* request;
  Errors* errors;
  std::string admin_type;
  std::string operation;
  void check_main_parameter(const std::string& parameter_name,
                            const std::vector<std::string>& valid_values);
  void check_parameter_rules(
      const std::vector<std::string>& all_possible_parameters,
      const std::vector<ParameterRule>& parameter_rules);
  void check_parameter_usage(const ParameterRule& parameter_rule);
  DataType get_parameter_type(const std::string& parameter_name);
  void check_parameter_type(const ParameterRule& parameter_rule,
                            const nlohmann::json& parameter);
};
}  // namespace RACFu

#endif
