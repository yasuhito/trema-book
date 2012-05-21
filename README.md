# セットアップ

Ubuntu を使う場合。

 * review (組版ソフト) をインストール: https://github.com/kmuto/review
 * texlive をインストール: sudo apt-get install texlive-full
 * ptexlive をインストール: このページ http://www1.pm.tokushima-u.ac.jp/~kohda/tex/ptexlive.html から ptexlive-base と ptexlive-extra を dpkg -i でインストール 

あとは rake が必要です。


# PDF を作る

 * rake pdf で trema.pdf ができる。
 * rake clean で消える。


# 書き方

文法はここにあります https://github.com/kmuto/review/blob/master/doc/format.rdoc
