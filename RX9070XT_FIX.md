# RX 9070XT (RDNA 4) Compatibility Fix

## Problem
The RX 9070XT and other RDNA 4 GPUs cause the `gpuagent` service to crash with an assertion failure:
```
Assertion `txt_power_dev_od_voltage.contains_title_key(kTAG_GFXCLK) || txt_power_dev_od_voltage.contains_title_key(kTAG_OD_SCLK)' failed.
```

This occurs because the precompiled AMD SMI library (ROCm 6.4.0) expects certain overdrive/voltage control sysfs files that don't exist on newer RDNA 4 GPUs.

## Solution
An LD_PRELOAD wrapper library intercepts the problematic `amdsmi_get_gpu_od_volt_info()` function and returns `NOT_SUPPORTED` instead of crashing.

## Installation

The fix has been installed to:
- **Wrapper library**: `/usr/local/lib/amdsmi_wrapper.so`
- **Systemd override**: `/etc/systemd/system/gpuagent.service.d/override.conf`
- **Source code**: `amdsmi_wrapper.c` (in this directory)

## Files

### amdsmi_wrapper.c
Source code for the LD_PRELOAD wrapper that intercepts AMD SMI library calls.

### gpuagent-override.conf
Systemd service override that loads the wrapper via LD_PRELOAD.

## Rebuilding the Wrapper

If you need to rebuild the wrapper:

```bash
gcc -shared -fPIC -o amdsmi_wrapper.so amdsmi_wrapper.c -ldl
sudo cp amdsmi_wrapper.so /usr/local/lib/
sudo systemctl restart gpuagent.service
```

## Verification

Check that the service is running:
```bash
sudo systemctl status gpuagent.service
```

You should see a message like:
```
amdsmi_wrapper: Intercepting amdsmi_get_gpu_od_volt_info - returning NOT_SUPPORTED for RX 9070XT compatibility
```

## Future
This workaround can be removed once AMD releases a ROCm version (likely 6.5+) with proper support for RDNA 4 GPUs. Monitor the [ROCm releases](https://github.com/ROCm/ROCm/releases) for updates.

## Impact
- ✅ GPU metrics collection works
- ✅ Service runs stably
- ❌ Overdrive/voltage control metrics not available (these wouldn't work anyway on RX 9070XT)
