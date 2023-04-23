#include "ensemble_cast.h"

VALUE rb_mEnsembleCast;

struct rb_captured_block *vm_bh_to_iseq_block(VALUE block_handler) {
  struct rb_captured_block *captured = ((void *)((block_handler) & ~0x03));
  return captured;
}

VALUE
vm_cf_block_handler(rb_control_frame_t *cfp) {
  const VALUE *ep = cfp->ep;
  return ep[-1];
}

VALUE
rb_tracepoint_call(VALUE _self) { // Rubyで記述されたメソッド
  struct rb_captured_block *prev2_cfp_bh =
      (struct rb_captured_block *)vm_bh_to_iseq_block(
          vm_cf_block_handler((rb_current_ec()->cfp + 2)));
  if (prev2_cfp_bh == NULL) {
    return Qnil;
  }
  if (prev2_cfp_bh->code.iseq == NULL) {
    return Qnil;
  }
  if (prev2_cfp_bh->code.iseq->body->param.size != 0) {
    return Qnil;
  }
  // ブロック引数が積まれるようにする
  prev2_cfp_bh->code.iseq->body->param.flags.has_lead = 1;
  prev2_cfp_bh->code.iseq->body->param.flags.ambiguous_param0 = 1;
  prev2_cfp_bh->code.iseq->body->param.size = 1;
  prev2_cfp_bh->code.iseq->body->param.lead_num = 1;
  prev2_cfp_bh->code.iseq->body->local_table_size = 1;
  return Qnil;
}

VALUE
rb_tracepoint_b_call(VALUE _self) {
  if ((rb_current_ec()->cfp + 3)->iseq == NULL) {
    return Qnil;
  }
  VALUE block_arg = *((rb_current_ec()->cfp + 2)->ep - 3); // 積んだ引数を取得
  (rb_current_ec()->cfp + 2)->self = block_arg; // selfを置き換え
  return Qnil;
}

void Init_ensemble_cast(void) {
  rb_mEnsembleCast = rb_define_module("EnsembleCast");

  rb_define_global_function("tracepoint_call", rb_tracepoint_call, 0);
  rb_define_global_function("tracepoint_b_call", rb_tracepoint_b_call, 0);
}
