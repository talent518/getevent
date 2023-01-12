#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/input.h>
#include <errno.h>
#include <getopt.h>

typedef struct {
	char *path;
	int fd;
} map_t;

int main(int argc, char *argv[]) {
	int i;
	int fd;
	int ret;
	int version;
	struct input_event event;
	
	map_t maps[16];
	int map_size = 0;
	char line[1024];
	char path[256];
	double delay = 0, prevdelay = -1;

	int flag = argc > 1 ? atoi(argv[1]) : 0;
	int N = flag ? 5 : 4;

	while(fgets(line, sizeof(line), stdin)) {
		if(flag) {
			if(line[0] != '[') continue;

			ret = sscanf(line, "[%lf] %[^:]: %x %x %x", &delay, path, &event.type, &event.code, &event.value);
		} else {
			if(strncmp(line, "/dev/", 5)) continue;

			ret = sscanf(line, "%[^:]: %x %x %x", path, &event.type, &event.code, &event.value);
		}
		if(ret == N) {
			fd = -1;
			for(i = 0; i < map_size; i++) {
				if(strcmp(path, maps[i].path) == 0) {
					fd = maps[i].fd;
					break;
				}
			}
			if(fd == -1) {
				fd = open(path, O_RDWR);
				if(fd == -1) {
					fprintf(stderr, "open %s failure: %s\n", path, strerror(errno));
					goto next;
				} else if(ioctl(fd, EVIOCGVERSION, &version)) {
					fprintf(stderr, "ioctl get device version failure: %s\n", strerror(errno));
					close(fd);
				} else {
					maps[map_size].path = strdup(path);
					maps[map_size].fd = fd;
					map_size ++;
				}
			}

			if(prevdelay >= 0) usleep((delay - prevdelay) * 1000000);
			prevdelay = delay;
			
			ret = write(fd, &event, sizeof(event));
			if(ret < sizeof(event)) {
				fprintf(stderr, "write event failed(%s %d %d %d): %s\n", path, event.type, event.code, event.value, strerror(errno));
			} else {
				fprintf(stdout, "%lf %s %x %x %x\n", delay, path, event.type, event.code, event.value);
			}
		} else {
			err:
			fprintf(stderr, "sscanf failure: %d\n", ret);
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
