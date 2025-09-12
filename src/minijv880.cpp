//
// minidexed.cpp
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
#include "minijv880.h"
#include "userinterface.h"
#include <assert.h>
#include <circle/devicenameservice.h>
#include <circle/gpiopin.h>
#include <circle/logger.h>
#include <circle/memory.h>
#include <circle/sound/hdmisoundbasedevice.h>
#include <circle/sound/i2ssoundbasedevice.h>
#include <circle/sound/pwmsoundbasedevice.h>
#include <circle/usb/usbmidihost.h>
#include <stdio.h>
#include <string.h>
#include <cstring>   // memcpy / memmove
#include <algorithm>

CMiniJV880 *CMiniJV880::s_pThis = 0;

LOGMODULE("minijv880");

CMiniJV880::CMiniJV880(CConfig *pConfig, CInterruptSystem *pInterrupt,
                       CGPIOManager *pGPIOManager, CI2CMaster *pI2CMaster, CSPIMaster *pSPIMaster,
                       FATFS *pFileSystem, CScreenDevice *mScreenUnbuffered)
    : CMultiCoreSupport(CMemorySystem::Get()), m_pConfig(pConfig),
      m_pFileSystem(pFileSystem), 
      m_Serial(pInterrupt, TRUE),
      m_pSoundDevice(0),
      screenUnbuffered(mScreenUnbuffered),
      m_bChannelsSwapped(pConfig->GetChannelsSwapped()),
      m_UI(this, pGPIOManager, pI2CMaster, pSPIMaster, pConfig),
      m_lastTick(0),
      m_lastTick1(0) {
  
      assert(m_pConfig);

      s_pThis = this;

      __atomic_store_n(&sample_write_idx, 0u, __ATOMIC_RELAXED);

  // select the sound device
  const char *pDeviceName = pConfig->GetSoundDevice();
  if (strcmp(pDeviceName, "i2s") == 0) {
    LOGNOTE("I2S mode");
    m_pSoundDevice = new CI2SSoundBaseDevice(
        pInterrupt, 32000, pConfig->GetChunkSize(), false, pI2CMaster,
        pConfig->GetDACI2CAddress(), CI2SSoundBaseDevice::DeviceModeTXOnly,
        2); // 2 channels - L+R
  } else if (strcmp(pDeviceName, "hdmi") == 0) {
#if RASPPI == 5
    LOGNOTE("HDMI mode NOT supported on RPI 5.");
#else
    LOGNOTE("HDMI mode");

    m_pSoundDevice =
        new CHDMISoundBaseDevice(pInterrupt, 32000, pConfig->GetChunkSize());

    // The channels are swapped by default in the HDMI sound driver.
    // TODO: Remove this line, when this has been fixed in the driver.
    m_bChannelsSwapped = !m_bChannelsSwapped;
#endif
  } else {
    LOGNOTE("PWM mode");

    m_pSoundDevice =
        new CPWMSoundBaseDevice(pInterrupt, 32000, pConfig->GetChunkSize());
  }
};

