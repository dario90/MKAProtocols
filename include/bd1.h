/** 
* \file bd1.h
*
* \brief Burmester-Desmedt key exchange
*
* Refer to: Burmester, M., Desmedt, Y.: A Secure and Efficient Conference Key Distribution System
*/

# include "util.h"

/**
* \brief BD context structure
*/
typedef struct {
	ecp_group grp; /** ecp group */
	mpi r;		   /** private exponent */
	ecp_point z;   /** public exponent */
	ecp_point X;   /** X value computed in the 2nd round (refer to the paper) */
} bd1_context;

/**
* \brief BD initialization function
*
* \param ctx BD context to be initialized
* \param id  id of the employed elliptic curve 
*
* \return    0 if successful
*/
int bd1_init(bd1_context *ctx,ecp_group_id id);

/**
* \brief BD generate ec point function
*
* \param ctx    BD context     
* \param olen   number of bytes written
* \param buf    output buffer
* \param buflen length of output buffer
* \param p_rng  initialization value for rng
*
* \return 0 if successful
*/
int bd1_gen_point(bd1_context *ctx,unsigned int *olen,unsigned char *buf,unsigned int buflen,void *p_rng);

/**
* \brief BD generate value X
*
* \param ctx     BD context     
* \param buf     input buffer
* \param buflen  lenght of input buffer
* \param olen    number of bytes written
* \param obuf    output buffer
* \param obuflen output buffer length
* \param p_rng   initialization value for rng
*
* \return 0 if successful
*/
int bd1_gen_value(bd1_context *ctx,unsigned char *buf,unsigned int buflen,unsigned int *olen,unsigned char *obuf,unsigned int obuflen,void *p_rng);

/**
* \brief BD compute key function
*
* \param ctx     BD context     
* \param buf     input buffer
* \param buflen  lenght of input buffer
* \param n       number of parties
* \param olen    number of bytes written
* \param obuf    output buffer
* \param obuflen output buffer length
* \param p_rng   initialization value for rng
*
* \return 0 if successful
*/
int bd1_compute_key(bd1_context *ctx,unsigned char *buf,unsigned int buflen,int n,unsigned int *olen,unsigned char *obuf,unsigned int obuflen,void *p_rng);


/**
* \brief MKA context free function
*
* \param ctx    MKA context     
*
* \return 0 if successful
*/
int bd1_free(bd1_context *ctx);
