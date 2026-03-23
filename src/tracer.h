/*
 * tracer.h  –  linear execution tracer for MCU/PCM emulation.
 *
 * Buffer: 1 310 720 records × 16 bytes = 20 MB.
 * When full, recording stops automatically. No data is ever overwritten.
 *
 * Usage from CMiniJV880:
 *   Tracer::Start();
 *   // (stops automatically when full, or call Stop() manually)
 *   Tracer::Save("/sdcard/jv_trace.bin");
 */
#pragma once
#include <stdint.h>
#include <stddef.h>

// ── Record types ─────────────────────────────────────────────────────────────
enum TraceKind : uint8_t {
    TR_INSTR    = 0,   // MCU instruction fetch
    TR_MEM_R    = 1,   // MCU memory read
    TR_MEM_W    = 2,   // MCU memory write
    TR_DEV_R    = 3,   // device-register read  (address 0x00–0x7F)
    TR_DEV_W    = 4,   // device-register write
    TR_IRQ      = 5,   // interrupt vector started
    TR_PCM_UPD  = 6,   // PCM_Update called (cycles target)
    TR_PCM_SAMP = 7,   // PCM sample pair posted (left, right as int16)
    TR_UART_RX  = 8,   // UART byte received
    TR_UART_TX  = 9,   // UART byte transmitted
    TR_ANALOG   = 10,  // analog sample taken (channel, value)
    TR_TIMER    = 11,  // timer compare match (which timer, frc value)
};

// ── 16-byte record ────────────────────────────────────────────────────────────
// kind         | payload            | a        | b        | c
// -------------|--------------------| ---------|----------|----------
// TR_INSTR     | (cp<<16)|pc        | cp       | opcode   | 0
// TR_MEM_R/W   | full address       | value    | 0        | 0
// TR_DEV_R/W   | 0                  | reg      | value    | 0
// TR_IRQ       | vector*4 addr      | level    | 0        | 0
// TR_PCM_UPD   | target_cycles_lo32 | 0        | 0        | 0
// TR_PCM_SAMP  | left<<16|right     | 0        | 0        | 0
// TR_UART_RX/TX| 0                  | byte     | 0        | 0
// TR_ANALOG    | 0                  | channel  | val_hi   | val_lo
// TR_TIMER     | frc                | timer_id | 0        | 0
struct TraceRecord {
    uint64_t cycles;   // MCU cycle counter at moment of event
    uint32_t payload;
    uint8_t  kind;
    uint8_t  a;
    uint8_t  b;
    uint8_t  c;
};
static_assert(sizeof(TraceRecord) == 16, "TraceRecord size mismatch");

// ── Tracer API ────────────────────────────────────────────────────────────────
namespace Tracer {

    // 1 310 720 records × 16 bytes = exactly 20 MB
    constexpr uint32_t BUF_SIZE = 1310720u;

    void     Init();        // zero buffer; call once at startup
    void     Start();       // begin capturing (resets write index)
    void     Stop();        // stop capturing
    bool     IsRunning();
    void     Push(const TraceRecord& rec); // called from hot paths; autostops when full
    int      Save(const char* path);       // returns records written, or -1
    uint32_t Count();                      // records captured so far

    // ── Inline log helpers ────────────────────────────────────────────────────

    inline void LogInstr(uint64_t cycles, uint8_t cp, uint16_t pc, uint8_t opcode) {
        if (!IsRunning()) return;
        TraceRecord r{};
        r.cycles  = cycles;
        r.kind    = TR_INSTR;
        r.payload = ((uint32_t)cp << 16) | pc;
        r.a = cp; r.b = opcode;
        Push(r);
    }

    inline void LogMemRead(uint64_t cycles, uint32_t address, uint8_t value) {
        if (!IsRunning()) return;
        TraceRecord r{};
        r.cycles = cycles; r.kind = TR_MEM_R;
        r.payload = address; r.a = value;
        Push(r);
    }

    inline void LogMemWrite(uint64_t cycles, uint32_t address, uint8_t value) {
        if (!IsRunning()) return;
        TraceRecord r{};
        r.cycles = cycles; r.kind = TR_MEM_W;
        r.payload = address; r.a = value;
        Push(r);
    }

    inline void LogDevRead(uint64_t cycles, uint8_t reg, uint8_t value) {
        if (!IsRunning()) return;
        TraceRecord r{};
        r.cycles = cycles; r.kind = TR_DEV_R;
        r.a = reg; r.b = value;
        Push(r);
    }

    inline void LogDevWrite(uint64_t cycles, uint8_t reg, uint8_t value) {
        if (!IsRunning()) return;
        TraceRecord r{};
        r.cycles = cycles; r.kind = TR_DEV_W;
        r.a = reg; r.b = value;
        Push(r);
    }

    inline void LogIRQ(uint64_t cycles, uint32_t vector_addr, uint8_t level) {
        if (!IsRunning()) return;
        TraceRecord r{};
        r.cycles = cycles; r.kind = TR_IRQ;
        r.payload = vector_addr; r.a = level;
        Push(r);
    }

    inline void LogPCMUpdate(uint64_t cycles, uint64_t target_cycles) {
        if (!IsRunning()) return;
        TraceRecord r{};
        r.cycles = cycles; r.kind = TR_PCM_UPD;
        r.payload = (uint32_t)(target_cycles & 0xFFFFFFFFu);
        Push(r);
    }

    inline void LogPCMSample(uint64_t cycles, int16_t left, int16_t right) {
        if (!IsRunning()) return;
        TraceRecord r{};
        r.cycles = cycles; r.kind = TR_PCM_SAMP;
        r.payload = ((uint32_t)(uint16_t)left << 16) | (uint16_t)right;
        Push(r);
    }

    inline void LogUARTRX(uint64_t cycles, uint8_t byte_val) {
        if (!IsRunning()) return;
        TraceRecord r{};
        r.cycles = cycles; r.kind = TR_UART_RX; r.a = byte_val;
        Push(r);
    }

    inline void LogUARTTX(uint64_t cycles, uint8_t byte_val) {
        if (!IsRunning()) return;
        TraceRecord r{};
        r.cycles = cycles; r.kind = TR_UART_TX; r.a = byte_val;
        Push(r);
    }

    inline void LogAnalog(uint64_t cycles, uint8_t channel, uint16_t value) {
        if (!IsRunning()) return;
        TraceRecord r{};
        r.cycles = cycles; r.kind = TR_ANALOG;
        r.a = channel;
        r.b = (uint8_t)(value >> 8);
        r.c = (uint8_t)(value & 0xFF);
        Push(r);
    }

    inline void LogTimer(uint64_t cycles, uint8_t timer_id, uint32_t frc) {
        if (!IsRunning()) return;
        TraceRecord r{};
        r.cycles = cycles; r.kind = TR_TIMER;
        r.payload = frc; r.a = timer_id;
        Push(r);
    }

} // namespace Tracer