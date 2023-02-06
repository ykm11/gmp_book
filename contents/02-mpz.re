= mpz - 多倍長整数
本章では、多倍長整数のmpz_tとmpz_classを扱います。
これからはプログラムを読んでいきます。

== mpz_t

=== mpz_tを使う前に

====[notoc] mpz_classはmpz_tのwrapperじゃないの？
前の章で、「mpz_classは厳密にはmpz_tのwrapperではない」といいました。まずはこの件を解決しましょうか。
//noindent
というわけで、gmp.hの中からmpz_tに関する記述を探します。

//list[][gmp.h]{
typedef struct
{
  int _mp_alloc;        /* Number of *limbs* allocated and pointed
                           to by the _mp_d field.  */
  int _mp_size;         /* abs(_mp_size) is the number of limbs the
                           last field points to.  If _mp_size is
                           negative this is a negative number.  */
  mp_limb_t *_mp_d;     /* Pointer to the limbs.  */
} __mpz_struct;

typedef __mpz_struct mpz_t[1];
//}

//noindent
見てわかるように、_mp_alloc, _mp_size, _mp_dというメンバを持つ構造体は、__mpz_structという識別子で定義されていますね。その下で、typedefによって__mpz_structのサイズ1の配列がmpz_tという型として定義されています。"厳密には"というのはこういうことです。
表記としてmpz_tを使っても問題ないので、本書では__mpz_structとは書かず、mpz_tと書くようにします（長いのは面倒とかそういう理由じゃないですよ）。
//noindent
mpz_tは構造体のインスタンスではなくアドレスなので、メンバ変数にアクセスするときはドット演算子ではなくアロー演算子を使います。また、mpz_tを関数の引数として渡すときに、ポインタ渡しとなることに注意しましょう。

====[notoc] mpz_tのメンバ変数
mpz_tの各メンバ変数を説明します。

 : _mp_alloc
    _mpd_dがメモリ確保している領域のサイズ。

 : _mp_size
    _mp_dが実際に使用している領域のサイズ。_mp_d[alloc-1]から順に値を見ていって、最初にnot 0が見つかるインデックス。
 : _mp_d
    mp_limb_t配列の先頭アドレス。


//noindent
mp_limb_tは32ビットか64ビットの符号なしlong intで、環境によって変わります。
//noindent
_mp_sizeは負数を取ることもでき、負のときには_mp_dの値が負数であることを意味します。_mp_sizeの1ビットを使って、isNegのようなフラグを実現しています。mpz_tインスタンスが使用するメモリサイズ（＝sizeof(mpz_t)）が16バイトちょうどなので、アライメントがいい感じになるわけです。
//noindent
また、計算の結果、桁数が増えるような場合（乗算に多い）は_mp_alloc, _mp_sizeが適宜更新されます。このときreallocが走ります。


====[notoc] mpz_tが扱える最大値
多倍長整数といえども限界はあるはずです。1つのmpz_tインスタンスが保持できる値の最大値の概算を出しましょう。
//noindent
まず、_mp_sizeが領域サイズを表していて、符号付きintなので最大値は@<m>$2^{31}-1$です。それから、64ビット環境を想定すると、mp_limb_tは領域1つにつき64ビットです。したがって、mpz_tが保持できる値の最大値は、@<m>$64*(2^{31}-1) \approx 2^{37}$ビットの値、つまり@<m>$2^{2^{37}}$くらいです。デカすぎて固定資産税がかかりそうですね。ここまで大きな値を扱うことはそうそうないので、最大値を気にする必要はないと思います。

=== mpz_tを使ってみよう
ではmpz_tを使ってみましょう。以下にコードを示します。
mpz_init関数, mpz_init_set_str関数によって、mpz_tインスタンスが初期化されています。

//list[][mpz_tの使用例][]{
#include <iostream>
#include <gmpxx.h>

int main() {
    mpz_t x, y, z;

    mpz_init(z);
    mpz_init_set_str(x, "d34d0000", 16);
    mpz_init_set_str(y, "b33f0000", 16);

    std::cout << x << std::endl;
    std::cout << y << std::endl;

    mpz_mul(z, x, y);
    std::cout << z << std::endl;
}
//}

//terminal[?][実行結果]{
$ g++ source.cpp -lgmpxx -lgmp -O3 && ./a.out
3545038848
3007250432
10660819607104782336
//}

