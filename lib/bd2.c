# include "bd2.h"

// context initialization
int bd2_init(bd2_context *ctx,ecp_group_id id) {
	if (ctx == NULL)
		return BAD_INPUT_DATA;

    ecdh_init(&ctx->context);
    ecp_point_init(&ctx->X);
    ecp_point_init(&ctx->key);
    return (ecp_use_known_dp(&ctx->context.grp,id));
}

/*
	Function to generate the key K, a simple wrapper around bd2_make_key
	It should be used only by the node deciding the key that will be propagated
*/
int bd2_make_key(bd2_context *ctx,void *p_rng) {
    int ret;
    mpi d;
    
	if (ctx == NULL)
		return BAD_INPUT_DATA;

	mpi_init(&d);

    MPI_CHK(ecp_gen_keypair(&ctx->context.grp,&d,&ctx->key,ctr_drbg_random,p_rng));
    
cleanup:
	mpi_free(&d);	
	return ret;
}

/*
	Function for exporting the key value on a buffer
*/
int bd2_export_key(bd2_context *ctx,unsigned int *olen,unsigned char *buf,unsigned int buflen) {
	if (mpi_size(&ctx->key.X) > buflen)
        return BUFFER_TOO_SHORT;

    *olen = ctx->context.grp.pbits / 8 + ((ctx->context.grp.pbits % 8) != 0);
    return mpi_write_binary(&ctx->key.X,buf,*olen);
}

// helper function 
int bd2_compute_shared_point(bd2_context *ctx,ecp_point *P,void *p_rng) {
	int ret;

	 /*
     * Make sure Q is a valid pubkey before using it
     */
    MPI_CHK(ecp_check_pubkey(&ctx->context.grp,&ctx->context.Qp));

	// computing the shared key as P = d*Qp
    MPI_CHK(ecp_mul(&ctx->context.grp,P,&ctx->context.d,&ctx->context.Qp,ctr_drbg_random,p_rng));

	// check if the shared key is 0
    if( ecp_is_zero(P))
    {
        ret = POLARSSL_ERR_ECP_BAD_INPUT_DATA;
        goto cleanup;
    }

cleanup:
	return ret;	
}


/*
	Function generating the value sent by node i to an adjacent node j 
	to allow it to derive the key: X = K + (Kij^-1)
*/
int bd2_make_val(bd2_context *ctx,unsigned int *olen,unsigned char *buf,unsigned int buflen,void *p_rng) {
	int ret;
    ecp_point P;

	if (ctx == NULL)
		return BAD_INPUT_DATA;

    ecp_point_init(&P);

	// first we compute Kij, the shared key between node i and j
	MPI_CHK(bd2_compute_shared_point(ctx,&P,p_rng));
		
	// computing X = key + (-P)
	ecp_opp(&ctx->context.grp,&P,&P);
	ecp_add(&ctx->context.grp,&ctx->X,&ctx->key,&P);

	// exporting on buffer
	MPI_CHK(ecp_tls_write_point(&ctx->context.grp,&ctx->X,ctx->context.point_format,
                                     olen,buf,buflen));
    
cleanup:	 
    ecp_point_free(&P);
	return(ret);
}

/*	
	Complementary to bd2_make_val. Import the value sent by adjacent node
*/
int bd2_import_val(bd2_context *ctx,unsigned char *buf,unsigned int buflen) {
    int ret;
	unsigned char *p;
 
	if (ctx == NULL)
		return BAD_INPUT_DATA;

	p = buf;
	if ((ret = ecp_tls_read_point(&ctx->context.grp,&ctx->X,(const unsigned char **) &p,buflen)) != 0)
		return ret;
	
	return 0;
}

/*
	Function to derive the key as: K = X + Kij 
*/
int bd2_calc_key(bd2_context *ctx,void *p_rng) {
	int ret;
    ecp_point P;

	if (ctx == NULL)
		return BAD_INPUT_DATA;

    ecp_point_init(&P);

	MPI_CHK(bd2_compute_shared_point(ctx,&P,p_rng));
	MPI_CHK(ecp_add(&ctx->context.grp,&ctx->key,&ctx->X,&P));

cleanup:
	ecp_point_free(&P);
	return ret;
}

int bd2_free(bd2_context *ctx) {
	if (ctx == NULL)
		return BAD_INPUT_DATA;
	
	ecdh_free(&ctx->context);
	ecp_point_free(&ctx->X);
	ecp_point_free(&ctx->key);

	return 0;
}

int bd2_make_public(bd2_context *ctx,unsigned int *olen,unsigned char *buf,unsigned int buflen,void *p_rng) {
	int ret;

	if (ctx == NULL)
		return BAD_INPUT_DATA;

	if ((ret = ecdh_make_public(&ctx->context,olen,buf,buflen,ctr_drbg_random,p_rng)) != 0)
		return ret; 

	return 0;
}

int bd2_read_public(bd2_context *ctx,const unsigned char *buf,unsigned int buflen) {
	int ret;

	if (ctx == NULL)
		return BAD_INPUT_DATA;

	if ((ret = ecdh_read_public(&ctx->context,buf,buflen)) != 0)
		return ret; 
	
	return 0;		
}