bool CMiniJV880::Initialize(void) {
  assert(m_pConfig);
  assert(m_pSoundDevice);

  n_mMCUcycles = m_pConfig->GetMCUcycles ();
  LOGNOTE("MCU cycles %d", n_mMCUcycles);
  //LOGNOTE("Temp: %d", m_CPUThrottle.GetTemperature ());

  if (!m_UI.Initialize ())
	{
    LOGERR("Failed to initialize UI");
		return false;
	}


	assert (m_pConfig);
	if (!m_Serial.Initialize(m_pConfig->GetMIDIBaudRate ())) 
    {
        LOGERR("Failed to initialize Serial MIDI");
        return false;
    }
	unsigned ser_options = m_Serial.GetOptions();
	// Ensure CR->CRLF translation is disabled for MIDI links
	ser_options &= ~(SERIAL_OPTION_ONLCR);
	m_Serial.SetOptions(ser_options);
  LOGNOTE("Serial MIDI Initialized");
  
  LOGNOTE("Loading emu files");
  uint8_t *rom1 = (uint8_t *)malloc(ROM1_SIZE);
  uint8_t *rom2 = (uint8_t *)malloc(ROM2_SIZE);
  uint8_t *nvram = (uint8_t *)malloc(NVRAM_SIZE);
  uint8_t *pcm1 = (uint8_t *)malloc(0x200000);
  uint8_t *pcm2 = (uint8_t *)malloc(0x200000);

  FIL f;
  unsigned int nBytesRead = 0;

  if (f_open(&f, "jv880_rom1.bin", FA_READ | FA_OPEN_EXISTING) != FR_OK) {
    LOGERR("Cannot open jv880_rom1.bin");
    return false;
  }
  f_read(&f, rom1, ROM1_SIZE, &nBytesRead);
  f_close(&f);
  if (f_open(&f, "jv880_rom2.bin", FA_READ | FA_OPEN_EXISTING) != FR_OK) {
    LOGERR("Cannot open jv880_rom2.bin");
    return false;
  }
  f_read(&f, rom2, ROM2_SIZE, &nBytesRead);
  f_close(&f);
  if (f_open(&f, "jv880_nvram.bin", FA_READ | FA_OPEN_EXISTING) != FR_OK) {
    LOGERR("Cannot open jv880_nvram.bin");
    return false;
  }
  f_read(&f, nvram, NVRAM_SIZE, &nBytesRead);
  f_close(&f);
  if (f_open(&f, "jv880_waverom1.bin", FA_READ | FA_OPEN_EXISTING) != FR_OK) {
    LOGERR("Cannot open jv880_waverom1.bin");
    return false;
  }
  f_read(&f, pcm1, 0x200000, &nBytesRead);
  f_close(&f);
  if (f_open(&f, "jv880_waverom2.bin", FA_READ | FA_OPEN_EXISTING) != FR_OK) {
    LOGERR("Cannot open jv880_waverom2.bin");
    return false;
  }
  f_read(&f, pcm2, 0x200000, &nBytesRead);
  f_close(&f);
  LOGNOTE("Emu files loaded");

  int ret = mcu.startSC55(rom1, rom2, pcm1, pcm2, nvram);
  LOGNOTE("startSC55 returned: %d", ret);
  free(rom1);
  free(rom2);
  free(nvram);
  free(pcm1);
  free(pcm2);

  // setup and start the sound device
  int Channels = 2; // 16-bit Stereo
  // Need 2 x ChunkSize / Channel queue frames as the audio driver uses
  // two DMA channels each of ChunkSize and one single single frame
  // contains a sample for each of all the channels.
  //
  // See discussion here: https://github.com/rsta2/circle/discussions/453
  if (!m_pSoundDevice->AllocateQueueFrames(2 * m_pConfig->GetChunkSize() /
                                           Channels)) {
    LOGERR("Cannot allocate sound queue");

    return false;
  }

  m_pSoundDevice->SetWriteFormat(SoundFormatSigned16, Channels);

  m_nQueueSizeFrames = m_pSoundDevice->GetQueueSizeFrames();

  m_pSoundDevice->Start();

  CMultiCoreSupport::Initialize();
  LOGNOTE("initialised");

  return true;
}

void CMiniJV880::Process(bool bPlugAndPlayUpdated) {

  m_UI.Process ();

  if (!bPlugAndPlayUpdated)
    return;

  if (m_pMIDIDevice == 0) {
    m_pMIDIDevice =
        (CUSBMIDIDevice *)CDeviceNameService::Get()->GetDevice("umidi1", FALSE);
    if (m_pMIDIDevice != 0) {
      m_pMIDIDevice->RegisterPacketHandler(USBMIDIMessageHandler);
      m_pMIDIDevice->RegisterRemovedHandler(DeviceRemovedHandler, this);
    }
  }
}

void CMiniJV880::USBMIDIMessageHandler(unsigned nCable, u8 *pPacket,
                                       unsigned nLength) {
  // LOGERR("CMiniJV880::USBMIDIMessageHandler");
  CMiniJV880 *pThis = static_cast<CMiniJV880 *>(s_pThis);
  if (!pPacket || nLength == 0) return;
  ParseMIDIData(pThis, pPacket, nLength);
  //pThis->mcu.postMidiSC55(pPacket, nLength);
}


