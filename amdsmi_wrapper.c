/*
 * LD_PRELOAD wrapper to fix RX 9070XT overdrive assertion failure
 * This intercepts the amdsmi_get_gpu_od_volt_info call and returns NOT_SUPPORTED
 * for GPUs that don't have the expected overdrive sysfs files.
 */

#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdint.h>

// AMD SMI status codes
#define AMDSMI_STATUS_SUCCESS 0
#define AMDSMI_STATUS_NOT_SUPPORTED 10

// Forward declaration of the real function type
typedef int (*amdsmi_get_gpu_od_volt_info_func)(void* processor_handle, void* odv);

// Our wrapper function
int amdsmi_get_gpu_od_volt_info(void* processor_handle, void* odv) {
    static amdsmi_get_gpu_od_volt_info_func real_func = NULL;

    // Get the real function on first call
    if (!real_func) {
        real_func = (amdsmi_get_gpu_od_volt_info_func)dlsym(RTLD_NEXT, "amdsmi_get_gpu_od_volt_info");
        if (!real_func) {
            fprintf(stderr, "amdsmi_wrapper: Failed to load real amdsmi_get_gpu_od_volt_info\n");
            return AMDSMI_STATUS_NOT_SUPPORTED;
        }
    }

    // Try calling the real function, but catch any crashes/failures gracefully
    // For now, just return NOT_SUPPORTED to avoid the assertion
    fprintf(stderr, "amdsmi_wrapper: Intercepting amdsmi_get_gpu_od_volt_info - returning NOT_SUPPORTED for RX 9070XT compatibility\n");
    return AMDSMI_STATUS_NOT_SUPPORTED;
}
