# これは何？

2013 年初頭に技術評論社から刊行予定の「OpenFlow 実践入門」の原稿データです。以下の手順で自分で PDF や epub、mobi を作って読むことができます。
ただし生成できるデータは「素人仕事」なので、プロが組版したきれいな本が欲しい人はぜひ本屋さんで買ってください。


# セットアップ

以下は Ubuntu の手順です。Mac でも同様の手順でできます。

 1. Ruby 環境と rake をインストール
 2. review (組版ソフト) の最新版をインストール: https://github.com/kmuto/review
 3. TeX Live 2012 をインストール: http://oku.edu.mie-u.ac.jp/~okumura/texwiki/?Linux%2FUbuntu#hf3fd9de


# PDF や epub、mobi を作ろう

rake コマンド一発でできます。

 * "rake pdf" で trema.pdf ができます。
 * "rake epub" で trema.epub ができます。
 * "rake mobi" で trema.mobi ができます (要 kindleGen)。


# 「作ってみた」レポート

 * Ubuntu での作りかた: 「Trema 本を読もう! (@stereocat さん)」 http://d.hatena.ne.jp/stereocat/20120619
 * Mac での作りかた: 「OpenFlow 実践入門　〜Tremaの本〜 (@q_megumi_p さん)」 http://katsumegu.wordpress.com/2012/08/08/openflow-%E5%AE%9F%E8%B7%B5%E5%85%A5%E9%96%80%E3%80%80%E3%80%9Ctrema%E3%81%AE%E6%9C%AC%E3%80%9C/
 * Kindle で読む: 「Trema本をKindleで読む (笹生健 さん)」http://www.takeru.org/2012/10/tremakindle.html


# 執筆に協力いただける方への情報

あなたも謝辞に名前を載せませんか!

 * まちがいやコメント等ありましたら、チケットを切ってください: https://github.com/yasuhito/trema-book/issues
 * 原稿のマークアップ文法はこちら: https://github.com/kmuto/review/blob/master/doc/format.rdoc
 * 貢献いただいた方リスト: https://github.com/yasuhito/trema-book/blob/master/acknowledgements.txt


# License

This book is released under the GNU General Public License version 3.0:

 * http://www.gnu.org/licenses/gpl-3.0.html
