#include "ensemble_cast.h"

VALUE rb_mEnsembleCast;

void
Init_ensemble_cast(void)
{
  rb_mEnsembleCast = rb_define_module("EnsembleCast");
}
