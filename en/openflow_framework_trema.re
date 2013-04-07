= OpenFlowフレームワークTrema

//lead{
Tremaを使うと楽しくOpenFlowプログラミングの世界が味わえます。これでいよいよあなたもSDNプログラマの仲間入りです!
//}

//indepimage[izakaya][][width=10cm]

== 作って分かるOpenFlow

第II部では、いよいよ実際にOpenFlowでネットワークをプログラムする方法を紹介します。職場や自宅のような中小規模ネットワークでもすぐに試せる実用的なコードを通じて、「OpenFlowって具体的に何に使えるの？」「OpenFlowコントローラってどう作るの？」というよくある疑問に答えていきます。題材はなるべく実用例を取り上げるようにし、またOpenFlowやネットワークの基礎から説明していくようにしました。このためネットワークの専門家はもちろん、普通のプログラマにもすんなり理解できる内容となっているはずです。

まずは、この第II部で使うOpenFlowプログラミングのためのフレームワーク、@<ruby>{Trema,トレマ}をあらためて紹介します。

== Tremaとは

TremaはOpenFlowコントローラを開発するためのRuby向けプログラミングフレームワークです。GitHub上でオープンに開発しており、GPL2ライセンスのフリーソフトウェアです。公開は2011年の4月と比較的新しいソフトウェアですが、その使いやすさから国内外の企業や大学、および研究機関などですでに数多く採用されています。

