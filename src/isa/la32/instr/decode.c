#include "../local-include/reg.h"
// #include <cpu/ifetch.h>
#include <memory/vaddr.h>

static inline uint32_t instr_fetch(vaddr_t *pc, int len) {
  uint32_t instr = vaddr_ifetch(*pc, len);
  (*pc) += len;
  return instr;
}

#include <cpu/decode.h>
#include "../local-include/rtl.h"

#define INSTR_LIST(f) f(lui) f(lw) f(sw) f(inv) f(nemu_trap)

def_all_EXEC_ID();

def_all_THelper();

static uint32_t get_instr(Decode *s) {
  return s->isa.instr.val;
}

// decode operand helper
#define def_DopHelper(name) \
  void concat(decode_op_, name) (Decode *s, Operand *op, word_t val, bool flag)

static def_DopHelper(i) {
  op->imm = val;
}

static def_DopHelper(r) {
  bool is_write = flag;
  static word_t zero_null = 0;
  op->preg = (is_write && val == 0) ? &zero_null : &gpr(val);
}

static def_DHelper(I) {
  decode_op_r(s, id_src1, s->isa.instr.i.rs1, false);
  decode_op_i(s, id_src2, s->isa.instr.i.simm11_0, false);
  decode_op_r(s, id_dest, s->isa.instr.i.rd, true);
}

static def_DHelper(U) {
  decode_op_i(s, id_src1, s->isa.instr.u.imm31_12 << 12, true);
  decode_op_r(s, id_dest, s->isa.instr.u.rd, true);
}

static def_DHelper(S) {
  decode_op_r(s, id_src1, s->isa.instr.s.rs1, false);
  sword_t simm = (s->isa.instr.s.simm11_5 << 5) | s->isa.instr.s.imm4_0;
  decode_op_i(s, id_src2, simm, false);
  decode_op_r(s, id_dest, s->isa.instr.s.rs2, false);
}

def_THelper(load) {
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", lw);
  return EXEC_ID_inv;
}

def_THelper(store) {
  def_INSTR_TAB("??????? ????? ????? 010 ????? ????? ??", sw);
  return EXEC_ID_inv;
}

def_THelper(main) {
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 00000 11", I     , load);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01000 11", S     , store);
  def_INSTR_IDTAB("??????? ????? ????? ??? ????? 01101 11", U     , lui);
  def_INSTR_TAB  ("??????? ????? ????? ??? ????? 11010 11",         nemu_trap);
  return table_inv(s);
};

int isa_fetch_decode(Decode *s) {
  s->isa.instr.val = instr_fetch(&s->snpc, 4);
  int idx = table_main(s);
  return idx;
}
