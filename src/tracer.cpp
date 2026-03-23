/*
 * tracer.cpp  –  linear execution tracer implementation.
 * Uses FatFs (f_open / f_write) matching the rest of the project.
 */
#include "tracer.h"
#include <string.h>
#include <stdlib.h>
#include <fatfs/ff.h>

namespace Tracer {

static TraceRecord*      s_buf       = nullptr;
static volatile uint32_t s_write_idx = 0;
static volatile int      s_running   = 0;

struct FileHeader {
    char     magic[8];    // "JVTRACE\0"
    uint32_t version;     // 1
    uint32_t record_size; // sizeof(TraceRecord) == 16
    uint32_t buf_size;    // BUF_SIZE
    uint32_t count;       // records actually saved
};

void Init() {
    if (!s_buf) {
        s_buf = (TraceRecord*)malloc(BUF_SIZE * sizeof(TraceRecord));
    }
    if (s_buf) {
        memset(s_buf, 0, BUF_SIZE * sizeof(TraceRecord));
    }
    __atomic_store_n(&s_write_idx, 0u, __ATOMIC_RELEASE);
    __atomic_store_n(&s_running,   0,  __ATOMIC_RELEASE);
}

void Start() {
    __atomic_store_n(&s_write_idx, 0u, __ATOMIC_RELEASE);
    __atomic_store_n(&s_running,   1,  __ATOMIC_RELEASE);
}

void Stop() {
    __atomic_store_n(&s_running, 0, __ATOMIC_RELEASE);
}

bool IsRunning() {
    return __atomic_load_n(&s_running, __ATOMIC_RELAXED) != 0;
}

void Push(const TraceRecord& rec) {
    if (!s_buf) return;
    uint32_t idx = __atomic_fetch_add(&s_write_idx, 1u, __ATOMIC_RELAXED);
    if (idx >= BUF_SIZE) {
        __atomic_store_n(&s_write_idx, BUF_SIZE, __ATOMIC_RELAXED);
        Stop();
        return;
    }
    s_buf[idx] = rec;
}

uint32_t Count() {
    uint32_t w = __atomic_load_n(&s_write_idx, __ATOMIC_ACQUIRE);
    return (w < BUF_SIZE) ? w : BUF_SIZE;
}

int Save(const char* path) {
    Stop();

    if (!s_buf) return -1;

    uint32_t count = Count();

    FIL file;
    FRESULT res = f_open(&file, path, FA_WRITE | FA_CREATE_ALWAYS);
    if (res != FR_OK) return -1;

    // Write header
    FileHeader hdr{};
    memcpy(hdr.magic, "JVTRACE", 8);
    hdr.version     = 1;
    hdr.record_size = sizeof(TraceRecord);
    hdr.buf_size    = BUF_SIZE;
    hdr.count       = count;

    UINT bw;
    res = f_write(&file, &hdr, sizeof(hdr), &bw);
    if (res != FR_OK || bw != sizeof(hdr)) {
        f_close(&file);
        return -1;
    }

    // Write records in chunks to avoid huge single f_write calls
    constexpr uint32_t CHUNK = 512; // records per chunk
    uint32_t remaining = count;
    uint32_t offset    = 0;
    while (remaining > 0) {
        uint32_t n = (remaining < CHUNK) ? remaining : CHUNK;
        UINT bytes = n * sizeof(TraceRecord);
        res = f_write(&file, &s_buf[offset], bytes, &bw);
        if (res != FR_OK || bw != bytes) {
            f_close(&file);
            return -1;
        }
        offset    += n;
        remaining -= n;
    }

    f_close(&file);
    return (int)count;
}

} // namespace Tracer