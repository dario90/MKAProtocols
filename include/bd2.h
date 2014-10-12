# include "util.h"
# include "polarssl/ecdh.h"

typedef struct {
    ecdh_context context;
    ecp_point X;
    ecp_point key;
} bd2_context;

/*
    bd2 specific operations
*/

int bd2_init(bd2_context *ctx,ecp_group_id id);
int bd2_make_key(bd2_context *ctx,void *p_rng);
int bd2_export_key(bd2_context *ctx,unsigned int *olen,unsigned char *buf,unsigned int buflen);
int bd2_make_val(bd2_context *ctx,unsigned int *olen,unsigned char *buf,unsigned int buflen,void *p_rng);
int bd2_import_val(bd2_context *ctx,unsigned char *buf,unsigned int buflen);
int bd2_calc_key(bd2_context *ctx,void *p_rng);
int bd2_free(bd2_context *ctx);

int bd2_make_public(bd2_context *ctx,unsigned int *olen,unsigned char *buf,unsigned int buflen,void *p_rng);
int bd2_read_public(bd2_context *ctx,const unsigned char *buf,unsigned int buflen);


