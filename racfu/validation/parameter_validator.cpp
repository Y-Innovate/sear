#include "parameter_validator.hpp"

namespace {

ParameterType::ParameterType(std::string name, DataType type) {
  this->name = name;
  this->type = type;
}

ParameterRule::ParameterRule(std::string name, ParameterUsage usage) {
  this->name  = name;
  this->usage = usage;
}

ParameterRule::ParameterRule(std::string name, ParameterUsage usage,
                             std::string alternate) {
  this->name      = name;
  this->usage     = usage;
  this->alternate = alternate;
}

OperationSpecificParameterRules::OperationSpecificParameterRules(
    std::string operation, std::vector<ParameterRule> parameter_rules) {
  this->operation       = operation;
  this->parameter_rules = parameter_rules;
}

AdminTypeParameterRules::AdminTypeParameterRules(
    std::string admin_type,
    std::vector<ParameterRule> profile_identification_parameter_rules,
    std::vector<OperationSpecificParameterRules>
        operation_specific_parameter_rules) {
  this->admin_type = admin_type;
  this->profile_identification_parameter_rules =
      profile_identification_parameter_rules;
  this->operation_specific_parameter_rules = operation_specific_parameter_rules;
}

static const std::vector<std::string> VALID_OPERATIONS  = {"add", "alter",
                                                           "delete", "extract"};
static const std::vector<std::string> VALID_ADMIN_TYPES = {
    "user",     "group",        "group-connection", "resource",
    "data-set", "racf-options", "permission"};

static const std::vector<ParameterType> PARAMETER_TYPES = {
    ParameterType("userid", String),       ParameterType("group", String),
    ParameterType("resource", String),     ParameterType("class", String),
    ParameterType("data_set", String),     ParameterType("traits", JSON),
    ParameterType("volume", String),       ParameterType("generic", Boolean),
    ParameterType("run_as_userid", String)};

static const std::vector<std::string> ALL_ADMIN_TYPE_PARAMETERS = {
    "userid", "group", "resource", "class", "data_set"};

static const std::vector<std::string> ALL_OPERATION_SPECIFIC_PARAMETERS = {
    "traits", "volume", "generic", "run_as_userid"};

static const std::vector<AdminTypeParameterRules> PARAMETER_RULES = {
    AdminTypeParameterRules(
        "user",
        {
            ParameterRule("userid", Required),
        },
        {OperationSpecificParameterRules(
             "add", {ParameterRule("traits", Required),
                     ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules(
             "alter", {ParameterRule("traits", Required),
                       ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules(
             "delete", {ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules("extract", {})}),
    AdminTypeParameterRules(
        "group",
        {
            ParameterRule("group", Required),
        },
        {OperationSpecificParameterRules(
             "add", {ParameterRule("traits", Required),
                     ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules(
             "alter", {ParameterRule("traits", Required),
                       ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules(
             "delete", {ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules("extract", {})}),
    AdminTypeParameterRules(
        "group-connection",
        {ParameterRule("userid", Required), ParameterRule("group", Required)},
        {OperationSpecificParameterRules(
             "alter", {ParameterRule("traits", Required),
                       ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules("extract", {})}),
    AdminTypeParameterRules(
        "resource",
        {
            ParameterRule("resource", Required),
            ParameterRule("class", Required),
        },
        {OperationSpecificParameterRules(
             "add", {ParameterRule("traits", Required),
                     ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules(
             "alter", {ParameterRule("traits", Required),
                       ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules(
             "delete", {ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules("extract", {})}),
    AdminTypeParameterRules(
        "data-set",
        {
            ParameterRule("data_set", Required),
        },
        {OperationSpecificParameterRules(
             "add", {ParameterRule("traits", Required),
                     ParameterRule("volume", Optional),
                     ParameterRule("generic", Optional),
                     ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules(
             "alter", {ParameterRule("traits", Required),
                       ParameterRule("volume", Optional),
                       ParameterRule("generic", Optional),
                       ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules(
             "delete", {ParameterRule("volume", Optional),
                        ParameterRule("generic", Optional),
                        ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules("extract", {})}),
    AdminTypeParameterRules(
        "racf-options", {},
        {OperationSpecificParameterRules(
             "alter", {ParameterRule("traits", Required),
                       ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules("extract", {})}),
    AdminTypeParameterRules(
        "permission",
        {ParameterRule("userid", OneOfRequired, "group"),
         ParameterRule("group", OneOfRequired, "userid"),
         ParameterRule("resource", Required), ParameterRule("class", Required)},
        {OperationSpecificParameterRules(
             "add", {ParameterRule("traits", Required),
                     ParameterRule("volume", Optional),
                     ParameterRule("generic", Optional),
                     ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules(
             "alter", {ParameterRule("traits", Required),
                       ParameterRule("volume", Optional),
                       ParameterRule("generic", Optional),
                       ParameterRule("run_as_userid", Optional)}),
         OperationSpecificParameterRules(
             "delete", {ParameterRule("volume", Optional),
                        ParameterRule("generic", Optional),
                        ParameterRule("run_as_userid", Optional)})})};
}  // namespace

namespace RACFu {
ParameterValidator::ParameterValidator(nlohmann::json* request,
                                       Errors* errors) {
  this->request = request;
  this->errors  = errors;
}

void ParameterValidator::validate_parameters() {
  // "operation" and "admin_type" are always required
  this->check_main_parameter(std::string("operation"), VALID_OPERATIONS);
  this->check_main_parameter(std::string("admin_type"), VALID_ADMIN_TYPES);
  if (!this->errors->empty()) {
    return;
  }
  this->admin_type = (*this->request)["admin_type"].get<std::string>();
  this->operation  = (*this->request)["operation"].get<std::string>();
  // Loop over the parameter rules for each admin type
  for (AdminTypeParameterRules admin_type_parameter_rules : PARAMETER_RULES) {
    if (this->admin_type == admin_type_parameter_rules.admin_type) {
      // Check profile identification parameters
      this->check_parameter_rules(
          ALL_ADMIN_TYPE_PARAMETERS,
          admin_type_parameter_rules.profile_identification_parameter_rules);
      // Check operation specific parameters
      for (OperationSpecificParameterRules operation_specific_parameter_rules :
           admin_type_parameter_rules.operation_specific_parameter_rules) {
        if (this->operation == operation_specific_parameter_rules.operation) {
          this->check_parameter_rules(
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
      errors->add_racfu_error_message("'" + parameter.key() +
                                      "' is not a valid parameter for the '" +
                                      this->admin_type + "' admin type");
    }
  }
  if (!this->errors->empty()) {
    return;
  }
}

void ParameterValidator::check_main_parameter(
    const std::string& parameter_name,
    const std::vector<std::string>& valid_values) {
  if (!this->request->contains(parameter_name)) {
    this->errors->add_racfu_error_message("'" + parameter_name +
                                          "' is a required parameter");
  } else if (!(*this->request)[parameter_name].is_string()) {
    this->errors->add_racfu_error_message("'" + parameter_name +
                                          "' must be a string value");
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
      this->errors->add_racfu_error_message("'" + parameter_value +
                                            "' is not a valid value for '" +
                                            parameter_name + "'");
    }
  }
}

void ParameterValidator::check_parameter_rules(
    const std::vector<std::string>& all_possible_parameters,
    const std::vector<ParameterRule>& parameter_rules) {
  std::vector<std::string> allowed_parameters;
  for (ParameterRule parameter_rule : parameter_rules) {
    this->check_parameter_usage(parameter_rule);
    allowed_parameters.push_back(parameter_rule.name);
  }
  // Check if the parameter is valid, but just not in this context.
  for (std::string parameter : all_possible_parameters) {
    int occurances = std::count(allowed_parameters.begin(),
                                allowed_parameters.end(), parameter);
    if (occurances == 0 && this->request->contains(parameter)) {
      this->errors->add_racfu_error_message(
          "'" + parameter + "' is not a valid parameter for the '" +
          this->admin_type + "' admin type and '" + this->operation +
          "' operation");
    }
  }
}

void ParameterValidator::check_parameter_usage(
    const ParameterRule& parameter_rule) {
  switch (parameter_rule.usage) {
    case Optional:
      if (this->request->contains(parameter_rule.name)) {
        this->check_parameter_type(parameter_rule,
                                   (*this->request)[parameter_rule.name]);
      }
      break;
    case Required:
      if (!this->request->contains(parameter_rule.name)) {
        this->errors->add_racfu_error_message(
            "'" + parameter_rule.name + "' is a required parameter for the '" +
            this->admin_type + "' admin type and '" + this->operation +
            "' operation");
        break;
      }
      this->check_parameter_type(parameter_rule,
                                 (*this->request)[parameter_rule.name]);
      break;
    case OneOfRequired:
      if ((!this->request->contains(parameter_rule.name) &&
           !this->request->contains(parameter_rule.alternate)) ||
          (this->request->contains(parameter_rule.name) &&
           this->request->contains(parameter_rule.alternate))) {
        // Only add an error if the alternate has not already beeen found to be
        // missing.
        if (std::find(this->checked_parameters.begin(),
                      checked_parameters.end(),
                      parameter_rule.alternate) == checked_parameters.end()) {
          this->errors->add_racfu_error_message(
              "'" + parameter_rule.name + "' or '" + parameter_rule.alternate +
              "' and only one of the two must be provided for the '" +
              this->admin_type + "' admin type and '" + this->operation +
              "' operation");
        }
      } else if (this->request->contains(parameter_rule.name)) {
        this->check_parameter_type(parameter_rule,
                                   (*this->request)[parameter_rule.name]);
      }
      break;
    default:
      break;
  }
  this->checked_parameters.push_back(parameter_rule.name);
}

DataType ParameterValidator::get_parameter_type(
    const std::string& parameter_name) {
  for (ParameterType parameter_type : PARAMETER_TYPES) {
    if (parameter_name == parameter_type.name) {
      return parameter_type.type;
    }
  }
  return Unknown;
}

void ParameterValidator::check_parameter_type(
    const ParameterRule& parameter_rule, const nlohmann::json& parameter) {
  switch (this->get_parameter_type(parameter_rule.name)) {
    case String:
      if (!parameter.is_string()) {
        this->errors->add_racfu_error_message("'" + parameter_rule.name +
                                              "' must be a string value");
      }
      break;
    case JSON:
      if (!parameter.is_structured()) {
        this->errors->add_racfu_error_message("'" + parameter_rule.name +
                                              "' must be a json value");
      }
      break;
    case Boolean:
      if (!parameter.is_boolean()) {
        this->errors->add_racfu_error_message("'" + parameter_rule.name +
                                              "' must be a boolean value");
      }
      break;
    default:
      break;
  }
}
}  // namespace RACFu
