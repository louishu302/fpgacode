#ifndef acpitable_h
#define acpitable_h
class acpi{
  public:
    acpi();
    ~acpi();
    unsigned int acpi_table();   //get acpi mmio base address
    unsigned int scan_pcie_tree(unsigned int vendorID,int sel); //get BDF
      // i==0x1 return Bus#  i==0x2 return Dev#   i==0x3 return function#
  private:
    unsigned int vendorID,BusN,DevN,FunN,mmioaddr, result;
    
};


#endif