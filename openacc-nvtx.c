/*
 *  Copyright 2015 NVIDIA Corporation
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */
#include <stdio.h>
#include <openacc.h>
// Prior to PGI 15.7 the profiling interface prototype was based on TR-14-2.
#if defined(__PGIC__) && ((__PGIC__ >= 15) && (__PGIC_MINOR__ < 7))
#include <pgi_acc_prof.h>
#else
#include <acc_prof.h>
#endif
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

#if defined(__PGIC__) && ((__PGIC__ >= 15) && (__PGIC_MINOR__ < 7))
  snprintf(name, bufsize, "%s:%d", profinfo->funcname, profinfo->lineno);
#else
  snprintf(name, bufsize, "%s:%d", profinfo->func_name, profinfo->line_no);
#endif

#if defined(__PGIC__) && ((__PGIC__ >= 15) && (__PGIC_MINOR__ < 7))
  switch(profinfo->eventtype) 
#else
  switch(profinfo->event_type) 
#endif
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
#if defined(__PGIC__) && ((__PGIC__ >= 15) && (__PGIC_MINOR__ < 7))
    case acc_ev_data_construct_enter_start :
#endif
    case acc_ev_enter_data_start :
#if defined(__PGIC__) && ((__PGIC__ >= 15) && (__PGIC_MINOR__ < 7))
    case acc_ev_update_construct_start :
#else
    case acc_ev_update_start :
#endif
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
#if defined(__PGIC__) && ((__PGIC__ >= 15) && (__PGIC_MINOR__ < 7))
    case acc_ev_data_construct_exit_end :
#endif
    case acc_ev_exit_data_end :
#if defined(__PGIC__) && ((__PGIC__ >= 15) && (__PGIC_MINOR__ < 7))
    case acc_ev_update_construct_end :
#else
    case acc_ev_update_end :
#endif
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
#if defined(__PGIC__) && ((__PGIC__ >= 15) && (__PGIC_MINOR__ < 7))
void acc_register_library(regroutine regFunc, regroutine unregFunc)
#else
void acc_register_library(acc_prof_reg regFunc, acc_prof_reg unregFunc, acc_prof_lookup lookup)
#endif
{
  fprintf(stderr, "REGISTERING NVTX LIBRARY\n");
  regFunc(acc_ev_compute_construct_start, oaccNvtxRegister, 0);
  regFunc(acc_ev_enter_data_start, oaccNvtxRegister, 0);
#if defined(__PGIC__) && ((__PGIC__ >= 15) && (__PGIC_MINOR__ < 7))
  regFunc(acc_ev_data_construct_enter_start, oaccNvtxRegister, 0);
  regFunc(acc_ev_data_construct_exit_end, oaccNvtxRegister, 0);
  regFunc(acc_ev_update_construct_start, oaccNvtxRegister, 0);
  regFunc(acc_ev_update_construct_end, oaccNvtxRegister, 0);
#else
  regFunc(acc_ev_update_start, oaccNvtxRegister, 0);
  regFunc(acc_ev_update_end, oaccNvtxRegister, 0);
#endif
  regFunc(acc_ev_wait_start, oaccNvtxRegister, 0);
  regFunc(acc_ev_implicit_wait_start, oaccNvtxRegister, 0);
  regFunc(acc_ev_compute_construct_end, oaccNvtxRegister, 0);
  regFunc(acc_ev_exit_data_end, oaccNvtxRegister, 0);
  regFunc(acc_ev_wait_end, oaccNvtxRegister, 0);
  regFunc(acc_ev_implicit_wait_end, oaccNvtxRegister, 0);
}

#if 0
void __acc_prof_register
	( acc_prof_register_t acc_register, acc_prof_register_t acc_unregister )
{
  acc_register_library(acc_register, acc_unregister);
}
#endif
