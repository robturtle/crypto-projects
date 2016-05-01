#include <iostream>
#include <string.h>
#include <stdio.h>

/* OpenSSL headers */

#include "openssl/err.h"
#include <openssl/evp.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/bio.h>
/* Initializing OpenSSL */
#include "base64.h"
using namespace std;
using namespace base64;

void handleErrors()
{
    char* buffer = new char[256];
    printf("Error: %s", ERR_error_string(ERR_get_error(), buffer));
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key, unsigned char *iv,
	unsigned char *ciphertext, unsigned char *tag)
{
	EVP_CIPHER_CTX *ctx;

	int len;

	int ciphertext_len;


	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

	/* Initialise the encryption operation. */
	if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
		handleErrors();

	/* Set IV length if default 12 bytes (96 bits) is not appropriate */
	if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 16, NULL))
		handleErrors();

	/* Initialise key and IV */
	if(1 != EVP_EncryptInit_ex(ctx, NULL, NULL, key, iv)) handleErrors();

	/* Provide the message to be encrypted, and obtain the encrypted output.
	 * EVP_EncryptUpdate can be called multiple times if necessary
	 */
	if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
		handleErrors();
	ciphertext_len = len;

	/* Finalise the encryption. Normally ciphertext bytes may be written at
	 * this stage, but this does not occur in GCM mode
	 */
	if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
	ciphertext_len += len;

	/* Get the tag */
	if(1 != EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_GET_TAG, 16, tag))
		handleErrors();

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	return ciphertext_len;
}

int main(int, char **argv)
{
    int keylength = 256;
    string text;
    //Time to get some input;
    cin >> text;

    // Handy trick to get #blocks needed. Adds an additional block and truncs to # blocks needed
    const unsigned int bufferSize = text.length()+1;
    int merc = keylength/8;
    const size_t encslength = ((bufferSize + merc) / merc) * merc;

    /* generate a key with a given length */
    unsigned char aes_key[keylength/8];

    //Give Default Value
    memset(aes_key, 0, keylength/8);

    //Generates Random key
    if (!RAND_bytes(aes_key, keylength/8))
    {
        exit(-1);
    }

    //Create a random iv, make two copies, one for dec, one for enc
    unsigned char iv_enc[32];
    unsigned char enc_tag[32];
    RAND_bytes(iv_enc, 32);

    //Create buffers for encryption and decryption
    unsigned char enc_out[encslength];
    memset(enc_out, 0, sizeof(enc_out));

    //Get enc bufferSize
    unsigned char aes_input[encslength];
    memset(aes_input, 'X', encslength);
    strncpy((char *) aes_input, text.c_str(), sizeof(aes_input));

    //Begin Ecnryption
    int outlen = encrypt((unsigned char*) text.data(), text.length(), aes_key, iv_enc, enc_out, enc_tag);

    cout
      << outlen << endl
      << encode(string((char *)enc_out, sizeof(enc_out)).c_str()) << endl
      << encode(string((char *)enc_tag, sizeof(enc_tag)).c_str()) << endl
      << encode(string((char *)aes_key, sizeof(aes_key)).c_str()) << endl
      << encode(string((char *)iv_enc,  sizeof(iv_enc)).c_str()) << endl
      ;

    return 0;
}
