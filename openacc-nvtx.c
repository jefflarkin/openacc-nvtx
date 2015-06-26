#include <stdio.h>
#include <openacc.h>
#include <pgi_acc_prof.h>
#include <wchar.h>
#include <nvToolsExt.h>
#include <nvToolsExtCudaRt.h>

#define bufsize 1024

void oaccNvtxRegister(acc_prof_info *profinfo, 
                      acc_event_info *eventinfo, 
                      acc_api_info *apiinfo)
{
  char name[bufsize];
  nvtxEventAttributes_t eventAttrib = {0}; 

  eventAttrib.version = NVTX_VERSION; 
  eventAttrib.size = NVTX_EVENT_ATTRIB_STRUCT_SIZE; 
  eventAttrib.colorType = NVTX_COLOR_ARGB; 

  snprintf(name, bufsize, "%s:%d", profinfo->funcname, profinfo->lineno);

  switch(profinfo->eventtype) 
  {
    case acc_ev_compute_construct_start :
      eventAttrib.color = 0x00ff00; 
      eventAttrib.messageType = NVTX_MESSAGE_TYPE_ASCII; 
      eventAttrib.message.ascii = name;
      nvtxRangePushEx(&eventAttrib);
      fprintf(stderr,"PUSH: %s\n", name);
      break;
    case acc_ev_data_construct_enter_start :
      eventAttrib.color = 0xff0000; 
      eventAttrib.messageType = NVTX_MESSAGE_TYPE_ASCII; 
      eventAttrib.message.ascii = name;
      nvtxRangePushEx(&eventAttrib);
      break;
    case acc_ev_compute_construct_end :
    case acc_ev_data_construct_enter_end :
      fprintf(stderr, "POP\n");
      nvtxRangePop();
      break;
  }
}

typedef void (*regroutine)( acc_event_t, acc_prof_callback_t, int );
void acc_register_library(regroutine regFunc, regroutine unregFunc)
{
  fprintf(stderr, "REGISTERING NVTX LIBRARY\n");
  regFunc(acc_ev_compute_construct_start, oaccNvtxRegister, 0);
  regFunc(acc_ev_data_construct_enter_start, oaccNvtxRegister, 0);
  regFunc(acc_ev_compute_construct_end, oaccNvtxRegister, 0);
  regFunc(acc_ev_data_construct_enter_start, oaccNvtxRegister, 0);
}

void __acc_prof_register
	( acc_prof_register_t acc_register, acc_prof_register_t acc_unregister )
{
  acc_register_library(acc_register, acc_unregister);
}