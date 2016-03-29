#pragma once

#include "..\Socket\BufferProcess.h"

namespace NS_REALNAME
{
	const int LEN_ID				= 30;
	const int LEN_NAME				= 50;
	const int LEN_IMAGE				= 65536 - 4096;

	const unsigned char COM_VER_1	    = 0x01;

	const unsigned char C_REGINFO		= 0x06;		//上传用户注册信息
	const unsigned char C_CAPIMG		= 0x07;		//上传用户注册图片
	const unsigned char C_IDIMG			= 0x08;		//上传用户证件图片

#pragma pack(push, 1)

	typedef struct _tag_id_img
	{
		char idtype;					//证件类型
		char IdNumber[LEN_ID + 1];		//证件号
		char name[LEN_NAME + 1];		//姓名
		int imageSize;					//图象大小
		char image[LEN_IMAGE];			//图象
	}ID_IMAGE;

#pragma pack(pop)

}