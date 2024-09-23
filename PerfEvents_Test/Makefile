obj-m += perf_ringbuffer_aux_op_begin.o perf_ringbuffer_aux_op_fd.o notifier.o raw_notifier_callchain_robust.o \
		proc_create.o signal_copy_from_user32.o ko_rb_free_aux.o ko_perf_aux_output_end.o rb_alloc_aux.o\
		ctest_signal_send_sig.o ctest_signal_sig_pend.o ctest_task_join_stop.o ctest_signal_delivered.o \
		ctest_signal_ignore.o ctest_signal_usb_asyncio.o ctest_signal_setup_done.o ctest_signal_force_sig_mceerr.o \
		ctest_signal_send_sig_perf.o ctest_signal_force_sig_other.o ctest_signal_exit_signals.o \
		ctest_signal_kernel_sigaction.o ko_perf_output_skip.o ko_sig_fault_send.o ko_signal_no_restart_syscall.o \
		
		

PWD = $(shell pwd)
KERNEL_DIR = /home/kanghong.chen/workspace/linux-rt-6.1.96/

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	rm -f *.ko *.mod.c *.symvers *.order *.mod
	find . -name "*.cmd" -exec rm -f {} +
