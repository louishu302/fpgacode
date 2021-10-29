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

using namespace std;

acpi::acpi()
{
  mmioaddr=acpi_table();
  cout<<"mmioaddr=0x"<<hex<<mmioaddr<<endl;
}

acpi::~acpi()
{

}

unsigned int acpi::acpi_table()
{
    //read efi system
    char buffer[80];
    unsigned int addr=0x0;
    int fd,result=0x0;
    unsigned int *lgcyBuf[0x1f];
    unsigned int i,data1,data2,j;

    FILE *f=fopen("/sys/firmware/efi/systab","r");
    if(f)
    {
      while (fgets(buffer, 80, f)) 
        {
			if (sscanf(buffer, "ACPI20=0x%x", &addr) == 1)
			     break;
		    }
		fclose(f);
    } 

  //read_legacy_systab
  if(!addr)
     {
       fd=open("/dev/mem",O_RDONLY);
        if(fd<0)
        printf("open device failed. fd=%d error message is: %s\n",fd, strerror(errno));           
       //address 4k aligned-  read-only memory space 0x0E0000~0x0FFFFF      
       
        for(i=0x0;i<0x20;i++)
        {          
          lgcyBuf[i]= (unsigned int*)mmap(NULL,4096,PROT_READ, MAP_SHARED, fd, 0x0E0000+(0x1000*i));         
          for(j=0x0;j<0x1000/8;j+=0x2)
          {
           data1=*(lgcyBuf[i]+j);
           data2=*(lgcyBuf[i]+j+1);            
           
           if((data1==0x20445352)&&(data2==0x20525450))   // data1== DSR  data2== RTP
           {                  
             result=1;    
             addr=(0xE0000+(i*0x1000)+(j*0x4));            
            // cout<<"debug: ACPI root table addr=0x"<<hex<<addr<<endl;
             break;             
           }       
            
           }

          munmap(lgcyBuf[i],4096);
          if(result)
           break;     
          
        }
         
         close(fd);
          
         if(!addr)
         {
          printf("CAN NOT find RSD PTR table!!!\n");
          exit(EXIT_FAILURE); 
         }
         
     } 
     
    
    unsigned int xsdt;
    unsigned int *acpiMapBuf,offset;
   //step 1: get xsdt table base address
     offset= (addr&0xFFF)/4;
     fd=open("/dev/mem",O_RDONLY);
     if(fd<0)
     printf("open device failed. fd=%d error message is: %s\n",fd, strerror(errno));
     //cout<<"debug:offset=0x"<<hex<<offset<<endl;

    //address 4k aligned-  read-only memory space 0x0E0000~0x0FFFFF
    acpiMapBuf =(unsigned int*) mmap(NULL,4096,PROT_READ, MAP_SHARED, fd, (addr&0xFFFFF000));
        
    xsdt=*(acpiMapBuf+offset+6);           //get xsdt table base address
    
    munmap(acpiMapBuf,4096);
      
    //printf("xsdt=0x%08X\n",xsdt);
  //step 2: get MCFG table base address 
  
    acpiMapBuf =(unsigned int*) mmap(NULL,4096,PROT_READ, MAP_SHARED, fd, (xsdt&0xFFFFF000));
   
    offset=(xsdt&0xFFF)/4;
    unsigned int length=*(acpiMapBuf+offset+1);
    unsigned int n=(length-0x24)/8;
    unsigned int entry[n-1];
    //cout<<"debug: n=%x"<<hex<<n<<endl;
   
    for(i=0;i<n;i++)
    { 
     entry[i]=*(acpiMapBuf+offset+9+(2*i));
    // printf("entry[0x%x]=0x%08x\n",i,entry[i]);
    } 
    munmap(acpiMapBuf,4096);
    close(fd);
     
    unsigned int *out_entry[n-1],acpidata;
         
    for(i=0;i<n;i++)
    {
       fd=open("/dev/mem",O_RDONLY);
       if(fd<0)
       printf("open device failed. fd=%d error message is: %s\n",fd, strerror(errno));
       else
       {
       // printf("open device successed!fd=%d\n",fd);
       } 
       out_entry[i]=(unsigned int*)mmap(NULL,4096,PROT_READ, MAP_SHARED, fd, (entry[i]&0xFFFFF000));
       offset=(entry[i]&0xFFF)/4;
       acpidata=*(out_entry[i]+offset);
      // printf("debug: acpidata=0x%08X\n",acpidata);
      if (acpidata==0x4746434D)  //match string 'MCFG'
      {
       mmioaddr=*(out_entry[i]+offset+11);
	     result=0x1;
       munmap(out_entry[i],4096);
       close(fd);
	     i=n;
      }
      else
      {  result=0x0;  
         munmap(out_entry[i],4096);
         close(fd);   
      }
     
    }

  
   if(result)
   {   
   return mmioaddr;
   }
   else
   {
    cout<<"Sorry! Can NOT find CFGMMIOBaseAddress!!!"<<endl;
    exit(EXIT_FAILURE);  //terminate due to error
   }
}

unsigned int acpi::scan_pcie_tree(unsigned int vendorID,int sel)
{
   unsigned int *pciMapBuf;    
    int fd;    
    unsigned short i,j,k,result;    
    BusN=0;   //EP device is impossible onto BUS0
    DevN=0;
    FunN=0;
    result=0;

    fd=open("/dev/mem",O_RDONLY);
    if(fd<0)
    {
     printf("open device failed. fd=%d error message is: %s\n",fd, strerror(errno)); 
    }   

    for(i=0;i<256;i++)   //i=bus#
    {
      for(j=0;j<0x20;j++)  //j=device#
      {
        for(k=0;k<0x8;k++)  //k=function#
	     {
	       unsigned int pciaddr=(mmioaddr|(i<<20)|(j<<15)|(k<<12));	           
           pciMapBuf =(unsigned int*) mmap(NULL,4096,PROT_READ, MAP_SHARED, fd, (pciaddr&0xFFFFF000));
            if(((*pciMapBuf)&0xFFFF)==vendorID)
     	    {				
             BusN=i;
             DevN=j;          
             FunN=k; 
             result=1;
             munmap(pciMapBuf,4096);
             break;                   
       	    }
		    munmap(pciMapBuf,4096);        
	     
	     }
      }  
     }

     close(fd);
     if(!result)
      {printf("VID=0x%x can NOT be found!!!\n",vendorID);
       assert(0);  //terminate due to error;
      }
     else
     {
      //printf("VID=0x%x found at bus=0x%x dev=0x%x fun=0x%x\n",VendorID,part.bus,part.dev,part.fun);
     }


   if(sel==0x1)
     return(BusN);
    else if(sel==0x2)
     return(DevN);
    else if(sel==0x3)
     return(FunN);
    else
    {
        cout<<"wrong input parameters which should be 0x1 or 0x2 or 0x3"<<endl;
        assert(0);
    }



}


