#ifndef BLOCKTEXT_H
#define BLOCKTEXT_H

#include "Color.h"
#include <string>
#include "Array2D.h"

namespace Utility
{
	template <class ColorChannel>
	class BlockText
	{
	public:
		BlockText()
		{
			SetColor(Graphics::Color<ColorChannel>(0, 0, 0));
		};
		void SetColor(const Graphics::Color<ColorChannel>& tColor);
		void Print(const std::string& sString, unsigned int uiX, unsigned int uiY,
			Graphics::Color<ColorChannel>* pBuffer, unsigned int uiPitch);
		void Print(const std::string& sString, unsigned int uiX, unsigned int uiY,
			Array2D< Graphics::Color<ColorChannel> >& tBuffer);
	private:
		void PrintCharacter(char c, unsigned int& uiX, unsigned int uiY,
			Graphics::Color<ColorChannel>* pBuffer, unsigned int uiPitch);
		Graphics::Color<ColorChannel>	m_oColor;
	};

	template <class ColorChannel>
	void BlockText<ColorChannel>::Print(const std::string& sString, unsigned int uiX, unsigned int uiY, Array2D<Graphics::Color<ColorChannel> >& tBuffer)
	{
		if (tBuffer.GetArray())
		{
			for (unsigned int ui = 0; ui < sString.size(); ++ui)
				PrintCharacter(sString[ui], uiX, uiY, tBuffer.GetArray(), tBuffer.GetWidth());
		}
	}

	template <class ColorChannel>
	void BlockText<ColorChannel>::Print(const std::string &sString, unsigned int uiX, unsigned int uiY,
		Graphics::Color<ColorChannel>* pBuffer, unsigned int uiPitch)
	{
		if (pBuffer)
		{
			for (unsigned int ui = 0; ui < sString.size(); ++ui)
				PrintCharacter(sString[ui], pBuffer, uiX, uiY, uiPitch);
		}
	}

