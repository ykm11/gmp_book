int
mpz_cmp (mpz_srcptr u, mpz_srcptr v) __GMP_NOTHROW
{
  mp_size_t  usize, vsize, dsize, asize;
  mp_srcptr  up, vp;
  int        cmp;

  usize = SIZ(u);
  vsize = SIZ(v);
  dsize = usize - vsize; // 筆者コメント：ここでオーバーフローする
  if (dsize != 0)
    return dsize;

  asize = ABS (usize);
  up = PTR(u);
  vp = PTR(v);
  MPN_CMP (cmp, up, vp, asize);
  return (usize >= 0 ? cmp : -cmp);
}