//noindent
mpz_tインスタンスが使用しているメモリ領域はfreeしないと開放されません。なんだか当たり前な気がしますが、放っておくとメモリが枯渇するので注意しましょう。追加の領域が必要になったときは勝手にreallocしてくれるのですが、メモリ開放は自分で行う必要があります。メモリの開放にはmpz_clear関数を使います。

//noindent
mpz_tインスタンスを初期化しなかった場合にどうなるか、気になりますよね? 次項でやります。

=== mpz_tインスタンスを初期化しないと・・
先に答えを言うと、初期化しなかったらプロセスが落ちます。何回も実行していると、稀に落ちることなく無事に終了することもあるのですが、まあバグなので潰しておきましょう。

//noindent
実際にプロセスが落ちる様子をお見せします。

//list[][mpz_tを初期化しないとプロセスが落ちる][]{
int main() {
    mpz_t x, y, z;

    //mpz_init(z);
    mpz_init_set_str(x, "d34d0000", 16);
    mpz_init_set_str(y, "b33f0000", 16);

    std::cout << x << std::endl;
    std::cout << y << std::endl;

    mpz_mul(z, x, y);
    std::cout << z << std::endl;
}
//}

//terminal[?][実行結果]{
$ g++ source.cpp -lgmpxx -lgmp -O3 && ./a.out
3545038848
3007250432
zsh: segmentation fault (core dumped)  ./a.out
//}

//noindent
セグフォしましたね。計画どおりです。
mpz_tインスタンスを初期化する前と後とで各メンバ変数がどうなっているのかをみてみます。
//list[][mpz_tをdumpしてみる][]{
#include <iostream>
#include <gmpxx.h>

void mpz_dump(const mpz_t r) {
    printf("_mp_alloc = %d\n", r->_mp_alloc);
    printf("_mp_size  = %d\n", r->_mp_size);
    printf("_mp_d     = %p\n\n", r->_mp_d);
}

int main() {
    mpz_t x;

    mpz_dump(x);
    mpz_init(x);
    mpz_dump(x);
}
//}

各メンバ変数を表示するmpz_dumpという関数を実装しました。これを実行してみると、初期化後は_mp_alloc, _mp_sizeに0が、_mp_dにはスタック領域のアドレスが入っていることがわかります。初期化前は..なんかすごいことになっていますよね。_mp_dがNULLです。この状態でmpz_の四則演算関数にmpz_tインスタンスを渡すと、NULLにアクセスすることになるのでセグフォが出ます。

どうしてmpz_tインスタンスのメンバ変数に変な値が入っているのか、スタックポインタを知っている読者の方ならわかると思います。mpz_tインスタンスを宣言しただけでは、スタックポインタがsizeof(mpz_t)分引き算されるだけです。このときの各メンバ変数の値はスタック次第ということになります。

//terminal[?][実行結果]{
$ g++ source.cpp -lgmpxx -lgmp -O3 && ./a.out
_mp_alloc = -1511941312
_mp_size  = 32546
_mp_d     = (nil)

_mp_alloc = 0
_mp_size  = 0
_mp_d     = 0x7f22a5bf1288
//}


@<img>{mpz_address}にmpz_tのメモリ配置図を示します。mpz_t自体はスタック領域に乗りますが、_mp_dはヒープ領域を指しています。
//image[mpz_address][mpz_tのアドレス][]


====[notoc] mpz_initの実装

mpz_init関数の実装をみていきます。

//list[][mpz_init(mpz/init.c)][]{
void
mpz_init (mpz_ptr x) __GMP_NOTHROW
{
  static const mp_limb_t dummy_limb=0xc1a0;
  ALLOC (x) = 0;
  PTR (x) = (mp_ptr) &dummy_limb;
  SIZ (x) = 0;
}
//}

//noindent
おっと、mpz_ptrなるものが出てきましたね。gmp.hでmpz_ptrを探すと、__mpz_struct*であることがわかりました。mpz_tもアドレスですので、mpz_init関数に引数として渡せます。

//noindent
ALLOC (x), PTR (x), SIZ (x)はマクロ定義です。動きについては説明しなくていいかもしれませんが、一応みておきます。gmp-impl.hでdefineされています。
//list[][mpz_ptrのtypedef(gmp.h)][]{
typedef __mpz_struct *mpz_ptr;
//}

