obj-m += ko_perf_pmu_resched.o ko_perf_event_enable.o ko_perf_event_addr_filters_sync.o ko_perf_event_stop.o \
		ko_perf_sched_cb_change.o ko_perf_pmu_sched_task.o ko_perf_event_inactive.o ko_perf_event_read_local.o \
		ko_exclusive_event_match.o ko_perf_event_refresh.o ko_perf_event_read_event.o ko_perf_get_page_size.o \
		ko_perf_event_match.o ko_perf_hrtimer.o ko_perf_rotate_ctx.o ko_perf_pending_task.o ko_perf_group_update_userpage.o \
		ko_perf_event_function_local.o ko_perf_output_read_one.o
		

PWD = $(shell pwd)
KERNEL_DIR = /home/kanghong.chen/workspace/linux-rt-6.1.96/


SPECIAL_OBJ := syscall_bpf_prog.o

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	rm -f $(filter-out $(SPECIAL_OBJ), $(wildcard *.o))
	rm -f *.ko *.mod.c *.symvers *.order *.mod
	find . -name "*.cmd" -exec rm -f {} +
