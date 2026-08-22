#include "tracer.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <fatfs/ff.h>

static const int BUF_SIZE = 1024 * 1024;
static TraceRecord* s_buf = nullptr;
static uint32_t s_write_idx = 0;

namespace Tracer {

volatile int s_running = 0;

void Init() {
    if (!s_buf) {
        s_buf = (TraceRecord*)malloc(BUF_SIZE * sizeof(TraceRecord));
    }
    if (s_buf) {
        memset(s_buf, 0, BUF_SIZE * sizeof(TraceRecord));
        s_write_idx = 0;
    }
}

void Start() {
    __atomic_store_n(&s_running, 1, __ATOMIC_RELEASE);
}

void Stop() {
    __atomic_store_n(&s_running, 0, __ATOMIC_RELEASE);
}

bool IsRunning() {
    return __atomic_load_n(&s_running, __ATOMIC_RELAXED) != 0;
}

void Push(const TraceRecord& rec) {
    if (!s_buf) return;
    
    uint32_t idx = __atomic_fetch_add(&s_write_idx, 1, __ATOMIC_RELAXED);
    
    if (idx >= BUF_SIZE) {
        __atomic_sub_fetch(&s_write_idx, 1, __ATOMIC_RELAXED);
        return;
    }
    
    s_buf[idx] = rec;
}

uint32_t Count() {
    uint32_t w = __atomic_load_n(&s_write_idx, __ATOMIC_RELAXED);
    return (w < BUF_SIZE) ? w : BUF_SIZE;
}

int Save(const char* path) {
    if (!s_buf) return -1;
    
    FIL file;
    if (f_open(&file, path, FA_WRITE | FA_CREATE_ALWAYS) != FR_OK) {
        return -1;
    }
    
    uint32_t count = Count();
    
    // Форматируем весь буфер в память
    char* out_buf = (char*)malloc(count * 192);
    if (!out_buf) {
        f_close(&file);
        return -1;
    }
    
    uint32_t total_len = 0;
    for (uint32_t i = 0; i < count; i++) {
        int len = snprintf(out_buf + total_len, 192, "%llu %02x:%04x %s\n",
                          (unsigned long long)s_buf[i].cycles,
                          s_buf[i].cp, s_buf[i].pc, s_buf[i].text);
        total_len += len;
    }
    
    // Пишем всё одним блоком
    UINT bw;
    FRESULT res = f_write(&file, out_buf, total_len, &bw);
    
    free(out_buf);
    f_close(&file);
    
    return (res == FR_OK && bw == total_len) ? 0 : -1;
}

} // namespace Tracer