//list[][SIZ, PTR, ALLOCのマクロ定義(gmp-impl.h)][]{
#define SIZ(x) ((x)->_mp_size)
#define PTR(x) ((x)->_mp_d)
#define ALLOC(x) ((x)->_mp_alloc)
//}

//noindent
話をmpz_init関数に戻します。_mp_alloc, _mp_sizeに0を代入して、_mp_dにはstaticな定数のアドレスを代入しています。0xc1a0という値の意味はわかりません。一旦初期化をしておけば、四則演算の関数に渡しても不正アクセスなどでプロセスが落ちることはありません。


====[notoc] mpz_init_set_strの実装
次にmpz_init_set_str関数です。重要なのは最後のmpz_set_str関数なのですが、ここを解説するには紙面と気力が足りないので、
mpz_init_set_strの解説だけとさせてください。

//list[][mpz_init_set_str (mpz/iset_str.c)][]{
int
mpz_init_set_str (mpz_ptr x, const char *str, int base)
{
  static const mp_limb_t dummy_limb=0xc1a0;
  ALLOC (x) = 0;
  PTR (x) = (mp_ptr) &dummy_limb;

  /* if str has no digits mpz_set_str leaves x->_mp_size unset */
  SIZ (x) = 0;

  return mpz_set_str (x, str, base);
}
//}

//noindent
mpz_init_set_strはmpz_ptrと文字列、基数を引数に取ります。
与えた文字列を基数で表現した値が必要とするメモリ確保したのち、確保したメモリの先頭アドレスをmpz_ptr->_mp_dに代入します。



====[notoc] mpz_init2の実装
例に示したmpz_init, mpz_init_set_str関数の他に、mpz_init2というものがあります。mpz_initがただ初期値を与えるだけなのに対して、mpz_init2ではあらかじめ決まったサイズのメモリ確保を行います。以下にmpz_init2の実装を示します。

//list[][mpz_init2 (mpz/init2.c)][]{
void
mpz_init2 (mpz_ptr x, mp_bitcnt_t bits)
{
  mp_size_t  new_alloc;

  bits -= (bits != 0);          /* Round down, except if 0 */
  new_alloc = 1 + bits / GMP_NUMB_BITS;

  if (sizeof (unsigned long) > sizeof (int)) /* param vs _mp_size field */
    {
      if (UNLIKELY (new_alloc > INT_MAX))
        {
          fprintf (stderr, "gmp: overflow in mpz type\n");
          abort ();
        }
    }

  PTR(x) = __GMP_ALLOCATE_FUNC_LIMBS (new_alloc);
  ALLOC(x) = new_alloc;
  SIZ(x) = 0;
}
//}

//noindent
引数にはmpz_ptrと、初期化時に確保する領域のサイズ（ビット数）を渡します。
指定したビット数分のメモリ確保を過不足なく行うため、

//texequation[][]{
\begin{aligned}
new\_alloc :&=& 1 + \frac{bits - 1}{GMP\_NUMB\_BITS}  \\
            &=& \frac{bits + (GMP\_NUMB\_BITS - 1)}{GMP\_NUMB\_BITS}
\end{aligned}
//}

//noindent
という計算が行われています。bits=0のときだけ特殊で、new_alloc=1と定義されます。
GMP_NUMB_BITSは次のように定義されています。

//list[][GMP_NUMB_BITSのマクロ定義 (gmp.h)][]{
#if ! defined (__GMP_WITHIN_CONFIGURE)
#define __GMP_HAVE_HOST_CPU_FAMILY_power   0
#define __GMP_HAVE_HOST_CPU_FAMILY_powerpc 0
#define GMP_LIMB_BITS                      64
#define GMP_NAIL_BITS                      0
#endif
#define GMP_NUMB_BITS     (GMP_LIMB_BITS - GMP_NAIL_BITS)
//}

//noindent
__GMP_ALLOCATE_FUNC_LIMBSのマクロ定義はgmp-impl.hにあります。

//list[][__GMP_ALLOCATE_FUNC_LIMBSのマクロ定義(gmp-impl.h)][]{
__GMP_DECLSPEC extern void * (*__gmp_allocate_func) (size_t);

