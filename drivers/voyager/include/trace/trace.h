/* SPDX-License-Identifier: GPL-2.0-only */
/*
 * Copyright (c) 2019-2021, The Linux Foundation. All rights reserved.
 */

#undef TRACE_SYSTEM
#define TRACE_SYSTEM schedwalt

#if !defined(_TRACE_WALT_H) || defined(TRACE_HEADER_MULTI_READ)
#define _TRACE_WALT_H

#include <linux/tracepoint.h>
#include <asm-generic/topology.h>

struct rq;
struct compute_energy_output;

TRACE_EVENT(walt_sched_compute_energy,

	TP_PROTO(struct task_struct *p, int eval_cpu,
		unsigned long eval_energy,
		unsigned long prev_energy,
		unsigned long best_energy,
		unsigned long best_energy_cpu,
		struct compute_energy_output *o),

	TP_ARGS(p, eval_cpu, eval_energy, prev_energy, best_energy,
		best_energy_cpu, o),

	TP_STRUCT__entry(
		__field(int,		pid)
		__array(char,		comm, TASK_COMM_LEN)
		__field(unsigned long,	util)
		__field(int,		prev_cpu)
		__field(unsigned long,	prev_energy)
		__field(int,		eval_cpu)
		__field(unsigned long,	eval_energy)
		__field(int,		best_energy_cpu)
		__field(unsigned long,	best_energy)
		__field(unsigned int,	cluster_first_cpu0)
		__field(unsigned int,	cluster_first_cpu1)
		__field(unsigned int,	cluster_first_cpu2)
		__field(unsigned long,	s0)
		__field(unsigned long,	s1)
		__field(unsigned long,	s2)
		__field(unsigned long,	m0)
		__field(unsigned long,	m1)
		__field(unsigned long,	m2)
		__field(u16,	c0)
		__field(u16,	c1)
		__field(u16,	c2)
	),

	TP_fast_assign(
		__entry->pid			= p->pid;
		memcpy(__entry->comm, p->comm, TASK_COMM_LEN);
		__entry->util			= task_util(p);
		__entry->prev_cpu		= task_cpu(p);
		__entry->prev_energy		= prev_energy;
		__entry->eval_cpu		= eval_cpu;
		__entry->eval_energy		= eval_energy;
		__entry->best_energy_cpu	= best_energy_cpu;
		__entry->best_energy		= best_energy;
		__entry->cluster_first_cpu0	= o->cluster_first_cpu[0];
		__entry->cluster_first_cpu1	= o->cluster_first_cpu[1];
		__entry->cluster_first_cpu2	= o->cluster_first_cpu[2];
		__entry->s0	= o->sum_util[0];
		__entry->s1	= o->sum_util[1];
		__entry->s2	= o->sum_util[2];
		__entry->m0	= o->max_util[0];
		__entry->m1	= o->max_util[1];
		__entry->m2	= o->max_util[2];
		__entry->c0	= o->cost[0];
		__entry->c1	= o->cost[1];
		__entry->c2	= o->cost[2];
	),

	TP_printk("pid=%d comm=%s util=%lu prev_cpu=%d prev_energy=%lu eval_cpu=%d eval_energy=%lu best_energy_cpu=%d best_energy=%lu, fcpu s m c = %u %u %u %u, %u %u %u %u, %u %u %u %u",
		__entry->pid, __entry->comm, __entry->util, __entry->prev_cpu,
		__entry->prev_energy, __entry->eval_cpu, __entry->eval_energy,
		__entry->best_energy_cpu, __entry->best_energy,
		__entry->cluster_first_cpu0, __entry->s0, __entry->m0, __entry->c0,
		__entry->cluster_first_cpu1, __entry->s1, __entry->m1, __entry->c1,
		__entry->cluster_first_cpu2, __entry->s2, __entry->m2, __entry->c2)
);

#endif /* _TRACE_WALT_H */

#undef TRACE_INCLUDE_PATH
#define TRACE_INCLUDE_PATH ../../drivers/voyager/include/trace/
#define TRACE_INCLUDE_FILE trace

#include <trace/define_trace.h>
