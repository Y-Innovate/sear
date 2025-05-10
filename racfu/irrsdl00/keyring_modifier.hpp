#ifndef __RACFU_KEYRING_MODIFIER_H_
#define __RACFU_KEYRING_MODIFIER_H_

#include "irrsdl00.hpp"
#include "security_request.hpp"

namespace RACFu {
class KeyringModifier {
 private:
  static void buildKeyringArgs(keyring_args_t *p_args,
                               SecurityRequest &request);
  static char *preserveRawRequest(const char *p_arg_area,
                                  const int &raw_request_length);

 public:
  void addOrDeleteKeyring(SecurityRequest &request);
  void addCertificate(SecurityRequest &request);
  void deleteOrRemoveCertificate(SecurityRequest &request);
};
}  // namespace RACFu

#endif
