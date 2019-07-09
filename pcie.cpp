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

pcie::pcie()
{
	BusN=0x0;
	DevN=0x0;
	FunN=0x0;
	mmioaddr=acpi_table();
	result=0x0;
	rcBusN=0x0;
	rcDevN=0x0;
	rcFunN=0x0;
	speed=0x0;
	width=0x0;
	L0sEn=0x0;
	L1En=0x0;
	L0sCap=0x0;
	L1Cap=0x0;

}

pcie::~pcie()
{

}

unsigned int pcie::iorw(unsigned short int port)     //io read word aligned
{
     unsigned int value;
     value=inw(port);
     return(value); 
}

void pcie::ioww(unsigned short int value, unsigned short int port)   //io write word aligned
{
   outw(value,port);   
}


unsigned int pcie::cfg_r(unsigned int vendorID,unsigned int offset)
 {
    unsigned int mmioaddr,phyAddr,poffset,data, *mapBuf;
    int fd;
    mmioaddr=acpi_table();
    BusN=scan_pcie_tree(vendorID,0x1);
    DevN=scan_pcie_tree(vendorID,0x2);
    FunN=scan_pcie_tree(vendorID,0x3);
    fd=open("/dev/mem",O_RDWR,0);
     if(fd<0)
     printf("open device failed. fd=%d error message is: %s\n",fd, strerror(errno));
     phyAddr= (mmioaddr|(BusN<<20)|(DevN<<15)|(FunN<<12))&0xFFFFF000 ; //4K memory page aligned
     poffset= offset/4;    //In one 4K page access - Dword aligned
     mapBuf = (unsigned int*) mmap(NULL,4096,PROT_READ|PROT_WRITE, MAP_SHARED, fd, phyAddr);
     data= *(mapBuf+poffset);
     munmap(mapBuf,4096);
     close(fd);
     return data; 
 }

void pcie::cfg_w(unsigned int vendorID,unsigned int offset,unsigned int data)
{
    unsigned int mmioaddr,phyAddr,poffset,*mapBuf;
    int fd;
    mmioaddr=acpi_table();
    BusN=scan_pcie_tree(vendorID,0x1);
    DevN=scan_pcie_tree(vendorID,0x2);
    FunN=scan_pcie_tree(vendorID,0x3);
    fd=open("/dev/mem",O_RDWR,0);
     if(fd<0)
     printf("open device failed. fd=%d error message is: %s\n",fd, strerror(errno));
     phyAddr= (mmioaddr|(BusN<<20)|(DevN<<15)|(FunN<<12))&0xFFFFF000 ; //4K memory page aligned
     poffset= offset/4;    //In one 4K page access - Dword aligned
     mapBuf = (unsigned int*) mmap(NULL,4096,PROT_READ|PROT_WRITE, MAP_SHARED, fd, phyAddr);
     *(mapBuf+poffset)=data;
     munmap(mapBuf,4096);
     close(fd);    

}

 


 unsigned int pcie::cfg_r(unsigned int BusN,unsigned int DevN,unsigned int FunN,unsigned int offset)
 {
    unsigned int mmioaddr,phyAddr,poffset,data, *mapBuf;
    int fd;
    mmioaddr=acpi_table();
    //read_pci_tree(vendorID);
    fd=open("/dev/mem",O_RDWR,0);
     if(fd<0)
     printf("open device failed. fd=%d error message is: %s\n",fd, strerror(errno));
     phyAddr= (mmioaddr|(BusN<<20)|(DevN<<15)|(FunN<<12))&0xFFFFF000 ; //4K memory page aligned
     poffset= offset/4;    //In one 4K page access - Dword aligned
     mapBuf = (unsigned int*) mmap(NULL,4096,PROT_READ|PROT_WRITE, MAP_SHARED, fd, phyAddr);
     data= *(mapBuf+poffset);
     munmap(mapBuf,4096);
     close(fd);
     return data; 
 }

void pcie::cfg_w(unsigned int BusN,unsigned int DevN, unsigned int FunN,unsigned int offset,unsigned int data)
{
    unsigned int mmioaddr,phyAddr,poffset,*mapBuf;
    int fd;
    mmioaddr=acpi_table();
   // read_pci_tree(vendorID);
    fd=open("/dev/mem",O_RDWR,0);
     if(fd<0)
     printf("open device failed. fd=%d error message is: %s\n",fd, strerror(errno));
     phyAddr= (mmioaddr|(BusN<<20)|(DevN<<15)|(FunN<<12))&0xFFFFF000 ; //4K memory page aligned
     poffset= offset/4;    //In one 4K page access - Dword aligned
     mapBuf = (unsigned int*) mmap(NULL,4096,PROT_READ|PROT_WRITE, MAP_SHARED, fd, phyAddr);
     *(mapBuf+poffset)=data;
     munmap(mapBuf,4096);
     close(fd);
     

}

