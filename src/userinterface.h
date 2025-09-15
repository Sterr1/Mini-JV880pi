//
// userinterface.h
//
// MiniDexed - Dexed FM synthesizer for bare metal Raspberry Pi
// Copyright (C) 2022  The MiniDexed Team
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
#ifndef _userinterface_h
#define _userinterface_h

#include "config.h"
#include "uibuttons.h"
#include <sensor/ky040.h>
#include <display/hd44780device.h>
#include <display/ssd1306device.h>
#include "drivers/ssd1306device24.h"
#include <display/st7789device.h>
#include <circle/gpiomanager.h>
#include <circle/writebuffer.h>
#include <circle/i2cmaster.h>
#include <circle/spimaster.h>

class CMiniJV880;

class CUserInterface
{
public:
	CUserInterface (CMiniJV880 *pMiniJV880, CGPIOManager *pGPIOManager, CI2CMaster *pI2CMaster, CSPIMaster *pSPIMaster, CConfig *pConfig);
	~CUserInterface (void);

	bool Initialize (void);

	void Process (void);
	bool LCDInit(void);
	void LCDWrite (const char *pString);		// Print to optional HD44780 display
	void TriggerUIButtonEvent(CUIButton::BtnEvent event);
	
	// MIDI button mappings - cached from config
    unsigned m_nMIDIButtonChannel;
    unsigned m_nMIDIPreview;
    unsigned m_nMIDILeft;
    unsigned m_nMIDIRight;
    unsigned m_nMIDIData;
    unsigned m_nMIDIToneSelect;
    unsigned m_nMIDIPatchPerform;
    unsigned m_nMIDIEdit;
    unsigned m_nMIDISystem;
    unsigned m_nMIDIRhythm;
    unsigned m_nMIDIUtility;
    unsigned m_nMIDIMute;
    unsigned m_nMIDIMonitor;
    unsigned m_nMIDICompare;
    unsigned m_nMIDIEnter;
	unsigned m_nMIDIUp;
	unsigned m_nMIDIDown;

private:

	void EncoderEventHandler (CKY040::TEvent Event);
	static void EncoderEventStub (CKY040::TEvent Event, void *pParam);
	void UIButtonsEventHandler (CUIButton::BtnEvent Event);
	static void UIButtonsEventStub (CUIButton::BtnEvent Event, void *pParam);

	CMiniJV880 *m_pMiniJV880;
	CGPIOManager *m_pGPIOManager;
	CI2CMaster *m_pI2CMaster;
	CSPIMaster *m_pSPIMaster;
	CConfig *m_pConfig;

	CCharDevice    *m_pLCD;
	CHD44780Device *m_pHD44780;
	CSSD1306Device *m_pSSD1306;
	CST7789Display *m_pST7789Display;
	CST7789Device  *m_pST7789;
	CWriteBufferDevice *m_pLCDBuffered;
	
	CUIButtons *m_pUIButtons;

	CKY040 *m_pRotaryEncoder;
	bool m_bSwitchPressed;
	u8 *screen_buffer;

	uint8_t m_lastLCDData[80] = {0}; //test

	unsigned m_lastTick;
	int m_scrollPosition[2] = {0, 0};
	int m_scrollDir[2] = {+1, +1}; 
	unsigned long m_lastScrollTime = 0;
	bool isPaused[2] = {true, true};  // Start paused
	bool isAtEnd[2] = {false, false};
	unsigned long pauseStartTime[2] = {0, 0};
	static const unsigned long SCROLL_INTERVAL = 1000000;
	static const unsigned long PAUSE_DURATION = 1500000;
	static const int ACTUAL_COLS = 24;

	
};

#endif
