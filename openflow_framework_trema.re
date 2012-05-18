= OpenFlow フレームワーク Trema

第 2 部では、OpenFlow を使ってネットワークをプログラミングする方法を紹
介します。職場や自宅のような中小規模ネットワークでもすぐに試せる実用的
なコードを通じて、「OpenFlow って具体的に何に使えるの？」というよくある
疑問に答えていきます。実例を混じえつつ OpenFlow やネットワークの基礎か
ら説明しますので、ネットワークの専門家はもちろん普通のプログラマもすん
なり理解できるはずです。

まずは、OpenFlow プログラミングのためのフレームワーク「Trema (トレマ)」
を紹介しましょう。

== Trema とは

Trema は、OpenFlow コントローラを開発するための Ruby および C 用のプロ
グラミングフレームワークです。Trema は GitHub 上で開発されており、
GPLv2 ライセンスのフリーソフトウェアとして公開されています。公開は
2011 年の 4 月と非常に新しいソフトウェアですが、その使いやすさから国内
外の大学や企業および研究機関などですでに採用されています。

Trema の情報は次のサイトから入手できます。

 * Trema ホームページ: http://trema.github.com/trema/
 * GitHub のページ: https://github.com/trema/
 * メーリングリスト: https://groups.google.com/group/trema-dev
 * Twitter アカウント: @trema_news

=== Trema フレームワーク

Trema を使えば、OpenFlow スイッチを持っていなくてもノート PC 1 台で
OpenFlow コントローラの開発とテストができます。これが Trema の「フレー
ムワーク」たるゆえんで、最も強力な機能です。第 2 部では、実際に Trema
を使っていろいろと実験しながら OpenFlow コントローラを作っていきます。

=== セットアップ

Trema は Linux 上で動作し、Ubuntu 10.04 以降および Debian GNU/Linux
6.0 の 32 ビットおよび 64 ビット版での動作が保証されています。テストは
されていませんが、その他の Linux ディストリビューションでも基本的には動
作するはずです。

Trema の提供する trema コマンドの実行には root 権限が必要です。まずは、
sudo を使って root 権限でコマンドを実行できるかどうか、sudo の設定ファ
イルを確認してください。

//cmd{
% sudo visudo
//}

sudo ができることを確認したら、Trema が必要とする gcc などの外部ソフト
ウェアを次のようにインストールします。

//cmd{
% sudo apt-get install git gcc make ruby ruby-dev irb libpcap-dev libsqlite3-dev
//}

次に Trema 本体をダウンロードします。Trema は GitHub 上で公開されており、
git を使って最新版が取得できます。

//cmd{
% git clone git://github.com/trema/trema.git
//}

Trema のセットアップには、「make install」のようなシステム全体へインス
トールする手順は不要です。ビルドするだけで使い始めることができます。ビ
ルドは次のコマンドを実行するだけです。

//cmd{
% ./trema/build.rb
//}

それでは早速、入門の定番 Hello, Trema! コントローラを Ruby で書いてみま
しょう。なお、第 2 部では Trema の Ruby ライブラリを使ったプログラミン
グを取り上げます。C ライブラリを使ったプログラミングの例については、
Trema の src/examples/ ディレクトリ以下を参照してください。第 2 部で使っ
た Ruby コードに加えて、同じ内容の C コードを見つけることができます。

== Hello, Trema!

trema ディレクトリの中に hello-trema.rb というファイルを作成し、エディ
タでリスト 1 のコードを入力してください。

//list[hello-trema.rb][Hello Trema! コントローラのソースコード (hello-trema.rb)]{
  class HelloController < Controller
    def start
      puts "Hello, Trema!"
    end
  end
//}

どうでしょうか? とてもシンプルに書けますね。

=== 実行してみよう

それでは早速実行してみましょう! 作成したコントローラは trema run コマン
ドで実行できます。この世界一短いOpenFlowコントローラ(？)は画面に
「Hello, Trema!」と出力します。

//cmd{
% cd trema
% ./trema run ./hello-trema.rb
Hello, Trema!  # Ctrl+c で終了
//}

いかがでしょうか？ Tremaを使うと、とても簡単にコントローラを書いて実行
できることがわかると思います。えっ？ これがいったいスイッチの何を制御し
たかって？ 確かにこのコントローラはほとんど何もしてくれませんが、Trema
でコントローラを書くのに必要な知識がひととおり含まれています。スイッチ
をつなげるのはちょっと辛抱して、まずはソースコードを見ていきましょう。

=== コントローラを定義する

Ruby で書く場合、すべてのコントローラは Controller クラスを継承します
(リスト 1-1 の協強調部分)。

//list[クラス定義][コントローラのクラスを定義する]{
  class HelloController < Controller
    def start
      puts "Hello, Trema!"
    end
  end
//}

このように Controller クラスを継承することで、コントローラに必要な基本
機能が HelloController クラスにこっそりと追加されます。

=== ハンドラを追加する

Trema はイベントドリブンなプログラミングモデルを採用しています。つまり、
OpenFlow メッセージの到着など各種イベントに対応するハンドラを定義してお
くと、イベントの発生時に対応するハンドラが呼び出されます。たとえば
start メソッド (リスト1-2 の強調部分) を定義しておくと、コントローラの
起動時にこれが自動的に呼ばれます。

//list[ハンドラ定義][起動時に呼ばれるハンドラを定義する]{
  class HelloController < Controller
    def start
      puts "Hello, Trema!"
    end
  end
//}  
  
== まとめ/参考文献

さて、これで Trema の基本はおしまいです。この章ではすべてのコントロー
ラのテンプレートとなるHello, Trema! コントローラを書きました。学んだこ
とは次の 2 つです。

 * コントローラは Ruby のクラスとして定義し、Controller クラスを継承することで必要なメソッドが取り込まれる。
 * コントローラクラスに各種イベントに対応するハンドラを定義することでロジックを実装できる。たとえば、起動イベントに対応するハンドラは start である。

次の章では、いよいよ実用的な OpenFlow コントローラを書き実際にスイッチ
をつないでみます。
