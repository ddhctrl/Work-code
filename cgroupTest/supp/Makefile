obj-m += pids_cancel_attach.o cgroupv2_e_css.o cgroupv2_finalize_control.o cgroupv2_rm_cftypes.o

PWD = $(shell pwd)
KERNEL_DIR = /home/kanghong.chen/workspace/linux-rt-6.1.96/

all:
	make -C $(KERNEL_DIR) M=$(PWD) modules
clean:
	make -C $(KERNEL_DIR) M=$(PWD) clean
