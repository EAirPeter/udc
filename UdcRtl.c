#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _MSC_VER
#define __declspec(...)
#endif

void UdcRtlPrintf(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    vprintf(fmt, args);
    va_end(args);
}

__declspec(noalias) void *UdcRtlAlloc(size_t cb) {
    void *ptr = calloc(1, cb);
    if (!ptr) {
        fputs("[FATAL] Failed to allocate memory.\n", stderr);
        exit(-1);
    }
    return ptr;
}

__declspec(noalias) void *UdcRtlReAlloc(void *ptr, size_t cb) {
    void *res = realloc(ptr, cb);
    if (!res) {
        fputs("[FATAL] Failed to allocate memory.\n", stderr);
        exit(-1);
    }
    return res;
}

__declspec(noalias) void *UdcRtlAllocArray(int nLen, size_t cbOff, size_t cbElem) {
    if (nLen < 0) {
        fputs("[FATAL] Array length cannot be less than 0.\n", stderr);
        exit(-1);
    }
    return UdcRtlAlloc(cbOff + cbElem * ((size_t) (unsigned) nLen));
}

bool UdcRtlInstanceOf(void *who, void *idx) {
    void *src = *(void **) who;
    while (src && src != idx)
        src = *(void **) src;
    return src == idx;
}

void UdcRtlCheckCast(void *who, void *idx) {
    if (!UdcRtlInstanceOf(who, idx)) {
        fputs("[FATAL] Invalid cast.\n", stderr);
        exit(-1);
    }
}

void UdcRtlCheckBound(int len, int idx) {
    if (idx < 0 || idx >= len) {
        fputs("[FATAL] Array access out of bound.\n", stderr);
        exit(-1);
    }
}

int UdcRtlReadInteger() {
    int val;
    if (scanf("%d", &val) != 1) {
        fputs("[FATAL] Invalid integer.\n", stderr);
        exit(-1);
    }
    return val;
}

char *UdcRtlReadLine() {
    size_t len = 0;
    size_t cap = 256;
    char *buf = (char *) UdcRtlReAlloc(0, cap);
    int ch = fgetc(stdin);
    while (ch != EOF && ch != '\0' && ch != '\n') {
        if (len + 1 >= cap) {
            cap <<= 1;
            buf = (char *) UdcRtlReAlloc(buf, cap);
        }
        buf[len++] = (char) ch;
        ch = fgetc(stdin);
    }
    buf[len] = '\0';
    return buf;
}

int UdcRtlStrCmp(const char *lhs, const char *rhs) {
    return strcmp(lhs, rhs);
}
