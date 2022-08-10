// SPDX-License-Identifier: GPL-2.0-only
/*
 * Copyright (c) 2020-2021, The Linux Foundation. All rights reserved.
 */

#include <linux/kmemleak.h>
#include "../../include/walt_extend.h"

#define CREATE_TRACE_POINTS
#include <trace/hooks/sched.h>

static void init_cpu_array(void)
{
	int i;

	cpu_array = kcalloc(num_sched_clusters, sizeof(cpumask_t *),
			GFP_ATOMIC | __GFP_NOFAIL);
	if (!cpu_array)
		SCHED_BUG_ON(1);

	for (i = 0; i < num_sched_clusters; i++) {
		cpu_array[i] = kcalloc(num_sched_clusters, sizeof(cpumask_t),
			GFP_ATOMIC | __GFP_NOFAIL);
		if (!cpu_array[i])
			SCHED_BUG_ON(1);
	}
}

static void build_cpu_array(void)
{
	int i;

	if (!cpu_array)
		SCHED_BUG_ON(1);
	/* Construct cpu_array row by row */
	for (i = 0; i < num_sched_clusters; i++) {
		int j, k = 1;

		/* Fill out first column with appropriate cpu arrays */
		cpumask_copy(&cpu_array[i][0], &sched_cluster[i]->cpus);
		/*
		 * k starts from column 1 because 0 is filled
		 * Fill clusters for the rest of the row,
		 * above i in ascending order
		 */
		for (j = i + 1; j < num_sched_clusters; j++) {
			cpumask_copy(&cpu_array[i][k],
					&sched_cluster[j]->cpus);
			k++;
		}

		/*
		 * k starts from where we left off above.
		 * Fill clusters below i in descending order.
		 */
		for (j = i - 1; j >= 0; j--) {
			cpumask_copy(&cpu_array[i][k],
					&sched_cluster[j]->cpus);
			k++;
		}
	}
}

int cpu_l2_sibling[NR_CPUS] = {[0 ... NR_CPUS-1] = -1};
static void find_cache_siblings(void)
{
	int cpu, cpu2;
	struct device_node *cpu_dev, *cpu_dev2, *cpu_l2_cache_node, *cpu_l2_cache_node2;

	for_each_possible_cpu(cpu) {
		cpu_dev = of_get_cpu_node(cpu, NULL);
		if (!cpu_dev)
			continue;

		cpu_l2_cache_node = of_parse_phandle(cpu_dev, "next-level-cache", 0);
		if (!cpu_l2_cache_node)
			continue;

		for_each_possible_cpu(cpu2) {
			if (cpu == cpu2)
				continue;

			cpu_dev2 = of_get_cpu_node(cpu2, NULL);
			if (!cpu_dev2)
				continue;

			cpu_l2_cache_node2 = of_parse_phandle(cpu_dev2, "next-level-cache", 0);
			if (!cpu_l2_cache_node2)
				continue;

			if (cpu_l2_cache_node == cpu_l2_cache_node2) {
				cpu_l2_sibling[cpu] = cpu2;
				break;
			}
		}
	}
}

static void walt_update_cluster_topology(void)
{
	init_cpu_array();
	build_cpu_array();
	find_cache_siblings();

	create_util_to_cost();
}

bool walt_disabled;

static void walt_init(struct work_struct *work)
{
	struct ctl_table_header *hdr;

	walt_cfs_init();
	hdr = register_sysctl_table(voyager_kernel_addon_base_table);
	kmemleak_not_leak(hdr);

        walt_update_cluster_topology();

	walt_disabled = false;
}

static DECLARE_WORK(walt_init_work, walt_init);

static int walt_module_init(void)
{

	schedule_work(&walt_init_work);

	return 0;
}

module_init(walt_module_init);
