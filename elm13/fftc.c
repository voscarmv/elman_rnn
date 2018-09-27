/*  The complex sequence in x[] is ordered real, imaginary, real,
    imaginary, ... .   

    For the tranform of a real sequence of n points, the array x[]
    has n+2 elements.

    This routine multiplies the forward transform by 2/n and the
    inverse transform by 1/2.  In the transforms of a real sequence,
    the zero-frequncy term of the forward transform is twice as large
    as the d.c. input.

    To compute the forward transform put rev = 0.
    To compute the reverse transform put rev nonzero.

    Input m0 is the base 2 log of n, where n is the number of
    either complex or real data samples.  */

#define CONJ 0
#define REALTR 1

static int n0 = 0;
static int n4 = 0;
static int msav = 0;

extern double PI;

double sin (double);

int realtr( double *, double *, int, int );
int bitrv( int, int );


int
fftc( double *x, int m0, double sine[], int rev )
/*
double x[];
int m0;
double sine[];
int rev;
*/
{
int th, nd, pth, nj, dth, m;
int n, n2, j, k, l, r;
double xr, xi, tr, ti, co, si;
double c;
/* double a, b, d, bc, cc, cs, bs, cc; */
/* int n1; */
double *p, *q;

if( m0 != msav )
	{
	msav = m0;

	/* Find n0 = 2**m0	*/
	n0 = 1;
	for( j=0; j<m0; j++ )
		n0 <<= 1;

	n4 = n0 >> 2;

	/* Calculate array of sines */
	xr = 2.0 * PI / n0;
	for( j=0; j<=n4; j++ )
		sine[j] = sin( j * xr );
	}

/* Conjugate the input for reverse transform.  */
#if CONJ
if( rev )
	{
	n2 = 2 * n0;
	for( k=1; k<n2; k += 2 )
		{
		x[k] = -x[k];
		}
	}
#endif

#if REALTR
n = n0 >> 1;	/* doing half length transform */
m = m0 - 1;
#else
n = n0;
m = m0;
#endif

#if REALTR
if( rev != 0 )
	realtr( x, sine, n, 1 );
#endif

/*  Complex Fourier Transform of n Complex Data Points */

/*	First, bit reverse the input data	*/

for( k=0; k<n; k++ )
	{
	j = bitrv( k, m );
	if( j > k )
		{ /* executed approx. n/2 times */
		p = &x[2*k];
		tr = *p++;
		ti = *p;
		q = &x[2*j+1];
		*p = *q;
		*(--p) = *(--q);
		*q++ = tr;
		*q = ti;
		}
	}


/*			Radix 2 Complex FFT			*/
n2 = n/2;
nj = 1;
pth = 1;
dth = 0;
th = 0;

for( l=0; l<m; l++ )
	{	/* executed log2(n) times, total */
	j = 0;
	do
		{	/* executed n-1 times, total */
#if REALTR
		r = th << 1;
#else
		r = th;
#endif
		si = sine[r];
		co = sine[ n4 - r ];
#if CONJ

#else
		if( rev )
			si = -si;
#endif
		if( j >= pth )
			{
			th -= dth;
			co = -co;
			}
		else
			th += dth;

		nd = j;

		do
			{ /* executed n/2 log2(n) times, total */
			r = (nd << 1) + (nj << 1);
			p = &x[ r ];
			xr = *p++;
			xi = *p;
			tr = xr * co + xi * si;
			ti = xi * co - xr * si;
			r = nd << 1;
			q = &x[ r ];
			xr = *q++;
			xi = *q;
			*p = xi - ti;
			*(--p) = xr - tr;
			*q = xi + ti;
			*(--q) = xr + tr;
			nd += nj << 1;
			}
		while( nd < n );
		}
	while( ++j < nj );

	n2 >>= 1;
	dth = n2;
	pth = nj;
	nj <<= 1;
	}

/* Scale the result. */

n2 = 2 * n;
if( rev )
	{
	/* Conjugate if reverse transform. */
#if CONJ
	for( k=1; k < n2; k += 2 )
		x[k] = -x[k];
#endif
#if REALTR

#else
	c = 0.5;
	for( k=0; k<n2; k++ )
		x[k] *= c;
#endif
	}
else
	{
	c = 2.0/n0;
	for( k=0; k<n2; k++ )
		x[k] *= c;
	}

#if REALTR
if( rev == 0 )
	realtr( x, sine, n, 0 );
#endif
return(0);
}



/*	Bit reverser	*/

int bitrv( int j, int m )
/* int j, m; */
{
register int j1, ans;
short k;

ans = 0;
j1 = j;

for( k=0; k<m; k++ )
	{
	ans = (ans << 1) + (j1 & 1);
	j1 >>= 1;
	}

return( ans );
}


/*	Singleton's special trick algorithm	*/
/*	converts to spectrum of real series	*/

int
realtr( double *xx, double *sine, int n, int rev )
/*
double xx[], sine[];
int n, rev;
*/
{
register double *p, *q;
double a, b, c, d, x, t, co;
double *ps, *pc;
int j;

if( rev == 0 )
	{
	xx[n+n] = xx[0];
	xx[n+n+1] = xx[1];
	}
p= &xx[0];
q = &xx[2*n];
ps = &sine[0];
pc = &sine[n4];
for( j=0; j<=n4; j++ )
	{	/* executed n/2 times */
	x = *p;
	t = *q;
	a = x + t;
	c = x - t;

	x = *(p+1);
	t = *(q+1);
	b = x + t;
	d = x - t;

	x = *ps++;
	if( rev )
		co = -( *pc-- );
	else
		co = *pc--;

	t = b * x + c * co;
	*(p+1) = 0.5*(d - t);
	*(q+1) = -0.5*( d + t );

	t = b * co - c * x;
	*p = 0.5*(a + t);
	*q = 0.5*(a - t);
	p += 2;
	q -= 2;
	}
/*
 j = 0: bs = cs = 0, bc = b, cc = c.
 *(p+1) = d - c = xi - ti - xr + tr
 *p = a + b = xr + tr + xi + ti
 *(q+1) = -(d + c) = -(xi - ti + xr - tr)
 *q = a - b = xr + tr - xi - ti.
*/
return(0);
}

