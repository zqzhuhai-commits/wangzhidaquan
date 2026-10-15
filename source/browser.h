#ifndef WEBNAV_BROWSER_H
#define WEBNAV_BROWSER_H

#include <switch.h>

typedef enum {
    BrowserMode_Standard,
    BrowserMode_Wifi
} BrowserMode;

typedef struct {
    Result rc;
    Result reply_rc;
    WebExitReason exit_reason;
    bool has_exit_reason;
    const char *stage;
} BrowserReport;

/* URL must already be normalized and validated by the caller. */
BrowserReport browser_open(BrowserMode mode, const char *url);

#endif
