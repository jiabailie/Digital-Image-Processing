// BaseCrackVerifyCode.cpp : Defines the exported functions for the DLL application.
//

#include "stdafx.h"
#include "BaseCrackVerifyCode.h"

namespace CrackVerifyCode
{
	void crackVerifyCode::CrackVerifyCode(const char* openFile, BYTE ret[CHARACTER_NUM])
	{
		graphics _graphics;

		_graphics.RemovePixels(openFile, ret);
	}
}