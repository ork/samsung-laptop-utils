/*
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Copyright (C) 2014 Ork <ork@olol.eu>
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include <glib.h>
#include <glib/gstdio.h>
#include <glib/gi18n.h>
#include <errno.h>
#include <fcntl.h>
#include <libnotify/notify.h>

// Helper macro

#define s_write(fd, CONSTANT)                  \
    do {                                       \
        int o_uid = getuid();                  \
        setuid(0);                             \
        write(fd, CONSTANT, sizeof(CONSTANT)); \
        setuid(o_uid);                         \
    } while (0)

// Settings

#define SYSTEM_FILE "/sys/devices/platform/samsung/performance_level"

gboolean do_notify(const gchar* power_mode)
{
    gchar* body = NULL;
    NotifyNotification* notif = NULL;
    notify_init("perf_level");

    body = g_strdup_printf(_("Now using power mode %s."), power_mode);
    if (!body)
        return FALSE;

    notif = notify_notification_new(_("Performance level"), body, "battery");
    if (!notif)
        return FALSE;

    if (!notify_notification_show(notif, NULL))
        return FALSE;

    g_object_unref(G_OBJECT(notif));
    notify_uninit();
    g_free(body);

    return TRUE;
}

int main()
{
    gchar buf[256];
    gchar* contents = NULL;
    GError* error = NULL;
    int fd;

    static const gchar* modes[] = {
        "silent",
        "normal",
        0
    };

    setlocale(LC_ALL, "");
    bindtextdomain("samsung-control-laptop", "/usr/share/locale");
    textdomain("samsung-control-laptop");

    fd = g_open(SYSTEM_FILE, O_RDWR, 0);
    if (fd < 0) {
        g_printerr(_("Could not open %s, aborting.\n"), SYSTEM_FILE);
        goto cleanup;
    }

    read(fd, &buf, sizeof(buf));
    contents = g_strstrip(buf); // Remove trailing carriage return

    // Loop over power level names
    for (int i = 0; modes[i] != '\0'; ++i) {
        if (g_strcmp0(contents, modes[i]) == 0) {
            if (modes[i + 1] == '\0') {
                s_write(fd, modes[0]);
                do_notify(modes[0]);
            } else {
                s_write(fd, modes[i + 1]);
                do_notify(modes[i + 1]);
            }
        }
    }

cleanup:
    if (!g_close(fd, &error)) {
        g_printerr("%s.\n", error->message);
    }

    return 0;
}
