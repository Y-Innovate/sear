#ifndef __RACFU_KEYRING_MODIFIER_H_
#define __RACFU_KEYRING_MODIFIER_H_

#include "irrsdl00.hpp"
#include "security_request.hpp"

namespace RACFu {
class KeyringModifier {
 private:
  static void buildKeyringModifyRequest(keyring_modify_arg_area_t *p_arg_area,
                                        std::string &owner,
                                        const std::string &keyring,
                                        uint8_t function_code);
  static char *preserveRawRequest(const char *p_arg_area,
                                  const int &raw_request_length);

 public:
  void addOrDeleteKeyring(SecurityRequest &request);
};
}  // namespace RACFu

#endif
