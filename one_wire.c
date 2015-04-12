/*
 * one_wire.c
 *
 *  Created on: Apr 11, 2015
 *      Author: james
 */


#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>

// struct to hold ds18b20 data for linked list
struct ds18b20 {
char devPath[128];
char devID[16];
char tempData[6];
struct ds18b20 *next;
};

// Find connected 1-wire devices. 1-wire driver creates entries for each device
// in /sys/bus/w1/devices on Beaglebone Black.  Create linked list.

int8_t findDevices(struct ds18b20 *d) {
DIR *dir;
        struct dirent *dirent;
struct ds18b20 *newDev;
        char path[] = "/sys/bus/w1/devices";
        int8_t i = 0;
        dir = opendir(path);
        if (dir != NULL)
        {
                while ((dirent = readdir(dir))) {
                        // 1-wire devices are links beginning with 28-
                        if (dirent->d_type == DT_LNK &&
                                        strstr(dirent->d_name, "28-") != NULL) {
newDev = malloc( sizeof(struct ds18b20) );
                                strcpy(newDev->devID, dirent->d_name);
                                // Assemble path to OneWire device
                                sprintf(newDev->devPath, "%s/%s/w1_slave", path, newDev->devID);
                                i++;
newDev->next = 0;
d->next = newDev;
d = d->next;
                        }
}
(void) closedir(dir);
        }
        else
{
                perror ("Couldn't open the w1 devices directory");
                return 1;
        }
return i;
}

int8_t readTemp(struct ds18b20 *d) {
while(d->next != NULL){
d = d->next;
int fd = open(d->devPath, O_RDONLY);
if(fd == -1)
        {
        perror ("Couldn't open the w1 device.");
                return 1;
        }
char buf[256];
ssize_t numRead;
        while((numRead = read(fd, buf, 256)) > 0) {
                strncpy(d->tempData, strstr(buf, "t=") + 2, 5);
                float tempC = strtof(d->tempData, NULL);
                printf("Device: %s  - ", d->devID);
                printf("Temp: %.3f C  ", tempC / 1000);
                printf("%.3f F\n\n", (tempC / 1000) * 9 / 5 + 32);
        }
        close(fd);
}
return 0;
}

int main (void) {
struct ds18b20 *rootNode;
struct ds18b20 *devNode;
// Load pin configuration. Ignore error if already loaded
system("echo BB-W1 > /sys/devices/bone_capemgr.9/slots 2>/dev/null");
while(1) {
rootNode = malloc( sizeof(struct ds18b20) );
devNode = rootNode;
int8_t devCnt = findDevices(devNode);
printf("\nFound %d devices\n\n", devCnt);
readTemp(rootNode);
// Free linked list memory
while(rootNode) {
// Start with current value of root node
devNode = rootNode;
// Save address of next devNode to rootNode before deleting current devNode
rootNode = devNode->next;
// Free current devNode.
free(devNode);
}
// Now free rootNode
free(rootNode);
}
return 0;
}

