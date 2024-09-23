#include <linux/bpf.h>
#include <bpf/bpf_helpers.h>

SEC("perf_event")
int handle_ksymbol_bpf_event(struct bpf_perf_event_data *ctx) {
    bpf_printk("BPF ksymbol event triggered!\n");
    return 0;
}

char LICENSE[] SEC("license") = "GPL";
