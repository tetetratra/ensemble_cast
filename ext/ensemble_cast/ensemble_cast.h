#ifndef ENSEMBLE_CAST_H
#define ENSEMBLE_CAST_H 1

#include "ruby.h"

#endif /* ENSEMBLE_CAST_H */

typedef struct rb_iseq_struct {
  VALUE flags;   /* 1 */
  VALUE wrapper; /* 2 */

  struct rb_iseq_constant_body *body; /* 3 */

  union {                                   /* 4, 5 words */
    struct iseq_compile_data *compile_data; /* used at compile time */

    struct {
      VALUE obj;
      int index;
    } loader;

    struct {
      struct rb_hook_list_struct *local_hooks;
      rb_event_flag_t global_trace_events;
    } exec;
  } aux;
} rb_iseq_t;

typedef struct rb_control_frame_struct {
  const VALUE *pc;       /* cfp[0] */
  VALUE *sp;             /* cfp[1] */
  const rb_iseq_t *iseq; /* cfp[2] */
  VALUE self;            /* cfp[3] / block[0] */
  const VALUE *ep;       /* cfp[4] / block[1] */
  const void *block_code;
  /* cfp[5] / block[2] */ /* iseq or ifunc or forwarded block handler */
  VALUE *__bp__;
  /* cfp[6] */ /* outside vm_push_frame, use vm_base_ptr instead. */

  // Return address for YJIT code
  void *jit_return;
} rb_control_frame_t;

typedef struct rb_ensure_entry {
  VALUE marker;
  VALUE (*e_proc)(VALUE);
  VALUE data2;
} rb_ensure_entry_t;

typedef struct rb_ensure_list {
  struct rb_ensure_list *next;
  struct rb_ensure_entry entry;
} rb_ensure_list_t;

enum method_missing_reason {
  MISSING_NOENTRY = 0x00,
  MISSING_PRIVATE = 0x01,
  MISSING_PROTECTED = 0x02,
  MISSING_FCALL = 0x04,
  MISSING_VCALL = 0x08,
  MISSING_SUPER = 0x10,
  MISSING_MISSING = 0x20,
  MISSING_NONE = 0x40
};

#define BITFIELD(type, name, size) type name : size

typedef struct rb_execution_context_struct {
  /* execution information */
  VALUE *vm_stack;      /* must free, must mark */
  size_t vm_stack_size; /* size in word (byte size / sizeof(VALUE)) */
  rb_control_frame_t *cfp;

  struct rb_vm_tag *tag;

  /* interrupt flags */
  // rb_atomic_t interrupt_flag;
  // rb_atomic_t interrupt_mask; /* size should match flag */
#if defined(USE_VM_CLOCK) && USE_VM_CLOCK
  uint32_t checked_clock;
#endif

  // rb_fiber_t *fiber_ptr;
  struct rb_thread_struct *thread_ptr;

  /* storage (ec (fiber) local) */
  struct rb_id_table *local_storage;
  VALUE local_storage_recursive_hash;
  VALUE local_storage_recursive_hash_for_trace;

  /* eval env */
  const VALUE *root_lep;
  VALUE root_svar;

  /* ensure & callcc */
  rb_ensure_list_t *ensure_list;

  /* trace information */
  struct rb_trace_arg_struct *trace_arg;

  /* temporary places */
  VALUE errinfo;
  VALUE passed_block_handler; /* for rb_iterate */

  uint8_t raised_flag; /* only 3 bits needed */

  /* n.b. only 7 bits needed, really: */
  BITFIELD(enum method_missing_reason, method_missing_reason, 8);

  VALUE private_const_reference;

  /* for GC */
  // struct {
  //   VALUE *stack_start;
  //   VALUE *stack_end;
  //   size_t stack_maxsize;
  //   RUBY_ALIGNAS(SIZEOF_VALUE) jmp_buf regs;
  // } machine;
} rb_execution_context_t;

struct rb_captured_block {
  VALUE self;
  const VALUE *ep;
  union {
    const rb_iseq_t *iseq;
    const struct vm_ifunc *ifunc;
    VALUE val;
  } code;
};

typedef struct rb_code_position_struct {
  int lineno;
  int column;
} rb_code_position_t;

typedef struct rb_code_location_struct {
  rb_code_position_t beg_pos;
  rb_code_position_t end_pos;
} rb_code_location_t;

typedef signed long rb_snum_t;

typedef struct rb_iseq_location_struct {
  VALUE pathobj;      /* String (path) or Array [path, realpath]. Frozen. */
  VALUE base_label;   /* String */
  VALUE label;        /* String */
  VALUE first_lineno; /* TODO: may be unsigned short */
  int node_id;
  rb_code_location_t code_location;
} rb_iseq_location_t;

