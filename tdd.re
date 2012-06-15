= Trema でテスト駆動開発

初期ファイナル・ファンタジーの伝説的プログラマ，ナーシャ・ジベリの「早
撃ち」エピソードを知っていますか？ 彼はヒーローのようにさっそうと現れ，
どんなプログラムでも電光石火で書いてしまいます。「見てくれの悪さは気に
しねえ。誰よりも早くやってやるぜ」。やがていくつかの伝説を残し，彼もプ
ロジェクトを去るときがやってきました。残った同僚たちは困りました。彼の
超絶プログラムは彼にしか理解できず，バグがあっても修正できないのです。
それに，変更しようとすると動かなくなってしまいます。「ナーシャ，カムバッ
ク！」でも彼はもう戻ってきません……。

こうした悲劇を防ぐ方法の1つがソフトウェアテストです。OpenFlowコントロー
ラのように動作シーケンスが複雑なソフトウェアが壊れていないことを確認す
るためには，ソフトウェアにより自動化されたテストが有効です。それに，き
ちんとしたテスト一式があればプログラム本体の理解もしやすく，修正も簡単
です。

TremaはOpenFlowコントローラをテストするためのテストツールが充実してい
ます。今回はこれを使って，簡単なコントローラ（リピータハブ）をテスト
ファースト形式で実装していきます。

ではさっそく実際の例を見ていきましょう。

== お題: リピータハブをテスト駆動で作る

まずは，リピータハブがどのように動作するかを説明しましょう。ホスト3台
のネットワークを考えてください。あるホストからパケットを送信すると，リ
ピータハブは入ってきたパケットを複製してほかのすべてのホストにばらまきます。

OpenFlow プロトコル的に何が起こっているかを図1に示します。host1がパケッ
トを送信すると，スイッチからコントローラにpacket_inが起こります。ここで
コントローラは「今後は同様のパケットをほかの全ポートへばらまけ（FLOOD）」
というflow_modを打ちます。また，packet_inを起こした最初のパケットをほか
のすべてのホスト（host2 とhost3）に明示的にpacket_outで届けます。

//image[repeater_hub][ホスト 3 台をつなげたリピータハブの動作]

== 最初のテスト

ではさっそく，リピータハブのテストコードを書いていきましょう。Tremaのテ
ストフレームワークはRubyのユニットテストツールRSpecと統合されています。
まだインストールしていない人は，「geminstall rspec」でインストールして
ください。また，TremaのAPIについては，こちらを参照してください。

#@warn(RSpec へのリンク)
#@warn(Trema API へのリンク)

テストコードの最初のバージョンは@<list>{repeater_hub_test_template}の
とおりです。最初の行は，テストに必要なTremaのライブラリを読み込みます。
describeで始まる do…end ブロックはテストの本体で，RepeaterHubコントローラ
のふるまいをここに記述（describe）する，という意味です。

//list[repeater_hub_test_template][リピータハブのテストのひな型 (@<tt>{spec/repeater-hub_spec.rb}]{
require File.join(File.dirname(__FILE__), "spec_helper")

describe RepeaterHub do
end
//}

RepeaterHubを定義していないのでエラーになることはわかりきっていますが，
テストを実行してみましょう。次のコマンドを実行すると，Tremaを起動したう
えでspec/repeater-hub_spec.rb（リスト1）のテストを実行します。

//cmd{
$ rspec -fs -c ./spec/repeater-hub_spec.rb
.../spec/repeater-hub_spec.rb:3: uninitialized constant
 RepeaterHub (Name Error)
//}

予想どおり，定数RepeaterHubが未定義というエラーで失敗しました。エラーを
修正するために，RepeaterHubクラスの定義を追加してみましょう（リスト2）。

//list[repeater_hub_fix_1][空のRepeaterHubクラスを追加してNameErrorを修正]{
require File.join(File.dirname(__FILE__), "spec_helper")

class RepeaterHub < Controller # ← 空のクラスを追加
end

describe RepeaterHub do
end
//}

本来，コントローラクラスは独立した.rbファイルに書きますが，今回は簡便さ
を優先し，テストコード上に直接書いているので注意してください。それでは
実行してみましょう。今度はパスするはずです。

//cmd{
$ rspec -fs -c spec/repeater-hub_spec.rb
No examples found.
Finished in 0.00003 seconds 0 examples, 0 failures
//}