__GMP_DECLSPEC void *__gmp_default_allocate (size_t);

#define __GMP_ALLOCATE_FUNC_TYPE(n,type) \
  ((type *) (*__gmp_allocate_func) ((n) * sizeof (type)))
#define __GMP_ALLOCATE_FUNC_LIMBS(n)   __GMP_ALLOCATE_FUNC_TYPE (n, mp_limb_t)
//}

//noindent
__gmp_allocate_funcがメモリ確保を行う関数で、ここを書き換えることにより、メモリ確保に好きな関数を利用できます。


以下に示すコードを使って、mpz_init2関数でmpz_tを初期化したときの各メンバ変数の値を確認してみます。

//list[][mpz_init2を使った初期化][]{
@<include>{sources/mpz_init2.cpp}
//}

//terminal[?][実行結果]{
$ g++ source.cpp -lgmpxx -lgmp -O3 && ./a.out
_mp_alloc = 4
_mp_size  = 0
_mp_d     = 0x55af0395ce70
//}

//noindent
_mp_sizeは0ですが、_mp_allocには4が代入されており、_mp_d_にはヒープ領域のアドレスが代入されていますね。

ついでに、__gmp_default_allocateで使われるメモリ確保の関数を調査します。
上のプログラムを使って、gdb-peda@<fn>{gdb-peda}でディスアセンブルします。

//footnote[gdb-peda][https://github.com/longld/peda]

//terminal[?][gdbで__gmp__default_allocateをディスアセンブル]{
gdb-peda$ disassemble __gmp_default_allocate
Dump of assembler code for function __gmp_default_allocate:
   0x00007ffff7b66970 <+0>:	push   rbx
   0x00007ffff7b66971 <+1>:	mov    rbx,rdi
   0x00007ffff7b66974 <+4>:	call   0x7ffff7b65cb0 <malloc@plt>
   0x00007ffff7b66979 <+9>:	test   rax,rax
   0x00007ffff7b6697c <+12>:	je     0x7ffff7b66980 <__gmp_default_allocate+16>
   0x00007ffff7b6697e <+14>:	pop    rbx
   0x00007ffff7b6697f <+15>:	ret
   0x00007ffff7b66980 <+16>:	mov    rax,QWORD PTR [rip+0x26b659]        # 0x7ffff7dd1fe0
   0x00007ffff7b66987 <+23>:	mov    rcx,rbx
   0x00007ffff7b6698a <+26>:	mov    esi,0x1
   0x00007ffff7b6698f <+31>:	lea    rdx,[rip+0x540fa]        # 0x7ffff7bbaa90
   0x00007ffff7b66996 <+38>:	mov    rdi,QWORD PTR [rax]
   0x00007ffff7b66999 <+41>:	xor    eax,eax
   0x00007ffff7b6699b <+43>:	call   0x7ffff7b66360 <__fprintf_chk@plt>
   0x00007ffff7b669a0 <+48>:	call   0x7ffff7b651c0 <abort@plt>
End of assembler dump.
//}

//noindent
mallocが使われていました。macOSの標準mallocは、Linuxのmallocと比べるとかなり遅いので、Microsoft社のmimallocを使うといいでしょう。

====[notoc] mpz_clearの実装
mpz_tの初期化の話をしたので、ついでにメモリ解放にも触れておきます。
//noindent
ではソースコード。
//list[][mpz_clear(mpz/clear.c)][]{
void
mpz_clear (mpz_ptr x)
{
  if (ALLOC (x))
    __GMP_FREE_FUNC_LIMBS (PTR (x), ALLOC(x));
}
//}

//noindent
ほとんどマクロですね。gmp-impl.hから__GMP_FREE_FUNC_LIMBSのマクロ定義を探します。

//list[][(gmp-impl.h)][]{
#define __GMP_FREE_FUNC_TYPE(p,n,type) (*__gmp_free_func) (p, (n) * sizeof (type))
#define __GMP_FREE_FUNC_LIMBS(p,n)     __GMP_FREE_FUNC_TYPE (p, n, mp_limb_t)
//}

//noindent
メモリ解放の関数__gmp_free_funcにx->_mp_dと(x->_mp_alloc * sizeof(mp_limb_t))を渡して開放してもらっています。

