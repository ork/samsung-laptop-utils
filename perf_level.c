#include <glib.h>
#include <glib/gstdio.h>
#include <errno.h>
#include <fcntl.h>
#include <libnotify/notify.h>

// Helper macros

#define g_streq(s1, s2) (g_strcmp0((s1), (s2)) == 0)
#define s_write(fd, CONSTANT) do {         \
    int o_uid = getuid();                  \
    setuid(0);                             \
    write(fd, CONSTANT, sizeof(CONSTANT)); \
    setuid(o_uid);                         \
} while (0)

// Settings

#define SYSTEM_FILE "/sys/devices/platform/samsung/performance_level"

#define NOTIFY_HEAD "Performance level"
#define NOTIFY_TEMP "Now using power mode %s."
#define NOTIFY_ICON "battery"

#define PERF_SILENT "silent"
#define PERF_NORMAL "normal"

void do_notify(gchar* power_mode)
{
    gchar notify_body[256];
    NotifyNotification* n;

    g_sprintf(notify_body, NOTIFY_TEMP, power_mode);

    notify_init(NOTIFY_HEAD);
    n = notify_notification_new(NOTIFY_HEAD, notify_body, NOTIFY_ICON);
    notify_notification_show(n, NULL);
    g_object_unref(G_OBJECT(n));
    notify_uninit();
}

int main()
{
    gchar buf[256];
    gchar *contents = NULL;
    GError* error = NULL;
    int fd;

    fd = g_open(SYSTEM_FILE, O_RDWR, 0);
    if (fd < 0) {
        g_printerr("Could not open %s, aborting.\n", SYSTEM_FILE);
        goto cleanup;
    }

    read(fd, &buf, sizeof(buf));
    contents = g_strstrip(buf); // Remove trailing carriage return

    if (g_streq(contents, PERF_SILENT)) {
        s_write(fd, PERF_NORMAL);
        do_notify(PERF_NORMAL);
    } else if (g_streq(contents, PERF_NORMAL)) {
        s_write(fd, PERF_SILENT);
        do_notify(PERF_SILENT);
    } else {
        g_printerr("Unknown value '%s', aborting.\n", contents);
    }

    cleanup:
    if (!g_close(fd, &error)) {
        g_printerr("%s.\n", error->message);
    }

    return 0;
}