void CMiniJV880::ParseMIDIData(CMiniJV880* pThis, const u8* pData, unsigned nLength)
{
    for (unsigned i = 0; i < nLength; i++)
    {
        u8 status = pData[i];

        if ((status & 0xF0) == 0xB0 && i + 2 < nLength) 
        {
            u8 ccNumber = pData[i + 1];
            u8 ccValue  = pData[i + 2];

            if (pThis->m_UI.m_nMIDIButtonChannel != 0) 
            {
                u8 channel         = status & 0x0F;
                u8 expectedChannel = pThis->m_UI.m_nMIDIButtonChannel - 1;

                // OMNI (17) 
                if (pThis->m_UI.m_nMIDIButtonChannel == 17 || expectedChannel == channel) 
                {
                    auto handleButton = [&](u8 confCC, CUIButton::BtnEvent ev) {
                        if (ccNumber == confCC) {
                            if (ccValue < 64) {
                                // нажали
                                pThis->m_UI.TriggerUIButtonEvent(ev);
                            } else {
                                pThis->m_UI.TriggerUIButtonEvent(CUIButton::BtnEventNone);
                            }
                            i += 2;
                            return true;
                        }
                        return false;
                    };

                    if (handleButton(pThis->m_UI.m_nMIDIPreview,      CUIButton::BtnEventPreview)) continue;
                    if (handleButton(pThis->m_UI.m_nMIDILeft,         CUIButton::BtnEventLeft)) continue;
                    if (handleButton(pThis->m_UI.m_nMIDIRight,        CUIButton::BtnEventRight)) continue;
                    if (handleButton(pThis->m_UI.m_nMIDIData,         CUIButton::BtnEventData)) continue;
                    if (handleButton(pThis->m_UI.m_nMIDIToneSelect,   CUIButton::BtnEventToneSelect)) continue;
                    if (handleButton(pThis->m_UI.m_nMIDIPatchPerform, CUIButton::BtnEventPatchPerform)) continue;
                    if (handleButton(pThis->m_UI.m_nMIDIEdit,         CUIButton::BtnEventEdit)) continue;
                    if (handleButton(pThis->m_UI.m_nMIDISystem,       CUIButton::BtnEventSystem)) continue;
                    if (handleButton(pThis->m_UI.m_nMIDIRhythm,       CUIButton::BtnEventRhythm)) continue;
                    if (handleButton(pThis->m_UI.m_nMIDIUtility,      CUIButton::BtnEventUtility)) continue;
                    if (handleButton(pThis->m_UI.m_nMIDIMute,         CUIButton::BtnEventMute)) continue;
                    if (handleButton(pThis->m_UI.m_nMIDIMonitor,      CUIButton::BtnEventMonitor)) continue;
                    if (handleButton(pThis->m_UI.m_nMIDICompare,      CUIButton::BtnEventCompare)) continue;
                    if (handleButton(pThis->m_UI.m_nMIDIEnter,        CUIButton::BtnEventEnter)) continue;

                    // Encoder
                    if (ccNumber == pThis->m_UI.m_nMIDIUp && ccValue < 64) {
                        pThis->m_UI.TriggerUIButtonEvent(CUIButton::BtnEventNone);
                        pThis->mcu.MCU_EncoderTrigger(1);
                        i += 2;
                        continue;
                    }
                    if (ccNumber == pThis->m_UI.m_nMIDIDown && ccValue < 64) {
                        pThis->m_UI.TriggerUIButtonEvent(CUIButton::BtnEventNone);
                        pThis->mcu.MCU_EncoderTrigger(0);
                        i += 2;
                        continue;
                    }
                }
            }

            i += 2;
        }
    }

    pThis->mcu.postMidiSC55(pData, nLength);
}

void CMiniJV880::DeviceRemovedHandler(CDevice *pDevice, void *pContext) {
  LOGERR("CMiniJV880::DeviceRemovedHandler");

  CMiniJV880 *pThis = static_cast<CMiniJV880 *>(pContext);
  assert(pThis != 0);

  if (pDevice == pThis->m_pMIDIDevice)
    pThis->m_pMIDIDevice = 0;
}