やった！ これで最初のテストにパスしました。

このようにテストファーストでは，最初にテストを書き，わざとエラーを起こ
してからそれを直すためのコードをちょっとだけ追加します。テストを実行し
た結果からのフィードバックを得ながら「テスト書く，コード書く」を何度も
くりかえしつつ最終的な完成形に近づけていくのです。

== パケット受信をテストする

では，リピータハブの動作をテストコードにしていきましょう。どんなテスト
シナリオが思いつくでしょうか？ とりあえず，こんなのはどうでしょう。

「ホスト3台（host1，host2，host3）がスイッチにつながっているとき，宛先
をhost2としたパケットをhost1が送ると，host2とhost3がパケットを受け取
る。」

テストコードはリスト3のようにitブロックの中に記述します。

//list[first_test_scenario][テストシナリオの定義]{
describe RepeaterHub do
  it "は，入ってきたパケットを他のすべてのポートに転送する" do
    # テストコードをここに書く
  end
end
//}

テストシナリオをテストコードに置き換えるには，シナリオの各ステップを
Given（前提条件），When（○○したとき），Then（こうなる）の3つに分解する
とうまく整理できます。

 * 【Given】ホスト3つ（host1，host2，host3）がスイッチにつながっているとき，
 * 【When】host1がhost2にパケットを送ると，
 * 【Then】host2とhost3がパケットを受け取る

では，Given，When，Thenの順にテストコードを書いていきます。

=== Given: ネットワークの構成

シナリオの前提条件（Given）として，テストを実行するホスト3台のネットワー
ク構成（図1）をリスト4 のように定義します。

//list[tdd_given][テストシナリオを実行するネットワーク構成の定義（Given）]{
describe RepeaterHub do
  it "は，入ってきたパケットを他のすべてのポートに転送する" do
    network { # ← ホスト3台，スイッチ1台のネットワーク
      vswitch("switch") { dpid "0xabc" }
      vhost("host1") { promisc "on" } # ← 自分宛ではないパケットも受け取る
      vhost("host2") { promisc "on" }
      vhost("host3") { promisc "on" }
      link "switch", "host1"
      link "switch", "host2"
      link "switch", "host3"
    }
  end
end
//}

これはネットワーク設定とまったく同じ文法ですね！ ここで，それぞれの仮
想ホストがpromiscオプション（自分宛でないパケットを受け取る）を"on"にし
ていることに注意してください。リピータハブではパケットがすべてのポート
にばらまかれるので，こうすることでどこからのパケットでも受信できるよう
にしておきます。

=== When: パケットの送信

Whenは「○○したとき」というきっかけになる動作を記述します。ここでは，
Givenで定義されたホストhost1からhost2にパケットを送ります。パケットを送
るコマンドは，trema send_packetsでした。テストコード中でもこれに似た
APIを使うことができます（リスト5）。

run(RepeaterHub)は，Givenで定義されたネットワークの上でRepeaterHubコン
トローラを動かし，続くブロックを実行するという意味です。

//list[tdd_when][テストパケットを送信（When）]{
describe RepeaterHub do
  it "は，入ってきたパケットを他のすべてのポートに転送する" do
    network {
      # （省略）
    }.run(RepeaterHub) {
      send_packets "host1", "host2"
    }
  end
end
//}

=== Then: 受信したパケットの数

Thenには「最終的にこうなるはず」というテストを書きます。ここでは，
「host2とhost3にパケットが1つずつ届くはず」を書けばよいですね（リスト
6）。

//list[tdd_then][受信パケット数のテスト（Then）]{
describe RepeaterHub do
  it "は，入ってきたパケットを他のすべてのポートに転送する" do
    network {
      # （省略）
    }.run(RepeaterHub) {
      send_packets "host1", "host2"

      vhost("host2").stats(:rx).should have(1).packets
      vhost("host3").stats(:rx).should have(1).packets
    }
  end
end
//}

vhost("ホスト名")は仮想ホストにアクセスするためのメソッドで，仮想ホスト
の受信パケットなどさまざまなデータを見ることができます。ここでは，受信
したパケットの数，つまり受信パケットカウンタstats(:rx)が1ということをテ
ストしています。

=== テストの実行

ではさっそく実行してみましょう。