	template <class ColorChannel>
	void BlockText<ColorChannel>::PrintCharacter(char c, unsigned int &uiX, unsigned int uiY,
		Graphics::Color<ColorChannel>* pBuffer, unsigned int uiPitch)
	{
		const unsigned int uiMaskSize = 6;
		unsigned char ucBitMask[uiMaskSize];
		switch (c)
		{
		case 'A':
		case 'a':
		{
			ucBitMask[0] = 0x18;
			ucBitMask[1] = 0x24;
			ucBitMask[2] = 0x42;
			ucBitMask[3] = 0x7E;
			ucBitMask[4] = 0x42;
			ucBitMask[5] = 0x42;
		}break;
		/*
0 1 2 3 4 5 6 7 8 9 A B C D E F
____|____ 0x00
___0|0___ 0x18
__0_|_0__ 0x24
_0__|__0_ 0x42
_000|000_ 0x7E
_0__|__0_ 0x42
_0__|__0_ 0x42
____|____ 0x00
*/
		case 'B':
		case 'b':
		{
			ucBitMask[0] = 0x78;
			ucBitMask[1] = 0x44;
			ucBitMask[2] = 0x78;
			ucBitMask[3] = 0x44;
			ucBitMask[4] = 0x44;
			ucBitMask[5] = 0x7C;
		}break;
		/*
		____|____ 0x00
		_000|0___ 0x78
		_0__|_0__ 0x44
		_0oo|o___ 0x78
		_0__|_o__ 0x44
		_0__|_0__ 0x44
		_0oo|o0__ 0x7C
		____|____ 0x00*/
		case 'C':
		case 'c':
		{
			ucBitMask[0] = 0x7E;
			ucBitMask[1] = 0x40;
			ucBitMask[2] = 0x40;
			ucBitMask[3] = 0x40;
			ucBitMask[4] = 0x40;
			ucBitMask[5] = 0x7F;
		}break;
		/*
		----|---- 0x00
		-000|000- 0x7E
		-0--|---- 0x40
		-0--|---- 0x40
		-0--|---- 0x40
		-0--|---- 0x40
		-000|0000 0x7F
		----|---- 0x00*/
		case 'D':
		case 'd':
		{
			ucBitMask[0] = 0x78;
			ucBitMask[1] = 0x44;
			ucBitMask[2] = 0x42;
			ucBitMask[3] = 0x42;
			ucBitMask[4] = 0x42;
			ucBitMask[5] = 0x7C;
		}break;
		/*
		----|---- 0x00
		-000|0--- 0x78
		-0--|-0-- 0x44
		-0--|--0- 0x42
		-0--|--0- 0x42
		-0--|--0- 0x42
		-000|00-- 0x7C
		--------*/
		case 'E':
		case 'e':
		{
			ucBitMask[0] = 0x7E;
			ucBitMask[1] = 0x40;
			ucBitMask[2] = 0x7E;
			ucBitMask[3] = 0x40;
			ucBitMask[4] = 0x40;
			ucBitMask[5] = 0x7E;
		}break;
		/*
		----|---- 0x00
		-000|000- 0x7E
		-0--|---- 0x40
		-000|000- 0x7E
		-0--|---- 0x40
		-0--|---- 0x40
		-000|000- 0x7E
		----|---- 0x00
		*/
		case 'F':
		case 'f':
		{
			ucBitMask[0] = 0x7E;
			ucBitMask[1] = 0x40;
			ucBitMask[2] = 0x7C;
			ucBitMask[3] = 0x40;
			ucBitMask[4] = 0x40;
			ucBitMask[5] = 0x40;
		}break;
		/*
----|---- 0x00
-000|000- 0x7E
-0--|---- 0x40
-000|00-- 0x7C
-0--|---- 0x40
-0--|---- 0x40
-0--|---- 0x40
----|---- 0x00
*/
		case 'G':
		case 'g':
		{
			ucBitMask[0] = 0x3C;
			ucBitMask[1] = 0x42;
			ucBitMask[2] = 0x40;
			ucBitMask[3] = 0x46;
			ucBitMask[4] = 0x42;
			ucBitMask[5] = 0x3C;
		}break;
		/*
		----|---- 0x00
		--00|00-- 0x3C
		-0--|--0- 0x42
		-0--|---- 0x40
		-0--|-00- 0x46
		-0--|--0- 0x42
		--00|00-- 0x3C
		----|---- 0x00
		*/
		case 'H':
		case 'h':
		{
			ucBitMask[0] = 0x42;
			ucBitMask[1] = 0x42;
			ucBitMask[2] = 0x7E;
			ucBitMask[3] = 0x42;
			ucBitMask[4] = 0x42;
			ucBitMask[5] = 0x42;
		}break;
		/*
		----|---- 0x00
		-0--|--0- 0x42
		-0--|--0- 0x42
		-000|000- 0x7E
		-0--|--0- 0x42
		-0--|--0- 0x42
		-0--|--0- 0x42
		----|---- 0x00
		*/
		case 'I':
		case 'i':
		{
			ucBitMask[0] = 0x7E;
			ucBitMask[1] = 0x10;
			ucBitMask[2] = 0x10;
			ucBitMask[3] = 0x10;
			ucBitMask[4] = 0x10;
			ucBitMask[5] = 0x7E;
		}break;
		/*
		----|---- 0x00
		-ooo|ooo- 0x7E
		'''o|'''' 0x10
		'''o|'''' 0x10
		'''0|'''' 0x10
		'''0|'''' 0x10
		'ooo|ooo' 0x7E
		''''|'''' 0x00
		*/
		case 'J':
		case 'j':
		{
			ucBitMask[0] = 0x7E;
			ucBitMask[1] = 0x08;
			ucBitMask[2] = 0x08;
			ucBitMask[3] = 0x08;
			ucBitMask[4] = 0x48;
			ucBitMask[5] = 0x78;
		}break;
		/*
		''''|'''' 0x00
		'ooo|ooo' 0x7E
		''''|o''' 0x08
		''''|o''' 0x08
		''''|o''' 0x08
		'o''|o''' 0x48
		'ooo|o''' 0x78
		''''|'''' 0x00
		*/
		case 'K':
		case 'k':
		{
			ucBitMask[0] = 0x42;
			ucBitMask[1] = 0x44;
			ucBitMask[2] = 0x78;
			ucBitMask[3] = 0x44;
			ucBitMask[4] = 0x42;
			ucBitMask[5] = 0x42;
		}break;
		/*
		''''|'''' 0x00
		'o''|''o' 0x42
		'o''|'o'' 0x44
		'ooo|o''' 0x78
		'o''|'o'' 0x44
		'o''|''o' 0x42
		'o''|''o' 0x42
		''''|'''' 0x00
		*/
		case 'L':
		case 'l':
		{
			ucBitMask[0] = 0x40;
			ucBitMask[1] = 0x40;
			ucBitMask[2] = 0x40;
			ucBitMask[3] = 0x40;
			ucBitMask[4] = 0x40;
			ucBitMask[5] = 0x7E;
		}break;
		/*
		''''|'''' 0x00
		'o''|'''' 0x40
		'o''|'''' 0x40
		'o''|'''' 0x40
		'o''|'''' 0x40
		'o''|'''' 0x40
		'ooo|ooo' 0x7E
		''''|'''' 0x00
		*/
		case 'M':
		case 'm':
		{
			ucBitMask[0] = 0x42;
			ucBitMask[1] = 0x66;
			ucBitMask[2] = 0x5A;
			ucBitMask[3] = 0x42;
			ucBitMask[4] = 0x42;
			ucBitMask[5] = 0x42;
		}break;
		/*
		''''|'''' 0x00
		'o''|''o' 0x42
		'oo'|'oo' 0x66
		'o'o|o'o' 0x5A
		'o''|''o' 0x42
		'o''|''o' 0x42
		'o''|''o' 0x42
		''''|'''' 0x00
		*/
		case 'N':
		case 'n':
		{
			ucBitMask[0] = 0x42;
			ucBitMask[1] = 0x62;
			ucBitMask[2] = 0x52;
			ucBitMask[3] = 0x4A;
			ucBitMask[4] = 0x46;
			ucBitMask[5] = 0x42;
		}break;
		/*
		''''|'''' 0x00
		'o''|''o' 0x42
		'oo'|''o' 0x62
		'o'o|''o' 0x52
		'o''|o'o' 0x4A
		'o''|'oo' 0x46
		'o''|''o' 0x42
		''''|'''' 0x00
		*/
		case 'O':
		case 'o':
		{
			ucBitMask[0] = 0x3C;
			ucBitMask[1] = 0x42;
			ucBitMask[2] = 0x42;
			ucBitMask[3] = 0x42;
			ucBitMask[4] = 0x42;
			ucBitMask[5] = 0x3C;
		}break;
		/*
		''''|'''' 0x00
		''oo|oo'' 0x3C
		'o''|''o' 0x42
		'o''|''o' 0x42
		'o''|''o' 0x42
		'o''|''o' 0x42
		''oo|oo'' 0x3C
		''''|'''' 0x00
		*/
		case 'P':
		case 'p':
		{
			ucBitMask[0] = 0x7C;
			ucBitMask[1] = 0x42;
			ucBitMask[2] = 0x42;
			ucBitMask[3] = 0x7C;
			ucBitMask[4] = 0x40;
			ucBitMask[5] = 0x40;
		}break;
		/*
		''''|'''' 0x00
		'ooo|oo'' 0x7C
		'o''|''o' 0x42
		'o''|''o' 0x42
		'ooo|oo'' 0x7C
		'o''|'''' 0x40
		'o''|'''' 0x40
		''''|'''' 0x00
		*/
		case 'Q':
		case 'q':
		{
			ucBitMask[0] = 0x3C;
			ucBitMask[1] = 0x42;
			ucBitMask[2] = 0x42;
			ucBitMask[3] = 0x4A;
			ucBitMask[4] = 0x46;
			ucBitMask[5] = 0x3C;
		}break;
		/*
		''''|'''' 0x00
		''oo|oo'' 0x3C
		'o''|''o' 0x42
		'o''|''o' 0x42
		'o''|o'o' 0x4A
		'o''|'oo' 0x46
		''oo|oo'' 0x3C
		''''|'''' 0x00
		*/
		case 'R':
		case 'r':
		{
			ucBitMask[0] = 0x7C;
			ucBitMask[1] = 0x42;
			ucBitMask[2] = 0x42;
			ucBitMask[3] = 0x7C;
			ucBitMask[4] = 0x42;
			ucBitMask[5] = 0x42;
		}break;
		/*
		''''|'''' 0x00
		'ooo|oo'' 0x7C
		'o''|''o' 0x42
		'o''|''o' 0x42
		'ooo|oo'' 0x7C
		'o''|''o' 0x42
		'o''|''o' 0x42
		''''|'''' 0x00
		*/
		case 'S':
		case 's':
		{
			ucBitMask[0] = 0x3E;
			ucBitMask[1] = 0x40;
			ucBitMask[2] = 0x7C;
			ucBitMask[3] = 0x02;
			ucBitMask[4] = 0x02;
			ucBitMask[5] = 0x7C;
		}break;
		/*
		''''|'''' 0x00
		''oo|ooo' 0x3E
		'o''|'''' 0x40
		'ooo|oo'' 0x7C
		''''|''o' 0x02
		''''|''o' 0x02
		'ooo|oo'' 0x7C
		''''|'''' 0x00
		*/
		case 'T':
		case 't':
		{
			ucBitMask[0] = 0x7E;
			ucBitMask[1] = 0x10;
			ucBitMask[2] = 0x10;
			ucBitMask[3] = 0x10;
			ucBitMask[4] = 0x10;
			ucBitMask[5] = 0x10;
		}break;
		/*
		''''|'''' 0x00
		'ooo|ooo' 0x7E
		'''o|'''' 0x10
		'''o|'''' 0x10
		'''o|'''' 0x10
		'''o|'''' 0x10
		'''o|'''' 0x10
		''''|'''' 0x00
		*/
		case 'U':
		case 'u':
		{
			ucBitMask[0] = 0x42;
			ucBitMask[1] = 0x42;
			ucBitMask[2] = 0x42;
			ucBitMask[3] = 0x42;
			ucBitMask[4] = 0x42;
			ucBitMask[5] = 0x3C;
		}break;
		/*
		''''|'''' 0x00
		'o''|''o' 0x42
		'o''|''o' 0x42
		'o''|''o' 0x42
		'o''|''o' 0x42
		'o''|''o' 0x42
		''oo|oo'' 0x3C
		''''|'''' 0x00
		*/
		case 'V':
		case 'v':
		{
			ucBitMask[0] = 0x42;
			ucBitMask[1] = 0x42;
			ucBitMask[2] = 0x42;
			ucBitMask[3] = 0x42;
			ucBitMask[4] = 0x24;
			ucBitMask[5] = 0x18;
		}break;
		/*
		''''|'''' 0x00
		'o''|''o' 0x42
		'o''|''o' 0x42
		'o''|''o' 0x42
		'o''|''o' 0x42
		''o'|'o'' 0x24
		'''o|o''' 0x18
		''''|'''' 0x00
		*/
		case 'W':
		case 'w':
		{
			ucBitMask[0] = 0x42;
			ucBitMask[1] = 0x42;
			ucBitMask[2] = 0x42;
			ucBitMask[3] = 0x5A;
			ucBitMask[4] = 0x66;
			ucBitMask[5] = 0x42;
		}break;
		/*
		''''|'''' 0x00
		'o''|''o' 0x42
		'o''|''o' 0x42
		'o''|''o' 0x42
		'o'o|o'o' 0x5A
		'oo'|'oo' 0x66
		'o''|''o' 0x42
		''''|'''' 0x00
		*/
		case 'X':
		case 'x':
		{
			ucBitMask[0] = 0x42;
			ucBitMask[1] = 0x24;
			ucBitMask[2] = 0x18;
			ucBitMask[3] = 0x24;
			ucBitMask[4] = 0x42;
			ucBitMask[5] = 0x42;
		}break;
		/*
		''''|'''' 0x00
		'o''|''o' 0x42
		''o'|'o'' 0x24
		'''o|o''' 0x18
		''o'|'o'' 0x24
		'o''|''o' 0x42
		'o''|''o' 0x42
		''''|'''' 0x00
		*/
		case 'Y':
		case 'y':
		{
			ucBitMask[0] = 0x42;
			ucBitMask[1] = 0x42;
			ucBitMask[2] = 0x24;
			ucBitMask[3] = 0x18;
			ucBitMask[4] = 0x10;
			ucBitMask[5] = 0x10;
		}break;
		/*
		''''|'''' 0x00
		'o''|''o' 0x42
		'o''|''o' 0x42
		''o'|'o'' 0x24
		'''o|o''' 0x18
		'''o|'''' 0x10
		'''o|'''' 0x10
		''''|'''' 0x00
		*/
		case 'Z':
		case 'z':
		{
			ucBitMask[0] = 0x7E;
			ucBitMask[1] = 0x04;
			ucBitMask[2] = 0x08;
			ucBitMask[3] = 0x10;
			ucBitMask[4] = 0x20;
			ucBitMask[5] = 0x7E;
		}break;
		/*
					''''|'''' 0x00
					'ooo|ooo' 0x7E
					''''|'o'' 0x04
					''''|o''' 0x08
					'''o|'''' 0x10
					''o'|'''' 0x20
					'ooo|ooo' 0x7E
					''''|'''' 0x00
					*/
		case '0':
		{
			ucBitMask[0] = 0x3C;
			ucBitMask[1] = 0x42;
			ucBitMask[2] = 0x42;
			ucBitMask[3] = 0x42;
			ucBitMask[4] = 0x42;
			ucBitMask[5] = 0x3C;

		}break;
		/*
		''oo|oo''
		'o''|''o'
		'o''|''o'
		'o''|''o'
		'o''|''o'
		''oo|oo''*/
		case '1':
		{
			ucBitMask[0] = 0x10;
			ucBitMask[1] = 0x30;
			ucBitMask[2] = 0x50;
			ucBitMask[3] = 0x10;
			ucBitMask[4] = 0x10;
			ucBitMask[5] = 0x7E;
		}break;
		/*
		'''o|''''
		''oo|''''
		'o'o|''''
		'''o|''''
		'''o|''''
		'ooo|ooo'*/
		case '2':
		{
			ucBitMask[0] = 0x3C;
			ucBitMask[1] = 0x44;
			ucBitMask[2] = 0x08;
			ucBitMask[3] = 0x10;
			ucBitMask[4] = 0x20;
			ucBitMask[5] = 0x7E;
		}break;
		/*
		''oo|oo''
		'o''|'o''
		''''|o'''
		'''o|''''
		''o'|''''
		'ooo|ooo'*/
		case '3':
		{
			ucBitMask[0] = 0x7E;
			ucBitMask[1] = 0x02;
			ucBitMask[2] = 0x7E;
			ucBitMask[3] = 0x02;
			ucBitMask[4] = 0x02;
			ucBitMask[5] = 0x7E;
		}break;
		/*
		'ooo|ooo'
		''''|''o'
		'ooo|ooo'
		''''|''o'
		''''|''o'
		'ooo|ooo'*/
		case '4':
		{
			ucBitMask[0] = 0x42;
			ucBitMask[1] = 0x42;
			ucBitMask[2] = 0x42;
			ucBitMask[3] = 0x7E;
			ucBitMask[4] = 0x02;
			ucBitMask[5] = 0x02;
		}break;
		/*
		'o''|''o'
		'o''|''o'
		'o''|''o'
		'ooo|ooo'
		''''|''o'
		''''|''o'*/
		case '5':
		{
			ucBitMask[0] = 0x7E;
			ucBitMask[1] = 0x40;
			ucBitMask[2] = 0x7E;
			ucBitMask[3] = 0x02;
			ucBitMask[4] = 0x02;
			ucBitMask[5] = 0x7E;
		}break;
		/*
		'ooo|ooo'
		'o''|''''
		'ooo|ooo'
		''''|''o'
		''''|''o'
		'ooo|ooo'*/
		case '6':
		{
			ucBitMask[0] = 0x7E;
			ucBitMask[1] = 0x40;
			ucBitMask[2] = 0x7E;
			ucBitMask[3] = 0x42;
			ucBitMask[4] = 0x42;
			ucBitMask[5] = 0x7E;
		}break;
		/*
		'ooo|ooo'
		'o''|''''
		'ooo|ooo'
		'o''|''o'
		'o''|''o'
		'ooo|ooo'*/
		case '7':
		{
			ucBitMask[0] = 0x7E;
			ucBitMask[1] = 0x02;
			ucBitMask[2] = 0x04;
			ucBitMask[3] = 0x08;
			ucBitMask[4] = 0x10;
			ucBitMask[5] = 0x10;
		}break;
		/*
		'ooo|ooo'
		''''|''o'
		''''|'o''
		''''|o'''
		'''o|''''
		'''o|''''*/
		case '8':
		{
			ucBitMask[0] = 0x7E;
			ucBitMask[1] = 0x42;
			ucBitMask[2] = 0x7E;
			ucBitMask[3] = 0x42;
			ucBitMask[4] = 0x42;
			ucBitMask[5] = 0x7E;
		}break;
		/*
		'ooo|ooo'
		'o''|''o'
		'ooo|ooo'
		'o''|''o'
		'o''|''o'
		'ooo|ooo'*/
		case '9':
		{
			ucBitMask[0] = 0x7E;
			ucBitMask[1] = 0x42;
			ucBitMask[2] = 0x7E;
			ucBitMask[3] = 0x02;
			ucBitMask[4] = 0x02;
			ucBitMask[5] = 0x02;
		}break;
		/*
		'ooo|ooo'
		'o''|''o'
		'ooo|ooo'
		''''|''o'
		''''|''o'
		''''|''o'*/
		case '.':
		{
			ucBitMask[0] = ucBitMask[1] = ucBitMask[2] = ucBitMask[3] = ucBitMask[4] = 0x00;
			ucBitMask[5] = 0x10;
		}break;
		case ':':
		{
			ucBitMask[0] = 0x00;
			ucBitMask[1] = 0x10;
			ucBitMask[2] = ucBitMask[3] = 0x00;
			ucBitMask[4] = 0x10;
			ucBitMask[5] = 0x00;
		}break;
		case '(':
		{
			ucBitMask[0] = 0x08;
			ucBitMask[1] = 0x10;
			ucBitMask[2] = 0x20;
			ucBitMask[3] = 0x20;
			ucBitMask[4] = 0x10;
			ucBitMask[5] = 0x08;
		}break;
		case ')':
		{
			ucBitMask[0] = 0x10;
			ucBitMask[1] = 0x08;
			ucBitMask[2] = 0x04;
			ucBitMask[3] = 0x04;
			ucBitMask[4] = 0x08;
			ucBitMask[5] = 0x10;
		}break;
		case '-':
		{
			ucBitMask[0] = 0x00;
			ucBitMask[1] = 0x00;
			ucBitMask[2] = 0x7E;
			ucBitMask[3] = 0x00;
			ucBitMask[4] = 0x00;
			ucBitMask[5] = 0x00;
		}break;
		default:
		{
			for (unsigned int ui = 0; ui < uiMaskSize; ++ui)
				ucBitMask[ui] = 0x00;
		}
		}

		unsigned int uiIndex = uiY*uiPitch + uiX;

		for (unsigned int ui = 0; ui < uiMaskSize; ++ui)
		{
			if (ucBitMask[ui] & 0x40)
				pBuffer[uiIndex + 1 + ui*uiPitch] = m_oColor;
			if (ucBitMask[ui] & 0x20)
				pBuffer[uiIndex + 2 + ui*uiPitch] = m_oColor;
			if (ucBitMask[ui] & 0x10)
				pBuffer[uiIndex + 3 + ui*uiPitch] = m_oColor;
			if (ucBitMask[ui] & 0x08)
				pBuffer[uiIndex + 4 + ui*uiPitch] = m_oColor;
			if (ucBitMask[ui] & 0x04)
				pBuffer[uiIndex + 5 + ui*uiPitch] = m_oColor;
			if (ucBitMask[ui] & 0x02)
				pBuffer[uiIndex + 6 + ui*uiPitch] = m_oColor;
		}
		uiX += 8;
	}

	template <class ColorChannel>
	void BlockText<ColorChannel>::SetColor(const Graphics::Color<ColorChannel> &tColor)
	{
		m_oColor = tColor;
	}
}

#endif