Tremaの情報は主に次の URL から入手できます。

 * Tremaホームページ：@<href>{http://trema.github.com/trema/}
 * GitHubのプロジェクトページ：@<href>{http://github.com/trema/}
 * メーリングリスト：@<href>{http://groups.google.com/group/trema-dev}
 * Twitterアカウント：@<href>{http://twitter.com/trema_news}

Tremaの特長をひとことで言うと、「Ruby on RailsやSinatraなどのWebフレームワークに影響を受けた、アジャイルなOpenFlowプログラミングフレームワークである」ということです。たとえば最近のアジャイル開発環境ではもはや当たり前ですが、Tremaは開発を効率化する便利なツールを数多く提供しています。このように開発サイクル全体の面倒を見てくれるところが Tremaの「フレームワーク」たるゆえんで、他のOpenFlow開発ツールとはまったく違うところです。

ここからは実際にこのTremaを使ってOpenFlowコントローラを作っていきます。まずはTremaをセットアップしましょう。

===[column] @<ruby>{友太郎,ゆうたろう}の質問: Tremaの由来ってなに？

よく「Tremaの由来って何ですか？」と聞かれるのですが、これは筆者の1人が大好きな大阪の「とれまレコード(@<href>{http://www.fumiyatanaka.com/})」というレコードレーベルの名前から取りました。テクノミュージックを中心にリリースする小さなレーベルですが、DJの間では世界的に知られています。

さてそもそもこの"とれま"とはいったい何でしょう。これは日本がバブルの頃、道路の「とまれ」という標示がよく「とれま」と間違えて描かれていたという事実が元になっています。このありえない誤植の原因は、バブル景気時代に急増した外国人労働者が日本語もままならないまま道路工事現場で働いていたということにあるそうです。由来を探ってみると意外と面白い事実に行き着くことってありますね。
   
ちなみに、Tremaの公式ロゴマークは@<img>{trema_logo}です。これはTwitterのTrema公式アカウント(@<tt>{@trema_news})のアイコンとしても使われています。

//image[trema_logo][Trema の公式ロゴマーク][width=5cm]

もちろん、こんなに大胆な道路標識は日本中どこを探してもありません。この本の編集者が画像編集ソフトで試しに作ったところ評判が良かったので、そのまま公式ロゴになりました。

===[/column]

== Tremaのセットアップ

TremaはLinux上で動作します。次のディストリビューションとバージョンでの動作を保証しています。

 * Ubuntu 10.04 以降 (i386/amd64, デスクトップ版)
 * Debian GNU/Linux 6.0 (i386/amd64)

なお保証はしていませんが、RedHatなどその他のLinuxディストリビューションでも動作するはずです。

Tremaの提供するtremaコマンドの実行にはroot権限が必要です。まずは、@<tt>{sudo}を使ってroot権限でコマンドを実行できるかどうか、@<tt>{sudo}の設定ファイルを確認してください。

@<tt>{sudo}が正しく設定できていることを確認したら、Tremaのインストールや実行に必要ないくつかのソフトウェアをインストールしましょう。

 * Rubyインタプリタ@<br>{}Trema は RubyとCで作成されていて、アプリケーションの記述にもRubyを使います。TremaではRuby のバージョン 1.8.7 が必要です。1.8.6 以前のバージョンやバージョン 1.9 以降では今のところ動作しません。
 * Rubyのパッケージ管理システムRubyGems(@<href>{https://rubygems.org/})
 * Trema本体。本書は、Tremaバージョン0.3.0を使用して執筆しています。
 * Tremaのコンパイルに必要な@<tt>{gcc}や@<tt>{make}、およびいくつかのライブラリ。

開発に必要なのはこれだけです。それでは、Ubuntuのパッケージ管理システム@<tt>{apt-get}を使って必要なパッケージを次のようにインストールしてください。もし他のディストリビューションを使いたい場合は、コマンド名とパッケージ名を適宜読み替えください@<fn>{ubuntu12.10}。

//footnote[ubuntu12.10][Ubuntu 12.10以降ではRubyのデフォルトバージョンがRuby1.9になっているので、@<tt>{ruby}, @<tt>{rubygems}, @<tt>{ruby-dev}をそれぞれTremaがサポートする@<tt>{ruby1.8}, @<tt>{rubygems1.8}, @<tt>{ruby-dev1.8}と読み替えてください。]

//cmd{
% sudo apt-get install gcc make ruby rubygems ruby-dev irb libpcap-dev libsqlite3-dev
//}

以上でTremaをインストールするための準備が整いました。続いてTremaをインストールするには、RubyGemsのパッケージで手軽にインストールする方法と、最新のソースコードを取得して自分でビルドする方法があります。それぞれ説明していきましょう。

=== パッケージで手軽にインストールする場合

TremaはRubyGemsを使って次のようにコマンド一発で簡単にインストールできます。

//cmd{
% gem install trema
//}

RubyGemsでインストールした場合、自動的にTremaのコマンド@<tt>{trema}にパスが通っているはずです。次のコマンドでバージョンが表示されればインストールは成功です。

//cmd{
% trema --version
trema version 0.3.0
//}

=== ソースコードから最新版をインストールする場合

最新版をインストールしたい人は、GitHubから自分でソースコードをダウンロードしてビルドすることもできます。まず、次のように@<tt>{git}を使って最新のソースコードを取得してください。

//cmd{
% git clone git://github.com/trema/trema.git
//}

次のコマンドを実行すると、Tremaが依存するRubyGemsのパッケージが自動的にインストールされます。

//cmd{
% cd trema
% gem install bundler
% bundle install
//}

次のコマンドでTremaをダウンロードしたディレクトリ以下にTremaがインストールされます。@<tt>{make install}のようなシステム全体へのインストール手順は不要ですので注意してください。

//cmd{
% ./build.rb
//}

次のコマンドで @<tt>{trema} コマンドが正しくインストールされたか確認してください。

//cmd{
% ./trema --version
trema version 0.3.0
//}

もし必要あればこのディレクトリにパスを通し、@<tt>{trema}コマンドが簡単に起動できるようにしておいてください。

さあ、これでTremaによるOpenFlow開発環境が整いました。それでは早速、入門の定番Hello, WorldをTremaで書いてみましょう。

== Hello, Trema!

今から書くアプリケーションは最も簡単なOpenFlowコントローラの一種で、画面に"Hello, Trema!"と表示するだけのものです。スイッチとはまったくやりとりしないスタンドアロンのアプリケーションですが、Tremaで作れるすべてのコントローラのテンプレートとなっています。

では、適当なディレクトリにエディタで@<tt>{hello-trema.rb}というファイルを開き、次のコードを入力してください。"@<tt>{.rb}"はRubyプログラムの標準的な拡張子です。なおRubyの文法は必要に応じておいおい説明しますので、もし分からなくても今のところは気にせずそのまま入力してください。

//emlist{
  class HelloTrema < Controller
    def start
      puts "Hello, Trema!"
    end
  end
//}

意味はまだわからないかもしれませんが、とてもシンプルに書けることはわかると思います。それでは細かい文法は後で見るとして「習うより慣れろ」でさっそく実行してみましょう。

=== 実行してみよう(@<tt>{trema run})

作成したコントローラは@<tt>{trema run}コマンドですぐに実行できます。Rubyはインタプリタ言語なので、コンパイルの必要はありません。ターミナルで次のように入力すると、この世界一短いOpenFlowコントローラはフォアグラウンドプロセスとして起動し、画面に「@<tt>{Hello, Trema!}」と出力します。起動したコントローラは Ctrl + C で停止できます。

//cmd{
% trema run ./hello-trema.rb
Hello, Trema!  # Ctrl+c で終了
%
//}

いかがでしょうか？@<tt>{trema}コマンドを使うと、とても簡単にコントローラを実行できることがわかると思います。@<tt>{trema}コマンドには他にもいくつかの機能がありますのでここで簡単に紹介しておきましょう。

== @<tt>{trema コマンド}

@<tt>{trema}コマンドはTrema唯一のコマンドラインツールであり、コントローラの起動やテストなど様々な用途に使います。たとえば先ほどの"Hello Trema!"で見たように、@<tt>{trema run}はコントローラを起動するためのコマンドです。起動したコントローラはOpenFlowスイッチと接続しメッセージをやりとりします。また、次の章以降で触れますが@<tt>{trema run}コマンドはオプションで仮想ネットワークを作ることもでき、作ったコントローラをこの仮想ネットワークの中でテストできます。このように、@<tt>{trema}コマンドはTremaフレームワークにおける中心的なツールで、あらゆるコントローラ開発の出発点と言えます(@<img>{trema_overview})。

//image[trema_overview][@<tt>{trema}コマンドでコントローラを実ネットワークや仮想ネットワークで実行][width=12cm]

@<tt>{trema}コマンドは@<tt>{git}や@<tt>{svn}コマンドと似たコマンド体系を持っており、@<tt>{trema}に続けて@<tt>{run}などのサブコマンドを指定することで様々な機能を呼び出します。こうしたコマンド体系を一般に「コマンドスイート」と呼びます。

一般的なコマンドスイートと同じく、サブコマンドの一覧は@<tt>{trema help}で表示できます。また、サブコマンド自体のヘルプは@<tt>{trema help [サブコマンド]}で表示できます。以下に@<tt>{trema help}で表示されるサブコマンド一覧をざっと紹介しておきましょう。いくつかのサブコマンドはまだ使い方を紹介していませんが、続く章で説明しますので今は目を通すだけでかまいません。

: @<tt>{trema run}
  コントローラをフォアグラウンドで実行する。@<tt>{--daemonize (-d)}オプションを付けるとコントローラをバックグラウンド(デーモンモード)として実行できる

: @<tt>{trema killall}
  バックグラウンドで起動しているTremaプロセス全体を停止する
  
: @<tt>{trema version}
  Tremaのバージョンを表示する。@<tt>{trema --version}と同じ

: @<tt>{trema ruby}
  TremaのRuby APIをブラウザで表示する

: @<tt>{trema kill [仮想スイッチ]}
  仮想ネットワーク内の指定したスイッチを停止する(@<chap>{switch_monitoring_tool}を参照)

: @<tt>{trema up [仮想スイッチ]}
  仮想ネットワークの指定したスイッチを再起動する(@<chap>{switch_monitoring_tool}を参照)

: @<tt>{trema send_packets [送信オプション]}
  仮想ネットワーク内でテストパケットを送信する(@<chap>{learning_switch}を参照)
  
: @<tt>{trema show_stats [仮想ホスト名]}
  仮想ネットワーク内の仮想ホストで送受信したパケットの統計情報を表示する(@<chap>{learning_switch}を参照)

: @<tt>{trema reset_stats}
  仮想ネットワーク内の仮想ホストで送受信したパケットの統計情報をリセットする(@<chap>{learning_switch}を参照)

: @<tt>{trema dump_flows [仮想スイッチ名]}
  仮想ネットワーク内の仮想スイッチのフローテーブルを表示する(@<chap>{learning_switch}を参照)

この章ではさきほど使った@<tt>{trema run}に加えて、Ruby APIを表示する@<tt>{trema ruby}コマンドを覚えておいてください。@<tt>{trema ruby}を実行するとデフォルトブラウザでTrema Ruby APIリファレンスのページが開きます(@<img>{trema_ruby})。プログラミング中いつでもコマンド一発でリファレンスを開けるので大変便利です。

#@warn(API のページが YARD のせいで壊れているので、直してからスクリーンショット撮り直し)
//image[trema_ruby][@<tt>{trema ruby} コマンドで Trema Ruby API リファレンスを表示したところ][width=12cm]

では、気になっていたRubyの文法にそろそろ進みましょう。第II部では今後もたくさんRubyを使いますが、その都度必要な文法を説明しますので心配はいりません。しっかりついてきてください。

== 即席 Ruby 入門

外国語の習得にも言えることですが、Rubyを習得する一番の近道は登場する品詞の種類を押さえておくことです。Rubyに出てくる名前(構成要素)には、その品詞を見分けるための手がかりとなる視覚的なヒントがかならずあります。名前に記号が使われていたり、最初の文字が大文字になっていたりするので、断片的なコードを見てもすぐにどんな品詞かわかります。品詞がわかれば、そのRubyコードがどんな構造かわかります。

これからそれぞれの品詞について簡単に説明しますが、最初からすべてが理解できなくとも構いません。しばらくすればHello, Trema!プログラムのあらゆる部分が識別できるようになっているはずです。

=== キーワード

Ruby にはたくさんの組み込みの語があり、それぞれに意味が与えられています。これらの語を変数として使ったり、自分の目的に合わせて意味を変えたりはできません。

//quote{
@<tt>{alias   and     BEGIN   begin   break   case    class   def     defined }
@<tt>{do      else    elsif   END     end     ensure  false   for     if }
@<tt>{in      module  next    nil     not     or      redo    rescue  retry }
@<tt>{return  self    super   then    true    undef   unless  until   when }
@<tt>{while   yield}
//}

このうち、「Hello Trema!」では@<tt>{class}と@<tt>{def}、そして@<tt>{end}キーワードを使いました。

//emlist{
@<ami>{class} HelloTrema < Controller
   @<ami>{def} start
     puts "Hello, Trema!"
   @<ami>{end}
@<ami>{end}
//}

@<tt>{class}キーワードは続く名前(@<tt>{HelloTrema})のクラスを定義します。このクラス定義は最後の5行目の@<tt>{end}までです。@<tt>{def}キーワードは続く名前(@<tt>{start})のメソッドを定義します。このメソッド定義は4行目の@<tt>{end}までです。この@<tt>{def}や@<tt>{class}で始まって@<tt>{end}で終わる領域のことをブロックと呼びます。すべての Ruby プログラムはこのブロックがいくつか組み合わさったものです。

=== 定数

@<tt>{Time} や @<tt>{Array} や @<tt>{PORT_NUMBER} など、大文字で始まる名前が定数です。定数はRubyの世界では英語や日本語などの自然言語における固有名詞に当たります。

英語でも固有名詞は大文字で始めることになっています。たとえばTokyo Tower(東京タワー)もそうです。東京タワーは動かすことができませんし、何か別なものに勝手に変えることもできません。このように、固有名詞は時間とともに変化しないものを指します。そして固有名詞と同様、Rubyの定数は一度セットすると変更できません。

//quote{
@<tt>{TokyoTower = "東京都港区芝公園4丁目2-8"}
//}

「Hello Trema!」の例では@<tt>{class}キーワードに続く@<tt>{HelloTrema}と、@<tt>{Controller}がそれぞれ大文字で始まるので定数です。つまり、クラス名は定数なので実行中にその名前を変えることはできません。

//emlist{
class @<ami>{HelloTrema} < @<ami>{Controller}
  def start
    puts "Hello, Trema!"
  end
end
//}

これで「Hello Trema!」の説明に必要な品詞の説明はおしまいです。それでは「Hello Trema!」の中身を読み解いていきましょう。

=== コントローラクラスの定義

キーワードの節で説明したように、@<tt>{class}キーワードに続く定数から@<tt>{end}までで定義されるブロックがクラス定義です。Tremaではすべてのコントローラはクラスとして定義され、かならずTremaの@<tt>{Controller}クラスを継承します。クラスを継承するには、

//quote{
@<tt>{class クラス名 < 親クラス名}
//}

//noindent
と書きます。

//emlist{
@<ami>{class HelloTrema < Controller}
  def start
    puts "Hello, Trema!"
  end
@<ami>{end}
//}

@<tt>{Controller}クラスを継承することで、コントローラに必要な基本機能が@<tt>{HelloTrema}クラスにこっそりと追加されます。雑多な初期化などの裏仕事を@<tt>{Controller}クラスが代わりにやってくれるわけです。

=== ハンドラメソッドの定義

さて、こうして定義した@<tt>{HelloTrema}はどこから実行が始まるのでしょうか？Cで言う@<tt>{main()}関数に当たるものがどこにも見あたらない気がします。

その答はTremaの動作モデルであるイベントドリブンモデルにあります。Tremaのコントローラは、様々なOpenFlowイベントに反応するイベントハンドラをまとめたクラスとして定義できます。それぞれのイベントハンドラは、対応するOpenFlowイベントが発生したときに自動的に呼び出されます。たとえば何かOpenFlowメッセージが到着したとき、もしそのメッセージに対応するハンドラメソッドがコントローラクラスに定義されていれば、Tremaがそのメソッドを発見して呼んでくれます。

Tremaでよく使われるイベントをここにリストアップします。

#@warn(第 II 部で使うハンドラをここですべて説明)

: @<tt>{start}
  コントローラの起動時に呼ばれる

: @<tt>{switch_ready}、@<tt>{switch_disconnected}
  スイッチがコントローラに接続または切断したときに呼ばれる(@<chap>{switch_monitoring_tool}にて詳説)

: @<tt>{packet_in}
  未知のパケットが到着したというPacket Inメッセージ到着時に呼ばれる(@<chap>{learning_switch}にて詳説)

: @<tt>{flow_removed}
  フローが消された時のFlow Removedメッセージ到着時に呼ばれる(@<chap>{traffic_monitor}にて詳説)

ハンドラメソッドの定義は、@<tt>{def}キーワードに続く名前から@<tt>{end}までで定義されるブロックです。たとえば@<tt>{HelloTrema}の例では@<tt>{start}ハンドラメソッドを定義しており、これがコントローラの起動イベント発生時、つまり@<tt>{trema run}でコントローラを起動したときに自動的に呼ばれます。@<tt>{start}ハンドラメソッド中の@<tt>{puts}はRuby組込みのメソッドで、Cの@<tt>{puts()}関数と同じく文字列を標準出力へ改行付きで出力します。

//emlist{
class HelloTrema < Controller
  @<ami>{def start}
    puts "Hello, Trema!"
  @<ami>{end}
end
//}

===[column] @<ruby>{取間,とれま}先生いわく: ハンドラメソッドの自動呼び出し

プログラミング経験の長い読者の中には、「ハンドラメソッドを定義しただけなのに、なぜTremaはこのメソッドを自動的にみつけられるんだろう？」と不思議に思った人がいるかもしれません。プログラム中にどういう関数があるか(=コンパイル時情報)をプログラム自身が知る(=実行時)ことはむずかしいからです。特にCではコンパイル時と実行時の間にはぶ厚いカーテンが引かれているので普通は無理です。

実は、Rubyにはイントロスペクション(リフレクションや自己反映計算とも呼ぶ)と呼ばれる機能があり、オブジェクトが自らの持つメソッドを実行時に調べることができます。たとえばPacket Inメッセージが到着したとき、コントローラはイントロスペクションして自分が@<tt>{packet_in}というメソッドを持っているかどうかを実行時に調べます。そしてもしみつかればそのメソッドを呼んであげるというわけです。この機能は@<tt>{Controller}クラスを継承したときに自動的にコントローラへと導入されます。

===[/column]

これで「Hello Trema!」の説明はおしまいです。Tremaで作るコントローラは基本的にこの「Hello, Trema!」と同じ構成をしています。つまり、これをベースにいくつか必要なハンドラメソッドを追加していけば、より複雑で実践的なコントローラを作ることができます。

== Tremaのファイル構成

最後にTremaのファイル構成を見ておきましょう。Tremaをダウンロードすると、いくつかのファイルとディレクトリがあることがわかります。次に主要なものを挙げましょう。

 * @<tt>{bin/}: 各種コマンドの本体が置かれるディレクトリ
 * @<tt>{build.rb}: ビルドスクリプト
 * @<tt>{cruise.rb}: すべてのテストコードを実行するテストスイート(Trema開発者向け)
 * @<tt>{features/}: 受け入れテスト一式(Trema開発者向け。)
 * @<tt>{ruby/}: Rubyライブラリのソースコード
 * @<tt>{spec/}: Rubyのユニットテスト一式(Trema開発者向け)
 * @<tt>{src/examples/}: サンプルアプリ
 * @<tt>{src/lib/}: Cライブラリのソースコード
 * @<tt>{tmp}: ログファイルやPIDファイルといった一時ファイルの置き場
 * @<tt>{trema}: @<tt>{trema}コマンド
 * @<tt>{unittests/}: Cのユニットテスト一式(Trema開発者向け)
 
この中でもTremaでコントローラを作りたい人が読むべきは、サンプルアプリ(@<tt>{[trema]/src/examples})です。

===[column] @<ruby>{取間,とれま}先生いわく: Trema のテスト

Tremaにはずいぶんたくさんのテストコードが付いていて、Trema 開発者がテストをとても重視していることがわかると思います。テストの実行頻度も徹底していて、開発者が新しいコードをコミットする度にすべてのテスト(@<tt>{cruise.rb} スクリプト)を自動的に実行することで、「いつダウンロードしても正しく動く」ことを保証しているのです。この手法をよく「継続的インテグレーション」と呼びます。

#@warn(テストランプと天井の蛍光灯がかぶって見づらいので、写真を撮り直し)
//image[ccrb][テストの実行結果を示すランプ][width=5cm]

Tremaを壊さないために、1つおもしろい工夫があります。@<img>{ccrb}はTrema開発者の机に置いてあるランプで、テストの実行結果をランプの色で視覚的にフィードバックします。テストがすべて通るとランプが緑色に光り、もしエラーが起こった場合には、ランプが赤く光り開発メンバー全員にメールが飛びます。これによって、万が一壊してしまっても必ず誰かが気付けるようにしています。

このしくみには、環境構築が手軽なCruiseControl.rb(@<href>{http://cruisecontrolrb.thoughtworks.com/})と自作プラグインを使っています。

===[/column]

== サンプルアプリ

サンプルアプリ(@<tt>{[trema]/src/examples/})は簡単なOpenFlowアプリケーションをたくさん含んでおり、実際のAPIの使い方を調べるのに便利です。以下におもなサンプルアプリをまとめます(括弧内は@<tt>{[trema]/src/examples/}内のディレクトリ名)。このうちいくつかは続く章で詳しく説明していきます。

: こんにちはTrema(@<tt>{hello_trema})
  この章で説明した「Hello Trema!」と表示するだけのサンプル。これを@<tt>{trema run}コマンドで実行すれば、手っ取り早くTremaを試すことができる(Tremaを始めたばかりの初心者向け)

: Packet In(@<tt>{packet_in})
  OpenFlowメッセージの中でも重要なPacket Inメッセージをハンドルするサンプル。OpenFlowメッセージハンドラの定義方法や、Packet Inメッセージの取り扱いの基本が学べる

: スイッチの監視(@<tt>{switch_monitor})
  スイッチがコントローラへ接続したり逆に切断したときのイベントを捕捉するサンプル。複数のハンドラを使った少し複雑なコントローラの実装が学べる(@<chap>{switch_monitoring_tool}にて詳説)

: OpenFlowメッセージのダンプ(@<tt>{dumper})
  コントローラが受け取るすべてのOpenFlowメッセージを文字列としてダンプするサンプル。さまざまなハンドラの書き方リファレンスとして役に立つ

: スイッチ情報(@<tt>{switch_info})
  スイッチの詳細情報を要求するFeatures Requestメッセージをコントローラに送信し、スイッチから受信したスイッチ情報を出力するサンプル。コントローラからスイッチへOpenFlowメッセージを送る方法が学べる

: リピータハブ(@<tt>{repeater_hub})
  いわゆるバカハブ(ダムハブ)の実装。重要なOpenFlowメッセージであるFlow ModとPacket Outの基本が学べる。@<chap>{tdd}では少し進んだ話題として、これを題材にコントローラのテスト駆動開発手法を学ぶ

: ラーニングスイッチ(@<tt>{learning_switch})
  普通のスイッチをエミュレートするサンプル。FDBなどスイッチの基本構成を学ぶことができる(@<chap>{learning_switch}で詳説)

: トラフィックモニタ(@<tt>{traffic_monitor})
  ラーニングスイッチを拡張し、ユーザごとのトラフィックを測れるようにしたもの。フローに含まれる統計情報の利用例として役に立つ(@<chap>{traffic_monitor}にて詳説)

: 複数スイッチ対応ラーニングスイッチ(@<tt>{multi_learning_switch})
  ラーニングスイッチの複数スイッチ版です。ラーニングスイッチとの違い、とくにスイッチごとに FDB を管理する部分に注目してください。

: シンプルルータ(@<tt>{simple_router})
  ルータの基本機能を実装したサンプル。ルータでのパケットの書き換えと転送、およびルーティングテーブルの実装などルータの基本が学べる(@<chap>{router_part1}および@<chap>{router_part2}で詳説)

Trema にはたくさんのAPIがあり、上述したサンプルではまだまだすべてを紹介しきれていません。新しいサンプルアプリを作った人は、ぜひGitHubでpullリクエストを送ってください。あなたの名前がTremaプロジェクトの貢献者リスト(@<href>{https://github.com/trema/trema/graphs/contributors})に載るかもしれません!

== まとめ

さて、これでTremaの基本はおしまいです。この章ではTremaをセットアップし、すべてのコントローラのテンプレートとなる「Hello, Trema!」コントローラを書きました。この章で学んだことを簡単にまとめてから、実践的なコントローラの開発に入っていくことにしましょう。

 * TremaはRubyGemsまたはソースコードからビルドしてインストールできる
 * コントローラは@<tt>{trema run}コマンドでコンパイル無しにすぐ実行できる
 * コントローラはRubyのクラスとして定義し、@<tt>{Controller}クラスを継承することで必要なメソッドや機能が取り込まれる
 * コントローラクラスに各種イベントに対応するハンドラを定義することでOpenFlowコントローラを実装できる。たとえば、起動イベントに対応するハンドラは@<tt>{start}
 * Tremaのファイル構成と主なサンプル一覧

これで基礎は十分にできました。次の章では、いよいよ実用的なOpenFlowコントローラを書き実際にスイッチをつないでみます。

== 参考文献

Rubyプログラミングが初めてという人達のために、この章では入門に役立つサイトや本をいくつか紹介します。

: Why's (Poignant) Guide to Ruby(@<href>{http://mislav.uniqpath.com/poignant-guide/})
  私は大学や職場でいろいろなプログラミング言語を勉強してきましたが、これほど読んでいて楽しい本に出会ったことはありません。この本はRuby界の謎の人物_why氏による風変りなRuby入門で、プログラミング言語の解説書にもかかわらずまるで小説やマンガのようにリラックスして読めます。この章のRubyの品詞の説明は、この本を参考にしました。(日本語版はこちら@<href>{http://www.aoky.net/articles/why_poignant_guide_to_ruby/})。

: TryRuby(@<href>{http://tryruby.org/})
  同じく_why氏によるブラウザで動くRuby環境です。Rubyを試してみたいけどインストールするのが面倒という人は、まずはここでRubyを試してみましょう。@<tt>{help}と打つと15分の短いRubyチュートリアルが始まります。

: プログラミングRuby第2版(Dave Thomas、Chad Fowler、Andrew Hunt著、田和勝、まつもとゆきひろ訳／オーム社)
  Rubyの完全なリファレンスです。本気でRubyを勉強したい人は持っていて損はしません。この本だけあれば十分です。
