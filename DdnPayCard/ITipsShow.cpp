#include "StdAfx.h"
#include "ITipsShow.h"
#include "TipsShow.h"


ITipsShow * CreateTipsShowObject()
{
	CTipsShow *pTipsShow = new CTipsShow();

	return pTipsShow;
}