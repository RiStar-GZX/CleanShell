#pragma once

#include <sys/socket.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/wireless.h>
#include <gtk/gtk.h>

// typedef struct sys_info_notify{
//     uint level;
//     char * content;
//     char * icon;
//     char * time;
// }sys_info_notify;

char * ling_sys_info_get_time_label();

bool ling_sys_get_battery_capacity(uint * capacity);

uint ling_sys_info_get_wlan_quality(const char * interface);

char * ling_sys_info_get_date_label(gboolean year,gboolean month,gboolean day,gboolean wday);

int ling_sys_set_volume(int percent);
int ling_sys_get_volume();

int ling_sys_set_brightness(int percent);
int ling_sys_get_brightness();