void CMiniJV880::Run(unsigned nCore) {
    assert(1 <= nCore && nCore < CORES);
    //int nSamples = 0;
    u8 buffer[64];

    if (nCore == 1) { // 1st core - serial MIDI
        while (true) {
            int nRead = m_Serial.Read(buffer, sizeof(buffer));
            if (nRead > 0)
                ParseMIDIData(this, buffer, nRead);
            CTimer::SimpleMsDelay(1);
        }
    } 
    else if (nCore == 2) { // 2nd core - MCU + audio output
        const int MCU_INSTR_BURST = 64;
        //unsigned log_counter = 0;
        while (true) {
            unsigned nFrames = m_nQueueSizeFrames - m_pSoundDevice->GetQueueFramesAvail();
            if (nFrames < m_nQueueSizeFrames / 2) {
                CTimer::SimpleMsDelay(1);
                continue;
            }
            int nSamples = (int)nFrames * 2;
            if (nSamples >= (int)AUDIO_BUFFER_SIZE) nSamples = (int)AUDIO_BUFFER_SIZE - 2;
            // allocate contiguous output buffer
            int16_t *out_buf = (int16_t*)malloc(nSamples * sizeof(int16_t));
            if (!out_buf) { CTimer::SimpleMsDelay(1); continue; }
            int out_pos = 0;
            while (out_pos < nSamples) {
                // 1) try to copy available audio first
                uint64_t w = __atomic_load_n(&sample_write_idx, __ATOMIC_ACQUIRE);
                uint64_t r = __atomic_load_n(&sample_read_idx,  __ATOMIC_RELAXED);
                uint64_t avail = w - r;
                if (avail > 0) {
                    uint32_t need = (uint32_t)(nSamples - out_pos);
                    uint32_t to_copy = (avail < need) ? (uint32_t)avail : need;
                    uint32_t idx = (uint32_t)(r & AUDIO_BUFFER_MASK);
                    uint32_t first = AUDIO_BUFFER_SIZE - idx;
                    if (first > to_copy) first = to_copy;
                    memcpy(&out_buf[out_pos], &sample_buffer[idx], first * sizeof(int16_t));
                    out_pos += first;
                    r += first;
                    uint32_t rem = to_copy - first;
                    if (rem) {
                        memcpy(&out_buf[out_pos], &sample_buffer[r & AUDIO_BUFFER_MASK], rem * sizeof(int16_t));
                        out_pos += rem;
                        r += rem;
                    }
                    __atomic_store_n(&sample_read_idx, r, __ATOMIC_RELEASE);
                    continue; // loop to fill remaining samples
                }
                // 2) if no samples ready, run bounded MCU burst to allow producer to progress
                int instr = 0;
                while (instr < MCU_INSTR_BURST) {
                    if (!mcu.mcu.ex_ignore)
                        mcu.MCU_Interrupt_Handle();
                    else
                        mcu.mcu.ex_ignore = 0;

                    if (!mcu.mcu.sleep)
                        mcu.MCU_ReadInstruction();
                    mcu.mcu.cycles += n_mMCUcycles;
                    __atomic_store_n(&mcu.mcu.cycles, mcu.mcu.cycles, __ATOMIC_RELEASE);
                    mcu.TIMER_Clock(mcu.mcu.cycles);
                    mcu.MCU_UpdateUART_RX();
                    mcu.MCU_UpdateUART_TX();
                    mcu.MCU_UpdateAnalog(mcu.mcu.cycles);
                    ++instr;
                }
                // then re-check available
            } // out_pos loop

            // write to audio device
            int len = nSamples * sizeof(int16_t);
            if (m_pSoundDevice->Write(out_buf, len) != len) {
                LOGERR("Sound data dropped");
            }
            free(out_buf);
        }
    }
    else if (nCore == 3) { // 3rd core - PCM Update
        constexpr uint64_t MCU_CLOCK_HZ = 12000000ull; // if your MCU clock differs, set accordingly
        constexpr uint32_t AUDIO_RATE = 32000u;
        constexpr uint64_t CYCLES_PER_SAMPLE = MCU_CLOCK_HZ / AUDIO_RATE; // 375 typical for H8@12MHz
        //constexpr uint64_t CYCLES_PER_SAMPLE_FP = CYCLES_PER_SAMPLE << 32; // fixed-point
        const uint32_t MAX_SAMPLES_PER_ITER = 128; // bound to avoid huge bursts

        uint64_t last_generated_cycles = __atomic_load_n(&mcu.mcu.cycles, __ATOMIC_RELAXED);
        //uint64_t cycles_acc_fp = 0; // optional accumulator if needed by PCM internals

        while (true) {
            uint64_t cycles_target = __atomic_load_n(&mcu.mcu.cycles, __ATOMIC_ACQUIRE);
            if (cycles_target <= last_generated_cycles) {
                CTimer::SimpleMsDelay(0);
                continue;
            }

            uint64_t cycles_avail = cycles_target - last_generated_cycles;
            uint64_t samples_to_gen = cycles_avail / CYCLES_PER_SAMPLE;
            while (samples_to_gen > 0) {
                uint32_t gen = (uint32_t) (samples_to_gen > MAX_SAMPLES_PER_ITER ? MAX_SAMPLES_PER_ITER : samples_to_gen);
                uint64_t pcm_target = last_generated_cycles + gen * CYCLES_PER_SAMPLE;

                // PCM_Update must advance internal pcm.cycles up to pcm_target and call MCU_PostSample for samples
                mcu.pcm.PCM_Update(pcm_target);

                last_generated_cycles = pcm_target;
                samples_to_gen -= gen;

                // tiny yield to let consumer/core2 progress and reduce bursts
                CTimer::SimpleMsDelay(0);
            }
        }
    }

}


void CMiniJV880::LogPCM(uint64_t logcyc1) {
 return;
  LOGNOTE("PCM Update running | MCU cycles: %u", mcu.mcu.cycles);
 return; 
}

void CMiniJV880::LogMCU(uint64_t logcyc,uint64_t  logwriteptr,int  logsleep,int  logex) {
return;
  LOGNOTE("MCU cycles: %u | sample_write_ptr: %u | sleep: %d | ex_ignore: %d",
                                mcu.mcu.cycles, sample_write_idx,
                                mcu.mcu.sleep, mcu.mcu.ex_ignore);
 return;
}