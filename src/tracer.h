#pragma once
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>

struct TraceRecord {
    uint64_t cycles;
    uint8_t  cp;
    uint16_t pc;
    char     text[128];
};

namespace Tracer {

extern volatile int s_running;

void Start();
void Stop();
bool IsRunning();

void Init();
int  Save(const char* path);
uint32_t Count();

inline void LogCPU(uint64_t cycles, uint8_t cp, uint16_t pc,
                   const char* fmt, ...)
{
    if (!IsRunning()) return;
     if (cp == 0 && pc == 0x878) return;
      if (cp == 0 && pc == 0x877) return;
       if (cp == 0 && pc == 0x88b) return;
        if (cp == 0 && pc == 0x88c) return;
    
    TraceRecord rec;
    rec.cycles = cycles;
    rec.cp = cp;
    rec.pc = pc;
    
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(rec.text, sizeof(rec.text), fmt, ap);
    va_end(ap);
    
    extern void Push(const TraceRecord& rec);
    Push(rec);
}

// Заглушки — компилируются в ноль
inline void LogInstr   (uint64_t, uint8_t, uint16_t, uint8_t)              {}
inline void LogMemRead (uint64_t, uint32_t, uint8_t)                       {}
inline void LogMemWrite(uint64_t, uint32_t, uint8_t)                       {}
inline void LogDevRead (uint64_t, uint8_t, uint8_t)                        {}
inline void LogDevWrite(uint64_t, uint8_t, uint8_t)                        {}
inline void LogIRQ     (uint64_t, uint32_t, uint8_t)                       {}
inline void LogPCMUpdate(uint64_t, uint64_t)                               {}
inline void LogPCMSample(uint64_t, int16_t, int16_t)                      {}
inline void LogUARTRX  (uint64_t, uint8_t)                                 {}
inline void LogUARTTX  (uint64_t, uint8_t)                                 {}
inline void LogAnalog  (uint64_t, uint8_t, uint16_t)                      {}
inline void LogTimer   (uint64_t, uint8_t, uint32_t)                      {}

} // namespace Tracer