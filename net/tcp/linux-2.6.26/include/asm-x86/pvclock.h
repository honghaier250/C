#ifndef _ASM_X86_PVCLOCK_H_
#define _ASM_X86_PVCLOCK_H_

#include <linux/clocksource.h>
#include <asm/pvclock-abi.h>

/* some helper functions for xen and kvm pv clock sources */
cycle_t pvclock_clocksource_read(struct pvclock_vcpu_time_info *src);
void pvclock_read_wallclock(struct pvclock_wall_clock *wall,
                struct pvclock_vcpu_time_info *vcpu,
                struct timespec *ts);

#endif /* _ASM_X86_PVCLOCK_H_ */
