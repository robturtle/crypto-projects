#ifndef CRYPTO_PROJECT_2_YANG_LIU_16_05_01
#define CRYPTO_PROJECT_2_YANG_LIU_16_05_01
#include <string>

namespace crypto {

extern std::string gen_key_16();

extern std::string aes_128_gcm_encrypt(const std::string& plaintext,
                                       const std::string& key);

extern std::string aes_128_gcm_decrypt(const std::string& base64,
                                       const std::string& key);

} /* ns crypto */

#endif /* CRYPTO_PROJECT_2_YANG_LIU_16_05_01 */
