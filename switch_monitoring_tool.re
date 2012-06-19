= スイッチ監視ツール

OpenFlow のメリットのひとつに、運用コストの削減がありました。OpenFlow
に対応したスイッチであれば、どんなにスイッチが増えてもリモートで集中的
に管理できます。

#@warn(「運用コストの削減」を説明した章へのリンクを張る)

集中管理の一例として、スイッチのモニタリングツールを作ります。このツー
ルは「今、ネットワーク中にどんなスイッチが動いているか」をリアルタイム
に表示します。何らかの障害で落ちてしまったスイッチを発見したり、予期し
ないスイッチがネットワークに接続されていることを検出するのに便利です。

スイッチモニタリングツールの動作を簡単に説明しましょう。

#@warn(あとで書く: スイッチモニタリングツールの動作)

OpenFlow スイッチは、起動すると指定した OpenFlow コントローラへ接続しま
す。コントローラは接続を検知すると、接続したスイッチの ID を「起動中の
スイッチ一覧」に追加します。逆にスイッチが何らかの原因で接続を切った場
合、コントローラがこれを検知して一覧を更新し、いなくなったスイッチを一
覧から削除します。

== 実行してみよう

それでは早速実行してみましょう。今回の例は実際にスイッチを接続しなけれ
ばならないので、trema に「どんなスイッチを接続するか」を設定ファイルで
渡すことにします。まずは @<list>{switch-monitor.conf} の内容のファイル
を @<tt>{switch-monitor.conf} として保存してください。詳細な文法は後の
章で説明しますので、

//list[switch-monitor.conf][仮想スイッチを 3 台定義 (@<tt>{switch-monitor.conf})]{
vswitch { datapath_id 0x1 }
vswitch { datapath_id 0x2 }
vswitch { datapath_id 0x3 }
//}

実行結果は次のようになります。

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
想スイッチ 3 台が起動し、@<tt>{switch-monitor} コントローラの
@<tt>{switch_ready} ハンドラによって捕捉され、このメッセージが出力され
ました。

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

うまくいきました！ おわかりのとおり、このメッセージは
@<tt>{switch_disconnected} ハンドラによって表示されたものです。

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

@warn(あとで構成を変えて書く)

== 仮想ネットワーク

それでは早速、スイッチの起動を検知するコードを書いてみましょう。なんと，
Trema を使えば OpenFlow スイッチを持っていなくてもこうしたコードを実行
してテストできます。いったいどういうことでしょうか？

その答えは、Trema の強力な機能の1つ，仮想ネットワーク構築機能にあります。
これは仮想 OpenFlow スイッチや仮想ホストを接続した仮想ネットワークを作
る機能です。この仮想ネットワークとコントローラを接続することによって、
物理的な OpenFlow スイッチやホストを準備しなくとも、開発マシン 1 台で
OpenFlow コントローラと動作環境を一度に用意して開発できます。もちろん、
開発したコントローラは実際の物理的な OpenFlow スイッチやホストで構成さ
れたネットワークでもそのまま動作します！

それでは仮想スイッチを起動してみましょう。

== ソースコード

OpenFlow スイッチは、起動すると OpenFlow コントローラへ接続しに行きます。
Trema では、スイッチとの接続が確立すると，コントローラの
@<tt>{switch_ready} ハンドラが呼ばれます。コントローラはスイッチ一覧リ
ストを更新し、新しく起動したスイッチをリストに追加します。逆にスイッチ
が何らかの原因で接続を切った場合、コントローラの
@<tt>{switch_disconnected} ハンドラが呼ばれます。コントローラはリストを
更新し、いなくなったスイッチをリストから削除します。

仮想スイッチを起動するには、仮想ネットワークの構成を記述した設定ファイ
ルを @<tt>{trema run} に渡します。たとえば、@<list>{two virtual switches}
の設定ファイルでは仮想スイッチ（@<tt>{vswitch}）を2台定義しています。

