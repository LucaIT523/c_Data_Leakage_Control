#include "pch.h"
#include "kncrypto.h"
#include "AES.h"
#include "SHA256.h"
#include "ClaSecuPolicy.h"

void kn_encrypt(unsigned char* p_pBuff, DWORD p_cchBuff)
{
	unsigned char pKey[32]; memset(pKey, 0, sizeof(pKey));

	ClaSecuPolicy::GetDlpCryptoKey(pKey);
	for (int i = 0; i < p_cchBuff; i++) {
		p_pBuff[i] = p_pBuff[i] ^ pKey[i % 32];
	}
}

void kn_decrypt(unsigned char* p_pBuff, DWORD p_cchBuff)
{
	unsigned char pKey[32]; memset(pKey, 0, sizeof(pKey));

	ClaSecuPolicy::GetDlpCryptoKey(pKey);
	for (int i = 0; i < p_cchBuff; i++) {
		p_pBuff[i] = p_pBuff[i] ^ pKey[i % 32];
	}
}

void kn_encrypt_with_pwd(unsigned char* p_pBuff, DWORD p_cchBuff, const wchar_t* p_pKey)
{
	SHA256 sha;
	wchar_t wszPwd[MAX_PATH]; memset(wszPwd, 0, sizeof(wszPwd));
	wcscpy_s(wszPwd, wcslen(p_pKey)+1, p_pKey);
	sha.update((const uint8_t*)wszPwd, sizeof(wszPwd));
	unsigned char pKey[32];
	memcpy(pKey, sha.digest().data(), 32);
	unsigned char pIV[32];
	sha.update("iv-pwd");
	memcpy(pIV, sha.digest().data(), 32);

	AES aes;
	unsigned char* p = aes.EncryptCBC(p_pBuff, (p_cchBuff / 32) * 32, pKey, pIV);
	memcpy(p_pBuff, p, p_cchBuff);
	delete[] p;
}

void kn_decrypt_with_pwd(unsigned char* p_pBuff, DWORD p_cchBuff, const wchar_t* p_pKey)
{
	SHA256 sha;
	wchar_t wszPwd[MAX_PATH]; memset(wszPwd, 0, sizeof(wszPwd));
	wcscpy_s(wszPwd, wcslen(p_pKey) + 1, p_pKey);
	sha.update((const uint8_t*)wszPwd, sizeof(wszPwd));
	unsigned char pKey[32];
	memcpy(pKey, sha.digest().data(), 32);
	unsigned char pIV[32];
	sha.update("iv-pwd");
	memcpy(pIV, sha.digest().data(), 32);

	AES aes;
	unsigned char* p = aes.DecryptCBC(p_pBuff, (p_cchBuff / 32) * 32, pKey, pIV);
	memcpy(p_pBuff, p, p_cchBuff);
	delete[] p;
}
