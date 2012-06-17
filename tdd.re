= Trema でテスト駆動開発

//lead{
「ワックスかける、ワックスとる」これはある偉大な空手マスターの言葉です
が、その精神性はソフトウェアテストに通じるものがあります。「テスト書く、
コード直す」この単純なくりかえしが徐々に大きな貯えとなり、将来的にはプ
ロジェクトに大きな見返りをもたらすのです。
//}

私はソフトウェアテストと聞くと、テストがまだ一般的でなかった 80 年代中
ごろのソフトウェア開拓時代、腕っぷしだけで稼いでいた天才ゲームプログラ
マ達に想いを馳せます。オレンジ色の小さなカセットにたくさんの要素を詰め
込んだ "スーパーマリオ" 。ゲームセンターのアーケードゲームを驚くほど忠
実に再現した "グラディウス II"。誰もテストのようなまどろっこしいことは
せず、爆発的なプログラミング技術とデバッグ能力だけで何でも作ってしまう
時代でした。そんな中でも、"ファイナル・ファンタジー" の伝説的プログラマ、
ナーシャ・ジベリの早撃ちエピソードは有名です。「コードのみてくれは気に
しねえ。誰よりも早くやってやるぜ」。彼はカウボーイのようにさっそうと現
場に現れ、どんなコードでも電光石火で書いてしまったそうです。

彼のコードは信じられないほど高速に動き ("ファイナル・ファンタジー II"
の飛行艇を思い出そう)、ゲームは 100 万本以上も売れましたが、ひとつだけ
困ったことがありました。彼の超絶コードは彼にしか理解できず，バグがあっ
ても誰も修正できないのです。問題は彼が現場を去った約 10 年後に起こりま
した。"ファイナル・ファンタジー III" のリメイク版を作ろうと彼の当時のコー
ドを調べたところ、バグに近いプログラミングテクニックが駆使されていたた
め動作原理がさっぱり理解できなかったのです。「ナーシャ・カムバック!」で
も彼は世界放浪の旅に出てしまい戻ってきません。結局、リメイク版の発売は
6 年も遅れてしまいました。

こうした悲劇を防ぐ代表的手法がソフトウェアテストです。OpenFlow のような
たくさんのスイッチとコントローラが絡み合う動作モデルでは、ソフトウェア
で自動化されたテストが無いとやってられません。それに、きちんとしたテス
ト一式はコード本体を理解する助けになり、リファクタリングや修正の大きな
味方でもあります。

Trema は OpenFlow コントローラ開発のためのテストツールが充実しています。
たとえばアジャイル開発者の大事な仕事道具、テスト駆動開発も Trema はサポー
トしています。要点をつかみやすくするため、本章では動作の単純ないわゆる
「バカハブ」、つまりリピータハブをテスト駆動で実装します。ではさっそく
実際の流れを見て行きましょう。

===[column] 友太郎の質問: テスト駆動とテストファーストの違いは？

#@warn(あとで書く)

===[/column]

== お題: リピータハブをテスト駆動で作る

まずは，リピータハブがどのように動作するかを説明しましょう。ホスト 3 台
のネットワークを考えてください。あるホストからパケットを送信すると，リ
ピータハブは入ってきたパケットを複製してほかのすべてのホストにばらまき
ます。

OpenFlow プロトコル的に何が起こっているかを@<img>{repeater_hub} に示し
ます。host1 がパケットを送信すると，スイッチからコントローラに
packet_in が起こります。ここでコントローラは「今後は同様のパケットをほ
かの全ポートへばらまけ（FLOOD）」という flow_mod を打ちます。また，
packet_in を起こした最初のパケットをほかのすべてのホスト（host2 と
host3）に明示的に packet_out で届けます。

//image[repeater_hub][ホスト 3 台をつなげたリピータハブの動作]

== 最初のテスト

