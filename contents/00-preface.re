= まえがき

本書を手に取っていただき、ありがとうございます。著者のykm11です。ゆうけむと読みます。ネットではトリと呼ばれることが多いです。

本書は、GMP(The GNU Multiple Precision Arithmetic Library)@<fn>{fn-gmp}の実装を解説した本です。
GMPを直訳するとGNUの多倍長精度の算術ライブラリ、つまり算術用の便利なライブラリです。便利なだけではなく、高速に動作するように実装されています。めちゃめちゃ速いです。速いと嬉しいですからね。

#@#この本を読むことでGMPの気持ちがわかるようになり、また多倍長精度の演算をしたくなったときに筆者のアイコンがチラつきます。

GMPを使用したソフトウェアの例としては、筆者がサイボウズ・ラボユースで開発していた楕円曲線暗号@<fn>{ykm11ec}があります。

//footnote[suchikeisan][多倍長精度数値計算 GNU MP, MPFR, QDによるプログラミング https://www.morikita.co.jp/books/mid/085491]
//footnote[fn-gmp][https://gmplib.org/]
//footnote[ykm11ec][https://github.com/ykm11/lab-youth/tree/master/ellipticCurve]

====[notoc] 本書の目的
GMPや他の数値計算用のライブラリを使った数値計算を扱う書籍は存在する@<fn>{suchikeisan}のですが、単にライブラリを用いて計算するだけに留まっていて、ライブラリがどのように実装されているかと深いところまで突っ込んだ書籍はまだありません。
そこで、本書はGMPの実装を理解することを目的に執筆されました。GMPがどのように実装されているかを理解することで、多倍長精度の演算を自分で実装する際の助けになります。また、GMPのおかしな挙動に遭遇したときにどうデバッグすればいいのか、どの変数・メンバを見ればいいのかはGMPの知識で解決できます。そういうわけで、GMPユーザが実装を理解しておくというのは大事なんですね。

GMPは巨大なプロジェクトですので、すべての実装を理解するには膨大な時間が必要になります。そのため、本書では筆者が適当に抜粋した関数や機能を解説します。
ただ、基本的には、筆者がGMPを使っているときに「ここの実装どうなってるんだろうか」と気になったものを抜粋しています。本当に適当に選んだわけではないと弁明しておきます。


====[notoc] 本書の対象読者

本書では次のような人を対象としています。

 * 多倍長精度の整数, 有理数, 浮動小数点数をC/C++で扱いたい人
 * GMPの実装に興味がある人（相当な変人かも）

本書は基本的にGMPの実装解説を取り扱いますが、はじめに使用例を示します。とりあえず使ってみたい！という人は使用例を参考にしてみてください（使うだけならネットに情報がありそうな気もしますが・・）。

====[notoc] 前提とする知識

本書を読むにあたり、次のような知識が必要となります。

 * C/C++言語の基礎知識
 * アセンブリの基礎知識

GMPのほとんどがC言語で実装されています。C++はそれほど出てきませんので、C言語の知識があれば読み進められると思います。C++に関してはclassという概念と演算子オーバーロードさえ知っていれば問題ありません。
また、アセンブリの基礎知識と書いていますが、高度な内容は扱いません（高度な内容ってなんだ）。そもそも筆者がそこまでアセンブリに詳しくないので、読めないよ！って人でも大丈夫です。ガンガンいきましょう。

====[notoc] 本書の配布形式
電子版のみで、無料公開としました（そもそもこの本の完成の目処が立っていないので印刷とかできません）。どうしてもぼくに投げ銭したいという方はAmazonの欲しい物リストから何か買ってくれると嬉しいです。

 * Amazon wishlist: https://www.amazon.jp/hz/wishlist/ls/23CZA18QR1CQ0


====[notoc] 問い合わせ先

本書に関する質問やお問い合わせは、筆者のTwitterまでお願いします。

 * URL: @<href>{https://twitter.com/_ykm11_}
#@# * Mail: ushiromiya3@gmail.com

====[notoc] 謝辞
感謝するぜ、読者と出会えたこれまでの全てに。

#@#本書はXXXX氏とXXXX氏にレビューしていただきました。
#@#この場を借りて感謝します。ありがとうございました。
