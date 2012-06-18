= Trema でテスト駆動開発

//lead{
「ワックスかける、ワックスとる」これは偉大な空手マスター、ミヤギさんの
言葉 (「ベスト・キッド」1984 年アメリカ映画) ですが、その精神性はソフト
ウェアテストに通じるものがあります。「テスト書く、コード直す」この単純
なくりかえしが徐々に大きな貯えとなり、将来的にはプロジェクトに大きな見
返りをもたらすのです。
//}

私はソフトウェアテストと聞くと、テストがまだ一般的でなかった 80 年代中
ごろのソフトウェア開拓時代、腕っぷしだけで稼いでいた天才ゲームプログラ
マ達に想いを馳せます。オレンジ色の小さなカセットにたくさんの要素を詰め
込んだ "スーパーマリオ" 。ゲームセンターのアーケードゲームを驚くほど忠
実に再現した "グラディウス II"。誰もソフトウェアテストのようなまどろっ
こしいことはせず、爆発的なプログラミング技術とデバッグ能力だけで何でも
作ってしまう時代でした。そんな中でも、"ファイナル・ファンタジー" の伝説
的プログラマ、ナーシャ・ジベリの早撃ちエピソードは有名です。「コードの
みてくれは気にしねえ。誰よりも早くやってやるぜ」。彼はカウボーイのよう
にさっそうと現場に現れ、どんなコードでも電光石火で書いてしまったそうで
す。

彼のコードは信じられないほど高速に動き ("ファイナル・ファンタジー II"
の飛行艇を思い出そう)、ゲームは 100 万本以上も売れましたが、ひとつだけ
困ったことがありました。彼の超絶コードは彼にしか理解できず，バグがあっ
ても誰も修正できないのです。本当の問題は彼が現場を去った約 10 年後に起
こりました。"ファイナル・ファンタジー III" のリメイク版を作ろうと当時の
彼のコードを掘り起こしたところ、ファミコンのバグに近いテクニックだらけ
で誰にも再現できなかったのです。「ナーシャ・カムバック!」でも彼は放浪癖
があるためつかまりません。結局、リメイク版の発売は 6 年も遅れてしまいま
した。

== 秘技:ソフトウェアテスト

こうした悲劇を防ぐ秘技の一つがソフトウェアテストです。きちんと整備され
たテストコードは、元のコードの仕様書のようなものです。しかもこの仕様書
は実際に動かせるのです! たくさんのスイッチを接続した OpenFlow ネットワー
クとコントローラの保守をまかされたとしましょう。もし前任者からテストコー
ドをもらえなければ、ファミコン時代に逆戻りです。テストさえもらえればコー
ド本体を理解しやすくなりますし、気楽にリファクタリングや修正ができます。
とくに OpenFlow ではスイッチとコントローラが複雑に絡み合い、しかもそれ
ぞれがステートを 持つので、ソフトウェアで自動化されたテストが無いとやっ
てられません。

Trema は OpenFlow コントローラ開発のためのテストツールが充実しています。
アジャイル開発者の大事な仕事道具、テスト駆動開発も Trema はサポートして
います。本章ではテスト駆動を使ったコントローラの開発テクニックを紹介し
ますが、要点をつかみやすくするため動作の単純ないわゆる「バカハブ」また
は「ダムハブ」、つまりリピータハブを採り上げます。ではさっそく実際の流
れを見て行きましょう。

===[column] 友太郎の質問: テスト駆動開発とテストファーストの違いは？

Q. 「テスト駆動開発とか、テストファーストとか似たような用語があって混乱
   してるんだ。この 2 つって何か違いはあるの？」

A. テストファーストはテスト駆動開発のステップの一部なので、テスト駆動開
   発のほうがより大きな概念になります。テスト駆動開発では、まず最初に失
   敗する見込みでテストを書き (このステップがテスト・ファースト)、次に
   このテストを通すためのコードを書きます。最後にコードをリファクタリン
   グして、クリーンにします。この 3 ステップを数分間隔で何度も回しなが
   ら開発するのがテスト駆動開発です。

===[/column]

== リピータハブの動き

まずは，リピータハブがどのように動くか説明しましょう。ホスト 3 台がリピー
タハブにつながったネットワークを考えてください (@<img>{repeater_hub})。
ホスト 1 からホスト 2 へパケットを送信すると，リピータハブは入ってきた
パケットを複製してほかのすべてのホストにばらまいてしまいます。つまり、
通信に関係の無いホスト 3 もホスト 2 あてのパケットを受信します。このよ
うに関係の無いホストにもパケットを送ってしまうので「バカ」ハブとか「ダ
ム」ハブと呼ばます。

//image[repeater_hub][ホスト 3 台をつなげたリピータハブの動作]

