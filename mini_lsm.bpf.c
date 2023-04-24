#include "vmlinux.h"

#include <bpf/bpf_core_read.h>
#include <bpf/bpf_helpers.h>
#include <bpf/bpf_tracing.h>

#include <stdarg.h>

#define BPF_MAX_LOOP_SIZE 100
#define BPF_MAX_PATH_LEN 32
#define BPF_MAX_COMM_LEN 16
#define SIGKILL 9

#define LOG(x, ...) 		bpf_printk(x, __VA_ARGS__)
#define LOG_INFO(x, ...) 	LOG("info: lsm_hook%s: " x, __FUNCTION__, __VA_ARGS__)
#define LOG_WARN(x, ...) 	LOG("warning: lsm_hook%s: " x, __FUNCTION__, __VA_ARGS__)

// dummy callback to have a hook with two attached programs
SEC("lsm/bprm_creds_for_exec")
int BPF_PROG(bprm_creds_for_exec_dummy , struct linux_binprm* bprm)
{
  return 0;
}

SEC("lsm/bprm_creds_for_exec")
int BPF_PROG(bprm_creds_for_exec , struct linux_binprm* bprm)
{
  int nlink = 0;
  char comm[BPF_MAX_COMM_LEN] = { 0 };
  char path[BPF_MAX_PATH_LEN] = { 0 };

  nlink = bprm->file->f_path.dentry->d_inode->__i_nlink;
  bpf_d_path(&bprm->file->f_path, path, sizeof(path));

  LOG_INFO("path=%s nlink=%d", path, nlink);

  if (!nlink) {
    bpf_get_current_comm(comm, sizeof(comm));
    LOG_WARN("fileless execution (%s:%lu)", comm, bpf_ktime_get_boot_ns());
    // bpf_send_signal(SIGKILL);
    // return 1;
  }

  return 0;
}

SEC("lsm/inode_unlink")
int BPF_PROG(inode_unlink, struct inode* inode_dir, struct dentry* dentry)
{
  struct task_struct* current = NULL;
  char comm[BPF_MAX_COMM_LEN] = { 0 };
  const struct inode *exe_inode = NULL, *target_inode = NULL;
  int i = 0;

  target_inode = dentry->d_inode;

  LOG_INFO("target_inode=0x%lx", target_inode);

  for (i = 0,
      current = bpf_get_current_task_btf(),
      exe_inode = current->mm->exe_file->f_inode;
      exe_inode && i < BPF_MAX_LOOP_SIZE;
       i++,
      current = BPF_CORE_READ(current, parent),
      exe_inode = BPF_CORE_READ(current, mm, exe_file, f_inode))
  {
    bpf_probe_read_kernel(&comm, sizeof(comm), &current->comm);
    LOG_INFO("exe_inode=0x%lx comm=%s", exe_inode, comm);

    if (target_inode == exe_inode) {
      bpf_get_current_comm(comm, sizeof(comm));
      LOG_WARN("self-deletion (%s:%lu)", comm, bpf_ktime_get_boot_ns());
      // bpf_send_signal(SIGKILL);
      // return 1;
    }
  }

  return 0;
}

char LICENSE[] SEC("license") = "Dual BSD/GPL";
