#ifndef CRYPTO_PROJECT_2_YANG_LIU_16_05_01
#define CRYPTO_PROJECT_2_YANG_LIU_16_05_01
#include <openssl/rsa.h>
#include <string>

namespace crypto {

  extern std::string gen_key_16();

  extern std::string aes_128_gcm_encrypt(const std::string& plaintext,
                                         const std::string& key);

  extern std::string aes_128_gcm_decrypt(const std::string& base64,
                                         const std::string& key);

  class Rsa {
  private:
    RSA *keypair;
    size_t maxEncSize;
    bool hasPub;
    bool hasPri;
  public:
    Rsa() {} // not usable till read*Key() called
    Rsa(const size_t keyLength, const size_t pubExponent=3);
    Rsa(const std::string& pubFname, const std::string& priFname);
  protected:
    void initNew(size_t keyLength = 2048, size_t pubExponent=3);
  public:
    void write(std::ostream& pubOut, std::ostream& priOut);
    void readPublicKey(const std::string& key);
    void readPrivateKey(const std::string& key);
  public:
    RSA* getKeypair() const;
    std::string readPlaintext(std::istream &in);
    /// read up to encrypt limit chars into plaintext
    void readPlaintext(std::istream &in, std::string &plaintext);
    std::string encrypt(const std::string& plaintext);
    std::string decrypt(const std::string& ciphertext);
  protected:
    void handleError(int retVal);
  };

} /* ns crypto */

#endif /* CRYPTO_PROJECT_2_YANG_LIU_16_05_01 */
