/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2019-2021, The Linux Foundation. All rights reserved.
 */

#include "../../../kernel/sched/sched.h"
#include "../../../kernel/sched/walt.h"

static const int sysctl_vals[] = { 0, 1, INT_MAX };

struct compute_energy_output {
	unsigned long	sum_util[MAX_CLUSTERS];
	unsigned long	max_util[MAX_CLUSTERS];
	u16		cost[MAX_CLUSTERS];
	unsigned int	cluster_first_cpu[MAX_CLUSTERS];
};

extern cpumask_t **cpu_array;
extern int cpu_l2_sibling[NR_CPUS];
extern unsigned int create_util_to_cost_stat;
extern unsigned int android_rvh_select_task_rq_fair;
extern struct ctl_table voyager_kernel_addon_base_table[];
extern inline unsigned long uclamp_task_util(struct task_struct *p);
extern inline int per_task_boost(struct task_struct *p);
extern inline int wake_to_idle(struct task_struct *p);
extern inline bool task_fits_max(struct task_struct *p, int cpu);
extern inline bool bias_to_this_cpu(struct task_struct *p, int cpu, int start_cpu);
extern inline bool task_demand_fits(struct task_struct *p, int cpu);
