#include <stdio.h>
#include <stdlib.h>
#include <sys/io.h>
#include "pci.h"

#define	CONFIG_ADDRESS 0x0CF8
#define	CONFIG_DATA 0x0CFC

#define BUS_COUNT 256
#define DIVICE_COUNT 32
#define FUNCTION_COUNT 8


//a stucture that stores data from the port
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

  int currentBus, currentDevice, currentFunction, i;

  for (currentBus = 0; currentBus < BUS_COUNT; currentBus++)                    //for each bus
    for (currentDevice = 0; currentDevice < DIVICE_COUNT; currentDevice++) {    //for each device
          //get data from the port
          struct PCIHeader currentHeader;
          if (getPCIHeader(currentBus, currentDevice, &currentHeader))
            continue;

          //print device and venrod id
          printf("\nVendorID: %7x, DeviceID: %7x -> ", currentHeader.vendorID, currentHeader.deviceID);

          //print vendor
          for (i = 0; i < PCI_VENTABLE_LEN; i++)
            if (PciVenTable[i].VenId == currentHeader.vendorID) {
              printf("%s, ", PciVenTable[i].VenFull);
              break;
            }

          //print device
          for (i = 0; i < PCI_DEVTABLE_LEN; i++)
            if (PciDevTable[i].VenId == currentHeader.vendorID && PciDevTable[i].DevId == currentHeader.deviceID) {
              printf("%s", PciDevTable[i].ChipDesc);
              break;
            }
    }
  printf("\n");
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
  header->vendorID = currentDeviceData & 0xFFFF;

  if (header->vendorID == 0x0000 || header->vendorID == 0xffff || header->deviceID == 0xffff)
    return 1;

  return 0;
}
