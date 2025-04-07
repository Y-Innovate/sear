#ifndef __RACFU_TRAITS_H_
#define __RACFU_TRAITS_H_

#include <string>
#include <unordered_map>
#include <vector>

namespace RACFu {

enum TraitType { STRING, BOOLEAN, LIST, UINT };

class Trait {
 public:
  std::string racfu_key_;
  std::string racf_key_;
  TraitType trait_type_;
  Trait(std::string racfu_key, std::string racf_key, TraitType trait_type)
      : racfu_key_(racfu_key), racf_key_(racf_key), trait_type_(trait_type) {}
};

class Traits {
 private:
  std::unordered_map<std::string, std::vector<TraitType>> traits_;
  // Add constructor that traverses a provided schema
  // and loads the traits into the trait mapping structure.
  // Add functions for getting trait mapping information.
};

}  // namespace RACFu

#endif
