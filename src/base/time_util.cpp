#include "src/base/time_util.h"

#include <string.h>
#include <sys/time.h>
#include <time.h>

#include "src/base/log.h"

/*localtime非线程安全
int get_time_now(string& str_now) {
        const string format = "%Y-%m-{} %H:%M:%S";
        time_t t = time(nullptr);
        char buf[128];
        struct tm* tm_time = localtime(&t);
        if (!tm_time)  {
                return -1;
        }
        strftime(buf, sizeof(buf), format.c_str(), tm_time);
        str_now = buf;
        if (!str_now.length())  {
                return -1;
        }
        return 0;
}
*/

string get_time_now() {
    char buf[128] = "0000-00-00 00:00:00.000000";
    struct timeval tv;
    struct tm timestamp;
    if (gettimeofday(&tv, nullptr) == -1) {
        CONSOLE_ERROR("gettimeofday failed.");
        return buf;
    }
    time_t now = tv.tv_sec;
    if (localtime_r(&now, &timestamp) == nullptr) {
        CONSOLE_ERROR("localtime_r failed.");
        return buf;
    }
    if (snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d.%06d", timestamp.tm_year + 1900, timestamp.tm_mon + 1,
                 timestamp.tm_mday, timestamp.tm_hour, timestamp.tm_min, timestamp.tm_sec, (int)(tv.tv_usec)) < 0) {
        CONSOLE_ERROR("snprintf failed.");
        return buf;
    }
    return buf;
}

time_t get_time_sec() {
    time_t ret = time(nullptr);
    if (ret == -1) {
        CONSOLE_ERROR("time failed.");
    }
    return ret;
}

time_t get_time_usec() {
    struct timeval tv;
    if (gettimeofday(&tv, nullptr) == -1) {
        CONSOLE_ERROR("gettimeofday failed.");
        return -1;
    }
    return tv.tv_sec * 1000 + tv.tv_usec / 1000;
}

int get_time_zone() {
    int time_zone = 0;
    time_t t1, t2;
    struct tm tm_local, tm_utc;
    struct timeval tv;
    if (gettimeofday(&tv, nullptr) == -1) {
        CONSOLE_ERROR("gettimeofday failed.");
        return time_zone;
    }
    time_t now = tv.tv_sec;

    if (localtime_r(&now, &tm_local) == nullptr) {
        CONSOLE_ERROR("localtime_r failed.");
        return time_zone;
    }
    if (gmtime_r(&now, &tm_utc) == nullptr) {
        CONSOLE_ERROR("gmtime_r failed.");
        return time_zone;
    }

    tm_local.tm_isdst = -1;
    t1 = mktime(&tm_local);
    if (t1 == -1) {
        CONSOLE_ERROR("mktime failed.");
        return time_zone;
    }
    tm_utc.tm_isdst = -1;
    t2 = mktime(&tm_utc);
    if (t2 == -1) {
        CONSOLE_ERROR("mktime failed.");
        return time_zone;
    }
    time_zone = (t1 - t2) / 3600;
    return time_zone;
}

time_t str_time2date(const string& str_time) {
    struct tm st_time;
    if (sscanf(str_time.c_str(), "%04d-%02d-%02d %02d:%02d:%02d", &st_time.tm_year, &st_time.tm_mon, &st_time.tm_mday,
               &st_time.tm_hour, &st_time.tm_min, &st_time.tm_sec) == EOF) {
        CONSOLE_ERROR("sscanf failed.");
        return 0;
    }
    st_time.tm_year -= 1900;
    st_time.tm_mon -= 1;
    st_time.tm_isdst = 0;
    time_t t = mktime(&st_time);
    if (t == -1) {
        CONSOLE_ERROR("mktime failed.");
        return 0;
    }
    return t;
}

string date2str_time(const time_t& time) {
    char date[30] = "0000-00-00 00:00:00";
    struct tm st_time;
    if (localtime_r(&time, &st_time) == nullptr) {
        CONSOLE_ERROR("localtime_r failed.");
        return date;
    }
    if (snprintf(date, sizeof(date), "%04d-%02d-%02d %02d:%02d:%02d", st_time.tm_year + 1900, st_time.tm_mon + 1, st_time.tm_mday,
                 st_time.tm_hour, st_time.tm_min, st_time.tm_sec) < 0) {
        CONSOLE_ERROR("snprintf failed.");
        return date;
    }
    return date;
}
