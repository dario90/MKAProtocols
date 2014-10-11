# include "util.h"

int ecp_opp( const ecp_group *grp, ecp_point *R, const ecp_point *P )
{
    int ret;

    /* Copy */
    if( R != P )
        MPI_CHK( ecp_copy( R, P ) );

    /* In-place opposite */
    if( mpi_cmp_int( &R->Y, 0 ) != 0 )
        MPI_CHK( mpi_sub_mpi( &R->Y, &grp->P, &R->Y ) );

cleanup:
    return( ret );
}

