= OpenFlow フレームワーク Trema

#@warn(気の利いたリード文をここに)

第 II 部では、OpenFlow を使ってネットワークをプログラミングする方法を紹
介します。職場や自宅のような中小規模ネットワークでもすぐに試せる実用的
なコードを通じて、「OpenFlow って具体的に何に使えるの？」というよくある
疑問に答えていきます。実例を混じえつつ OpenFlow やネットワークの基礎か
ら説明しますので、ネットワークの専門家はもちろん普通のプログラマもすん
なり理解できるはずです。

まずは、OpenFlow プログラミングのためのフレームワーク @<ruby>{Trema,トレマ} を紹介しましょう。

== Trema とは

Trema は OpenFlow コントローラを開発するための Ruby 用のプログラミング
フレームワークです。Trema は github 上でオープンに開発しており、GPL2 ラ
イセンスのフリーソフトウェアとして公開されています。公開は 2011 年の 4
月と比較的新しいソフトウェアですが、その使いやすさから国内外の大学や企
業および研究機関などですでに数多く採用されています。

Trema の情報は次のサイトから入手できます。

 * Trema ホームページ: @<href>{http://trema.github.com/trema/}
 * github のプロジェクトページ: @<href>{http://github.com/trema/}
 * メーリングリスト: @<href>{http://groups.google.com/group/trema-dev}
 * Twitter アカウント: @<href>{http://twitter.com/trema_news}

Trema を使えば、OpenFlow スイッチを持っていなくても Linux のインストー
ルされたノート PC 1 台でOpenFlow コントローラの開発とテストができます。
これが Trema の「フレームワーク」たるゆえんで、最も強力な機能です。第
II 部では、実際に Trema を使っていろいろと実験しながら OpenFlow コント
ローラを作っていきます。それではまず Trema をセットアップしましょう。

===[column] @<ruby>{友太郎,ゆうたろう}の質問: Trema の由来ってなに？

#@warn(友太郎のアイコン)

Q. 「こんにちは! 僕は最近 OpenFlow に興味を持ったプログラマ，友太郎です。
     Trema っておもしろい名前だけど、何か由来はあるんですか？」
     
A. 作者の一人が大好きな、大阪の「とれまレコード
   (@<tt>{http://www.fumiyatanaka.com/})」という小さいレコードレーベル
   の名前から取りました。この「とれまレコード」の "とれま" の由来ですが、
   日本がバブルの頃に道路標識の「とまれ」を「とれま」と誤植したケースが
   頻発したことが由来です。このありえない道路標識の原因は、バブル当時に
   急増した日本語の苦手な外国人労働者達が間違えて「とれま」と道路に描い
   てしまったことにあります。
   
   ちなみに、Trema の公式ロゴマークはこんな画像です
   (@<img>{trema_logo})。これは Twitter の Trema 公式アカウント
   (@<tt>{@trema_news}) のアイコンとしても使われて います。

//image[trema_logo][Trema のロゴ][scale=0.7]

   もちろんこんなに大胆な道路標識は日本中どこを探してもありません。この
   本の編集者が画像編集ソフトで作ったものを気に入った作者達が、そのまま
   公式ロゴとして使っています。

===[/column]

== Trema のセットアップ

Trema は Linux 上で動作し、次のディストリビューションとバージョンでの動
作を保証しています。

 * Ubuntu 12.04, 11.10, 11.04, 10.10, 10.04 (i386/amd64, デスクトップエディション)
 * Debian GNU/Linux 6.0 (i386/amd64)

なお保証はしませんが、その他の RedHat などの Linux ディストリビューショ
ンでも動作するはずです。

Trema の提供する trema コマンドの実行には root 権限が必要です。まずは、
@<tt>{sudo} を使って root 権限でコマンドを実行できるかどうか、
@<tt>{sudo} の設定ファイルを確認してください。

//cmd{
% sudo visudo
//}

@<tt>{sudo} ができることを確認したら、Trema のインストールに必要な
RubyGems と、コンパイルに必要な @<tt>{gcc} などの外部ソフトウェアを次の
ようにインストールします。

//cmd{
% sudo apt-get install gcc make ruby rubygems ruby-dev irb file libpcap-dev \
  libsqlite3-dev
//}

=== 簡単にインストールする場合

次に Trema 本体をインストールします。Trema は RubyGems の @<tt>{gem} コ
マンドを使って次のようにコマンド一発で簡単にインストールできます。

//cmd{
% gem install trema
//}

=== ソースコードから最新版をインストールする場合

最新版をインストールしたい人は、github から自分でソースコードをダウンロー
ドしてビルドすることもできます。まず、次のように @<tt>{git} を使って最
新のソースコードを取得してください。

//cmd{
% git clone git://github.com/trema/trema.git
//}

Trema のインストールには、@<tt>{make install} のようなシステム全体への
インストール手順は不要です。次のコマンドを実行してビルドするだけで使い
始めることができます。

//cmd{
% ./trema/build.rb
//}

さあ、これで Trema のコントローラ開発環境が整いました。それでは早速、入
門の定番 Hello, Trema! コントローラを Ruby で書いてみましょう。

== Hello, Trema!

適当なディレクトリに @<tt>{hello-trema.rb} というファイルを作成し、エディ
タで @<list>{hello-trema.rb} のコードを入力してください。"@<tt>{.rb}"
は Ruby プログラムの標準的な拡張子です。なお Ruby の文法は後で説明しま
すの で、今のところは気にせずそのまま入力してください。

//list[hello-trema.rb][Hello Trema! コントローラのソースコード (@<tt>{hello-trema.rb})]{
  class HelloTrema < Controller
    def start
      puts "Hello, Trema!"
    end
  end
//}

とてもシンプルに見えますね。それでは細かい文法は脇に置いておいて「習う
より慣れろ」でさっそく実行してみましょう。

=== 実行してみよう (@<tt>{trema run})

作成したコントローラは @<tt>{trema run} コマンドですぐに実行できます。
Ruby はインタプリタ言語なので、コンパイルの必要はありません。ターミナル
で次のように入力すると、この世界一短い OpenFlow コントローラはフォアグ
ラウンドプロセスとして起動し、画面に@<tt>{Hello, Trema!} と出力します。
起動したコントローラは Ctrl + C で停止できます。

//cmd{
% trema run ./hello-trema.rb
Hello, Trema!  # Ctrl+c で終了
%
//}

いかがでしょうか？ @<tt>{trema} コマンドを使うと、とても簡単にコントロー
ラを書いてすぐに実行できることがわかると思います。@<tt>{trema} コマンド
には他にもいくつかの機能がありますのでここで簡単に紹介しましょう。

== @<tt>{trema コマンド}

@<tt>{trema} コマンドは Trema の唯一のコマンドであり、コントローラの起
動や仮想ネットワークの構築、テストなど様々な用途に使います。たとえば先
ほどの "Hello Trema!" で見たように、@<tt>{trema run} はコントローラを起
動するためのコマンドです。起動したコントローラは実ネットワークと接続し、
OpenFlow スイッチを制御します。また、@<tt>{trema run} コマンドはオプショ
ンを指定すると仮想ネットワークを作ることもでき、コントローラをこの仮想
ネットワークの中で実行できます (@<img>{trema_overview})。

//image[trema_overview][@<tt>{trema} コマンドの概要]

@<tt>{trema} コマンドは @<tt>{git} や @<tt>{svn} コマンドと似たコマンド
体系を持っており、@<tt>{trema} に続けて @<tt>{run} などのサブコマンドを
指定することで様々な機能を呼び出します。こうしたコマンド体系を一般に "
コマンドスイート" と呼びます。一般的なコマンドスイートと同じく、サブコ
マンドの一覧は @<tt>{trema help} で表示できます。また、サブコマンド自体
のヘルプは @<tt>{trema help [サブコマンド]} で表示できます。

では @<tt>{trema} のサブコマンド一覧をざっと見ておきましょう。

: @<tt>{trema run}
  コントローラをフォアグラウンドで実行します。@<tt>{--daemonize (-d)}
  オプションを付けるとコントローラをバックグラウンド (デーモンモード)
  として実行できます。

: @<tt>{trema killall}
  バックグラウンドで起動している Trema プロセス全体を停止します。
  
: @<tt>{trema version}
  Trema のバージョンを表示します。

: @<tt>{trema ruby}
  Trema の Ruby API を表示します。

: @<tt>{trema kill [仮想スイッチまたはリンク]}
  指定した仮想ネットワークのスイッチやリンクを停止します。
  @<chap>{switch_monitoring_tool} で紹介。

: @<tt>{trema up [仮想スイッチまたはリンク]}
  指定した仮想ネットワークのスイッチやリンクを再起動します。
  @<chap>{switch_monitoring_tool} で紹介。

: @<tt>{trema send_packets [送信オプション]}
  仮想ネットワークでテストパケットを送信します。
  @<chap>{learning_switch} で紹介。
  
: @<tt>{trema show_stats [仮想ホスト名]}
  仮想ネットワークの仮想ホストで送受信したパケットの統計情報を表示しま
  す。@<chap>{learning_switch} で紹介。

: @<tt>{trema reset_stats}
  仮想ネットワークの仮想ホストで送受信したパケットの統計情報をリセット
  します。@<chap>{learning_switch} で紹介。

: @<tt>{trema dump_flows [仮想スイッチ名]}
  仮想ネットワークの仮想スイッチのフローテーブルを表示します。
  @<chap>{learning_switch} で紹介。

いくつかのサブコマンドはまだ使っていませんが、続く章で使いかたを説明し
ます。ひとまずこの章ではコントローラを起動する @<tt>{trema run} と、
Ruby API を表示する @<tt>{trema ruby} コマンドを覚えておいてください。
@<tt>{trema ruby} を実行するとデフォルトブラウザで Trema Ruby API のペー
ジが開きます。プログラミング中のリファレンスをコマンド一発で開けるので
大変便利です (@<img>{trema_ruby})。

#@warn(API のページが YARD のせいで壊れているので、直してからスクリーンショット撮り直し)
//image[trema_ruby][@<tt>{trema ruby} コマンドで Ruby API をブラウザで表示]

では、気になっていた Ruby の文法にそろそろ進みましょう。第 II 部では今
後もたくさん Ruby を使いますが、その都度必要な文法を説明しますので心配
はいりません。しっかりついてきてください。

== 即席 Ruby 入門

Ruby を習得する一番の近道は、外国語の習得でもそうですが登場する品詞の種
類を押さえておくことです。Ruby の品詞には、かならず見分けるための手がか
りとなる視覚的なヒントがあります。記号が使われていたり、最初の文字が大
文字になっていたりするので、断片的なコードを見てもすぐにどんな品詞かわ
かります。品詞がわかれば、その Ruby コードがどんな構造かわかります。

それぞれの品詞の見かけに注意してください。それぞれの品詞について簡単に
説明しますが、別に説明が全部理解できなくとも構いません。この節の終わる
頃には "Hello, Trema!" プログラムのあらゆる部分が識別できるようになって
いるはずです。

=== キーワード

Ruby にはたくさんの組み込みの語があり、それぞれに意味が与えられています。
これらの語を変数として使ったり、自分の目的に合わせて意味を変えたりはで
きません。

//quote{
@<tt>{alias   and     BEGIN   begin   break   case    class   def     defined }
@<tt>{do      else    elsif   END     end     ensure  false   for     if }
@<tt>{in      module  next    nil     not     or      redo    rescue  retry }
@<tt>{return  self    super   then    true    undef   unless  until   when }
@<tt>{while   yield}
//}

このうち、"Hello Trema!" の例では @<tt>{class} と @<tt>{def},
@<tt>{end} キーワードを使いました。

//listnum[キーワード][Hello Trema ソースコード内のキーワード]{
@<ami>{class} HelloTrema < Controller
   @<ami>{def} start
     puts "Hello, Trema!"
   @<ami>{end}
@<ami>{end}
//}

@<tt>{class} キーワードは続く名前 (@<tt>{HelloTrema}) のクラスを定義し
ます。このクラス定義は最後の 5 行目の @<tt>{end} までです。@<tt>{def}
キーワードは続く名前 (@<tt>{start}) のメソッドを定義します。このメソッ
ド定義は 4 行目 の @<tt>{end} までです。

=== 定数

定数は Ruby の世界では固有名詞に当たり、必ず大文字で始まります。
@<tt>{Time} や @<tt>{Array} や @<tt>{PORT_NUMBER} などがその例です。

英語でも固有名詞は大文字で始めることになっています。たとえば Tokyo
Tower (東京タワー) もそうです。東京タワーは動かすことができません。東京
タワーを何か別なもののことだと勝手に決めることもできません。固有名詞と
いうのはそういうものです。固有名詞はある特定の、通常は時間とともに変化
しないものを指しています。固有名詞と同様、Ruby の定数は一度セットすると
変更できません。

//quote{
TokyoTower = "東京都港区芝公園 4 丁目 2-8"
//}

"Hello Trema!" の例では @<tt>{class} キーワードに続く
@<tt>{HelloTrema} というクラス名と、@<tt>{Controller} が固有名詞でした。
後で説明しますが、この @<tt>{Controller} は Trema の Ruby ライブラリが
提供するクラス名です。

//listnum[定数][Hello Trema ソースコード内の定数]{
class @<ami>{HelloTrema} < @<ami>{Controller}
  def start
    puts "Hello, Trema!"
  end
end
//}

これで "Hello Trema!" の説明に必要な品詞の説明はおしまいです。それでは
"Hello Trema!" の中身を読み解いていきましょう。

=== コントローラクラスの定義

すべてのコントローラはクラスとして定義され、Trema の @<tt>{Controller}
クラスを継承します (@<list>{コントローラクラス定義}の 1 行目)。
Ruby でクラスを継承するには、@<tt>{class [クラス名] < [親クラス名]} と書きます。

//listnum[コントローラクラス定義][コントローラのクラスを定義する]{
@<ami>{class HelloTrema < Controller}
  def start
    puts "Hello, Trema!"
  end
end
//}

このように @<tt>{Controller} クラスを継承することで、コントローラに必要
な基本機能が @<tt>{HelloTrema} クラスにこっそりと追加されます。雑多な初
期化などの裏仕事を @<tt>{Controller} クラスが代わりにやってくれるわけです。

=== ハンドラメソッドの定義

Trema はイベントドリブンなプログラミングモデルを採用しています。Trema
が定義する各種イベントに対応するハンドラを定義しておくと、イベントが発
生したときに対応するハンドラが自動的に呼び出されます。たとえば
@<tt>{start} メソッド (@<list>{ハンドラ定義}の 2〜4 行目) を定義してお
くと、コントローラの起動時にこれが Trema によって自動的に呼ばれます。

//listnum[ハンドラ定義][起動時に呼ばれるハンドラを定義する]{
class HelloTrema < Controller
  @<ami>{def start}
    @<ami>{puts "Hello, Trema!"}
  @<ami>{end}
end
//}  

@<tt>{start} メソッドの中の @<tt>{puts} は Ruby 組込みのメソッドで、C
の @<tt>{puts()} と同じく指定した文字列を標準出力へ改行付きで出力します。

これで "Hello Trema!" の説明はおしまいです。Trema のコントローラは基本
的に "Hello, Trema!" と同じ構成をしています。つまり、@<tt>{Controller}
を継承するクラスにいくつかハンドラメソッドを追加すればコントローラのできあがりです。

== Trema のファイル構成

最後に Trema のファイル構成を見ておきましょう。Trema をダウンロードする
と、いくつかのファイルとディレクトリがあることがわかります。次に主要な
ものを挙げましょう。

 * @<tt>{build.rb}: ビルドスクリプト。
 * @<tt>{cruise.rb}: すべてのテストコードを実行するテストスイート。Trema 開発者向け。
 * @<tt>{features/}: 受け入れテスト一式。Trema 開発者向け。
 * @<tt>{ruby/}: Ruby ライブラリのソースコード。
 * @<tt>{spec/}: Ruby のユニットテスト一式。Trema 開発者向け。
 * @<tt>{src/examples/}: サンプルアプリ。
 * @<tt>{src/lib/}: C ライブラリのソースコード。
 * @<tt>{tmp}: ログファイルや PID ファイルといった一時ファイルの置き場。
 * @<tt>{trema}: trema コマンド本体。
 * @<tt>{unittests/}: C のユニットテスト一式。Trema 開発者向け。
 
この中でも Trema で何か作りたい人が真っ先に読むべきは、サンプルアプリ
(@<tt>{[trema]/src/examples}) です。

===[column] @<ruby>{取間,とれま}先生曰く: Trema のテスト

Trema にはずいぶんたくさんのテストコードが付いていて、Trema 開発者がテ
ストをとても重視していることがわかるじゃろう。テストの実行頻度も徹底し
ていて、開発者が新しいコードをコミットする度にすべてのテスト
(@<tt>{cruise.rb} スクリプト) を自動的に実行し、「いつダウンロードして
も正しく動く」ことを保証しているのじゃ。これを難しい言葉で "継続的イン
テグレーション" と呼ぶ。

#@warn(テストランプと天井の蛍光灯がかぶって見づらいので、写真を撮り直し)
//image[ccrb][テストの実行結果を示すランプ][scale=0.5]

Trema を壊さないために、ひとつおもしろい工夫があるのじゃ。@<img>{ccrb}
はTrema 開発者の机に置いてあるランプで、テストの実行結果をランプの色で
視覚的にフィードバックする。テストがすべて通るとランプが緑色に光る。も
しエラーが起こった場合には、ランプが赤く光り開発メンバー全員にメールが
飛ぶ。これによって、万が一壊してしまっても必ず誰かが気付けるようにして
いるのじゃ。

このしくみには、環境構築が手軽な CruiseControl.rb
(@<tt>{http://cruisecontrolrb.thoughtworks.com/}) と自作プラグインを使っ
ているぞ。

===[/column]

== サンプルアプリ

サンプルアプリ (@<tt>{[trema]/src/examples/}) は簡単な OpenFlow アプリ
ケーションをたくさん含んでおり、実際の API の使い方を調べるのに便利です。
以下におもなサンプルアプリをまとめます (括弧内は
@<tt>{[trema]/src/examples/} 内のディレクトリ名)。簡単な順に上から並べ
ていますので、この順に読んでいくことをおすすめします。またこのうちいく
つかは続く章で詳しく説明していきます。

: こんにちは Trema (@<tt>{hello_trema})
  この第 II 部で説明する Trema プログラミングのすべての基本となる
  "Hello Trema!" と表示するだけのサンプルです。これを @<tt>{trema run}
  コマンドで実行すれば、手っ取り早く Trema を試すことができます。Trema
  を始めたばかりの初心者向け。

: Packet In (@<tt>{packet_in})
  OpenFlow メッセージの中でも重要な Packet In メッセージをハンドルする
  サンプルです。OpenFlow メッセージハンドラの定義方法や、Packet In メッ
  セージの取り扱いの基本が学べます。

: スイッチの監視 (@<tt>{switch_monitor})
  スイッチがコントローラへ接続したり逆に切断したときのイベントを捕捉す
  るサンプルです。複数のハンドラを使った少し複雑なコントローラの実装が
  学べます。@<chap>{switch_monitoring_tool} で詳しく説明します。

: OpenFlow メッセージのダンプ (@<tt>{dumper})
  コントローラが受け取るすべての OpenFlow メッセージを文字列としてダン
  プするサンプルです。さまざまな OpenFlow メッセージのハンドラの書き方
  のリファレンスとして役に立ちます。

: スイッチ情報 (@<tt>{switch_info})
  スイッチの詳細情報を要求する Features Request メッセージをコントロー
  ラに送信し、スイッチから受信したスイッチ情報を出力するサンプルです。
  コントローラからスイッチへ OpenFlow メッセージを送る方法が学べます。

: リピータハブ (@<tt>{repeater_hub})
  いわゆるバカハブ (ダムハブ) の実装です。重要な OpenFlow メッセージで
  ある Flow Mod と Packet Out の基本が学べます。少し進んだ話題として、
  @<chap>{tdd} ではこれを題材にコントローラのテスト駆動開発手法を学びま
  す。

: ラーニングスイッチ (@<tt>{learning_switch})
  普通のスイッチ (レイヤ 2 スイッチ) をエミュレートするサンプルです。
  FDB などスイッチの基本構成を学ぶことができます。詳しくは
  @<chap>{learning_switch} で説明します。

: トラフィックモニタ (@<tt>{traffic_monitor})
  ラーニングスイッチを拡張し、ユーザごとのトラフィックを測れるようにし
  たものです。フローに含まれる統計情報の利用例として役に立ちます。詳し
  くは @<chap>{traffic_monitor} で説明します。

: 複数スイッチ対応ラーニングスイッチ (@<tt>{multi_learning_switch})
  ラーニングスイッチの複数スイッチ版です。ラーニングスイッチとの違い、
  とくにスイッチごとに FDB を管理する部分に注目してください。

Trema にはたくさんの API があり、この表に掲載したサンプルではまだまだす
べてを紹介しきれていません。新しいサンプルアプリを作った人は、ぜひ
github で pull リクエストを送ってください。あなたの名前が Trema プロジェ
クトの貢献者リスト
(@<tt>{https://github.com/trema/trema/graphs/contributors}) に載るかも
しれません!

== まとめ

さて、これで Trema の基本はおしまいです。この章ではすべてのコントローラ
のテンプレートとなる Hello, Trema! コントローラを書きました。学んだこと
は次の 4 つです。

 * コントローラは @<tt>{trema run} コマンドでコンパイル無しにすぐ実行で
   きます。
 * コントローラは Ruby のクラスとして定義し、@<tt>{Controller} クラスを
   継承することで必要なメソッドが取り込まれます。
 * コントローラクラスに各種イベントに対応するハンドラを定義することでロ
   ジックを実装できます。たとえば、起動イベントに対応するハンドラは
   start です。
 * Trema のファイル構成をおおざっぱに見ました。続く章では Trema のサン
   プルアプリ (@<tt>{[trema]/src/examples}) のいくつかを題材に、
   OpenFlow プログラミングを学んでいきます。

次の章では、いよいよ実用的な OpenFlow コントローラを書き実際にスイッチ
をつないでみます。

== 参考文献

Ruby プログラミングが初めてという人達のために、この章では入門に役立つサ
イトや本をいくつか紹介します。

: Why's (Poignant) Guide to Ruby (@<tt>{http://mislav.uniqpath.com/poignant-guide/})
  私は大学や職場でいろいろなプログラミング言語を勉強してきましたが、こ
  れほど読んでいて楽しい本に出会ったことはありません。この本は Ruby 界
  の謎の人物 _why 氏による風変りな Ruby 入門本で、まるで小説やマンガの
  ようにリラックスして読めます。この章の Ruby の品詞の説明は、この本を
  参考にしました。日本語版はこちら @<tt>{http://www.aoky.net/articles/why_poignant_guide_to_ruby/}

: TryRuby (@<tt>{http://tryruby.org/})
  同じく _why 氏によるブラウザで動く Ruby 環境です。Ruby を試してみたい
  けどインストールするのが面倒という人は、まずはここで Ruby を試してみ
  ましょう。@<tt>{help} と打つと 15 分の短い Ruby チュートリアルが始ま
  ります。

: プログラミング Ruby (Dave Thomas ら著、オーム社)
  Ruby の完全なリファレンスです。本気で Ruby を勉強したい人は持っていて
  損はしません。この本だけあれば十分です。
