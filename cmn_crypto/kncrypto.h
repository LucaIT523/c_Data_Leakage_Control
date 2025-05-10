#pragma once

void kn_encrypt(unsigned char* p_pBuff, DWORD p_cchBuff);
void kn_decrypt(unsigned char* p_pBuff, DWORD p_cchBuff);

void kn_encrypt_with_pwd(unsigned char* p_pBuff, DWORD p_cchBuff, const wchar_t* p_pKey);
void kn_decrypt_with_pwd(unsigned char* p_pBuff, DWORD p_cchBuff, const wchar_t* p_pKey);
