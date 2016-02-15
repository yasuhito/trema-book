# Trema本 [![Gitter](https://badges.gitter.im/Join%20Chat.svg)][gitter]

[Trema][trema]を使ったOpenFlowプログラミングを解説するフリー書籍です. 対応スイッチの多いOpenFlowバージョン1.3.xを対象にしています. プログラミング言語はRubyバージョン2.0以降です.

* ブラウザで読む: https://github.com/yasuhito/trema-book
* PDFで読む: (準備中)
* Gitterチャットに参加する: https://gitter.im/yasuhito/trema-book
* Twitterのハッシュタグ: [#Trema本](https://twitter.com/hashtag/Trema%E6%9C%AC)


## 書籍版

各種書店で入手可能です。

![カバー](https://raw.github.com/yasuhito/trema-book/develop/images/cover.png "本のカバー")

- [Amazonで購入](http://www.amazon.co.jp/o/ASIN/4774179833/)
- [ヨドバシ.comで購入](http://www.yodobashi.com/-増補改訂版-クラウド時代のネットワーク技術-OpenFlow実践入門-Software-Design-plus-単行本/pd/100000009002526970/)
- [7netショッピングで購入](http://7net.omni7.jp/detail_isbn/978-4-7741-7983-4)
- [hontoで購入](http://honto.jp/netstore/pd-book_27703803.html)


## もくじ

```
■1章：OpenFlowの仕組み
　1.1　ソフトウェアで楽をする
　1.2　ネットワークもソフトウェア制御
　1.3　OpenFlowの動作モデル
　1.4　OpenFlowのうれしさ
　1.5　OpenFlowで気をつけること
　1.6　まとめ

■2章：OpenFlowの仕様
　2.1　OpenFlowの標準仕様
　2.2　スイッチとコントローラ間のやり取り
　2.3　フローエントリの中身
　2.4　マルチプルテーブル
　2.5　まとめ

■3章：Hello, Trema!
　3.1　作ってわかるOpenFlow
　3.2　Tremaとは
　3.3　Trema実行環境のセットアップ
　3.4　Hello, Trema!
　3.5　tremaコマンド
　3.6　即席Ruby入門
　3.7　まとめ

■4章：スイッチ監視ツール
　4.1　ネットワークを集中管理しよう
　4.2　インストール
　4.3　実行してみよう
　4.4　ソースコード解説
　4.5　まとめ

■5章：マイクロベンチマークCbench
　5.1　Cbenchベンチマークとは
　5.2　インストール
　5.3　実行してみよう
　5.4　ソースコード解説
　5.5　マルチスレッド化する
　5.6　無理やり高速化する
　5.7　まとめ

■6章：インテリジェントなパッチパネル
　6.1　便利なインテリジェント・パッチパネル
　6.2　OpenFlow版インテリジェント・パッチパネル
　6.3　実行してみよう
　6.4　ソースコード解説
　6.5　まとめ
　
■7章：すべての基本、ラーニングスイッチ
　7.1　ラーニングスイッチとは何か
　7.2　イーサネットスイッチの仕組み
　7.3　OpenFlow版イーサネットスイッチ（ラーニングスイッチ）の仕組み
　7.4　実行してみよう
　7.5　ラーニングスイッチのソースコード
　7.6　まとめ

■8章：OpenFlow 1.3版ラーニングスイッチ
　8.1　OpenFlow 1.0版ラーニングスイッチの問題点
　8.2　マルチプルテーブル
　8.3　OpenFlow 1.3でのPacket In
　8.4　OpenFlow 1.3版ラーニングスイッチの仕組み
　8.5　ソースコード解説
　8.6　まとめ

■9章：Tremaでテスト駆動開発
　9.1　仕様書としてのテストコード
　9.2　リピータハブの動き
　9.3　どこまでテストするか？
　9.4　テストに使うツール
　9.5　パケット受信をテストする
　9.6　リファクタリング
　9.7　まとめ

■10章：生活ネットワークのOpenFlowへの移行
　10.1　どんどん規模を広げていこう
　10.2　ケガしないためのヘルメット
　10.3　私たちの失敗談
　10.4　OpenFlowへの移行パターン
　10.5　逆流防止フィルタ
　10.6　まとめ

■11章：ネットワーク機器を作ろう：ファイアウォール
　11.1　透過型ファイアウォール
　11.2　BlockRFC1918コントローラ
　11.3　BlockRFC1918のソースコード
　11.4　PassDelegatedコントローラ
　11.5　PassDelegatedのソースコード
　11.6　まとめ

■12章：ネットワーク機器を作ろう：ルータ（前編）
　12.1　ルータとスイッチの違い
　12.2　イーサネットだけならルータは不要？
　12.3　ルータの動作
　12.4　ソースコード解説
　12.5　まとめ

■13章：ネットワーク機器を作ろう：ルータ（後編）
　13.1　宛先ホストをまとめる
　13.2　ネットワーク宛てのエントリをまとめる
　13.3　RoutingTableのソースコード
　13.4　実行してみよう
　13.5　まとめ

■14章：ネットワーク機器を作ろう：ルータ（マルチプルテーブル編）
　14.1　マルチプルテーブル版ルータのテーブル構成
　14.2　マルチプルテーブル版ルータの動作例
　14.3　実行してみよう
　14.4　まとめ

■15章：ネットワークトポロジの検出
　15.1　美しい大規模ネットワーク
　15.2　トポロジ検出の仕組み
　15.3　実行してみよう
　15.4　トポロジコントローラのソースコード
　15.5　まとめ

■16章：大量のスイッチを制御
　16.1　複数のスイッチを制御する
　16.2　最短パスを計算する
　16.3　実行してみよう
　16.4　ルーティングスイッチのソースコード
　16.5　OpenFlowを使う利点
　16.6　まとめ

■17章：仮想ネットワークの実現
　17.1　ネットワークをスライスに分ける
　17.2　スライスの実現方法
　17.3　インストール
　17.4　REST APIを使う
　17.5　REST API一覧
　17.6　スライス機能付きスイッチの実装
　17.7　スライス機能付きスイッチのソースコード
　17.8　まとめ

■18章：OpenVNetで本格的な仮想ネットワーク
　18.1　OpenVNetとは
　18.2　エッジ仮想化の利点
　18.3　OpenVNetの全体アーキテクチャ
　18.4　OpenVNetの主な機能
　18.5　使ってみる
　18.6　OpenVNetを応用した実用例
　18.7　まとめ
```


## ソースから本をビルドする
[![Build Status](http://img.shields.io/travis/yasuhito/trema-book/develop.svg?style=flat)][travis]
[![Dependency Status](http://img.shields.io/gemnasium/yasuhito/trema-book.svg?style=flat)][gemnasium]

```
git clone git@github.com:yasuhito/trema-book.git
cd trema-book
bundle install
bundle exec rake
```


## 執筆に参加する

誰でもいろいろな形でTrema本の執筆に参加できます.

もし誤字脱字を見つけたら, [チャット][gitter]で指摘してください. GitHubに慣れている場合はイシューを切ってもらうか, ソースコードを直接修正してPull Requestを出してもらえると助かります.

もちろん, 内容についてのコメントや追加内容も歓迎します.

原稿の書式は[Asciidoc](http://asciidoc.org/)です. 詳しい書き方についてはAsciidocのコンパイラである[Asciidoctorのユーザマニュアル](http://asciidoctor.org/docs/user-manual/)を参照してください.

貢献していただいた方は書籍版の「謝辞」にもれなくお名前が載ります.

![謝辞](https://raw.github.com/yasuhito/trema-book/develop/images/acknowledgements.jpg "謝辞")


## ライセンス

This book is released under the GNU General Public License version 3.0:

 * http://www.gnu.org/licenses/gpl-3.0.html


[trema]: https://github.com/trema/trema
[travis]: http://travis-ci.org/yasuhito/trema-book
[gitter]: https://gitter.im/yasuhito/trema-book
[gemnasium]: https://gemnasium.com/yasuhito/trema-book