unsigned int pcie::mmio_r(unsigned long addr)
 {
   unsigned int *mapBuf,offset,data;
    int fd;   
    unsigned long pAddr; 
    fd=open("/dev/mem",O_RDWR,0);
     if(fd<0)
     printf("open device failed. fd=%d error message is: %s\n",fd, strerror(errno));
     //address 4k aligned  
     pAddr = addr&0xFFFFFFFFFFFFF000;    //4K memory page aligned
     offset= ((addr&0xFFF)/4);     //In one 4K page access - Dword aligned
     mapBuf =(unsigned int*) mmap(NULL,4096,PROT_READ|PROT_WRITE, MAP_SHARED, fd, pAddr);
     data=*(mapBuf+offset);
     munmap(mapBuf,4096);
     close(fd);
     return data; 

 }

void pcie::mmio_w(unsigned long addr,unsigned int data)
{
    unsigned int *mapBuf,offset;
    int fd;   
    unsigned long pAddr; 
    fd=open("/dev/mem",O_RDWR,0);
     if(fd<0)
     printf("open device failed. fd=%d error message is: %s\n",fd, strerror(errno));
     //address 4k aligned  
     pAddr = addr&0xFFFFFFFFFFFFF000;    //4K memory page aligned
     offset= ((addr&0xFFF)/4);     //In one 4K page access - Dword aligned
     mapBuf =(unsigned int*) mmap(NULL,4096,PROT_READ|PROT_WRITE, MAP_SHARED, fd, pAddr);
     *(mapBuf+offset)=data;
     munmap(mapBuf,4096);
     close(fd);
}

unsigned int pcie::get_rcBDF(int sel,unsigned int vendorID)
{
    unsigned int *mapBuf,mmioaddr, phyAddr,data;
    int fd;
    BusN=scan_pcie_tree(vendorID,0x1);    
    DevN=scan_pcie_tree(vendorID,0x2);
    FunN=scan_pcie_tree(vendorID,0x3);     
    rcFunN=0x0;  //root port function N=0
    rcBusN=0x0;
    rcDevN=0x0;
    mmioaddr=acpi_table();
    
    fd=open("/dev/mem",O_RDWR,0);
     if(fd<0)
     printf("open device failed. fd=%d error message is: %s\n",fd, strerror(errno));

     for(rcBusN=0;rcBusN<0x100;rcBusN++)
      {
        for(rcDevN=0;rcDevN<0x20;rcDevN++)
        { 
          for(rcFunN=0;rcFunN<8;rcFunN++)
         {
           phyAddr= (mmioaddr|(rcBusN<<20)|(rcDevN<<15)|(rcFunN<<12))&0xFFFFF000 ; //4K memory page aligned
           mapBuf = (unsigned int*) mmap(NULL,4096,PROT_READ|PROT_WRITE, MAP_SHARED, fd, phyAddr);
           data=(*(mapBuf+0x6))&0x0000FF00;      //offset 0x19: secondary Bus#
           data>>=8;
           //cout<<"data=0x"<<hex<<data<<endl;
           if((data==BusN)&&(sel==0x1))
           {
               munmap(mapBuf,4096); 
               close(fd);            
               return(rcBusN);
           }
           else if((data==BusN)&&(sel==0x2))
           {
                 munmap(mapBuf,4096); 
               close(fd);            
               return(rcDevN);
           }

           else if ((data==BusN)&&(sel==0x3))
           {
               munmap(mapBuf,4096); 
               close(fd);            
               return(rcFunN);
           }
           
           munmap(mapBuf,4096); 

         }
        }
      }
     
     close(fd);
     if(rcBusN==0x100)
     {
      cout<<"can't find root port"<<endl;
      exit(EXIT_FAILURE);  
     }
     return(0);
}

unsigned int pcie::get_cap(unsigned int BusN,unsigned int DevN,unsigned int FunN,int sel)
  {
     unsigned int data, capID, capptr, tempptr;
     //unsigned int data1,data2;
     unsigned int data3,data4;
     //rcBusN=get_rcBusN();
     //rcDevN=get_rcDevN();
     //rcFunN=get_rcFunN();
     tempptr=cfg_r(BusN,DevN,FunN,0x34);

     do{
       data=cfg_r(BusN,DevN,FunN,tempptr);
       capID=data&0xFF;
       if(capID==0x10)
         capptr=tempptr;
       tempptr=(data&0xFF00)>>8;       
     }while(capID!=0x10);
     //data1=cfg_r(BusN,DevN,FunN,capptr+0x4);
     //data2=cfg_r(BusN,DevN,FunN,capptr+0x8);
     data3=cfg_r(BusN,DevN,FunN,capptr+0xC);
     data4=cfg_r(BusN,DevN,FunN,capptr+0x10);
     
     speed=(data4&0xF0000)>>16;  //1-Gen1; 2-Gen2; 3-Gen3; 4-Gen4
     width=(data4&0x3F00000)>>20; //1-x1; 2-x2; 4-x4; 8-x8; C-x12; 0x10-x16; 0x20-x32
     L0sEn=data4&0x1;  
     L1En=data4&0x2;
     L0sCap=data3&0x400;
     L1Cap=data3&0x800;

    if(sel==1)
      return(speed);
    else if(sel==2)
      return(width);
    else if(sel==3)
      return(L0sEn);
    else if(sel==4)
      return(L1En);
    else if(sel==5)
      return(L0sCap);
    else if(sel==6)
      return(L1Cap);  
    else
    {
      return(0);
    }
    
  }