//noindent
メモリ確保、開放の関数はプログラマが自由に変更することができます。自作したmalloc, free関数を使ってみるのもいいかもしれませんね。

=== mpz関数群
接頭辞にmpz_がつく関数をここではmpz関数と呼び、本節ではmpz関数の解説を行います。
ソースコードをそのまま貼り付けると、プログラムのために数ページ割くことになるので、コメントやあまり重要でない箇所に関しては省略しています。プログラム全体を見たい方は本家のソースコードを参照してください。

====[notoc] mpz_add, mpz_sub
mpz_add, mpz_subはそれぞれ加算、減算を行う関数です。
まずソースコードを示してから、上から順に処理内容を解説していきます。
表示する行数を削減するため変数宣言を省略していますが、型が明らかものばかりなので読解は難しくないと思います。

//list[][(mpz/aors.h)][lineno=on,linenowidth=3]{
@<include>{sources/aors.h}
//}

mpz_add, mpz_subの実装はaors.hに記述されていて、ソースコードを共有しています（処理がほぼ同じなので）。
ifdefを駆使してmpz_addとmpz_subをそれぞれ実装しています。

====[notoc] mpz_mul
mpz_mulは名前のとおり、乗算を行う関数です。
先程と同様に、まずソースコードを示してから、上から順に処理内容を解説していきます。
//list[][mpz/mul.c][lineno=on,linenowidth=3]{
void
mpz_mul (mpz_ptr w, mpz_srcptr u, mpz_srcptr v)
{
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
//}

//noindent
まず4行目から8行目にかけては、@<m>$u, v$のサイズを取り出して、計算結果の正負を取得しています。

次に、@<m>$u, v$のサイズを比較して、@<m>$u > v$となるように値を適宜スワップします。こうすることで、16行目から30行目の処理において、@<m>$v$のサイズだけを確認すればよくなります。
@<m>$v$のサイズが0のとき（つまり@<m>$v == 0$）、計算結果は当然@<m>$0$なので、@<m>$w$のサイズに@<m>$0$をセットして終了します。@<m>$v$のサイズが1のときは、@<m>$u$のサイズに1足した数だけメモリ確保を行い、mpn_mul_1関数を実行して乗算を行います、
mpn_mul_1は本章の後半で説明しますが、処理内容を簡単に説明すると、多倍長整数(mp_limb_t*)と符号なし整数(mp_limb_t)の乗算です。
一般に、@<m>$n$桁と@<m>$ｍ$桁の数の乗算結果は@<m>$n+m$桁です。したがって、@<m>$w$のサイズは@<m>$u$のサイズに1を足した数になります。
小さい方のサイズが@<m>$0$もしくは@<m>$1$のときは計算が比較的容易でした。ここからは、どちらのサイズも@<m>$2$以上のときの処理です。

50行目から64行目では、計算結果を格納するメモリが十分に確保されているかを確認し、足りなければ領域を追加するためにreaqllocが走ります。この間の細かい処理について説明していきます。
40行目のif文では、@<m>$w$がメモリ確保を既に行っているかどうかを確認しています。既にメモリ確保をしている場合、現在確保している領域を開放してから、必要なサイズ(@<m>$usize + vsize$)のメモリ確保を行います。このとき、@<m>$w$が@<m>$u, v$のどちらかと同じメモリを指していたらメモリ解放を保留します。@<m>$w$と一緒に@<m>$u, v$の領域まで開放されしまったら後々の計算に影響が出るためです。mpz_mul(x, x, y)のような記述をしているときに該当します。

次に追加のメモリ確保が必要でないとき（53~64行）です。@<m>$w$が@<m>$u$, @<m>$v$と同じアドレスを指しているときは、新たにメモリ領域を確保して、値を退避させます。MPN_COPYは、アドレスとサイズを渡すと値をコピーしてくれる処理のマクロです。そこそこ長いので省略させてください。


66行目のアドレス比較は、2乗の計算を行うmpn_sqr関数への切り替えるかどうかを判断しています。2乗の計算は、普通に乗算を行うよりも2乗専用の関数/アルゴリズムを使ったほうが速いです。2乗のアルゴリズムについてはIntelさんがドキュメントを出しているので、興味があればそちら@<fn>{intel_sqr_docs}を参照してください。同じ桁数の乗算のコストを1とすると、2乗のコストは0.8程度になるといわれています。
アドレスが一致しない場合はmpn_mul関数を実行して乗算を行います。mpn_mul関数は本章の後半で登場します。

最後は、計算結果の符号をセットして、保留していたメモリ解放を行って終わりです。

//footnote[intel_sqr_docs][https://www.intel.com/content/dam/www/public/us/en/documents/white-papers/large-integer-squaring-ia-paper.pdf]


== mpz_class
多倍長整数を扱うのに便利なmpz_tでしたが、インスタンスは初期化が必要ですし（初期化忘れたらプロセスが落ちる）煩わしいですよね。

=== コンストラクタ
1章でmpz_classの使用例を示しました。初期化はコンストラクタがやってくれていましたね。
mpz_classはmpz_tから煩わしさを取り除いた僕らの希望です。C++に感謝しましょう。ありがとう！！


=== 代入時の扱い

//list[][mpz_class move][]{
#include <iostream>
#include <gmpxx.h>

int main() {
    mpz_class x(100);
    mpz_class y = x;

    printf("x->_mp_d = %p\n", (x.get_mpz_t())->_mp_d);
    printf("y->_mp_d = %p\n", (y.get_mpz_t())->_mp_d);
}
//}

//terminal[][実行結果]{
$ g++ source.cpp -lgmpxx -lgmp -O3 && ./a.out
x->_mp_d = 0x55a3b9b4be70
y->_mp_d = 0x55a3b9b4be90
//}

実行してみると、ちゃんと異なるアドレスが表示されました。@<m>$y$の値が変わっても、@<m>$x$の値が一緒に変わることはありません。

//noindent
上のプログラムの実行ファイルをディスアセンブルし、コンストラクタ呼び出しに該当する部分を探します。

//terminal[][コンストラクタによる初期化]{
$ objdump -d -M intel
 (略)
 9e8:	e8 83 ff ff ff       	call   970 <__gmpz_init_set_ui@plt>
 9ed:	48 89 de             	mov    rsi,rbx
 9f0:	48 89 ef             	mov    rdi,rbp
 9f3:	e8 48 ff ff ff       	call   940 <__gmpz_init_set@plt>
 (略)
//}

@<m>$x$がmpz_init_set_uiによって初期化され、次に@<m>$y$がmpz_init_setによって初期化されます。


== mpn関数群
これまでの節でmpz_t, mpz_classと解説してきました。mpz_tをwrapして使いやすくしたものがmpz_classでしたね。
mpn関数というのは、関数の接頭辞にmpn_がついたものをいいます。これまで見てきたmpzの関数内でもmpn関数が使われています。また、mpn関数は各アーキテクチャで可能な限り高速に動くよう設計されています。ほんとかなぁと思いますが、Low-level Functions@<fn>{mpn_docs}に"The mpn functions are designed to be as fast as possible, ..."と書いてあります。
mpz関数の中でmpn関数が使われているので、mpn関数を使いやすくしたものがmpz_tだと考えることもできます。
本節では、最もプリミティブなmpn関数について解説します。

//footnote[mpn_docs][https://gmplib.org/manual/Low_002dlevel-Functions]

=== mpnの概要
mpn関数はmpz関数と違ってメモリ確保をやってくれません。プログラマが自分でメモリ管理を行います。
使用するサイズの最大値がわかっていればmp_limb_tの配列（スタック領域）をうまく使うことができ、malloc/freeを呼び出す必要がないので、処理速度が上がります。サイズが不定の場合は、mallocを使って必要な分のメモリを確保（ヒープ領域）するようになるでしょう。これならmpz_tにメモリ管理を任せたほうがいいですね。

使用するサイズの最大値が決まっている場合というのは、たとえば有限体を実装するときです。有限体の標数(素数)が@<m>$n$ビットとすると、四則演算の結果は最大でも@<m>$2n$ビットにしかなりません。このような場合はmpn関数を使った実装による高速化が望めるでしょう。筆者が開発していた楕円曲線暗号では、mpz_classとmpn関数の両方で実装しています。



=== mpn関数の実装

====[notoc] mpn_mul_1

====[notoc] mpn_mul





== おまけ
この節では小ネタを紹介します。

==== スタック領域を使用するmpz_t
mpz_tインスタンスの_mp_dはヒープ領域を指しています、mallocを使ってメモリ確保するのですから当然ですよね。
実は、_mp_dはスタック領域の配列を指すようにもできます。といっても、筆者は値を表示したいときくらいしか使いません。
mpz_tインスタンスの各メンバ変数に値をセットする関数set_mpz_tを以下のコードに示します。

//list[][mp_limb_t*をmpz_tにセット][]{
#include <iostream>
#include <gmpxx.h>

void mpz_dump(const mpz_t r) {
    printf("_mp_alloc = %d\n", r->_mp_alloc);
    printf("_mp_size  = %d\n", r->_mp_size);
    printf("_mp_d     = %p\n", r->_mp_d);
}

void set_mpz_t(mpz_t r, const mp_limb_t *x, size_t n) {
    r->_mp_alloc = n;
    while(n > 0 && x[n-1] == 0) {
        --n;
    }
    r->_mp_size = n;
    r->_mp_d = (mp_limb_t *)x;
}

int main() {
    mpz_t t;
    mp_limb_t x[4] = {1, 2, 3, 0};
    set_mpz_t(t, x, 4);

    mpz_dump(t);
}
//}

//noindent
set_mpz_tにはmpz_tインスタンスとmp_limb_t*、サイズを渡します。

//terminal[][実行結果][]{
$ g++ source.cpp -lgmpxx -lgmp && ./a.out
_mp_alloc = 4
_mp_size  = 3
_mp_d     = 0x7ffd682cbb40
//}

//noindent
実行してみると、正しく値がセットされていることがわかります。_mp_dはスタック領域のアドレスを指していますね。
各メンバ変数への値のセットを自分でやっているので、mpz_initを使って初期化する必要がありません。

注意すべき点は、_mp_dがスタック領域を指しているときにrealloc/freeを発生させないことです。
douoble freeというエラー文と共にプロセスが終了します。
そもそもmallocで確保していないメモリ（スタック領域）をfreeするのが良くないですよね。


==== mpz_cmpのバグ
ver6.2.0まで、mpz_tの比較関数mpz_cmpにバグがありました。Release noteには、"A possible overflow of type int is avoided for mpz_cmp on huge operands."とあります。オーバーロードの可能性があったようです。実際のコードをみてみましょう。

//list[][ver6.2.0以前のmpz_cmp][]{
@<include>{sources/mpz_cmp_before.c}
//}

mpz_cmpの機能は、2つのmpz_tインスタンスu, vを取って、u=vなら0を、u>vなら正の数を、u<vなら負の数を返します。
まず、u,vのサイズの差を計算しています。サイズ（ビット幅）の異なる値の比較が容易だからですね。

//texequation[][]{
\begin{aligned}
mpz\_cmp(u, v) = 
\begin{cases}
0 & (u = v) \cr
Positive \,\, Num & (u > v) \cr 
Negative \,\, Num & (u < v)
\end{cases}
\end{aligned}
//}

具体例で説明します。usizeが10, vsizeが8だとすると、明らかにuが大きいです。mpz_cmpは@<m>$10-8=2$を返すことになり、正の値なので正しい動作となります。片方が負の場合で同様に確かめられます。

では、次の場合はどうでしょうか。usizeを@<m>$2^{31}-1$とします。_mp_sizeは符号付きのintですので、これは最大値です。
ここで、vsizeが負の数のときは@<m>$usize-vsize=2^{31}-1 + |vsize|$でオーバーフローしてしまい、mpz_cmpは負の数を返します。想定外の動作となっています。_mp_sizeが@<m>$2^{31}-1$程度の値を使うことはあまりないのですが、バグはバグです。
次のように修正されました。

//list[][ver6.2.1のmpz_cmp][]{
@<include>{sources/mpz_cmp_after.c}
//}

引き算をするとオーバーフローの可能性があるので、サイズの比較を行うようにしています。
MPN_CMPはgmp-impl.hの中で__GMPN_CMPのマクロとして定義されていて、__GMPN_CMPの中身はgmp.hに記述されています。

//texequation[][]{
\begin{aligned}
mpz\_cmp(u, v) = 
\begin{cases}
0 & (u = v) \cr
1 & (u > v) \cr 
-1 & (u < v)
\end{cases}
\end{aligned}
//}

==== mpn_zero vs memset
mp_limb_t*をゼロ埋めするとき、mpn_zeroを使う方法とmemsetを使う方法の2つがあります。for文はひとまず忘れてください。
どちらを使うべきかというのを、実行時間の面で決めることにします。速いほうが嬉しいので、実験で実行時間を測定してみます。実験に使用したコード群はここ@<fn>{memset-mpnzero}に置いてあります。

//footnote[memset-mpnzero][https://gist.github.com/ykm11/7d8b48d628c281ebe21004572f2c41e7]

mpn_zeroとmemsetでそれぞれ、(1)固定長でゼロ埋め、(2)可変長でゼロ埋め、の4パターン用意しました。
結果としては、固定長のmemsetが最も速く、次いで可変長のmemsetです。mpn_zeroは固定長、可変長にかかわらず同じくらいです。

固定長のmemsetが速いのは、memsetを呼び出さずにSIMD命令（128ビットレジスタなど）で0埋めができるからです。
可変長の場合はmemsetを呼び出さざるをえないので、関数コールのコストが上乗せされます。
SIMD命令が使用できないマシンでは測定結果が変わるはずなので、古いPCなどを所持している方は実験してみると面白いと思います。。

memsetに関してはこのくらいにして、mpn_zeroについて見ていきましょう。
mpn_zeroの実装を以下に示します。

//list[][mpn_zeroの実装(mpn/generic/zero.c)][]{
void
mpn_zero (mp_ptr rp, mp_size_t n)
{
  mp_size_t i;

  rp += n;
  for (i = -n; i != 0; i++)
    rp[i] = 0;
}
//}

//noindent
引数にポインタとサイズを受け、for文で順にアクセスして0をセットしています。
これはよくある方法だと思います。ただ、コンパイル時にサイズが不定なので、for文のループが何回繰り返されるかは実行するまでわかりません。そのため、この関数はベクトル化がしにくくなっています。

GMPをビルドするとzero.oというオブジェクトファイルが生成されるので、objdumpを使ってアセンブリを見てみます。

//list[][zero.oのアセンブリ][]{
0000000000000000 <__gmpn_zero>:
   0:   48 89 f0                mov    rax,rsi
   3:   48 8d 14 f5 00 00 00    lea    rdx,[rsi*8+0x0]
   a:   00
   b:   48 f7 d8                neg    rax
   e:   48 85 f6                test   rsi,rsi
  11:   74 1a                   je     2d <__gmpn_zero+0x2d>
  13:   48 01 fa                add    rdx,rdi
  16:   66 2e 0f 1f 84 00 00    nop    WORD PTR cs:[rax+rax*1+0x0]
  1d:   00 00 00
  20:   48 c7 04 c2 00 00 00    mov    QWORD PTR [rdx+rax*8],0x0
  27:   00
  28:   48 ff c0                inc    rax
  2b:   75 f3                   jne    20 <__gmpn_zero+0x20>
  2d:   c3                      ret
//}

//noindent
zero.cのコードをそのままアセンブルに落とし込んだ内容になっていると思います。

ちなみにサイズが固定長のときのmemsetはコンパイルすると次のようになります。実験のコードをコンパイルしたものです。
//list[][固定長memset][]{
0000000000000000 <_Z3fffPm>:
   0:	66 0f ef c0          	pxor   xmm0,xmm0
   4:	0f 11 07             	movups XMMWORD PTR [rdi],xmm0
   7:	0f 11 47 10          	movups XMMWORD PTR [rdi+0x10],xmm0
   b:	c3                   	ret
//}

//noindent
128ビットのレジスタであるxmm0レジスタを使っています。
SIMD命令が使用できるという条件付きですが、ゼロ埋めにはmemsetを使うのが良いといえます。


==== GMP on M1 MBP
ver6.2.1で、ArmベースのアーキテクチャであるM1チップのサポートが始まりました。
しかし、実は問題を抱えています。arm64向けに実装されたコードでx18レジスタを使うのですが、M1において、x18レジスタはリザーブされています。基本的に触っちゃいけないわけですね。テストも問題なく通るらしく、実行中にプロセスが終了するようなことはまだ報告されていません（2020.02.14現在）。どういうコードを書けばプロセスが落ちるのかを研究するのも面白そうですね。
