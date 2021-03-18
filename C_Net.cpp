//////////////////////////////////////////////////////////////////////////////////
// [ Net_Class_Source ]
//////////////////////////////////////////////////////////////////////////////////

#include "C_Net.hpp"

//////////////////////////////////////////////////////////////////////////////////
// [ init ]
//////////////////////////////////////////////////////////////////////////////////
int C_Net::init(){

   if(CNInterface.create() != C_NET_INTERFACE_READY){
      cout << "ERROR: CNInterface.create" << endl;
      return(C_NET_ERROR);
   }
   
   CNRaw.signal_data().connect(sigc::mem_fun(*this, &C_Net::on_raw_data));

   return(C_NET_READY);
}
//////////////////////////////////////////////////////////////////////////////////
// [ start ]
//////////////////////////////////////////////////////////////////////////////////
int C_Net::start(const S_Net_Interface* pSInterface){
   
   if(!pSInterface || bsneak) return(C_NET_ERROR);
   
   CDA_Result.clear();

   if(CNRaw.open(pSInterface) != C_NET_RAW_READY)                      return(C_NET_ERROR);
   if(CNRaw.start(C_NET_ID, pBuffer, C_NET_BUFFER) != C_NET_RAW_READY) return(C_NET_ERROR);
   
   bsneak = true;
   
   while(bsneak){}

   return(C_NET_READY);
}
//////////////////////////////////////////////////////////////////////////////////
// [ stop ]
//////////////////////////////////////////////////////////////////////////////////
int C_Net::stop(){
   
   if(!bsneak) return(C_NET_ERROR);
   
   bsneak = false;
   
   CNRaw.stop();
   CNRaw.close();

   return(C_NET_READY);
}
//////////////////////////////////////////////////////////////////////////////////
// [ on_raw_data ]
//////////////////////////////////////////////////////////////////////////////////
void C_Net::on_raw_data(int id, int cData){
   
   if(cData < (int)(cETHERNET_HEADER)) return;

   ETHERNET_HEADER* pRCV_ethhdr = (ETHERNET_HEADER*)pBuffer;
   
   switch(pRCV_ethhdr->Type){
      
      case ETH_TYP_ARP:{
        
         if(cData < (int)(cETHERNET_HEADER + cARP_HEADER)) return;
         
         ARP_HEADER* pRCV_arp = (ARP_HEADER*)(pBuffer + cETHERNET_HEADER);

         *((DWORD*)SResult.MAC)    = *((DWORD*)pRCV_arp->ARP_MAC_S);
         *((WORD*)SResult.MAC + 2) = *((WORD*)pRCV_arp->ARP_MAC_S + 2);

         *((DWORD*)SResult.IP) = *((DWORD*)pRCV_arp->ARP_IP_S);
         
         CDA_Result.push_back(SResult);
         
         sig_packet();
         
         break;
      }
      case ETH_TYP_IPv4:{
         
         if(cData < (int)(cETHERNET_HEADER + cIP_HEADER)) return;
         
         IP_HEADER* pRCV_ip = (IP_HEADER*)(pBuffer + cETHERNET_HEADER);
         
         *((DWORD*)SResult.MAC)    = *((DWORD*)pRCV_ethhdr->MAC_S);
         *((WORD*)SResult.MAC + 2) = *((WORD*)pRCV_ethhdr->MAC_S + 2);

         *((DWORD*)SResult.IP) = *((DWORD*)pRCV_ip->IP_IP_S);
         
         CDA_Result.push_back(SResult);
         
         sig_packet();
         
         break;
      }
   }
}
