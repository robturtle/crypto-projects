#ifndef CRYPTO_PROJECT_1_CODE_BREAKER_YANG_LIU_1023
#define CRYPTO_PROJECT_1_CODE_BREAKER_YANG_LIU_1023
#include <iostream>
#include "config.hpp"
#include "setup.hpp"

namespace cipher {

  class CodeBreaker {

  public:
    CodeBreaker(const std::string &path): dictionaries(analyze_dictionaries(path)) {}
    CodeBreaker(): CodeBreaker(RESOURCE("plaintext_dictionary.txt")) {}

    std::string solve(std::string ciphertext) {
      words ciphers = split(ciphertext);
      words plains = _solve(ciphers);
      return join(plains);
    }

  protected:
    virtual words _solve(const words &ciphers);

    std::vector<Dictionary> dictionaries;

  }; /* CodeBreaker */


} /* cipher */
#endif /* CRYPTO_PROJECT_1_CODE_BREAKER_YANG_LIU_1023 */
