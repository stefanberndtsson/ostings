#ifndef OSTIS_CPU_H
#define OSTIS_CPU_H

struct cpu;

#include "common.h"
#include "hw.h"
#include "mmu.h"
#include "cpu_internal.h"
#include "cpu_external.h"
#include "cpu_exec.h"

#define MASK_X 0xffef
#define MASK_N 0xfff7
#define MASK_Z 0xfffb
#define MASK_V 0xfffd
#define MASK_C 0xfffe

#define MASK_XN (MASK_X&MASK_N)
#define MASK_XZ (MASK_X&MASK_Z)
#define MASK_XV (MASK_X&MASK_V)
#define MASK_XC (MASK_X&MASK_C)
#define MASK_NZ (MASK_N&MASK_Z)
#define MASK_NV (MASK_N&MASK_V)
#define MASK_NC (MASK_N&MASK_C)
#define MASK_ZV (MASK_Z&MASK_V)
#define MASK_ZC (MASK_Z&MASK_C)
#define MASK_VC (MASK_V&MASK_C)

#define MASK_XNZ (MASK_XN&MASK_Z)
#define MASK_XNV (MASK_XN&MASK_V)
#define MASK_XNC (MASK_XN&MASK_C)
#define MASK_XZV (MASK_XZ&MASK_V)
#define MASK_XZC (MASK_XZ&MASK_C)
#define MASK_XVC (MASK_XV&MASK_C)
#define MASK_NZV (MASK_NZ&MASK_V)
#define MASK_NZC (MASK_NZ&MASK_C)
#define MASK_NVC (MASK_NV&MASK_C)
#define MASK_ZVC (MASK_ZV&MASK_C)

#define MASK_XNZV (MASK_XNZ&MASK_V)
#define MASK_XNZC (MASK_XNZ&MASK_C)
#define MASK_XNVC (MASK_XNV&MASK_C)
#define MASK_XZVC (MASK_XZV&MASK_C)
#define MASK_NZVC (MASK_NZV&MASK_C)

#define MASK_XNZVC (MASK_XNZV&MASK_C)

#define SET_X(cpu, x) cpu_set_flags(cpu, MASK_X, (x), (0), (0), (0), (0))
#define SET_N(cpu, n) cpu_set_flags(cpu, MASK_N, (0), (n), (0), (0), (0))
#define SET_Z(cpu, z) cpu_set_flags(cpu, MASK_Z, (0), (0), (z), (0), (0))
#define SET_V(cpu, v) cpu_set_flags(cpu, MASK_V, (0), (0), (0), (v), (0))
#define SET_C(cpu, c) cpu_set_flags(cpu, MASK_C, (0), (0), (0), (0), (c))

#define SET_XN(cpu, x, n) cpu_set_flags(cpu, MASK_XN, (x), (n), (0), (0), (0))
#define SET_XZ(cpu, x, z) cpu_set_flags(cpu, MASK_XZ, (x), (0), (z), (0), (0))
#define SET_XV(cpu, x, v) cpu_set_flags(cpu, MASK_XV, (x), (0), (0), (v), (0))
#define SET_XC(cpu, x, c) cpu_set_flags(cpu, MASK_XC, (x), (0), (0), (0), (c))
#define SET_NZ(cpu, n, z) cpu_set_flags(cpu, MASK_NZ, (0), (n), (z), (0), (0))
#define SET_NV(cpu, n, v) cpu_set_flags(cpu, MASK_NV, (0), (n), (0), (v), (0))
#define SET_NC(cpu, n, c) cpu_set_flags(cpu, MASK_NC, (0), (n), (0), (0), (c))
#define SET_ZV(cpu, z, v) cpu_set_flags(cpu, MASK_ZV, (0), (0), (z), (v), (0))
#define SET_ZC(cpu, z, c) cpu_set_flags(cpu, MASK_ZC, (0), (0), (z), (0), (c))
#define SET_VC(cpu, v, c) cpu_set_flags(cpu, MASK_VC, (0), (0), (0), (v), (c))

