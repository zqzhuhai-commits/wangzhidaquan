#include "browser.h"

BrowserReport browser_open(BrowserMode mode, const char *url) {
    BrowserReport report = {0};
    if (mode == BrowserMode_Wifi) {
        WebWifiConfig config;
        WebWifiReturnValue reply = {0};
        /* With no connection-test URL, the applet does not use a network
           profile UUID/revision or poll Nintendo's connection-test service.
           This is an experimental alternate applet, not a proven workaround. */
        webWifiCreate(&config, NULL, url, (Uuid){0}, 0);
        report.stage = "wifi-show";
        report.rc = webWifiShow(&config, &reply);
        if (R_SUCCEEDED(report.rc)) report.reply_rc = reply.res;
        return report;
    }

    WebCommonConfig config;
    WebCommonReply reply = {0};
    report.stage = "create";
    report.rc = webPageCreate(&config, url);
    if (R_SUCCEEDED(report.rc)) {
        report.stage = "whitelist";
        report.rc = webConfigSetWhitelist(&config, "^https?://.*");
    }
    if (R_SUCCEEDED(report.rc)) {
        report.stage = "footer";
        report.rc = webConfigSetFooter(&config, true);
    }
    if (R_SUCCEEDED(report.rc)) {
        report.stage = "pointer";
        report.rc = webConfigSetPointer(&config, true);
    }
    if (R_SUCCEEDED(report.rc)) {
        report.stage = "web-show";
        report.rc = webConfigShow(&config, &reply);
    }
    if (R_SUCCEEDED(report.rc)) {
        report.stage = "exit-reason";
        report.rc = webReplyGetExitReason(&reply, &report.exit_reason);
        report.has_exit_reason = R_SUCCEEDED(report.rc);
    }
    return report;
}
