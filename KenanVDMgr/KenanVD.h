#pragma once

#define GD_ERR_VD_SUCCESS		0
#define GD_ERR_VD_START			(0x90001000)
#define GD_ERR_VD_NO_VF			(GD_ERR_VD_START+0x1)	//	0x90001001
#define GD_ERR_VD_FAIL_CREATE	(GD_ERR_VD_START+0x2)	//	0x90001002
#define GD_ERR_VD_NO_V			(GD_ERR_VD_START+0x3)	//	0x90001003
#define GD_ERR_VD_FAIL_MOUNT	(GD_ERR_VD_START+0x4)	//	0x90001004
#define GD_ERR_VD_FAIL_DISMOUNT	(GD_ERR_VD_START+0x5)	//	0x90001005


/*
DLL_EXPORT unsigned int VD_CREATE(const wchar_t* p_wszPath, const wchar_t* p_wszPwd, int p_nSize)
DLL_EXPORT unsigned int VD_MOUNT_INIT()
DLL_EXPORT unsigned int VD_MOUNT(const wchar_t* p_wszPath, const wchar_t* p_wszPwd, const wchar_t p_zDrv)
*/