//cmd{
Failure/Error: vhost("host2").stats(:rx).should
 have( 1 ).packets
expected 1 packets, got 0
//}

失敗しました。「host2はパケットを1つ受信するはずが，0 個だった」という
エラーです。RepeaterHubの中身をまだ実装していないので当たり前ですね。す
ぐにはなおせそうにないので，ひとまずこのテストは保留（pending）とし，あ
とで復活することにしましょう（リスト7）。

//list[tdd_pending][すぐに修正できないテストを保留（pending）にする]{
describe RepeaterHub do
  it "は，入ってきたパケットを他のすべてのポートに転送する" do
    pending "あとで実装する" ← この行を追加する
    network {
      # （省略）
//}

今度は実行結果が次のように変わり，エラーが出なくなります。

//cmd{
Pending：
1） は，入ってきたパケットを他のすべての ポートに転送する
# あとで実装する
//}

ここでの失敗の原因は，いきなりすべてを実装しようとしたことでした。以降
では，リピータハブの動作を図1の①と②の2段階に分け，1つずつテストファース
トで実装していくことにしましょう。

== フローエントリのテスト

まずは，スイッチにフローエントリができることをテストしてみましょう（図
1の①）。テストシナリオは次のようになります。

 * 【Given】ホスト3つ（host1，host2，host3）がスイッチにつながっているとき，
 * 【When】host1 がhost2 にパケットを送ると，
 * 【Then】パケットをばらまくフローエントリをスイッチに追加する

では，これをテストコードにしてみましょう。GivenとWhenは最初のテストシナ
リオと同じで，Thenだけが異なります。パケットをばらまく処理はFLOODですの
でリスト8のようになります。

//list[test_flow_entry][フローエントリのテスト]{
it "は，パケットをばらまくフローエントリをスイッチに追加する" do
  network {
    # （省略）
  }.run(RepeaterHub) {
    send_packets "host1", "host2"
    vswitch("switch").should have(1).flows
    vswitch("switch").flows.first.actions.should == "FLOOD"
  }
end
//}

ネットワーク構成のコード（network {…… }の部分）をコピペしてしまっていま
すが，あとできれいにするので気にしないでください。エラーになることを見
越しつつ，さっそく実行すると，次のエラーになります。

//cmd{
Failure/Error: vswitch("switch").should have(1).flows
expected 1 flows, got 0
//}

「スイッチにフローエントリが1つあるはずがなかった」というエラーです。で
は，flow_modを打ち込むコードをRepeaterHubクラスに追加して，もう一度テス
トしてみましょう（リスト9）。

//list[tdd_sending_flow_mod][flow_modをスイッチに打ち込む]{
class RepeaterHub < Controller
  def packet_in dpid, message
    send_flow_mod_add dpid
  end
end
//}

//cmd{
Failure/Error: vswitch("switch").flows.first.actions.
should == "FLOOD"
  expected: "FLOOD"
    got: "drop" (using ==)
//}

別のエラーになりました。「アクションが"FLOOD"でなく"drop"だった」と怒ら
れています。たしかに，さきほどのflow_modにはアクションを設定していなかっ
たので，デフォルトのアクションであるdrop（パケットを破棄する）になって
しまっています。flow_modにパケットをばらまくアクションを定義してみましょ
う（リスト10）。

//list[tdd_adding_action_to_flow_mod][flow_modにアクションを追加]{
リスト10　flow_modにアクションを追加
class RepeaterHub < Controller
  def packet_in dpid, message
    send_flow_mod_add(
      dpid,
      :actions => ActionOutput.new( OFPP_FLOOD )
    )
  end
end
//}

今度はテストが通りました！ それでは，もう少しThenを詳細化し，フローの特
徴を細かくテストしてみましょう（リスト11）。

//list[tdd_test_src_dst][フローのsrcとdstのテストを追加]{
describe RepeaterHub do
  it "は，入ってきたパケットを他のすべてのポートに転送する" do
    network {
      vswitch("switch") { dpid "0xabc" }
      vhost("host1") { promisc "on"; ip "192.168.0.1" }
      vhost("host2") { promisc "on"; ip "192.168.0.2" }
      vhost("host3") { promisc "on"; ip "192.168.0.3" }
      link "switch", "host1"
      link "switch", "host2"
      link "switch", "host3"
    }.run(RepeaterHub) {
      send_packets "host1", "host2"
      vswitch("switch").should have(1).flows
      flow = vswitch("switch").flows.first
      flow.actions.should == "FLOOD"
      flow.nw_src.should == "192.168.0.1"
      flow.nw_dst.should == "192.168.0.2"
    }
  end
end
//}

ここではホストにIPアドレスを振り，フローのsrcとdstがこのアドレスに正し
く設定されているかをチェックしています。実行してみましょう。

//cmd{
Failure/Error: flow.nw_src.should == "192.168.0.1"
  expected: "192.168.0.1"
    got: nil (using ==)
//}

失敗しました。フローの srcには，パケット送信元であるhost1 のIP アドレス
192.168.0.1がセットされるべきですが，何もセットされていません。では，
flow_mod で:match を指定して，この値がセットされるようにします（リスト
12）。

//list[tdd_set_match_to_flow_mod][flow_modメッセージに:matchをセット]{
class RepeaterHub < Controller
  def packet_in dpid, message
    send_flow_mod_add(
      dpid,
      :match => ExactMatch.from(message),
      :actions => ActionOutput.new( OFPP_FLOOD )
    )
  end
end
//}

テストにパスしました！ これで，フローエントリが正しくスイッチに書き込ま
れていることまで（図1の①）をテストできました。

=== テストコードのリファクタリング

テストが通ったので，最後にコードの重複部分をまとめておきましょう。同じ
network{ ……}が重複しているので，aroundブロックを使って1箇所にまとめます
（リスト13）。

//list[tdd_refactoring][共通部分をaroundブロックに移すことでコードの重複をなくす]{
describe RepeaterHub do
  around do | example |
    network {
      （省略）
    }.run(RepeaterHub) {
      example.run ← それぞれのitブロックをここで実行
    }
  end

  it "は，入ってきたパケットを他のすべてのポートに転送する" do
    send_packets "host1", "host2"
    pending "あとで実装する"
    ……
  end

  it "は，パケットをばらまくフローエントリをスイッチに追加する" do
    send_packets "host1", "host2"
    vswitch("switch").should have(1).flows
    ……
  end
end
//}

== 再びパケットの受信をテスト

いよいよ完成間近です。パケットがhost2とhost3に届くことをテストします
（図1の②）。最初のテストの保留マーク（pending）を消して，再び実行してみ
ましょう。

//cmd{
Failure/Error: vhost("host2").stats(:rx).should
 have( 1 ).packets
  expected 1 packets, got 0
//}

失敗してしまいました。host2 がパケットを受信できていません。そういえば，
flow_modしただけではパケットは送信されないので，明示的にpacket_outして
やらないといけないのでしたね（リスト14）。さっそく実行してみましょう。

//list[tdd_add_packet_out][RepeaterHubにpacket_out処理を追加]{
class RepeaterHub < Controller
  def packet_in dpid, message
    send_flow_mod_add(
      dpid,
      :match => ExactMatch.from(message),
      :actions => ActionOutput.new(OFPP_FLOOD)
    )
    send_packet_out(
      dpid,
      :packet_in => message,
      :actions => ActionOutput.new(OFPP_FLOOD)
    )
  end
end
//}

//cmd{
RepeaterHub
  は，入ってきたパケットを他のすべてのポートに転送する
  は，パケットをばらまくフローエントリをスイッチに追加する
Finished in 15.66 seconds
2 examples, 0 failures
//}

すべてのテストに通りました！ これでリピータハブとテストコード一式が完成
です。

== まとめ/参考文献

Tremaのユニットテストフレームワークを使ってリピータハブを作りました。
Tremaのsrc/examplesディレクトリの下にはテストコードのサンプルがいくつか
ありますので，本格的にテストコードを書く人は参考にしてください。今回学
んだことは次の2つです。

 * コントローラをユニットテストする方法を学びました。Trema はRuby のユ
   ニットテストフレームワークRSpecと統合されており，仮想スイッチのフロー
   テーブルや仮想ホストの受信パケット数などについてのテストを書けます。
 * テストをGiven，When，Thenの3ステップに分けて分析／設計する方法を学び
   ました。それぞれのステップをRSpecのテストコードに置き換えることで，
   テストコードが完成します。