struct rb_iseq_constant_body {
  enum iseq_type {
    ISEQ_TYPE_TOP,
    ISEQ_TYPE_METHOD,
    ISEQ_TYPE_BLOCK,
    ISEQ_TYPE_CLASS,
    ISEQ_TYPE_RESCUE,
    ISEQ_TYPE_ENSURE,
    ISEQ_TYPE_EVAL,
    ISEQ_TYPE_MAIN,
    ISEQ_TYPE_PLAIN
  } type; /* instruction sequence type */

  unsigned int iseq_size;
  VALUE *iseq_encoded; /* encoded iseq (insn addr and operands) */

  /**
   * parameter information
   *
   *  def m(a1, a2, ..., aM,                    # mandatory
   *        b1=(...), b2=(...), ..., bN=(...),  # optional
   *        *c,                                 # rest
   *        d1, d2, ..., dO,                    # post
   *        e1:(...), e2:(...), ..., eK:(...),  # keyword
   *        **f,                                # keyword_rest
   *        &g)                                 # block
   * =>
   *
   *  lead_num     = M
   *  opt_num      = N
   *  rest_start   = M+N
   *  post_start   = M+N+(*1)
   *  post_num     = O
   *  keyword_num  = K
   *  block_start  = M+N+(*1)+O+K
   *  keyword_bits = M+N+(*1)+O+K+(&1)
   *  size         = M+N+O+(*1)+K+(&1)+(**1) // parameter size.
   */

  struct {
    struct {
      unsigned int has_lead : 1;
      unsigned int has_opt : 1;
      unsigned int has_rest : 1;
      unsigned int has_post : 1;
      unsigned int has_kw : 1;
      unsigned int has_kwrest : 1;
      unsigned int has_block : 1;

      unsigned int ambiguous_param0 : 1; /* {|a|} */
      unsigned int accepts_no_kwarg : 1;
      unsigned int ruby2_keywords : 1;
    } flags;

    unsigned int size;

    int lead_num;
    int opt_num;
    int rest_start;
    int post_start;
    int post_num;
    int block_start;

    const VALUE *opt_table; /* (opt_num + 1) entries. */
    /* opt_num and opt_table:
     *
     * def foo o1=e1, o2=e2, ..., oN=eN
     * #=>
     *   # prologue code
     *   A1: e1
     *   A2: e2
     *   ...
     *   AN: eN
     *   AL: body
     * opt_num = N
     * opt_table = [A1, A2, ..., AN, AL]
     */

    const struct rb_iseq_param_keyword {
      int num;
      int required_num;
      int bits_start;
      int rest_start;
      const ID *table;
      VALUE *default_values;
    } * keyword;
  } param;

  rb_iseq_location_t location;

  /* insn info, must be freed */
  struct iseq_insn_info {
    const struct iseq_insn_info_entry *body;
    unsigned int *positions;
    unsigned int size;
  } insns_info;

  const ID *local_table; /* must free */

  /* catch table */
  struct iseq_catch_table *catch_table;

  /* for child iseq */
  const struct rb_iseq_struct *parent_iseq;
  struct rb_iseq_struct *local_iseq; /* local_iseq->flip_cnt can be modified */

  union iseq_inline_storage_entry *is_entries;
  struct rb_call_data *call_data; // struct rb_call_data calls[ci_size];

  struct {
    rb_snum_t flip_count;
    VALUE script_lines;
    VALUE coverage;
    VALUE pc2branchindex;
    VALUE *original_iseq;
  } variable;

  unsigned int local_table_size;
  unsigned int is_size;
  unsigned int ci_size;
  unsigned int stack_max; /* for stack overflow check */

  char catch_except_p; /* If a frame of this ISeq may catch exception, set TRUE
                        */
  // If true, this ISeq is leaf *and* backtraces are not used, for example,
  // by rb_profile_frames. We verify only leafness on VM_CHECK_MODE though.
  // Note that GC allocations might use backtraces due to
  // ObjectSpace#trace_object_allocations.
  // For more details, see: https://bugs.ruby-lang.org/issues/16956
  bool builtin_inline_p;
  struct rb_id_table *outer_variables;

  const rb_iseq_t *mandatory_only_iseq;

#if USE_MJIT
  /* The following fields are MJIT related info.  */
  VALUE(*jit_func)
  (struct rb_execution_context_struct *,
   struct rb_control_frame_struct
       *);                   /* function pointer for loaded native code */
  long unsigned total_calls; /* number of total calls with `mjit_exec()` */
  struct rb_mjit_unit *jit_unit;
#endif

  // rb_yjit_block_array_array_t
  //     yjit_blocks; // empty, or has a size equal to iseq_size
};

extern rb_execution_context_t *rb_current_ec(void);
