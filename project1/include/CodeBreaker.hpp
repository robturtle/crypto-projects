#ifndef CRYPTO_PROJECT_1_CODE_BREAKER_YANG_LIU_1023
#define CRYPTO_PROJECT_1_CODE_BREAKER_YANG_LIU_1023
#include "config.hpp"
#include "setup.hpp"

namespace cipher {
  static std::vector<std::vector<std::string>> dictionaries = cipher::load_dictionaries(RESOURCE("plaintext_dictionary.txt"));

  using words = std::vector<std::string>;

  class CodeBreakerBase {

  public:
    std::string solve(std::string ciphertext) {
      words ciphers = split(ciphertext);
      words plains = _solve(ciphers);
      return join(plains);
    }

  protected:
    virtual words _solve(words ciphers) = 0;

  }; /* CodeBreakerBase */


  class CodeBreaker: public CodeBreakerBase {
  protected:
    words _solve(words ciphers);
  }; /* CodeBreaker */


} /* cipher */
#endif /* CRYPTO_PROJECT_1_CODE_BREAKER_YANG_LIU_1023 */
