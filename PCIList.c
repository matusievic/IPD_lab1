#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>

#define	CONFIG_ADDRESS 0x0CF8
#define	CONFIG_DATA 0x0CFC

#define BUS_COUNT 256
#define DIVICE_COUNT 32
#define FUNCTION_COUNT 8

struct PCIHeader {
  unsigned int deviceID;
  unsigned int vendorID;
};

int getPCIHeader(int bus, int device, struct PCIHeader* header);

int main() {
    if (iopl(3)) {                                              //allow access to all I/O ports
        printf("Cannot change the I/O privilege level!\n");
        return 1;
    }

  int currentBus, currentDevice, currentFunction;

  for (currentBus = 0; currentBus < BUS_COUNT; currentBus++)                    //for each bus
    for (currentDevice = 0; currentDevice < DIVICE_COUNT; currentDevice++) {      //for each device
          struct PCIHeader currentHeader;
          if (getPCIHeader(currentBus, currentDevice, &currentHeader))
            continue;
          printf("VendorID: %d, DeviceID: %d\n", currentHeader.vendorID, currentHeader.deviceID);
    }
  return 0;
}

/**
* This fucntion fill a PCIHeader struct
* @return 0 if a device exists, 1 if it not exists
*/
int getPCIHeader(int bus, int device, struct PCIHeader* header) {
  int currentDeviceAddress = (bus << 16) | (device << 11) | (1 << 31);
  outl(currentDeviceAddress, CONFIG_ADDRESS);

  int currentDeviceData = inl(CONFIG_DATA);
  header->deviceID = (currentDeviceData >> 16) & 0xFFFF;
  header->vendorID = (currentDeviceData >> 0) & 0xFFFF;

  if (header->vendorID == 0x0000 || header->vendorID == 0xffff || header->deviceID == 0xffff)
    return 1;

  return 0;
}
