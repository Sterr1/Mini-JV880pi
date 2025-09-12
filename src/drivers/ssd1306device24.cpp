//
// ssd1306device24.cpp
//
#include "ssd1306device24.h"
#include <circle/timer.h>
#include <circle/types.h>
#include <circle/util.h>
#include <assert.h>
#include "font5x8.h"

enum TSSD1306Command : u8
{
	SetMemoryAddressingMode    = 0x20,
	SetColumnAddress           = 0x21,
	SetPageAddress             = 0x22,
	SetStartLine               = 0x40,
	SetContrast                = 0x81,
	SetChargePump              = 0x8D,
	EntireDisplayOnResume      = 0xA4,
	SetNormalDisplay           = 0xA6,
	SetMultiplexRatio          = 0xA8,
	SetDisplayOff              = 0xAE,
	SetDisplayOn               = 0xAF,
	SetDisplayOffset           = 0xD3,
	SetDisplayClockDivideRatio = 0xD5,
	SetPrechargePeriod         = 0xD9,
	SetCOMPins                 = 0xDA,
	SetVCOMHDeselectLevel      = 0xDB,
};

// Compile-time (constexpr) font/graphics conversion functions.
namespace
{
	using CharData = u8[8];

	// Iterate through each row of the character data and collect bits for the nth column
	static constexpr u8 SingleColumn(const CharData& CharData, u8 nColumn)
	{
		u8 bit = 4 - nColumn;  // 5-1-nColumn for 5-width font
		u8 column = 0;

		for (u8 i = 0; i < 8; ++i)
			column |= (CharData[i] >> bit & 1) << i;

		return column;
	}

	// Templated array-like structure with precomputed font data
	template<size_t N, class F>
	class Font
	{
	public:
		using Column = u8;
		using ColumnData = Column[5];  // 5 columns for 5x8 font

		constexpr Font(const CharData(&CharData)[N], F Function) : mCharData{ {0} }
		{
			for (size_t i = 0; i < N; ++i)
				for (u8 j = 0; j < 5; ++j)
					mCharData[i][j] = Function(CharData[i], j);
		}

		const ColumnData& operator[](size_t nIndex) const { return mCharData[nIndex]; }

	private:
		ColumnData mCharData[N];
	};
}

// Single height version of the font
constexpr auto FontSingle = Font<FONT5X8_SIZE, decltype(SingleColumn)>(Font5x8, SingleColumn);

CSSD1306Device24::CSSD1306Device24 (unsigned nWidth, unsigned nHeight,
				CI2CMaster *pI2CMaster, u8 nAddress,
			    bool rotated, bool mirrored)
:	CCharDevice (SSD1306_COLUMNS24, SSD1306_ROWS24),
	m_nWidth (nWidth),
	m_nHeight (nHeight),
	m_pI2CMaster (pI2CMaster),
	m_nAddress (nAddress),
	m_bRotated (rotated),
	m_bMirrored (mirrored),
	m_FrameBuffers{{0x40, {0}}, {0x40, {0}}},
	m_nCurrentFrameBuffer(0)
{
}

CSSD1306Device24::~CSSD1306Device24 (void)
{
}

boolean CSSD1306Device24::Initialize24 (void)
{
	assert(m_pI2CMaster != nullptr);

	if (!(m_nHeight == 32 || m_nHeight == 64) || !(m_nWidth == 128 || m_nWidth == 132))
		return false;

	const bool bIsSSD1305 = m_nWidth == 132;
	const u8 nMultiplexRatio  = m_nHeight - 1;
	const u8 nCOMPins         = (m_nHeight == 32 && !bIsSSD1305) ? 0x02 : 0x12;
	const u8 nColumnAddrRange = m_nWidth - 1;
	const u8 nPageAddrRange   = m_nHeight / 8 - 1;
	const u8 nSegRemap        = (m_bRotated && !m_bMirrored) || (!m_bRotated && m_bMirrored) ? 0xA0 : 0xA1;
	const u8 nCOMScanDir      = m_bRotated ? 0xC0 : 0xC8;

	const u8 InitSequence[] =
	{
		SetDisplayOff,
		SetDisplayClockDivideRatio,	0x80,
		SetMultiplexRatio,		nMultiplexRatio,
		SetDisplayOffset,		0x00,
		SetStartLine | 0x00,
		SetChargePump,			0x14,
		SetMemoryAddressingMode,	0x00,
		nSegRemap,
		nCOMScanDir,
		SetCOMPins,			nCOMPins,
		SetContrast,			0x7F,
		SetPrechargePeriod,		0x22,
		SetVCOMHDeselectLevel,		0x20,
		EntireDisplayOnResume,
		SetNormalDisplay,
		SetDisplayOn,
		SetColumnAddress,		0x00,	nColumnAddrRange,
		SetPageAddress,			0x00,	nPageAddrRange,
	};

	for (u8 nCommand : InitSequence)
		WriteCommand24(nCommand);

	return CCharDevice::Initialize();
}

