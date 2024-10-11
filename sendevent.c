#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <errno.h>
#include <time.h>
#include <sys/time.h>

typedef struct {
	char *path;
	int fd;
} map_t;

#define MAP_SIZE 16
static map_t maps[MAP_SIZE];
static int map_size = 0;


char *nowtime(void) {
	static char buf[64];
	struct timeval tv = {0, 0};
	struct tm tm;

	gettimeofday(&tv, NULL);
	localtime_r(&tv.tv_sec, &tm);

	snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d.%06ld",
		tm.tm_year + 1900, tm.tm_mon, tm.tm_mday,
		tm.tm_hour, tm.tm_min, tm.tm_sec,
		tv.tv_usec
	);

	return buf;
}

#define LOGE(fmt, args...) do { \
	if(errno) { \
		char *err = strerror(errno); \
		fprintf(stderr, "[%s] " fmt ": %s\n", nowtime(), args, err); \
	} else { \
		fprintf(stderr, "[%s] " fmt "\n", nowtime(), args); \
	} \
} while(0)

#define LOGI(fmt, args...) fprintf(stdout, "[%s] " fmt "\n", nowtime(), args)

int main(int argc, char *argv[]) {
	int i;
	int fd;
	int ret;
	int version;
	struct input_event event;
	
	char linebuf[1024];
	char path[256];
	double delay = 0, prevdelay = -1;

	int flag = argc > 1 ? atoi(argv[1]) : 0;
	int N = flag ? 5 : 4;
	long int usec = 0;
	int lineno = 0;

	while(fgets(linebuf, sizeof(linebuf), stdin)) {
		lineno ++;
		if(flag) {
			if(linebuf[0] != '[') continue;

			ret = sscanf(linebuf, "[%lf] %[^:]: %hx %hx %x", &delay, path, &event.type, &event.code, &event.value);
		} else {
			if(strncmp(linebuf, "/dev/", 5)) continue;

			ret = sscanf(linebuf, "%[^:]: %hx %hx %x", path, &event.type, &event.code, &event.value);
		}
		if(ret == N) {
			if(prevdelay >= 0) {
				usec = (delay - prevdelay) * 1000000;
				usleep(usec);
			}
			prevdelay = delay;
			
			fd = -1;
			for(i = 0; i < map_size; i++) {
				if(strcmp(path, maps[i].path) == 0) {
					fd = maps[i].fd;
					break;
				}
			}
			if(fd == -1) {
				if(map_size >= MAP_SIZE) {
					LOGE("The path index buffer is full(new: %s) at line %d", path, lineno);
					goto next;
				}
				fd = open(path, O_RDWR);
				if(fd == -1) {
					LOGE("open %s failure", path);
					goto next;
				} else if(ioctl(fd, EVIOCGVERSION, &version)) {
					LOGE("ioctl get device version failure(path: %s) at line %d", path, lineno);
					close(fd);
					goto next;
				} else {
					maps[map_size].path = strdup(path);
					maps[map_size].fd = fd;
					map_size ++;
				}
			}

			gettimeofday(&event.time, NULL);

			ret = write(fd, &event, sizeof(event));
			if(flag) {
				if(ret < sizeof(event)) {
					LOGE("write event failed(%lf(%.3lf ms) %s %04x %04x %08x) at line %d", delay, usec / 1000.0f, path, event.type, event.code, event.value, lineno);
				} else {
					LOGI("[LNO:%d] %lf(%.3lf ms) %s %04x %04x %08x", lineno, delay, usec / 1000.0f, path, event.type, event.code, event.value);
				}
			} else {
				if(ret < sizeof(event)) {
					LOGE("write event failed(%s %04x %04x %08x) at line %d", path, event.type, event.code, event.value, lineno);
				} else {
					LOGI("[LNO:%d] %s %04x %04x %08x", lineno, path, event.type, event.code, event.value);
				}
			}
		} else {
			LOGE("sscanf failure(ret: %d) at line %d", ret, lineno);
		}
		next:
		fflush(stdout);
		fflush(stderr);
	}

	for(i = 0; i < map_size; i ++) {
		free(maps[i].path);
		close(maps[i].fd);
	}

	return 0;
}
