#include "keyring_modifier.hpp"

#include "irrsdl00.hpp"
#include "racfu_error.hpp"

namespace RACFu {
void KeyringModifier::addKeyring(SecurityRequest &request) {
  uint8_t function_code = request.getFunctionCode();

  /*************************************************************************/
  /* Keyring Modify                                                        */
  /*************************************************************************/
  if (function_code == KEYRING_ADD_FUNCTION_CODE) {
    std::string owner   = request.getOwner();
    std::string keyring = request.getKeyring();

    auto unique_ptr =
        std::make_unique<char[]>(sizeof(keyring_modify_arg_area_t));
    keyring_modify_arg_area_t *p_arg_area =
        reinterpret_cast<keyring_modify_arg_area_t *>(unique_ptr.get());

    KeyringModifier::buildKeyringModifyRequest(p_arg_area, owner, keyring,
                                               function_code);

    request.setRawRequestLength((int)sizeof(keyring_modify_arg_area_t));
    Logger::getInstance().debug("Keyring modify request buffer:");
    Logger::getInstance().hexDump(reinterpret_cast<char *>(p_arg_area),
                                  request.getRawRequestLength());
    request.setRawRequestPointer(KeyringModifier::preserveRawRequest(
        reinterpret_cast<char *>(p_arg_area), request.getRawRequestLength()));

    Logger::getInstance().debug("Calling IRRSDL00 ...");
    IRRSDL00::addKeyring(request, p_arg_area);
    Logger::getInstance().debug("Done");
  }

  // Check Return Codes
  if (request.getSAFReturnCode() != 0 or request.getRACFReturnCode() != 0 or
      request.getRACFReasonCode() != 0 or
      request.getRawResultPointer() == nullptr) {
    request.setRACFuReturnCode(4);
    // Raise Exception if Modify Failed.
    const std::string &admin_type = request.getAdminType();
    throw RACFuError("unable to modify '" + admin_type + "'");
  }

  request.setRACFuReturnCode(0);
}

void KeyringModifier::buildKeyringModifyRequest(
    keyring_modify_arg_area_t *p_arg_area, std::string &owner,
    const std::string &keyring, uint8_t function_code) {
  std::memset(p_arg_area, 0, sizeof(keyring_modify_arg_area_t));

  /***************************************************************************/
  /* Set Modify Arguments                                                    */
  /***************************************************************************/
  p_arg_area->args.ALET_SAF_rc   = ALET;
  p_arg_area->args.ALET_RACF_rc  = ALET;
  p_arg_area->args.ALET_RACF_rsn = ALET;

  // Automatically convert lowercase userid to uppercase.
  std::transform(owner.begin(), owner.end(), owner.begin(),
                 [](unsigned char c) { return std::toupper(c); });

  // Copy userid
  std::memset(&p_arg_area->args.RACF_user_id[0], 0, 10);
  p_arg_area->args.RACF_user_id[0] = owner.length();
  std::memcpy(&p_arg_area->args.RACF_user_id[1], owner.c_str(), owner.length());
  // Encode userid as IBM-1047.
  __a2e_l(&p_arg_area->args.RACF_user_id[1], owner.length());

  // Copy keyring
  std::memset(&p_arg_area->args.ring_name[0], 0, 239);
  p_arg_area->args.ring_name[0] = keyring.length();
  std::memcpy(&p_arg_area->args.ring_name[1], keyring.c_str(),
              keyring.length());
  // Encode keyring as IBM-1047.
  __a2e_l(&p_arg_area->args.ring_name[1], keyring.length());
}

char *KeyringModifier::preserveRawRequest(const char *p_arg_area,
                                          const int &raw_request_length) {
  try {
    auto request_unique_ptr = std::make_unique<char[]>(raw_request_length);
    Logger::getInstance().debugAllocate(request_unique_ptr.get(), 64,
                                        raw_request_length);
    std::memset(request_unique_ptr.get(), 0, raw_request_length);
    std::memcpy(request_unique_ptr.get(), p_arg_area, raw_request_length);
    char *p_raw_request = request_unique_ptr.get();
    request_unique_ptr.release();
    return p_raw_request;
  } catch (const std::bad_alloc &ex) {
    std::perror(
        "Warn - Unable to allocate space to preserve the raw request.\n");
    return nullptr;
  }
}
}  // namespace RACFu
