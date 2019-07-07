#ifndef pcie_h
#define pcie_h
#include "acpitable.h"

class pcie:public acpi{
  public:
    pcie();
    ~pcie();

    unsigned int iorw(unsigned short int port);
    void ioww(unsigned short int value, unsigned short int port);
    unsigned int cfg_r(unsigned int vendorID,unsigned int offset);
    void cfg_w(unsigned int vendorID,unsigned int offset,unsigned int data);
    unsigned int cfg_r(unsigned int BusN,unsigned int DevN,unsigned int FunN,unsigned int offset);
    void cfg_w(unsigned int BusN,unsigned int DevN, unsigned int FunN,unsigned int offset,unsigned int data);
    unsigned int mmio_r(unsigned long addr);
    void mmio_w(unsigned long addr,unsigned int data);
    unsigned int get_rcBDF(int sel);//sel: 0x1-rcBusN;0x2-rcDevN;0x3-rcFunN
    unsigned int get_cap(unsigned int BusN,unsigned int DevN,unsigned int FunN,int sel);
     //sel:0x1-speed; 0x2-width;0x3-L0sEn;0x4-L1En;0x5-L0sCap;0x6-L1Cap

    
  private:
    unsigned int vendorID,BusN,DevN,FunN,mmioaddr,result;
    unsigned int rcBusN,rcDevN,rcFunN;
    unsigned int speed,width,L0sEn,L1En,L0sCap,L1Cap;
    
};


#endif