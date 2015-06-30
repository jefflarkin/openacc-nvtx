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
#ifdef _DEBUG
      fprintf(stderr,"PUSH COMPUTE: %s\n", name);
#endif
      break;
    case acc_ev_data_construct_enter_start :
    case acc_ev_enter_data_start :
//    case acc_ev_update_start :
    case acc_ev_update_construct_start :
#ifdef _DEBUG
      fprintf(stderr, "PUSH DATA: %s\n", name);
#endif
      eventAttrib.color = 0xff0000; 
      eventAttrib.messageType = NVTX_MESSAGE_TYPE_ASCII; 
      eventAttrib.message.ascii = name;
      nvtxRangePushEx(&eventAttrib);
      break;
    case acc_ev_wait_start :
      eventAttrib.color = 0xff00ff; 
      eventAttrib.messageType = NVTX_MESSAGE_TYPE_ASCII; 
      eventAttrib.message.ascii = name;
      nvtxRangePushEx(&eventAttrib);
#ifdef _DEBUG
      fprintf(stderr,"PUSH WAIT: %s\n", name);
#endif
      break;
    case acc_ev_implicit_wait_start :
      eventAttrib.color = 0xffff00; 
      eventAttrib.messageType = NVTX_MESSAGE_TYPE_ASCII; 
      eventAttrib.message.ascii = name;
      nvtxRangePushEx(&eventAttrib);
#ifdef _DEBUG
      fprintf(stderr,"PUSH WAIT: %s\n", name);
#endif
      break;
    case acc_ev_compute_construct_end :
    case acc_ev_data_construct_exit_end :
    case acc_ev_exit_data_end :
//    case acc_ev_update_end :
    case acc_ev_update_construct_end :
    case acc_ev_wait_end :
    case acc_ev_implicit_wait_end :
#ifdef _DEBUG
      fprintf(stderr, "POP\n");
#endif
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
  regFunc(acc_ev_enter_data_start, oaccNvtxRegister, 0);
//  regFunc(acc_ev_update_start, oaccNvtxRegister, 0);
  regFunc(acc_ev_update_construct_start, oaccNvtxRegister, 0);
  regFunc(acc_ev_wait_start, oaccNvtxRegister, 0);
  regFunc(acc_ev_implicit_wait_start, oaccNvtxRegister, 0);
  regFunc(acc_ev_compute_construct_end, oaccNvtxRegister, 0);
  regFunc(acc_ev_data_construct_exit_end, oaccNvtxRegister, 0);
  regFunc(acc_ev_exit_data_end, oaccNvtxRegister, 0);
//  regFunc(acc_ev_update_end, oaccNvtxRegister, 0);
  regFunc(acc_ev_update_construct_end, oaccNvtxRegister, 0);
  regFunc(acc_ev_wait_end, oaccNvtxRegister, 0);
  regFunc(acc_ev_implicit_wait_end, oaccNvtxRegister, 0);
}

void __acc_prof_register
	( acc_prof_register_t acc_register, acc_prof_register_t acc_unregister )
{
  acc_register_library(acc_register, acc_unregister);
}