//list[two virtual switches][仮想ネットワークに仮想スイッチを2台追加]{
vswitch { datapath_id 0xabc }
vswitch { datapath_id 0xdef }
//}

それぞれに指定されている @<tt>{datapath_id} (@<tt>{0xabc}、
@<tt>{0xdef}) はネットワークカードにおける MAC アドレスのような存在で、
スイッチを一意に特定する ID として使われます。OpenFlow の規格によると、
64 ビットの一意な整数値を OpenFlow スイッチ 1 台ごとに割り振ることになっ
ています。仮想スイッチでは好きな値を設定できるので、かぶらないように適
当な値をセットしてください。

@warn(あとで解説ごとにソースコードを網かけつきで書く)

//list[switch-monitor.rb][@<tt>{SwitchMonitor} コントローラ]{
class SwitchMonitor < Controller
  periodic_timer_event :show_switches, 10 ――③

  def start
    @switches = []
  end

  def switch_ready datapath_id ――①
    @switches << datapath_id.to_hex
    info "Switch #{ datapath_id.to_hex } is UP"
  end

  def switch_disconnected datapath_id ――②
    @switches -= [datapath_id.to_hex ]
    info "Switch #{ datapath_id.to_hex } is DOWN"
  end

  private ――③
  def show_switches
    info "All switches = " + @switches.sort.join( ", " )
  end
end
//}

=== スイッチの起動を捕捉する

それでは、さきほど定義したスイッチを起動してコントローラから捕捉してみ
ましょう。スイッチの起動イベントを捕捉するには @<tt>{switch_ready} ハン
ドラを書きます（リスト3-①）。

@warn(あとで該当部分のソースコードを載せる)

@<tt>{@switches} は現在起動しているスイッチのリストを管理するインスタン
ス変数で、新しくスイッチが起動するとスイッチの @<tt>{datapath_id} が追
加されます。また、@<tt>{puts} メソッドで @<tt>{datapath_id} を表示しま
す。

=== スイッチの停止を捕捉する

同様に、スイッチが落ちて接続が切れたイベントを捕捉してみましょう。この
ためのハンドラは @<tt>{switch_disconnected} です（リスト3-②）。

@warn(あとで該当部分のソースコードを載せる)

スイッチの切断を捕捉すると、切断したスイッチの @<tt>{datapath_id} をス
イッチ一覧 @<tt>{@switches} から除きます。また、@<tt>{datapath_id} を
@<tt>{puts} メソッドで表示します。

=== スイッチの一覧を表示する

最後に、スイッチの一覧を定期的に表示する部分を作ります。一定時間ごとに
何らかの処理を行いたい場合には、タイマー機能を使います。リスト3-③のよう
に、一定の間隔で呼びたいメソッドと間隔（秒数）を
@<tt>{periodic_timer_event} で指定すると、指定されたメソッドが呼ばれま
す。ここでは、スイッチの一覧を表示するメソッド @<tt>{show_switches} を
10 秒ごとに呼び出します。

@warn(あとで該当部分のソースコードを載せる)

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

次回はいよいよ本格的なコントローラとして、トラフィック集計機能のあるレ
イヤ 2 スイッチを作ります。初歩的なレイヤ2スイッチング機能と、誰がどの
くらいネットワークトラフィックを発生させているかを集計する機能を
OpenFlow で実現します。


===[column] 友太郎の質問: @<tt>{switch_ready} ってなに？

Q.「OpenFlow の仕様を読んでみたけど、どこにも @<tt>{switch_ready} って
出てこなかったよ？ OpenFlow にそんなイベントが定義されてるの？」

A. @<tt>{switch_ready} は Trema 独自のイベントで、スイッチが Trema に接
続し指示が出せるようになった段階でコントローラに送られます。実は、
@<tt>{switch_ready} の裏では図○の一連の処理が行われており、Trema が
OpenFlow プロトコルの詳細をうまくカーペットの裏に隠してくれているのです。

@warn(あとでシーケンス図を載せる)

図A @<tt>{switch_ready} イベントが起こるまで

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
