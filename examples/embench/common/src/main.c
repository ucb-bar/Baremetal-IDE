#include "trigger.h"
#include "rocketcore.h"
#include "support.h"

int __attribute__ ((used))
main (int argc __attribute__ ((unused)),
      char *argv[] __attribute__ ((unused)))
{
  int i;
  volatile int result;
  int correct;

  //   initialise_board ();
  initialise_benchmark ();
  warm_caches (WARMUP_HEAT);

  start_trigger ();
  result = benchmark ();
  stop_trigger ();

  /* bmarks that use arrays will check a global array rather than int result */

  correct = verify_benchmark (result);

  return (!correct);

}				/* main () */