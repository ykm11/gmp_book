= mpq - 多倍長有理数
本章では、多倍長有理数のmpq_tとmpq_classを扱います。

== mpq_t

=== mpq_tを使う前に
mpq_tの定義をgmp.hから探します。

//list[][mpq_tの定義(gmp.h)][]{
typedef struct
{
  __mpz_struct _mp_num;
  __mpz_struct _mp_den;
} __mpq_struct;

typedef __mpq_struct mpq_t[1];
//}

//noindent
mpz_tと同様に、__mpq_structという識別子で構造体が定義されて、そのあとにmpq_tがtypedefされています。
メンバ変数は__mpz_structの_mp_numと_mp_denですね。分子と分母です。
ということは、__mpz_structさえわかってしまえばmpq_tは難しいものではないですね。

=== mpq_tの初期化


====[notoc] mpq_set_str
mpq_tに値をセットするmpq_set_str関数を解説します。実装は以下のとおりです。

//list[][mpq_set_strの実装(mpq/set_str.c)][]{
int
mpq_set_str (mpq_ptr q, const char *str, int base)
{
  const char  *slash;
  char        *num;
  size_t      numlen;
  int         ret;

  slash = strchr (str, '/');
  if (slash == NULL)
    {
      SIZ(DEN(q)) = 1;
      MPZ_NEWALLOC (DEN(q), 1)[0] = 1;

      return mpz_set_str (mpq_numref(q), str, base);
    }

  numlen = slash - str;
  num = __GMP_ALLOCATE_FUNC_TYPE (numlen+1, char);
  memcpy (num, str, numlen);
  num[numlen] = '\0';
  ret = mpz_set_str (mpq_numref(q), num, base);
  __GMP_FREE_FUNC_TYPE (num, numlen+1, char);

  if (ret != 0)
    return ret;

  return mpz_set_str (mpq_denref(q), slash+1, base);
}
//}

//noindent
mpz_init_set_strと同じように、mpq_tインスタンスと文字列、基数を渡します。文字列の中にはスラッシュ（/）を含めることができます。たとえば、 mpq_set_str(q, "1/3", 10)と書くと、q->_mp_numに1が、q->_mp_denに3がセットされます。

文字列にスラッシュが含まれていた場合、スラッシュの前と後に分割して、mpz_set_str関数を使って_mp_numと_mp_denにそれぞれ値をセットします。スラッシュが含まれない場合は、_mp_denには1をセットし、mpz_set_str関数を使って_mp_numに値をセットします。


mpq_init_set_strではなくmpq_set_strであることに注意してください。mpq_tは初期化と同時に初期値を与える関数がありません。mpq_initで初期化してからmpq_set_strで値をセットします。


=== mpq関数群

====[notoc] mpq_mul
有理数の乗算関数mpq_mulの解説です。先に有理数の乗算を復習しましょう。
op1が@<m>$\frac{a}{b}$, op2が@<m>$\frac{c}{d}$で表され、それぞれ既約分数であるとします。
これらの積@<m>$op1 \cdot op2 = \frac{ac}{bd}$が既約分数でないときは、約分して既約分数にする必要があります。約分せずに放置すると、無駄なメモリを使用する可能性があるからです。

まずは愚直な方法です。
@<m>$\frac{ac}{bd}$を既約分数で表すには、分子分母を@<m>$GCD(ac, bd)$で割ればいいですよね。
例)  @<m>$\frac{3}{8} \cdot \frac{4}{9} = \frac{12}{72} = \frac{12}{72} \cdot  \frac{  \frac{1}{12} }{ \frac{1}{12}} = \frac{1}{6}$

この方法の問題点は、乗算で桁数が増えてから除算を行うことです。
mpz_tが確保するメモリ領域は増える一方なので、桁数はなるべく小さく抑えたいというモチベーションがあります。
mpq_mulでは、先に約分してから乗算するように実装されています。


ではmpq_mul関数の実装をみていきます。長いので、重要な部分に絞って解説します。残りの部分はメモリ確保のためのサイズ計算が主です。

//list[][mpq_mulの実装を部分的に抜粋(mpq/mul.c)][]{
  mpz_gcd (gcd1, NUM(op1), DEN(op2));
  mpz_gcd (gcd2, NUM(op2), DEN(op1));

  mpz_divexact_gcd (tmp1, NUM(op1), gcd1);
  mpz_divexact_gcd (tmp2, NUM(op2), gcd2);

  mpz_mul (NUM(prod), tmp1, tmp2);

  mpz_divexact_gcd (tmp1, DEN(op2), gcd1);
  mpz_divexact_gcd (tmp2, DEN(op1), gcd2);

  mpz_mul (DEN(prod), tmp1, tmp2);
//}

//noindent
やはりマクロが出てきます。gmp-impl.h, gmp.hから
NUM, DEN, mpq_numref, mpq_denrefの定義を探します。

//list[][NUM, DENのマクロ定義(gmp-impl.h)][]{
#define NUM(x) mpq_numref(x)
#define DEN(x) mpq_denref(x)
//}

//list[][numref, denrefのマクロ定義(gmp.h)][]{
#define mpq_numref(Q) (&((Q)->_mp_num))
#define mpq_denref(Q) (&((Q)->_mp_den))
//}

//noindent
計算のアルゴリズムの触れていきます。
//noindent
まず、@<m>$t := GCD(a, d), \, s :=  GCD(b, c)$を計算しておきます。  
//noindent
@<m>$tmp1 := \frac{a}{t}, \, tmp2 := \frac{c}{s}$と除算してから、@<m>$NUM(prod) := tmp1 \cdot tmp2$。  
//noindent
分母も同様に、@<m>$t, s$を使って、@<m>$tmp1 := \frac{d}{t}, \, tmp2 := \frac{b}{s}$と除算してから。
@<m>$DEN(prod) := tmp1 \cdot tmp2$。


//noindent
正しく計算が行われていることを確認します。
//texequation[][]{
\begin{aligned}
\frac{NUM(prod)}{DEN(prod)} =  \frac{ \frac{a}{t} \frac{c}{s}  }{ \frac{d}{t} \frac{b}{s} } = \frac{ac}{bd} \cdot \frac{\frac{1}{ts}}{\frac{1}{ts}} = \frac{ac}{bd}
\end{aligned}
//}

//noindent
OK。
除算で桁数を落としてから乗算するので、メモリを無駄遣いせずに済みます。

op1=op2のとき、つまり2乗のときの処理もついでに解説します。mpq_mulの上部に記述されています。
//list[][2乗への切り替え(mpq/mul.c)][]{
  if (op1 == op2)
    {
      /* No need for any GCDs when squaring. */
      mpz_mul (mpq_numref (prod), mpq_numref (op1), mpq_numref (op1));
      mpz_mul (mpq_denref (prod), mpq_denref (op1), mpq_denref (op1));
      return;
    }
//}

単に分子分母をそれぞれ2乗しているだけですね。シンプルです。ここで注意ですが、op1==op2の比較は、値を比較しているのではなく、ポインタを比較しています。値が同じでもポインタが違う場合は通常の乗算が行われます。

== mpq_class

