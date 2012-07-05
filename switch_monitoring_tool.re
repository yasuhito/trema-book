= スイッチ監視ツール

#@warn(「運用コストの削減」を説明した章へのリンクを張る)
OpenFlow のメリットのひとつに、運用コストの削減がありました。OpenFlow に対応したスイッチであれば、どんなに数が増えてもリモートで集中的に管理できるのでした。本章ではこの集中管理ツールの一例として、スイッチの監視ツールを作ります。このツールは「今、ネットワーク中にどんなスイッチが動いているか」をリアルタイムに表示します。何らかの障害で落ちてしまったスイッチを発見したり、予期しないスイッチがネットワークに接続されていることを検出するのに便利です。

//indepimage[switch_monitor_overview]

スイッチ監視ツールの動きは次のようになります。OpenFlow スイッチは、起動すると指定した OpenFlow コントローラへ接続します。コントローラはスイッチが接続してきたことを検知すると、接続したスイッチを "起動中のスイッチ" リストに追加します。逆にスイッチが何らかの原因で接続を切った場合、コントローラはこれを検知していなくなったスイッチをリストから削除します。つまりスイッチの接続イベントと切断イベントを捕捉してリストを更新する、たったこれだけです。

== SwitchMonitor コントローラ

適当なディレクトリに switch-monitor.rb というファイルを作り、エディタで@<list>{switch-monitor.rb} のコードを入力してください。今回も文法は後まわしで先に実行してみますので、細かいところは気する必要はありません。そのまま入力してしまってください。

//list[switch-monitor.rb][@<tt>{SwitchMonitor} コントローラ]{
class SwitchMonitor < Controller
  periodic_timer_event :show_switches, 10


  def start
    @switches = []
  end


  def switch_ready datapath_id
    @switches << datapath_id.to_hex
    info "Switch #{ datapath_id.to_hex } is UP"
  end


  def switch_disconnected datapath_id
    @switches -= [datapath_id.to_hex ]
    info "Switch #{ datapath_id.to_hex } is DOWN"
  end


  # 以下プライベートメソッド
  private


  def show_switches
    info "All switches = " + @switches.sort.join( ", " )
  end
end
//}

=== 実行してみよう

それでは早速実行してみましょう。でも、スイッチ監視ツールは実際にスイッチを接続してみなければおもしろくありません。どうすればいいでしょうか？なんと，Trema を使えば OpenFlow スイッチを持っていなくてもこうしたコードを実行してテストできます。いったいどういうことでしょうか？その答えは、Trema の強力な機能の1つ，仮想ネットワーク構築機能にあります。これは仮想OpenFlow スイッチや仮想ホストを接続した仮想ネットワークを作る機能です。この仮想ネットワークとコントローラを接続することによって、物理的なOpenFlow スイッチやホストを準備しなくとも、開発マシン 1 台で OpenFlow コントローラと動作環境を一度に用意して開発できます。もちろん、開発したコントローラは実際の物理的な OpenFlow スイッチやホストで構成されたネットワークでもそのまま動作します！

仮想スイッチを起動するには、仮想ネットワークの構成を記述した設定ファイルを @<tt>{trema run} の "-c" オプションに渡します。たとえば、仮想スイッチ（@<tt>{vswitch}）を 2 台定義するには@<list>{two virtual switches} のような設定ファイルを書きます。

//list[two virtual switches][仮想スイッチ 2 台を仮想ネットワークに追加]{
vswitch { datapath_id 0xabc }
vswitch { datapath_id 0xdef }
//}

それぞれに指定されている @<tt>{datapath_id} (@<tt>{0xabc}、@<tt>{0xdef}) はネットワークカードにおける MAC アドレスのような存在で、スイッチを一意に特定する ID として使われます。OpenFlow の規格によると、64 ビットの一意な整数値を OpenFlow スイッチ 1 台ごとに割り振ることになっています。仮想スイッチでは好きな値を設定できるので、かぶらないように適当な値をセットしてください。

それでは @<list>{switch-monitor.conf} の内容のファイルを
@<tt>{switch-monitor.conf} として保存してください。

