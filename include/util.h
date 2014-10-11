# include "polarssl/ecp.h"
# include "polarssl/ecdh.h"
# include "polarssl/bignum.h"
# include "polarssl/ctr_drbg.h"

# define BAD_INPUT_DATA            -1
# define BUFFER_TOO_SHORT		   -2

int ecp_opp(const ecp_group *grp,ecp_point *R,const ecp_point *P);
