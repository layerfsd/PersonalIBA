#pragma once

typedef struct  RegUserData_tag
{
	unsigned int cbCount;
	char szName[32 + 1];
	char szCertificateID[20 + 1];
	char szCertificateType[4 + 1];
	char szAddr[32 + 1];
	char szCountry[12 + 1];
	char szOther[32 + 1];
	char szPassword[20 + 1];
	char Sex;
}RegUserData, *PRegUserData;