//list[switch-monitor.conf][仮想スイッチを 3 台定義 (@<tt>{switch-monitor.conf})]{
vswitch { datapath_id 0x1 }
vswitch { datapath_id 0x2 }
vswitch { datapath_id 0x3 }
//}

設定ファイルは trema run コマンドの -c オプションとして渡せます。実行結
果は次のようになります。

//cmd{
% ./trema run ./switch-monitor.rb -c ./switch-monitor.conf
Switch 0x3 is UP
Switch 0x2 is UP
Switch 0x1 is UP
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
……
//}

@<tt>{switch-monitor} コントローラが起動すると設定ファイルで定義した仮
想スイッチ 3 台が起動し、@<tt>{switch-monitor} コントローラのによって捕
捉され、このメッセージが出力されました。

===[column] 友太郎の質問: datapath ってなに？

Q.「スイッチに付いている ID を datapath ID って呼ぶのはわかったけど、いっ
たい datapath ってなに？ スイッチのこと？」

A.実用的には「datapath = OpenFlow スイッチ」と考えて問題ありません。

「データパス」でググると、「CPU は演算処理を行うデータパスと，指示を出
すコントローラから構成されます」というハードウェア教科書の記述がみつか
ります。つまり、ハードウェアの世界では一般に「筋肉にあたる部分＝データ
パス」「脳にあたる部分＝コントローラ」という分類をするようです。

OpenFlow の世界でも同じ用法が踏襲されています。OpenFlow のデータパスは
パケット処理を行うスイッチを示し、その制御を行うソフトウェア部分をコン
トローラと呼びます。

===[/column]

=== スイッチを停止/起動してみる

それでは、スイッチの切断がうまく検出されるか確かめてみましょう。スイッ
チを停止するコマンドは @<tt>{trema kill} です。別ターミナルを開き、次の
コマンドでスイッチ @<tt>{0x3} を落としてみてください。

//cmd{
% ./trema kill 0x3
//}

すると、@<tt>{trema run} を動かしたターミナルに次の出力が表示されているはずです。

//cmd{
% ./trema run ./switch-monitor.rb -c ./switch-monitor.conf
Switch 0x3 is UP
Switch 0x2 is UP
Switch 0x1 is UP
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
……
Switch 0x3 is DOWN
//}

うまくいきました！それでは、逆にさきほど落としたスイッチを起動してみま
しょう。

//cmd{
% ./trema up 0x3
//}

ターミナルの出力がこうなっていれば成功です。

//cmd{
% ./trema run ./switch-monitor.rb -c ./switch-monitor.conf
Switch 0x3 is UP
Switch 0x2 is UP
Switch 0x1 is UP
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
All switches = 0x1, 0x2, 0x3
……
Switch 0x3 is DOWN
……
Switch 0x3 is UP
//}

== ソースコード

スイッチモニタリングツールのソースコードは@<list>{switch-monitor.rb}
の通りになります。それではさっそく読んでいきましょう。

OpenFlow スイッチは、起動すると OpenFlow コントローラへ接続しに行きます。
Trema では、スイッチとの接続が確立すると，コントローラの
@<tt>{switch_ready} ハンドラが呼ばれます。コントローラはスイッチ一覧リ
ストを更新し、新しく起動したスイッチをリストに追加します。逆にスイッチ
が何らかの原因で接続を切った場合、コントローラの
@<tt>{switch_disconnected} ハンドラが呼ばれます。コントローラはリストを
更新し、いなくなったスイッチをリストから削除します。

#@warn(あとで解説ごとにソースコードを網かけつきで書く)

=== スイッチの起動を捕捉する

スイッチの起動イベントを捕捉するには @<tt>{switch_ready} ハンドラをコ
ントローラクラスに追加します。

//list[switch_ready][スイッチの起動を捕捉する @<tt>{switch_ready} ハンドラ]{
class SwitchMonitor < Controller
  # ...

  def switch_ready datapath_id
    @switches << datapath_id.to_hex
    info "Switch #{ datapath_id.to_hex } is UP"
  end

  # ...
end
//}

