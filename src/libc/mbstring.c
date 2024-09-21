#include "mbstring.h"
#include "locale.h"
#include "string.h"

int mbtowc(wchar_t* dest, const char* s, size_t num) { return _current_locale.ctype_cmpt_ptr->decode_mb(dest, s, num); }

int __mbtowc_noconv(wchar_t* dest, const char* src, size_t n) {
    if (src == 0) {
        return 0;
    }

    if (n == 0) {
        return -1;
    }

    if (dest) {
        *dest = (unsigned char)*src;
    }

    if (!*src) {
        return 0;
    }

    return 1;
}

int __wctomb_noconv(char* src, wchar_t cr) {
    if (src == 0) {
        return 0;
    }

    *src = cr;
    return 1;
}

static inline int wctomb(char* src, wchar_t wchar) { return _current_locale.ctype_cmpt_ptr->encode_wc(src, wchar); }

size_t wcstombs(char* src, const wchar_t* pwcs, size_t n) {
    int chars_written = 0;
    int result;
    char temp[3];
    wchar_t* source;

    if (!src || !pwcs) {
        return 0;
    }

    source = (wchar_t*)pwcs;

    while (chars_written <= n) {
        if (!*source) {
            *(src + chars_written) = '\0';
            break;
        } else {
            result = wctomb(temp, *source++);

            if ((chars_written + result) <= n) {
                strncpy(src + chars_written, temp, result);
                chars_written += result;
            } else {
                break;
            }
        }
    }

    return chars_written;
}
