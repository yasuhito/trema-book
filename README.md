# これは何？

![カバー](https://raw.github.com/yasuhito/trema-book/master/cover.png "本のカバー")

http://www.amazon.co.jp/dp/4774154652/ref=cm_sw_r_tw_dp_0SlTqb1NGYDN9

 * 書名: クラウド時代のネットワーク技術 OpenFlow実践入門 (SoftwareDesign plusシリーズ)
 * 著者: 高宮安仁、鈴木一哉
 * 体裁: A5版、328ページ
 * 発売日: 2012/12/24 (予定)
 * 価格: 3,360円 (本体3,200円)
 * 発行: 技術評論社

の原稿データです。以下の手順で自分で PDF や epub、mobi を作って読むことができます。
ただし生成できるデータは「素人仕事」なので、プロが編集・組版したきれいな本が欲しい人はぜひ Amazon や街の本屋さんで買ってください。


# セットアップ

以下は Ubuntu の手順です。Mac でも同様の手順でできます。

 1. Ruby 環境と rake をインストール
 2. review (組版ソフト) の最新版をインストール: https://github.com/kmuto/review
 3. TeX Live 2012 をインストール


# PDF や epub、mobi を作ろう

rake コマンド一発でできます。

 * "rake pdf" で trema.pdf ができます。
 * "rake epub" で trema.epub ができます。
 * "rake mobi" で trema.mobi ができます (要 kindleGen)。


# 「作ってみた」レポート

 * Ubuntu での作りかた: 「Trema 本を読もう! (@stereocat さん)」 http://d.hatena.ne.jp/stereocat/20120619
 * Mac での作りかた: 「OpenFlow 実践入門　〜Tremaの本〜 (@q_megumi_p さん)」 http://katsumegu.wordpress.com/2012/08/08/openflow-%E5%AE%9F%E8%B7%B5%E5%85%A5%E9%96%80%E3%80%80%E3%80%9Ctrema%E3%81%AE%E6%9C%AC%E3%80%9C/
 * Kindle で読む: 「Trema本をKindleで読む (笹生健 さん)」http://www.takeru.org/2012/10/tremakindle.html


# License

This book is released under the GNU General Public License version 3.0:

 * http://www.gnu.org/licenses/gpl-3.0.html