@<tt>{@switches} は現在起動しているスイッチのリストを管理するインスタン
ス変数で、新しくスイッチが起動するとスイッチの @<tt>{datapath_id} が追
加されます。また、@<tt>{info} メソッドで @<tt>{datapath_id} を表示しま
す。

=== スイッチの切断を捕捉する

同様に、スイッチが落ちて接続が切れたイベントを捕捉してみましょう。この
ためのハンドラは @<tt>{switch_disconnected} です。

//list[switch_disconnected][スイッチの切断を捕捉する @<tt>{switch_disconnected} ハンドラ]{
class SwitchMonitor < Controller
  # ...

  def switch_disconnected datapath_id
    @switches -= [datapath_id.to_hex ]
    info "Switch #{ datapath_id.to_hex } is DOWN"
  end

  # ...
end
//}

スイッチの切断を捕捉すると、切断したスイッチの @<tt>{datapath_id} をス
イッチ一覧 @<tt>{@switches} から除きます。また、@<tt>{datapath_id} を
@<tt>{info} メソッドで表示します。

=== スイッチの一覧を表示する

最後に、スイッチの一覧を定期的に表示する部分です。一定時間ごとに何らか
の処理を行いたい場合には、タイマー機能を使います。一定の間隔で呼びたい
メソッドと間隔（秒数）を@<tt>{periodic_timer_event} で指定すると、指定
されたメソッドが呼ばれます。ここでは、スイッチの一覧を表示するメソッド
@<tt>{show_switches} を10 秒ごとに呼び出します。

//list[timer][スイッチの一覧を 10 秒ごとに表示する]{
class SwitchMonitor < Controller
  periodic_timer_event :show_switches, 10

  # ...

  # 以下プライベートメソッド
  private


  def show_switches
    info "All switches = " + @switches.sort.join( ", " )
  end
end
//}

== まとめ/参考文献

スイッチの動作状況を監視するスイッチモニタを作りました。学んだことは次の3つです。

 * OpenFlow ネットワークはパケットを処理するスイッチ（datapath）と、ス
   イッチを制御するソフトウェア（コントローラ）から構成される。Trema は、
   このコントローラを書くためのプログラミングフレームワークである
 * Trema は仮想ネットワーク構築機能を持っており、OpenFlow スイッチを持っ
   ていなくてもコントローラの開発やテストが可能。たとえば、仮想ネットワー
   クに仮想スイッチを追加し、任意の datapath ID を設定できる
 * コントローラは OpenFlow の各種イベントに対応するハンドラを定義するこ
   とでスイッチをコントロールできる。たとえば、@<tt>{switch_ready} と
   @<tt>{switch_disconnected} ハンドラでスイッチの起動と切断イベントに
   対応するアクションを書ける

===[column] 友太郎の質問: @<tt>{switch_ready} ってなに？

Q.「OpenFlow の仕様を読んでみたけど、どこにも @<tt>{switch_ready} って
出てこなかったよ？ OpenFlow にそんなイベントが定義されてるの？」

A. @<tt>{switch_ready} は Trema 独自のイベントで、スイッチが Trema に接
続し指示が出せるようになった段階でコントローラに送られます。実は、
@<tt>{switch_ready} の裏では@<img>{switch_ready} の一連の処理が行われて
おり、Trema が OpenFlow プロトコルの詳細をうまくカーペットの裏に隠してく
れているのです。

//image[switch_ready][@<tt>{switch_ready} イベントが起こるまで]

最初に、スイッチとコントローラがしゃべる OpenFlow プロトコルが合ってい
るか確認します。OpenFlow の HELLO メッセージを使ってお互いのプロトコル
バージョンを確認し、うまく会話できそうか確認します。

次は、スイッチを識別するための datapath ID の取得です。datapath ID のよ
うなスイッチ固有の情報は、スイッチに対して OpenFlow の Features
Request メッセージを送ることで取得できます。成功した場合、datapath ID
やポート数などの情報が Features Reply メッセージに乗ってやってきます。

最後にスイッチを初期化します。スイッチに以前の状態が残っていると、コン
トローラが管理する情報と競合が起こるため、初期化することでこれを避けま
す。これら一連の処理が終わると、ようやく @<tt>{switch_ready} がコントロー
ラに通知されます。