ではさっそく，リピータハブのテストコードを書いていきましょう。Trema の
テストフレームワークは Ruby のユニットテストツール
@<href>{http://rspec.info/, RSpec} と統合されています。まだインストール
していない人は，@<tt>{gem install rspec} でインストールしてください。ま
た，Trema の API は
@<href>{http://rubydoc.info/github/trema/trema/master/frames/,こちら}で
参照できます。

テストコードの最初のバージョンは@<list>{repeater_hub_test_template}のと
おりです。最初の行は，テストに必要な Trema のライブラリを読み込みます。
@<tt>{describe} で始まる @<tt>{do...end} ブロックはテストの本体で，
RepeaterHub コントローラのふるまいをここに記述（describe）する，という
意味です。

//list[repeater_hub_test_template][リピータハブのテストのひな型 (@<tt>{spec/repeater-hub_spec.rb})]{
require File.join(File.dirname(__FILE__), "spec_helper")

describe RepeaterHub do
end
//}

まだ @<tt>{RepeaterHub} クラスを定義していないのでエラーになることはわ
かりきっていますが，テストを実行してみましょう。次のコマンドを実行する
と，Trema を起動したうえで
@<tt>{spec/repeater-hub_spec.rb}(@<list>{repeater_hub_test_template})の
テストを実行します。

//cmd{
$ rspec -fs -c ./spec/repeater-hub_spec.rb
.../spec/repeater-hub_spec.rb:3: uninitialized constant RepeaterHub (Name Error)
//}

予想どおり，@<tt>{RepeaterHub} クラスが未定義というエラーで失敗しました。
エラーを修正するために，@<tt>{RepeaterHub} クラスの定義を追加してみましょ
う（@<list>{add_repeater_hub_class}）。

//list[add_repeater_hub_class][空の @<tt>{RepeaterHub} クラスを追加して NameError を修正]{
require File.join(File.dirname(__FILE__), "spec_helper")

class RepeaterHub < Controller # 空のクラスを追加
end

describe RepeaterHub do
end
//}

本来，コントローラクラスは独立した @<tt>{.rb} ファイルに書きますが，今
回は簡便さを優先し，テストコード上に直接書いているので注意してください。

それでは実行してみましょう。今度はパスするはずです。

//cmd{
$ rspec -fs -c spec/repeater-hub_spec.rb
No examples found.
Finished in 0.00003 seconds 0 examples, 0 failures
//}

やった！ これで最初のテストにパスしました。

このようにテスト駆動では，最初にテストを書き，わざとエラーを起こしてか
らそれを直すためのコードをちょっとだけ追加します。テストを実行した結果
からのフィードバックを得ながら「テスト書く，コード書く」を何度もくりか
えしつつ最終的な完成形に近づけていくのです。

== パケット受信をテストする

では，リピータハブの動作をテストコードにしていきましょう。どんなテスト
シナリオが思いつくでしょうか？ とりあえず，こんなのはどうでしょう。

@<strong>{シナリオ 1:}「ホスト3台（host1，host2，host3）がスイッチにつ
ながっているとき，宛先を host2 としたパケットを host1が 送ると，host2
と host3 がパケットを受け取る。」

テストコードは@<list>{first_test_scenario}のように @<tt>{it} ブロックの
中に記述します。

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
 * 【When】host1 が host2 にパケットを送ると，
 * 【Then】host2 と host3 がパケットを受け取る

では，Given，When，Then の順にテストコードを書いていきます。

=== Given: ネットワークの構成

シナリオの前提条件（Given）として，テストを実行するホスト3台のネットワー
ク構成 (@<img>{repeater_hub}) を@<list>{tdd_given} のように定義します。

//list[tdd_given][テストシナリオを実行するネットワーク構成の定義（Given）]{
describe RepeaterHub do
  it "は，入ってきたパケットを他のすべてのポートに転送する" do
    network { # ホスト3台，スイッチ1台のネットワーク
      vswitch("switch") { dpid "0xabc" }
      vhost("host1") { promisc "on" } # 自分宛ではないパケットも受け取る
      vhost("host2") { promisc "on" }
      vhost("host3") { promisc "on" }
      link "switch", "host1"
      link "switch", "host2"
      link "switch", "host3"
    }
  end
end
//}

これはネットワーク設定とまったく同じ文法ですね！ ここで，それぞれの仮想
ホストが @<tt>{promisc} オプション（自分宛でないパケットを受け取る）を
@<tt>{"on"} にしていることに注意してください。リピータハブではパケット
がすべてのポートにばらまかれるので，こうすることでどこからのパケットで
も受信できるようにしておきます。

=== When: パケットの送信

When は「○○したとき」というきっかけになる動作を記述します。ここでは，
Given で定義されたホスト host1 から host2 にパケットを送ります。パケッ
トを送るコマンドは，@<tt>{trema send_packets} でした。もちろん、テスト
コード中でもこれに似た API を使うことができます（@<list>{tdd_when}）。

@<tt>{run(RepeaterHub)} は，Given で定義されたネットワークの上で
@<tt>{RepeaterHub} コントローラを動かし，続くブロックを実行するという意
味です。

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

Then には「最終的にこうなるはず」というテストを書きます。ここでは，
「host2 と host3 にパケットが1つずつ届くはず」を書けばよいですね
（@<list>{tdd_then}）。

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

@<tt>{vhost("ホスト名")} は仮想ホストにアクセスするためのメソッドで，仮
想ホストの受信パケットなどさまざまなデータを見ることができます。ここで
は，受信したパケットの数，つまり受信パケットカウンタ @<tt>{stats(:rx)}
が 1 ということをテストしています。

=== テストの実行

ではさっそく実行してみましょう。

//cmd{
Failure/Error: vhost("host2").stats(:rx).should have( 1 ).packets
expected 1 packets, got 0
//}

失敗しました。「host2 はパケットを 1 つ受信するはずが，0 個だった」とい
うエラーです。@<tt>{RepeaterHub} クラスの中身をまだ実装していないので当
たり前ですね。すぐにはなおせそうにないので，ひとまずこのテストは保留
（pending）とし，あとで復活することにしましょう（@<list>{tdd_pending}）。

//list[tdd_pending][すぐに修正できないテストを保留（pending）にする]{
describe RepeaterHub do
  it "は，入ってきたパケットを他のすべてのポートに転送する" do
    pending "あとで実装する" # この行を追加する
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
では，リピータハブの動作を@<img>{repeater_hub} の 1 と 2 の 2 段階に分
け，1 つずつテスト駆動で実装していくことにしましょう。

== フローエントリのテスト

まずは，スイッチにフローエントリができることをテストしてみましょう
（@<img>{repeater_hub} の 1）。

テストシナリオ 2 は次のようになります。

 * 【Given】ホスト 3 つ（host1，host2，host3）がスイッチにつながっているとき，
 * 【When】host1 が host2 にパケットを送ると，
 * 【Then】パケットをばらまくフローエントリをスイッチに追加する

では，これをテストコードにしてみましょう。Given と When は最初のテスト
シナリオと同じで，Then だけが異なります。パケットをばらまく処理は
FLOOD ですので@<list>{test_flow_entry} のようになります。

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

ネットワーク構成のコード（@<tt>{network {...\}} の部分）をコピペしてし
まっていますが，あとできれいにするので気にしないでください。エラーにな
ることを見越しつつ，さっそく実行すると，次のエラーになります。

//cmd{
Failure/Error: vswitch("switch").should have(1).flows
expected 1 flows, got 0
//}

「スイッチにフローエントリが1つあるはずがなかった」というエラーです。で
は，flow_mod を打ち込むコードを @<tt>{RepeaterHub} クラスに追加して，も
う一度テストしてみましょう（@<list>{tdd_sending_flow_mod}）。

//list[tdd_sending_flow_mod][flow_mod をスイッチに打ち込む]{
class RepeaterHub < Controller
  def packet_in dpid, message
    send_flow_mod_add dpid
  end
end
//}

//cmd{
Failure/Error: vswitch("switch").flows.first.actions.should == "FLOOD"
  expected: "FLOOD"
    got: "drop" (using ==)
//}

別のエラーになりました。「アクションが "FLOOD" でなく "drop" だった」と
怒られています。たしかに，さきほどの flow_mod にはアクションを設定して
いなかったので，デフォルトのアクションである drop（パケットを破棄する）
になってしまっています。flow_mod にパケットをばらまくアクションを定義し
てみましょう（@<list>{tdd_adding_action_to_flow_mod}）。

//list[tdd_adding_action_to_flow_mod][flow_mod にアクションを追加]{
class RepeaterHub < Controller
  def packet_in dpid, message
    send_flow_mod_add(
      dpid,
      :actions => ActionOutput.new( OFPP_FLOOD )
    )
  end
end
//}

今度はテストが通りました！ それでは，もう少し Then を詳細化し，フローの
特徴を細かくテストしてみましょう（@<list>{tdd_test_src_dst}）。

//list[tdd_test_src_dst][フローの src と dst のテストを追加]{
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

ここではホストに IP アドレスを振り，フローの src と dst がこのアドレス
に正しく設定されているかをチェックしています。実行してみましょう。

//cmd{
Failure/Error: flow.nw_src.should == "192.168.0.1"
  expected: "192.168.0.1"
    got: nil (using ==)
//}

失敗しました。フローの src には，パケット送信元である host1 の IP アド
レス 192.168.0.1 がセットされるべきですが，何もセットされていません。で
は，flow_mod で @<tt>{:match} を指定して，この値がセットされるようにし
ます（@<list>{tdd_set_match_to_flow_mod}）。

//list[tdd_set_match_to_flow_mod][flow_mod メッセージに @<tt>{:match} をセット]{
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
れていることまで（@<img>{repeater_hub} の 1）をテストできました。

=== テストコードのリファクタリング

テストが通ったので，最後にコードの重複部分をまとめておきましょう。同じ
@<tt>{network{...\}} が重複しているので，RSpec の @<tt>{around} ブロッ
クを使って 1 箇所にまとめます（@<list>{tdd_refactoring}）。

//list[tdd_refactoring][共通部分を @<tt>{around} ブロックに移すことでコードの重複をなくす]{
describe RepeaterHub do
  around do | example |
    network {
      （省略）
    }.run(RepeaterHub) {
      example.run  # それぞれのitブロックをここで実行
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

いよいよ完成間近です。パケットが host2 と host3 に届くことをテストしま
す（@<img>{repeater_hub} の 2）。最初のテストの保留マーク（pending）を
消して，再び実行してみましょう。

//cmd{
Failure/Error: vhost("host2").stats(:rx).should
 have( 1 ).packets
  expected 1 packets, got 0
//}

失敗してしまいました。host2 がパケットを受信できていません。そういえば，
flow_mod しただけではパケットは送信されないので，明示的に packet_out し
てやらないといけないのでしたね。packet_out を追加します
（@<list>{tdd_add_packet_out}）。

//list[tdd_add_packet_out][@<tt>{RepeaterHub} に packet_out 処理を追加]{
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

さっそく実行してみましょう。

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

Trema のユニットテストフレームワークを使ってリピータハブを作りました。
Trema の @<tt>{src/examples} ディレクトリの下にはテストコードのサンプル
がいくつかありますので，本格的にテストコードを書く人は参考にしてくださ
い。今回学んだことは次の2つです。

 * コントローラをユニットテストする方法を学びました。Trema は Ruby のユ
   ニットテストフレームワーク RSpec と統合されており，仮想スイッチのフ
   ローテーブルや仮想ホストの受信パケット数などについてのテストを書けま
   す。
 * テストを Given，When，Then の3ステップに分けて分析／設計する方法を学
   びました。それぞれのステップを RSpec のテストコードに置き換えること
   で，テストコードが完成します。
