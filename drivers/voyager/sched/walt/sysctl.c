// SPDX-License-Identifier: GPL-2.0-only

#include "../../include/walt_extend.h"

#define SYSCTL_ZERO	((void *)&sysctl_vals[0])
#define SYSCTL_ONE	((void *)&sysctl_vals[1])
#define SYSCTL_INT_MAX	((void *)&sysctl_vals[2])

unsigned int sysctl_sched_suppress_region2;
unsigned int sysctl_sched_asymcap_boost;
unsigned int sysctl_walt_ext_disable = 0; //disable by default

static struct ctl_table walt_table[] = {
	{
		.procname       = "sched_suppress_region2",
		.data           = &sysctl_sched_suppress_region2,
		.maxlen         = sizeof(unsigned int),
		.mode           = 0644,
		.proc_handler   = proc_dointvec_minmax,
		.extra1		= SYSCTL_ZERO,
		.extra2		= SYSCTL_ONE,
	},
	{
		.procname	= "sched_asymcap_boost",
		.data		= &sysctl_sched_asymcap_boost,
		.maxlen		= sizeof(unsigned int),
		.mode		= 0644,
		.proc_handler	= proc_douintvec_minmax,
		.extra1		= SYSCTL_ZERO,
		.extra2		= SYSCTL_ONE,
	},
	{}
};

struct ctl_table voyager_kernel_addon_base_table[] = {
	{
		.procname	= "voager_sched_addon",
		.mode		= 0555,
		.child		= walt_table,
	},
	{}
};


