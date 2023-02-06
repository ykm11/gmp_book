void
mpz_mul (mpz_ptr w, mpz_srcptr u, mpz_srcptr v)
{
  mp_size_t usize;
  mp_size_t vsize;
  mp_size_t wsize;
  mp_size_t sign_product;
  mp_ptr up, vp;
  mp_ptr wp;
  mp_ptr free_me;
  size_t free_me_size;
  mp_limb_t cy_limb;
  TMP_DECL;

  usize = SIZ (u);
  vsize = SIZ (v);
  sign_product = usize ^ vsize;
  usize = ABS (usize);
  vsize = ABS (vsize);

  if (usize < vsize)
    {
      MPZ_SRCPTR_SWAP (u, v);
      MP_SIZE_T_SWAP (usize, vsize);
    }

  if (vsize == 0)
    {
      SIZ (w) = 0;
      return;
    }

  if (vsize == 1)
    {
      wp = MPZ_REALLOC (w, usize+1);
      cy_limb = mpn_mul_1 (wp, PTR (u), usize, PTR (v)[0]);
      wp[usize] = cy_limb;
      usize += (cy_limb != 0);
      SIZ (w) = (sign_product >= 0 ? usize : -usize);
      return;
    }

  TMP_MARK;
  free_me = NULL;
  up = PTR (u);
  vp = PTR (v);
  wp = PTR (w);

  wsize = usize + vsize;
  if (ALLOC (w) < wsize) {
    if (ALLOC (w) != 0) {
	    if (wp == up || wp == vp) {
          free_me = wp;
          free_me_size = ALLOC (w);
	    }
	    else
	     (*__gmp_free_func) (wp, (size_t) ALLOC (w) * GMP_LIMB_BYTES);
    }

    ALLOC (w) = wsize;
    wp = __GMP_ALLOCATE_FUNC_LIMBS (wsize);
    PTR (w) = wp;
  }
  else {
      if (wp == up) {
	    up = TMP_ALLOC_LIMBS (usize);
	    if (wp == vp)
	        vp = up;
	    MPN_COPY (up, wp, usize);
	  }
      else if (wp == vp) {
         vp = TMP_ALLOC_LIMBS (vsize);
         MPN_COPY (vp, wp, vsize);
	  }
  }

  if (up == vp) {
      mpn_sqr (wp, up, usize);
      cy_limb = wp[wsize - 1];
  }
  else {
      cy_limb = mpn_mul (wp, up, usize, vp, vsize);
  }

  wsize -= cy_limb == 0;

  SIZ (w) = sign_product < 0 ? -wsize : wsize;
  if (free_me != NULL)
    (*__gmp_free_func) (free_me, free_me_size * GMP_LIMB_BYTES);
  TMP_FREE;
}

