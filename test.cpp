#include <sys/io.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <sys/stat.h>
#include <getopt.h>
#include <assert.h>
#include <dirent.h>
#include <iomanip>
#include "../include/acpitable.h"
#include "../include/pcie.h"

using namespace std;

int main()
{
    unsigned int BusN,DevN,FunN,mmioaddr,rcBusN,rcDevN,rcFunN,speed,width,L0sEn,L1En,L0sCap,L1Cap;
    unsigned int rcspeed,rcwidth,rcL0sEn,rcL1En,rcL0sCap,rcL1Cap;
   // unsigned int vendorID=0x1179;   //Toshiba
    unsigned int vendorID=0x8086;   //Intel Ethernet card

    pcie t;
    BusN=t.scan_pcie_tree(vendorID,0x1);
    DevN=t.scan_pcie_tree(vendorID,0x2);
    FunN=t.scan_pcie_tree(vendorID,0x3);
    cout<<"BusN="<<BusN<<"/t DevN="<<DevN<<"/t FunN="<<FunN<<endl;
    mmioaddr=t.acpi_table();
    cout<<"acpi mmio base address=0x"<<hex<<mmioaddr<<endl;
    rcBusN=t.get_rcBDF(0x1);
    rcDevN=t.get_rcBDF(0x2);
    rcFunN=t.get_rcBDF(0x3);
    cout<<"rcBusN="<<rcBusN<<"/t rcDevN="<<rcDevN<<"/t rcFunN="<<rcFunN<<endl;

    /*
    speed=t.get_cap(BusN,DevN,FunN,0x1);
    width=t.get_cap(BusN,DevN,FunN,0x2);
    L0sEn=t.get_cap(BusN,DevN,FunN,0x3);
    L1En=t.get_cap(BusN,DevN,FunN,0x4);
    L0sCap=t.get_cap(BusN,DevN,FunN,0x5);
    L1Cap=t.get_cap(BusN,DevN,FunN,0x6);

    rcspeed=t.get_cap(rcBusN,rcDevN,rcFunN,0x1);
    rcwidth=t.get_cap(rcBusN,rcDevN,rcFunN,0x2);
    rcL0sEn=t.get_cap(rcBusN,rcDevN,rcFunN,0x3);
    rcL1En=t.get_cap(rcBusN,rcDevN,rcFunN,0x4);
    rcL0sCap=t.get_cap(rcBusN,rcDevN,rcFunN,0x5);
    rcL1Cap=t.get_cap(rcBusN,rcDevN,rcFunN,0x6);

    cout<<"================================================================================="<<endl;
    cout<<"     BusN /t DevN /t FunN /t speed /t width /t L0sEn /t L1En /t L0sCap /t L1Cap  "<<endl;
    cout<<"EP:"<<BusN<<"/t"<<DevN<<"/t"<<FunN<<"/t"<<speed<<"/t"<<width<<"/t"<<L0sEn<<"/t"<<L1En<<"/t"<<L0sCap<<"/t"<<L1Cap<<endl; 
    cout<<"RC:"<<rcBusN<<"/t"<<rcDevN<<"/t"<<rcFunN<<"/t"<<rcspeed<<"/t"<<rcwidth<<"/t"<<rcL0sEn<<"/t"<<rcL1En<<"/t"<<rcL0sCap<<"/t"<<rcL1Cap<<endl;
    cout<<"========================================================================================"<<endl;
    */
    return(0);
}