void CSSD1306Device24::DevClearCursor (void)
{
	Clear24(TRUE);
}

void CSSD1306Device24::DevSetCursorMode (boolean bVisible)
{
}

void CSSD1306Device24::DevSetChar (unsigned nPosX, unsigned nPosY, char chChar)
{
	DrawChar24 (chChar, nPosX, nPosY, FALSE, FALSE);
}

void CSSD1306Device24::DevSetCursor (unsigned nCursorX, unsigned nCursorY)
{
}

void CSSD1306Device24::DevUpdateDisplay (void) {
	WriteFrameBuffer24(true);
}

void CSSD1306Device24::WriteCommand24(u8 nCommand) const
{
	const u8 Buffer[] = { 0x80, nCommand };
	m_pI2CMaster->Write(m_nAddress, Buffer, sizeof(Buffer));
}

void CSSD1306Device24::WriteFrameBuffer24(bool bForceFullUpdate) const
{
	WriteCommand24(SetStartLine | 0x00);

	const size_t nFrameBufferSize = m_nWidth * m_nHeight / 8;
	const bool bNeedsUpdate = bForceFullUpdate || memcmp(m_FrameBuffers[0].FrameBuffer, m_FrameBuffers[1].FrameBuffer, nFrameBufferSize) != 0;

	if (bNeedsUpdate)
		m_pI2CMaster->Write(m_nAddress, &m_FrameBuffers[m_nCurrentFrameBuffer], sizeof(TFrameBufferUpdatePacket24::DataControlByte) + nFrameBufferSize);
}

void CSSD1306Device24::SwapFrameBuffers24()
{
	m_nCurrentFrameBuffer = (m_nCurrentFrameBuffer + 1) % 2;
}

void CSSD1306Device24::DrawChar24(char chChar, u8 nCursorX, u8 nCursorY, bool bInverted, bool bDoubleWidth)
{
	const size_t nRowOffset    = nCursorY * m_nWidth;
	const size_t nColumnOffset = nCursorX * (bDoubleWidth ? 10 : 5);
	u8* pFrameBuffer           = m_FrameBuffers[m_nCurrentFrameBuffer].FrameBuffer;

	if (chChar < ' ')
		chChar = ' ';

	size_t charIndex = static_cast<u8>(chChar - ' ');
	if (charIndex >= FONT5X8_SIZE)
		charIndex = 0;

	for (u8 i = 0; i < 5; ++i)
	{
		u8 nFontColumn = FontSingle[charIndex][i];

		if (i > 0 && bInverted)
			nFontColumn ^= 0xFF;

		const size_t nOffset = nRowOffset + nColumnOffset + (bDoubleWidth ? i * 2 : i);

		if (nOffset < sizeof(m_FrameBuffers[0].FrameBuffer))
			pFrameBuffer[nOffset] = nFontColumn;
			
		if (bDoubleWidth && nOffset + 1 < sizeof(m_FrameBuffers[0].FrameBuffer))
			pFrameBuffer[nOffset + 1] = nFontColumn;
	}
}

void CSSD1306Device24::Flip24()
{
	WriteFrameBuffer24();
	SwapFrameBuffers24();
}

void CSSD1306Device24::Print24(const char* pText, u8 nCursorX, u8 nCursorY, bool bClearLine, bool bImmediate)
{
	if (bClearLine)
	{
		for (u8 nChar = 0; nChar < nCursorX; ++nChar)
			DrawChar24(' ', nChar, nCursorY);
	}

	u8 currentX = nCursorX;
	while (*pText && currentX < SSD1306_COLUMNS24)
	{
		DrawChar24(*pText++, currentX, nCursorY);
		++currentX;
	}

	if (bClearLine)
	{
		while (currentX < SSD1306_COLUMNS24)
			DrawChar24(' ', currentX++, nCursorY);
	}

	if (bImmediate)
		WriteFrameBuffer24(true);
}

void CSSD1306Device24::Clear24(bool bImmediate)
{
	u8* pFrameBuffer = m_FrameBuffers[m_nCurrentFrameBuffer].FrameBuffer;
	memset(pFrameBuffer, 0, m_nWidth * m_nHeight / 8);

	if (bImmediate)
		WriteFrameBuffer24(true);
}

void CSSD1306Device24::SetBacklightState24(bool bEnabled)
{
	m_bBacklightEnabled = bEnabled;
	WriteCommand24(bEnabled ? SetDisplayOn : SetDisplayOff);
}
