# Trema本 [![Gitter](https://badges.gitter.im/Join%20Chat.svg)][gitter]

[Trema][trema]を使ったOpenFlowプログラミングを解説するフリー書籍です. 対応スイッチの多いOpenFlowバージョン1.3.xを対象にしています. プログラミング言語はRubyバージョン2.0です.

* ブラウザで読む: https://github.com/yasuhito/trema-book
* PDFで読む: (準備中)
* Gitterチャットに参加する: https://gitter.im/yasuhito/trema-book
* Twitterのハッシュタグ: [#Trema本](https://twitter.com/hashtag/Trema%E6%9C%AC)


## 書籍版

OpenFlow1.0に対応した前バージョンのTrema本は、書籍として各種書店で入手可能です.

![カバー](https://raw.github.com/yasuhito/trema-book/develop/images/cover.png "本のカバー")

印刷版:

- [Amazonで購入](http://www.amazon.co.jp/dp/4774154652/)
- [ヨドバシ.comで購入](http://www.yodobashi.com/ec/product/100000009001711327/index.html)
- [7netショッピングで購入](http://www.7netshopping.jp/books/detail/-/isbn/9784774154657)
- [hontoで購入](http://honto.jp/netstore/pd-book_25440213.html)
- [紀伊國屋書店で購入](https://www.kinokuniya.co.jp/f/dsg-01-9784774154657)
- [楽天ブックスで購入](http://books.rakuten.co.jp/rb/12122925/)

電子版:

- [Amazonで購入](http://www.amazon.co.jp/o/ASIN/B00CP2SFNA)
- [Google Booksで購入](https://books.google.co.jp/books/about/%E3%82%AF%E3%83%A9%E3%82%A6%E3%83%89%E6%99%82%E4%BB%A3%E3%81%AE%E3%83%8D%E3%83%83%E3%83%88%E3%83%AF%E3%83%BC%E3%82%AF.html?id=Dw0-tiAvGTsC&redir_esc=y)
- [Gihyo Digital Publishingで購入](https://gihyo.jp/dp/ebook/2013/978-4-7741-5516-6)
- [楽天ブックスで購入](http://books.rakuten.co.jp/rk/63308d148b9737949f026c0e5bb1f694/)


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