#define SET_XNZ(cpu, x, n, z) cpu_set_flags(cpu, MASK_XNZ, (x), (n), (z), (0), (0))
#define SET_XNV(cpu, x, n, v) cpu_set_flags(cpu, MASK_XNV, (x), (n), (0), (v), (0))
#define SET_XNC(cpu, x, n, c) cpu_set_flags(cpu, MASK_XNC, (x), (n), (0), (0), (c))
#define SET_XZV(cpu, x, z, v) cpu_set_flags(cpu, MASK_XZV, (x), (0), (z), (v), (0))
#define SET_XZC(cpu, x, z, c) cpu_set_flags(cpu, MASK_XZC, (x), (0), (z), (0), (c))
#define SET_XVC(cpu, x, v, c) cpu_set_flags(cpu, MASK_XVC, (x), (0), (0), (v), (c))
#define SET_NZV(cpu, n, z, v) cpu_set_flags(cpu, MASK_NZV, (0), (n), (z), (v), (0))
#define SET_NZC(cpu, n, z, c) cpu_set_flags(cpu, MASK_NZC, (0), (n), (z), (0), (c))
#define SET_NVC(cpu, n, v, c) cpu_set_flags(cpu, MASK_NVC, (0), (n), (0), (v), (c))
#define SET_ZVC(cpu, z, v, c) cpu_set_flags(cpu, MASK_ZVC, (0), (0), (z), (v), (c))

#define SET_XNZV(cpu, x, n, z, v) cpu_set_flags(cpu, MASK_XNZV, (x), (n), (z), (v), (0))
#define SET_XNZC(cpu, x, n, z, c) cpu_set_flags(cpu, MASK_XNZC, (x), (n), (z), (0), (c))
#define SET_XNVC(cpu, x, n, v, c) cpu_set_flags(cpu, MASK_XNVC, (x), (n), (0), (v), (c))
#define SET_XZVC(cpu, x, z, v, c) cpu_set_flags(cpu, MASK_XZVC, (x), (0), (z), (v), (c))
#define SET_NZVC(cpu, n, z, v, c) cpu_set_flags(cpu, MASK_NZVC, (0), (n), (z), (v), (c))

#define SET_XNZVC(cpu, x, n, z, v, c) cpu_set_flags(cpu, MASK_XNZVC, (x), (n), (z), (v), (c))

#define CHK_MASK(size) (size==INSTR_BYTE ? 0x80 : size==INSTR_WORD ? 0x8000 : 0x80000000)
#define CHK_MASKED(size, val) ((val)&CHK_MASK(size))
#define VAL_MASK(size) (size==INSTR_BYTE ? 0xff : size==INSTR_WORD ? 0xffff : 0xffffffff)
#define VAL_MASKED(size, val) ((val)&VAL_MASK(size))

#define CHK_N(size, val) (val&CHK_MASK(size))
#define CHK_Z(val) (val==0)
#define CHK_V_PART1(size, src, dst, result) (!CHK_MASKED(size,src)&&CHK_MASKED(size,dst)&&!CHK_MASKED(size,result))
#define CHK_V_PART2(size, src, dst, result) (CHK_MASKED(size,src)&&!CHK_MASKED(size,dst)&&CHK_MASKED(size,result))
#define CHK_V(size, src, dst, result) (CHK_V_PART1(size, src, dst, result) || CHK_V_PART2(size, src, dst, result))
#define CHK_C_PART1(size, src, dst) (CHK_MASKED(size,src)&&!CHK_MASKED(size,dst))
#define CHK_C_PART2(size, dst, result) (CHK_MASKED(size,result)&&!CHK_MASKED(size,dst))
#define CHK_C_PART3(size, src, result) (CHK_MASKED(size,src)&&CHK_MASKED(size,result))
#define CHK_C(size, src, dst, result) (CHK_C_PART1(size, src, dst) || CHK_C_PART2(size, dst, result) || CHK_C_PART3(size, src, result))
  
struct cpu {
  struct cpu_internal *internal;
  struct cpu_external *external;
  struct cpu_exec *exec;
  struct hw **hws;
  struct mmu *mmu;
};

void cpu_tick(struct hw *);
struct cpu *cpu_setup(struct hw **);
void cpu_set_reset_pin(struct cpu *);
void cpu_clr_reset_pin(struct cpu *);
void cpu_set_flags(struct cpu *, WORD, int, int, int, int, int);
void cpu_instr_register(struct cpu *, WORD, WORD, struct instr *);
void cpu_debug_info(struct cpu *);

#endif /* OSTIS_CPU_H */
