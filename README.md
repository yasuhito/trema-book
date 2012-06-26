# これは何？

10 月中旬に技術評論社から刊行予定の「OpenFlow 実践入門」の原稿データです。以下の手順で自分で PDF や epub を作って読むことができます。
ただし生成できるデータは「素人仕事」なので、プロが組版したきれいな本が欲しい人はぜひ本屋さんで買ってください。


# セットアップ

以下は Ubuntu の手順です。Mac でも同様の手順でできます。

 * Ruby 環境と rake をインストール
 * review (組版ソフト) の最新版をインストール: https://github.com/kmuto/review
 * texlive をインストール: sudo apt-get install texlive-full
 * ptexlive をインストール: このページ http://www1.pm.tokushima-u.ac.jp/~kohda/tex/ptexlive.html から ptexlive-base と ptexlive-extra を dpkg -i でインストール 


# PDF や epub を作る

ファイルの生成はすべて rake コマンド一発でできます。

 * "rake pdf" で trema.pdf ができます。
 * "rake epub" で trema.epub ができます。


# 執筆に協力いただける方への情報

あなたも謝辞に名前を載せませんか!

 * まちがいやコメント等ありましたら、チケットを切ってください: https://github.com/yasuhito/trema-book/issues
 * 原稿のマークアップ文法はこちら: https://github.com/kmuto/review/blob/master/doc/format.rdoc
 * 貢献いただいた方リスト: https://github.com/yasuhito/trema-book/blob/master/acknowledgements.txt


# License

This book is released under the GNU General Public License version 3.0:

 * http://www.gnu.org/licenses/gpl-3.0.html