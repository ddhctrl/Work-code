obj-m += ctest_cgroup_misc.o ctest_cgroup_get_from_file.o ctest_cgroup_v1_attach_task.o \
ctest_cgroup_cpuset_vma_alloc.o ctest_cgroup_get_from_id.o ctest_cgroup_path_kfs_id.o \
ctest_task_cgroup_path.o ctest_cgroup_parse_float.o ctest_cgroup_sk_clone.o ctest_cgroupv1_stats_build.o \
ctest_cpuset_mems.o ctest_cpuset_mem_spread_node.o ctest_cpuset_fallback.o ctest_cgroup_cancel_fork.o

PWD = $(shell pwd)
KERNEL_DIR = /home/kanghong.chen/workspace/linux-rt-6.1.96/

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
