#ifndef CRYPTO_PROJECT_1_CODE_BREAKER_YANG_LIU_1023
#define CRYPTO_PROJECT_1_CODE_BREAKER_YANG_LIU_1023
#include <iostream>
#include <atomic>
#include "config.hpp"
#include "setup.hpp"

namespace cipher {

  class CodeBreaker {

  public:
    std::atomic<bool> stop{false}; // tell it to abort the test

    std::string solve(std::string ciphertext, const std::vector<Dictionary> &dicts) {
      words ciphers = split(ciphertext);
      words plains = _solve(ciphers, dicts);
      return join(plains);
    }

  protected:
    bool crack(const Dictionary &dict, const std::vector<std::vector<int> > &codes,
               std::vector<std::vector<int> >::iterator it);

    virtual words _solve(const words &ciphers, const std::vector<Dictionary> & dicts);

  }; /* CodeBreaker */


} /* cipher */
#endif /* CRYPTO_PROJECT_1_CODE_BREAKER_YANG_LIU_1023 */
