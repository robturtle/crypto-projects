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
using namespace std;

void handleErrors()
{
    char* buffer = new char[256];
    printf("Error: %s", ERR_error_string(ERR_get_error(), buffer));
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *tag, unsigned char *key, unsigned char *iv,
	unsigned char *plaintext)
{
	EVP_CIPHER_CTX *ctx;
	int len;
	int plaintext_len;
	int ret;

	/* Create and initialise the context */
	if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

	/* Initialise the decryption operation. */
	if(!EVP_DecryptInit_ex(ctx, EVP_aes_256_gcm(), NULL, NULL, NULL))
		handleErrors();

	/* Set IV length. Not necessary if this is 12 bytes (96 bits) */
	if(!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_IVLEN, 16, NULL))
		handleErrors();

	/* Initialise key and IV */
	if(!EVP_DecryptInit_ex(ctx, NULL, NULL, key, iv)) handleErrors();

	/* Provide the message to be decrypted, and obtain the plaintext output.
	 * EVP_DecryptUpdate can be called multiple times if necessary
	 */
	if(!EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
		handleErrors();
	plaintext_len = len;

	/* Set expected tag value. Works in OpenSSL 1.0.1d and later */
	if(!EVP_CIPHER_CTX_ctrl(ctx, EVP_CTRL_GCM_SET_TAG, 16, tag))
		handleErrors();

	/* Finalise the decryption. A positive return value indicates success,
	 * anything else is a failure - the plaintext is not trustworthy.
	 */
	ret = EVP_DecryptFinal_ex(ctx, plaintext + len, &len);

	/* Clean up */
	EVP_CIPHER_CTX_free(ctx);

	if(ret > 0)
	{
		/* Success */
		plaintext_len += len;
		return plaintext_len;
	}
	else
	{
		/* Verify failed */
		return -1;
	}
}

int main(int, char **argv)
{
    int keylength = 256;
    int merc = keylength/8;
    int outlen;
    string enc_outT;
    string enc_tagT;
    string aes_keyT;
    string iv_encT;
    cin >> outlen >> enc_outT >> enc_tagT >> aes_keyT >> iv_encT;

    //Create buffers for encryption and decryption
    unsigned char enc_out[outlen];
    memset(enc_out, 'X', outlen);
    strncpy((char *) enc_out, enc_outT.c_str(), sizeof(outlen));

    unsigned char enc_tag[merc];
    memset(enc_tag, 'X', merc);
    strncpy((char *) enc_tag, enc_tagT.c_str(), merc);

    unsigned char aes_key[merc];
    memset(aes_key, 'X', merc);
    strncpy((char *) aes_key, aes_keyT.c_str(), merc);

    unsigned char iv_enc[merc];
    memset(iv_enc, 'X', merc);
    strncpy((char *) iv_enc, iv_encT.c_str(), merc);

    unsigned char dec_out[outlen];
    memset(dec_out, 'X', outlen);

    //Begin Ecnryption
    int outlen2 = decrypt(enc_out, outlen, enc_tag, aes_key, iv_enc, dec_out);

    printf("%u\n%s\n", outlen2, dec_out);
    return 0;
}