これを OpenFlow で実装すると@<img>{repeater_hub_openflow} のようになり
ます。ホスト 1 がパケットを送信すると，スイッチからコントローラに
packet_in が起こります。ここでコントローラは「今後は同様のパケットをほ
かの全ポートへばらまけ（FLOOD）」という flow_mod を打ちます。また，
packet_in を起こしたホスト 1 からのパケットをほかのすべてのホスト
（ホスト 2 と ホスト 3）に明示的に packet_out で届けます。

//image[repeater_hub_openflow][OpenFlow 版リピータハブ]

== どこまでテストするか？

設計も決まったので、テストを書き始める前にテスト戦略を決めます。テスト
戦略とは言い換えると「どこまでテストするか？」ということです。これはむ
ずかしい問題なので、ソフトウェアテスト界の賢人達の言葉を借りることにし
ます。

テスト駆動開発の第一人者、ケントベックは @<tt>{stackoverflow.com} の
「どれだけ深くテストをすればいいか？」というトピック
@<fn>{stackoverflow}に次の投稿をしています。

//footnote[stackoverflow][@<tt>{http://stackoverflow.com/questions/153234/how-deep-are-your-unit-tests}]

#@warn(訳が正しいかチェック)
//quote{
私は動くコードに対してお金をもらっていて、テストコードでもらっているの
ではない。なので、私の信条は、あるレベルの確信が得られる最低限のテスト
しかしないというものである (このレベルは産業界の水準からすると高いかも
しれないが、これは私の思い上がりかもしれない)。自分がある種のまちがいを
犯さないとわかっていれば (コンストラクタで間違った変数をセットするとか)、
そのためのテストはしない。テストエラーには意味を持たせたいと思っている
ので、複雑な条件を持つロジックにはとりわけ注意を払う。チームでコードを
書く場合は、チーム全体でまちがいやすい箇所を注意深くテストする、という
ように戦略を変える。
//}

Ruby on Rails の作者として有名な DHH は、彼の勤める 37signals のブログ
で次のように語っています@<fn>{37signals_blog}。
//footnote[37signals_blog][@<tt>{http://37signals.com/svn/posts/3159-testing-like-the-tsa}]

#@warn(訳が正しいかチェック)
//quote{
コードのすべて行にはコストがかかる。それを書くのには時間がかかり、それ
を更新するのにも時間がかかり、読んで理解するのにも時間がかかるからだ。
なので、テストで得られる利益はそれを作るコストより大きくなくてはいけな
いということになる。やりすぎなテストはその定義からして間違っている。
//}

2 人の言葉をまとめるとこうです:

 * テストのやりすぎはよくない
 * 動くと確信が得られる最低限のテストを書こう
 * その程度はプログラマやチームによって異なる

つまり、ケースバイケースでやりすぎないように気をつけようということです。

== リピータハブのテスト戦略

では実際に、リピータハブをどこまでテストするかざっくりと決めましょう。
最低限しなければいけないテストシナリオはこうなるはずです。

//quote{
@<strong>{シナリオ 1:}「ホスト 3 台（ホスト 1, 2，3）がスイッチにつながっ
ているとき，ホスト 1 が ホスト 2 へパケットを送ると，ホスト 2, 3 がパケッ
トを受け取る。」
//}

人によっては、思った通りのフローがスイッチに書き込まれたか気になる人も
いるでしょう。その場合の追加のテストシナリオはこうなります。

//quote{
@<strong>{シナリオ 2:} 「ホスト 3 台（ホスト 1, 2, 3）がスイッチにつな
がっているとき，ホスト 1 が ホスト 2 へパケットを送ると，パケットをホス
ト 2, 3 へばらまくフローエントリがスイッチに書き込まれる」
//}

さきほど言ったように、このシナリオ 2 を実装するかどうかはプログラマやチー
ムの方針次第です。不安になる人はシナリオ 2 もテストしてください。
OpenFlow に慣れていてシナリオ 1 だけで十分と言う人は、それでかまいませ
ん。以降の説明ではシナリオ 1 と 2 を両方実装してみます。

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

== 参考文献

#@warn(ケントベックの TDD 本と、リファクタリング本もリストアップすべき。)

: stackoverflow (@<tt>{stackoverflow.com})
  私はテストに限らずプログラミングでわからないことがあると、この Q&A サ
  イトを検索します。ユーザは役に立つ回答に点数をつけることができ、また
  ケント・ベックなど有名人が回答してくれることが多いので、質の高い解答
  を読むことができます。さきほどのケント・ベックのポストはなんと 200 ポ
  イント以上の点数を稼いでいました!

: Signal vs. Noise (@<tt>{http://37signals.com/svn})
  Ruby on Rails を作った小さな会社 37signals のブログです。ソフトウェア
  界の常識を次々と破ってきた会社だけあって、記事それぞれが非常に刺激的
  で過激なポストにみちあふれています。DHH など有名ハッカーの生の声を聞
  きたい人にもおすすめ。
