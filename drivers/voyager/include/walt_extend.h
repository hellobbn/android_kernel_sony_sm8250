/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2019-2021, The Linux Foundation. All rights reserved.
 */

#include "sched_ext.h"

extern __read_mostly unsigned int walt_scale_demand_divisor;
#define walt_scale_demand(d) ((d)/walt_scale_demand_divisor)

/*Sysctl related interface*/
extern unsigned int sysctl_walt_ext_disable;
extern unsigned int sysctl_sched_suppress_region2;
extern unsigned int sysctl_sched_asymcap_boost;
extern int walt_init_stop_handler(struct ctl_table *table, int write,
				       void __user *buffer, size_t *lenp,
				       loff_t *ppos);

extern void walt_cfs_init(void);

extern bool walt_disabled;

extern void create_util_to_cost(void);

extern inline int walt_same_cluster(int src_cpu, int dst_cpu)
{
	return cpu_rq(src_cpu)->cluster == cpu_rq(dst_cpu)->cluster;
}

static inline unsigned int walt_get_idle_exit_latency(struct rq *rq)
{
	struct cpuidle_state *idle = idle_get_state(rq);

	if (idle)
		return idle->exit_latency;

	return 0; /* CPU is not idle */
}

static inline bool walt_get_rtg_status(struct task_struct *p)
{
	struct related_thread_group *grp;
	bool ret = false;

	rcu_read_lock();

	grp = task_related_thread_group(p);
	if (grp)
		ret = grp->skip_min;

	rcu_read_unlock();

	return ret;
}

