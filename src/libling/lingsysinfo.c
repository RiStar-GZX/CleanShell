#include "lingsysinfo.h"
#include <libnotify/notify.h>


char * ling_sys_info_get_time_label(){
    time_t curTime = time(NULL);
    struct tm *Tm = localtime(&curTime);
    char * str=malloc(10);
    char hour[10];
    char min[10];
    if(Tm->tm_hour<10)sprintf(hour,"0%d",Tm->tm_hour);
    else sprintf(hour,"%d",Tm->tm_hour);
    if(Tm->tm_min<10)sprintf(min,"0%d",Tm->tm_min);
    else sprintf(min,"%d",Tm->tm_min);
    sprintf(str,"%s:%s",hour,min);
    return str;
}

char * ling_sys_info_get_date_label(gboolean year,gboolean month,gboolean day,gboolean wday){
    time_t curTime = time(NULL);
    struct tm *Tm = localtime(&curTime);
    char * str=malloc(100);
    char yearc[10]="";
    char monthc[10]="";
    char dayc[10]="";
    char wdayc[10]="";
    if(year)sprintf(yearc,"%d年 ",Tm->tm_year+1900);
    if(month)sprintf(monthc,"%d月",Tm->tm_mon+1);
    if(day)sprintf(dayc,"%d日 ",Tm->tm_mday);
    if(wday){
        uint wd = Tm->tm_wday;
        if(wd==0)sprintf(wdayc,"星期日");
        if(wd==1)sprintf(wdayc,"星期一");
        if(wd==2)sprintf(wdayc,"星期二");
        if(wd==3)sprintf(wdayc,"星期三");
        if(wd==4)sprintf(wdayc,"星期四");
        if(wd==5)sprintf(wdayc,"星期五");
        if(wd==6)sprintf(wdayc,"星期六");
    }
    sprintf(str,"%s%s%s%s",yearc,monthc,dayc,wdayc);
    return str;
}

bool ling_sys_get_battery_capacity(uint * capacity){
    FILE *fp;
    //获取电量
    fp = fopen("/sys/class/power_supply/BAT0/capacity", "r");
    if (fp == NULL) {
        perror("Error opening capacity file");
        return 0;
    }
    fscanf(fp, "%d", capacity);
    fclose(fp);

    //获取充电信息
    fp = fopen("/sys/class/power_supply/BAT0/status", "r");
    if (!fp) return -1;

    char status[32];
    fgets(status, sizeof(status), fp);
    fclose(fp);

    bool is_charging;
    if (strstr(status, "Charging")) is_charging = TRUE;
    else if (strstr(status, "Not charging")) is_charging = TRUE;
    else is_charging = FALSE;

    return is_charging;
}

uint ling_sys_info_get_wlan_quality(const char * interface){
    //    int sockfd;
    //    struct iwreq wrq;
    //    struct iw_statistics stats;

    //    // 创建socket
    //    if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    //        perror("socket failed");
    //        return 0;
    //    }

    //    // 设置接口名称
    //    strncpy(wrq.ifr_name, interface, IFNAMSIZ);
    //    wrq.u.data.pointer = (caddr_t) &stats;
    //    wrq.u.data.length = sizeof(stats);

    //    // 使用ioctl获取质量指标
    //    if (ioctl(sockfd, SIOCGIWSTATS, &wrq) < 0) {
    //        perror("ioctl failed");
    //        close(sockfd);
    //        return 0;
    //    }

    //    // 关闭socket
    //    close(sockfd);

    //    // 注意，这里的qual.qual通常是0-70或0-93的数值，具体取决于驱动和配置。
    //    return stats.qual.qual;
    return 66;
}

int ling_sys_set_volume(int percent) {
    char cmd[100];
    sprintf(cmd,"pamixer --set-volume %d",percent);
    system(cmd);
    return 1;
}


int ling_sys_get_volume() {
    FILE *fp;
    char buffer[10];
    int volume = 0;

    // 执行pamixer命令获取音量
    fp = popen("pamixer --get-volume", "r");
    if (fp == NULL) {
        perror("popen failed");
        return -1;
    }

    // 读取命令输出
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        volume = atoi(buffer);
    }else return -1;

    pclose(fp);
    return volume;
}

int ling_sys_set_brightness(int percent) {
    char cmd[100];
    sprintf(cmd,"brightnessctl set %d%%",percent);
    system(cmd);
    return 1;
}

int ling_sys_get_brightness() {
    FILE *fp;
    char buffer[10];
    int brightness = 0;
    int max = 0;

    fp = popen("brightnessctl m", "r");
    if (fp == NULL) {
        perror("popen failed");
        return -1;
    }
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        max = atoi(buffer);
    }else return -1;
    pclose(fp);

    fp = popen("brightnessctl g", "r");
    if (fp == NULL) {
        perror("popen failed");
        return -1;
    }
    if (fgets(buffer, sizeof(buffer), fp) != NULL) {
        brightness = atoi(buffer);
    }else return -1;

    pclose(fp);
    //if(brightness==0)return 0;
    //g_print("\n\nas:%d %d %d\n\n",brightness*100/max,brightness,max);
    return (brightness*100)/max;
}

//GList * ling_sys_info_get_notify(GList * info_list){
//    return NULL;
//}

//void ling_sys_info_clear_notify(GList * info_list){

//}

//void ling_sys_info_del_notify(){

//}


// #include <libnotify/notify.h>
// #include <dbus/dbus.h>
// #include <stdio.h>
// #include <unistd.h>
// #include <stdint.h>

// // 发送通知函数
// void send_notification(const char *title, const char *message) {
//     NotifyNotification *n = notify_notification_new(title, message, "dialog-information");
//     notify_notification_set_timeout(n, 3000);
//     notify_notification_show(n, NULL);
//     g_object_unref(G_OBJECT(n));
// }

// // 接收通知回调
// static DBusHandlerResult handle_message(DBusConnection *conn, DBusMessage *msg, void *user_data) {
//     if (dbus_message_is_signal(msg, "org.freedesktop.Notifications", "NotificationClosed")) {
//         uint32_t id;
//         dbus_message_get_args(msg, NULL, DBUS_TYPE_UINT32, &id, DBUS_TYPE_INVALID);
//         printf("通知ID %d 已关闭\n", id);
//     }
//     return DBUS_HANDLER_RESULT_HANDLED;
// }

// int main() {
//     // 初始化通知系统
//     notify_init("Notification Demo");
//     DBusConnection *conn = dbus_bus_get(DBUS_BUS_SESSION, NULL);

//     // 设置消息过滤器
//     dbus_bus_add_match(conn,
//         "type='signal',interface='org.freedesktop.Notifications'", NULL);
//     dbus_connection_add_filter(conn, handle_message, NULL, NULL);

//     // 示例：发送测试通知
//     send_notification("系统提示", "通知系统已启动");

//     // 主循环处理消息
//     while (1) {
//         dbus_connection_read_write_dispatch(conn, 1000);
//         sleep(1);
//     }

//     notify_uninit();
//     return 0;
// }
