= Trema でテスト駆動開発

#@warn(それぞれのソースコードを別ファイルに分けて include する)

//lead{
ソフトウェアテストは総合的なスキルを必要とする最高峰の奥義です。「テストを書き、コードを直す」この正確なくりかえしが徐々に大きな蓄えとなり、将来的にプロジェクトに豊富な見返りをもたらしてくれます。
//}

== 天才プログラマ達の伝説

筆者が小学生だった80年代の家庭用ゲーム開拓時代には、腕っぷしだけで稼ぐ天才ゲームプログラマ達の伝説がたくさんありました。色とりどりの小さなカセットに驚くほどたくさんの要素を詰め込んだ名作ファミコンゲームの数々。爆発的なプログラミング技術とデバッグ能力だけで何でも作ってしまう時代でした。そんな中でも、「ファイナル・ファンタジー」の伝説的プログラマ、ナーシャ・ジベリの早撃ちエピソードは有名です。「コードのみてくれやちょっとぐらいのバグは気にしねえ。誰よりも速くやってやるぜ」。彼はカウボーイのようにさっそうと現場に現れ、どんなコードでも電光石火で書いてしまったそうです。

彼のコードは信じられないほど高速に動き、ゲームは100万本以上も売れましたが、1つだけ困ったことがありました。彼の超絶コードは彼にしか理解できず、バグがあっても誰も修正できないのです。本当の問題は彼が現場を去った約10年後に起こりました。新しいハード向けにリメイク版を作ろうと当時の彼のコードを掘り起こしたところ、謎のテクニックだらけで誰にも動作を再現できなかったのです。「ナーシャ・カムバック!」でも彼はどこにいるのかわかりません。結局、リメイク版の発売は6年も遅れてしまいました。

== 奥義、ソフトウェアテスト

こうした悲劇を防ぐ奥義の一つがソフトウェアテストです。きちんと整備されたテストコードは、元のコードの仕様書のようなものです。しかもこの仕様書は実際に動かせるのです! たくさんのスイッチを接続したOpenFlowネットワークとコントローラの保守をまかされたとしましょう。もし前任者からテストコードをもらえなければ、ファミコン時代に逆戻りです。逆に、テストさえもらえればコード本体を理解しやすくなりますし、気楽にリファクタリングや修正ができます。とくにOpenFlowではスイッチとコントローラが複雑に絡み合い、しかもそれぞれがステートを持つので、ソフトウェアで自動化されたテストがないとやってられません。

TremaはOpenFlowコントローラ開発のためのテストツールが充実しています。アジャイル開発者の大事な仕事道具、テスト駆動開発もTremaはサポートしています。本章ではテスト駆動を使ったコントローラの開発テクニックを紹介します。要点をつかみやすくするため動作の単純ないわゆる「バカハブ(ダムハブ)」、つまりリピータハブを取り上げます。ではさっそく実際の流れを見て行きましょう。

===[column] 友太郎の質問：テスト駆動開発とテストファーストの違いは？

「テスト駆動開発」や「テストファースト」など似たような用語に混乱している人も多いと思います。この2つの違いは何でしょうか。

テストファーストはテスト駆動開発のステップの一部なので、テスト駆動開発のほうがより大きな概念になります。テスト駆動開発では、まずは失敗する見込みでテストを書き (このステップがテストファースト)、次にこのテストを通すためのコードを書きます。最後にコードをリファクタリングして、クリーンにします。この3ステップを数分間隔で何度も回しながら開発するのがテスト駆動開発です。

===[/column]

== リピータハブの動き

まずは、リピータハブがどのように動くか説明しましょう。ホスト3台がリピータハブにつながったネットワークを考えてください(@<img>{repeater_hub})。ホスト1からホスト2へパケットを送信すると、リピータハブは入ってきたパケットを複製して他のすべてのホストにばらまいてしまいます。つまり、通信に関係のないホスト3もホスト2あてのパケットを受信します。このように関係のないホストにもパケットを送ってしまうので「バカ」ハブとか「ダム」ハブと呼びます。

//image[repeater_hub][ホスト 3 台をつなげたリピータハブの動作][width=10cm]

これをOpenFlowで実装すると@<img>{repeater_hub_openflow}のようになります。ホスト1がパケットを送信すると、スイッチからコントローラにPacket Inが起こります。ここでコントローラは「今後は同様のパケットを他の全ポートへばらまけ(FLOOD)」というFlow Modを打ちます。また、Packet Inを起こしたホスト1からのパケットを他のすべてのホスト(ホスト2とホスト3)に明示的にPacket Outで届けます。

//image[repeater_hub_openflow][OpenFlow版リピータハブ][width=10cm]

== どこまでテストするか？

設計も決まったので、テストを書き始める前にテスト戦略を決めます。テスト戦略とは言い換えると「どこまでテストするか？」ということです。これはむずかしい問題なので、ソフトウェアテスト界の賢人達の言葉を借りることにします。

テスト駆動開発の第一人者、ケント・ベックは@<tt>{stackoverflow.com}の「どれだけ深くテストをすればいいか？」というトピック@<fn>{stackoverflow}に次の投稿をしています。

//footnote[stackoverflow][@<href>{http://stackoverflow.com/questions/153234/how-deep-are-your-unit-tests}]

#@warn(訳が正しいかチェック)
//quote{
私は動くコードに対してお金をもらっていて、テストコードでもらっているのではない。なので、私の信条は、あるレベルの確信が得られる最低限のテストしかしないというものである(このレベルは産業界の水準からすると高いかもしれないが、これは私の思い上がりかもしれない)。自分がある種のまちがいを犯さないとわかっていれば(コンストラクタで間違った変数をセットするとか)、そのためのテストはしない。テストエラーには意味を持たせたいと思っているので、複雑な条件を持つロジックにはとりわけ注意を払う。チームでコードを書く場合は、チーム全体でまちがいやすい箇所を注意深くテストする、というように戦略を変える。
//}

Ruby on Railsの作者として有名なDavid Heinemeier Hansson氏(以下、DHH)は、彼の勤める37signalsのブログで次のように語っています@<fn>{37signals_blog}。

//footnote[37signals_blog][@<href>{http://37signals.com/svn/posts/3159-testing-like-the-tsa}]

#@warn(訳が正しいかチェック)
//quote{
コードのすべての行にはコストがかかる。それを書くのには時間がかかり、それを更新するのにも時間がかかり、読んで理解するのにも時間がかかるからだ。したがって、テストで得られる利益はそれを書くコストよりも大きくすることが絶対である。やりすぎなテストはその定義からすると間違っている。
//}

2人の言葉をまとめるとこうです。

 * テストのやりすぎはよくない
 * 動くと確信が得られる最低限のテストを書こう
 * その程度はプログラマやチームによって異なる

つまり、「ケースバイケースでやりすぎないように気をつけよう」ということです。

== リピータハブのテスト戦略

では実際に、リピータハブをどこまでテストするかざっくりと決めましょう。最低限しなければいけないテストシナリオはこうなるはずです。

//quote{
@<strong>{シナリオ1}：ホスト1、2、3がスイッチにつながっているとき、ホスト1がホスト2へパケットを送ると、ホスト2、3がパケットを受け取る
//}

人によっては、思ったとおりのフローエントリがスイッチに書き込まれたか気になる人もいるでしょう。その場合の追加のテストシナリオはこうなります。

//quote{
@<strong>{シナリオ2}：ホスト1、2、3がスイッチにつながっているとき、ホスト1がホスト2へパケットを送ると、パケットをホスト2、3へばらまくフローエントリがスイッチに書き込まれる
//}

さきほど言ったように、このシナリオ2を実装するかどうかはプログラマやチームの方針次第です。不安になる人はシナリオ2もテストしてください。OpenFlowに慣れていてシナリオ1だけで十分と言う人は、それでかまいません。以降の説明ではシナリオ1と2を両方実装してみます。

== 最初のテスト

ではさっそく、リピータハブのテストコードを書いていきましょう。TremaのテストフレームワークはRubyのユニットテストツール@<href>{http://rspec.info/, RSpec}と統合されています。まだインストールしていない人は、@<tt>{gem install rspec}でインストールしてください。

テストコードの最初のバージョンは@<list>{repeater_hub_test_template}のとおりです。最初の@<tt>{require}で始まる行は、テストに必要なTremaのライブラリを読み込みます。@<tt>{describe}で始まる@<tt>{do...end}ブロックはテストの本体で、RepeaterHubコントローラのふるまいをここに記述(describe)する、という意味です。

//list[repeater_hub_test_template][リピータハブのテストのひな型(@<tt>{spec/repeater-hub_spec.rb})]{
require File.join( File.dirname( __FILE__ ), "spec_helper" )

describe RepeaterHub do
end
//}

まだ@<tt>{RepeaterHub}クラスを定義していないのでエラーになることはわかりきっていますが、テストを実行してみましょう。次のコマンドを実行すると、Tremaを起動したうえで@<tt>{spec/repeater-hub_spec.rb}(@<list>{repeater_hub_test_template})のテストを実行します。

//cmd{
$ rspec -fs -c ./spec/repeater-hub_spec.rb
.../spec/repeater-hub_spec.rb:3: uninitialized constant RepeaterHub (Name Error)
//}

予想どおり、@<tt>{RepeaterHub}クラスが未定義というエラーで失敗しました。エラーを修正するために、@<tt>{RepeaterHub}クラスの定義を追加してみましょう(@<list>{add_repeater_hub_class})。

//list[add_repeater_hub_class][空の@<tt>{RepeaterHub}クラスを追加して@<tt>{NameError}を修正]{
require File.join( File.dirname( __FILE__ ), "spec_helper" )

class RepeaterHub < Controller # 空のクラスを追加
end

describe RepeaterHub do
end
//}

本来、コントローラクラスは独立した@<tt>{.rb}ファイルに書きますが、今回は簡便さを優先し、テストコード内に直接書いていることに注意してください。

それでは実行してみましょう。今度はパスするはずです。

//cmd{
$ rspec -fs -c spec/repeater-hub_spec.rb
No examples found.
Finished in 0.00003 seconds 0 examples, 0 failures
//}

やりました！これで最初のテストにパスしました。

このようにテスト駆動開発では、最初にテストを書き、わざとエラーを起こしてからそれを直すためのコードをちょっとだけ追加します。テストを実行した結果からのフィードバックを得ながら「テストを書き、コードを直す」を何度もくりかえしつつ最終的な完成形に近づけていくのです。

== パケット受信をテストする

では、リピータハブの動作をテストコードにしていきましょう。最初のテストシナリオを思い出してください。

//quote{
@<strong>{シナリオ1}：ホスト3台(ホスト1、2、3)がスイッチにつながっているとき、ホスト1がホスト2へパケットを送ると、ホスト2、3がパケットを受け取る
//}

テストコードは@<list>{first_test_scenario}のように@<tt>{it}ブロックの中に記述します。"@<tt>{describe RepeaterHub}"の部分と続けて読むと、「it(RepeaterHub)は、入ってきたパケットを他のすべてのポートに転送する」と読めます。このように@<tt>{describe}で指したコンポーネントの仕様(spec)を記述するのがRSpec(Ruby Spec)と呼ばれるゆえんです。

//list[first_test_scenario][テストシナリオの定義]{
describe RepeaterHub do
  it "は、入ってきたパケットを他のすべてのポートに転送する" do
    # テストコードをここに書く
  end
end
//}

テストシナリオをテストコードに置き換えるには、シナリオの各ステップをGiven(前提条件)、When(○○したとき)、Then(こうなる)の3つに分解するとうまく整理できます。

 * Given：ホスト3台(ホスト1、2、3)がスイッチにつながっているとき、
 * When：ホスト1がホスト2へパケットを送ると、
 * Then：ホスト2、3がパケットを受け取る。

では、Given、When、Thenの順にテストコードを書いていきます。

=== Given：ネットワークの構成

シナリオの前提条件(Given)として、テストを実行するホスト3台のネットワーク構成(@<img>{repeater_hub})を@<list>{tdd_given}のように定義します。

//list[tdd_given][テストシナリオを実行するネットワーク構成の定義(Given)]{
describe RepeaterHub do
  it "は、入ってきたパケットを他のすべてのポートに転送する" do
    network { # ホスト3台、スイッチ1台のネットワーク
      vswitch( "switch" ) { dpid "0xabc" }
      vhost( "host1" ) { promisc "on" } # 自分宛ではないパケットも受け取る
      vhost( "host2" ) { promisc "on" }
      vhost( "host3" ) { promisc "on" }
      link "switch", "host1"
      link "switch", "host2"
      link "switch", "host3"
    }
  end
end
//}

#@warn(ネットワーク設定の文法を説明している章へリンク)

Tremaの仮想ネットワーク設定とまったく同じ文法ですね！ここで、それぞれの仮想ホストが@<tt>{promisc}オプション(自分宛でないパケットを受け取る)を@<tt>{on}にしていることに注意してください。リピータハブはパケットをすべてのポートにばらまくので、こうすることでホストがどこからのパケットでも受信できるようにしておきます。

=== When：パケットの送信

#@warn(trema send_packets を説明している章を参照)

Whenは「○○したとき」というきっかけになる動作を記述します。ここでは、Givenで定義されたホストhost1からhost2にパケットを送ります。パケットを送るコマンドは、@<tt>{trema send_packets}でした。もちろん、テストコード中でもこれに似たAPIを使えます(@<list>{tdd_when})。

//list[tdd_when][テストパケットを送信(When)]{
describe RepeaterHub do
  it "は、入ってきたパケットを他のすべてのポートに転送する" do
    network {
      vswitch( "switch" ) { dpid "0xabc" }
      vhost( "host1" ) { promisc "on" }
      vhost( "host2" ) { promisc "on" }
      vhost( "host3" ) { promisc "on" }
      link "switch", "host1"
      link "switch", "host2"
      link "switch", "host3"
    }.run( RepeaterHub ) {
      # host1からhost2へテストパケットをひとつ送信
      send_packets "host1", "host2"
    }
  end
end
//}

@<tt>{run(RepeaterHub)} は、Givenで定義されたネットワークの上で@<tt>{RepeaterHub}コントローラを動かし、続くブロックを実行するという意味です。

=== Then：受信したパケットの数

Thenには「最終的にこうなるはず」というテストを書きます。ここでは、「ホスト2、3がパケットを受け取るはず」を書けばよいですね(@<list>{tdd_then})。

//list[tdd_then][受信パケット数のテスト(Then)]{
describe RepeaterHub do
  it "は、入ってきたパケットを他のすべてのポートに転送する" do
    network {
      vswitch( "switch" ) { dpid "0xabc" }
      vhost( "host1" ) { promisc "on" }
      vhost( "host2" ) { promisc "on" }
      vhost( "host3" ) { promisc "on" }
      link "switch", "host1"
      link "switch", "host2"
      link "switch", "host3"
    }.run( RepeaterHub ) {
      send_packets "host1", "host2"

      # host2とhost3がひとつずつパケットを受け取る
      vhost( "host2" ).stats( :rx ).should have( 1 ).packets
      vhost( "host3" ).stats( :rx ).should have( 1 ).packets
    }
  end
end
//}

@<tt>{vhost("ホスト名")}は仮想ホストにアクセスするためのメソッドで、仮想ホストの受信パケットなどさまざまなデータを取得できます。ここでは、受信したパケットの数、つまり受信パケットカウンタ@<tt>{stats(:rx)}が1ということをテストしています。

=== テストの実行

ではさっそく実行してみましょう。

//cmd{
Failure/Error: vhost("host2").stats(:rx).should have(1).packets
expected 1 packets, got 0
//}

失敗しました。「host2 はパケットを1つ受信するはずが、0個だった」というエラーです。@<tt>{RepeaterHub}クラスの中身をまだ実装していないので当たり前ですね。すぐにはなおせそうにないので、ひとまずこのテストは保留(pending)とし、あとで復活することにしましょう(@<list>{tdd_pending})。

//list[tdd_pending][すぐに修正できないテストを保留(pending)にする]{
describe RepeaterHub do
  it "は、入ってきたパケットを他のすべてのポートに転送する" do
    pending "あとで実装する" # この行を追加する
    network {
      vswitch( "switch" ) { dpid "0xabc" }
      vhost( "host1" ) { promisc "on" }
      vhost( "host2" ) { promisc "on" }
      vhost( "host3" ) { promisc "on" }
      link "switch", "host1"
      link "switch", "host2"
      link "switch", "host3"
    }.run( RepeaterHub ) {
      send_packets "host1", "host2"

      # host2とhost3がひとつずつパケットを受け取る
      vhost( "host2" ).stats( :rx ).should have( 1 ).packets
      vhost( "host3" ).stats( :rx ).should have( 1 ).packets
    }
  end
end
//}

今度は実行結果が次のように変わり、エラーが出なくなります。

//cmd{
Pending：
1) は、入ってきたパケットを他のすべてのポートに転送する
# あとで実装する
//}

ここで分かれ道です。このテストシナリオ1だけで十分な人は@<tt>{RepeaterHub}本体の実装に進んでください。さらに段階を踏んでテストを書いたほうがスッキリする人は、次のテストシナリオ2を実装します。

== フローエントリのテスト

スイッチにフローエントリができることをテストしてみましょう。さっそくテストシナリオ2をGiven、When、Thenに分解すると次のようになります。

 * Given：ホスト1、2、3がスイッチにつながっているとき、
 * When：ホスト1がホスト2へパケットを送ると、
 * Then：パケットをばらまくフローエントリをスイッチに追加する。

GivenとWhenは最初のテストシナリオと同じで、Thenだけが異なります。では、これをテストコードにしてみましょう。パケットをばらまくアクションはFLOODですので@<list>{test_flow_entry}のようになります。

#@warn(比較対象が "FLOOD" と文字列なのがなんか変。シンボルとかクラス名にすべき？)

//list[test_flow_entry][フローエントリのテスト]{
describe RepeaterHub do
  it "は、入ってきたパケットを他のすべてのポートに転送する" do
    # (省略)
  end


  it "は、パケットをばらまくフローエントリをスイッチに追加する" do
    network {
      vswitch( "switch" ) { dpid "0xabc" }
      vhost( "host1" ) { promisc "on" }
      vhost( "host2" ) { promisc "on" }
      vhost( "host3" ) { promisc "on" }
      link "switch", "host1"
      link "switch", "host2"
      link "switch", "host3"
    }.run( RepeaterHub ) {
      send_packets "host1", "host2"
      vswitch( "switch" ).should have( 1 ).flows
      vswitch( "switch" ).flows.first.actions.should == "FLOOD"
    }
  end
end
//}

ネットワーク構成のコード(@<tt>{network {...\}}の部分)をコピー＆ペーストしてしまっていますが、あとできれいにするので気にしないでください。エラーになることを見越しつつ、さっそく実行すると、次のエラーになります。

//cmd{
Failure/Error: vswitch("switch").should have(1).flows
expected 1 flows, got 0
//}

「スイッチにフローエントリが1つあるはずが、なかった」というエラーです。では、Flow Modを打ち込むコードを@<tt>{RepeaterHub}クラスに追加して、もう一度テストしてみましょう(@<list>{tdd_sending_flow_mod})。

//list[tdd_sending_flow_mod][Flow Modをスイッチに打ち込む]{
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

別のエラーになりました。「アクションが"FLOOD"でなく"drop"だった」と怒られています。たしかに、さきほどのFlow Modにはアクションを設定していなかったので、デフォルトのアクションであるdrop(パケットを破棄する) になってしまっています。Flow Modにパケットをばらまくアクションを定義してみましょう(@<list>{tdd_adding_action_to_flow_mod})。

//list[tdd_adding_action_to_flow_mod][Flow Modにアクションを追加]{
class RepeaterHub < Controller
  def packet_in dpid, message
    send_flow_mod_add(
      dpid,
      :actions => ActionOutput.new( OFPP_FLOOD )
    )
  end
end
//}

今度はテストが通りました！それでは、もう少しThenを詳細化し、フローエントリの特徴を細かくテストしてみます(@<list>{tdd_test_src_dst})。

//list[tdd_test_src_dst][フローエントリの@<tt>{nw_src}と@<tt>{nw_dst}のテストを追加]{
describe RepeaterHub do
  it "は、入ってきたパケットを他のすべてのポートに転送する" do
    # (省略)
  end


  it "は、パケットをばらまくフローエントリをスイッチに追加する" do
    network {
      vswitch( "switch" ) { dpid "0xabc" }
      vhost( "host1" ) { promisc "on"; ip "192.168.0.1" }
      vhost( "host2" ) { promisc "on"; ip "192.168.0.2" }
      vhost( "host3" ) { promisc "on"; ip "192.168.0.3" }
      link "switch", "host1"
      link "switch", "host2"
      link "switch", "host3"
    }.run( RepeaterHub ) {
      send_packets "host1", "host2"
      vswitch( "switch" ).should have( 1 ).flows
      flow = vswitch( "switch" ).flows.first
      flow.actions.should == "FLOOD"
      flow.nw_src.should == "192.168.0.1"
      flow.nw_dst.should == "192.168.0.2"
    }
  end
end
//}

ここではホストにIPアドレスを振り、フローエントリの@<tt>{nw_src}と@<tt>{nw_dst}がこのアドレスに正しく設定されているかをチェックしています。実行してみましょう。

//cmd{
Failure/Error: flow.nw_src.should == "192.168.0.1"
  expected: "192.168.0.1"
    got: nil (using ==)
//}

失敗しました。フローエントリの@<tt>{nw_src}には、パケット送信元であるhost1のIPアドレス192.168.0.1がセットされるべきですが、何もセットされていません。では、Flow Modで@<tt>{:match}を指定して、この値がセットされるようにします(@<list>{tdd_set_match_to_flow_mod})。

//list[tdd_set_match_to_flow_mod][Flow Modメッセージに@<tt>{:match}をセット]{
class RepeaterHub < Controller
  def packet_in dpid, message
    send_flow_mod_add(
      dpid,
      :match => ExactMatch.from( message ),
      :actions => ActionOutput.new( OFPP_FLOOD )
    )
  end
end
//}

テストにパスしました！ここまでやれば、フローエントリが正しくスイッチに書き込まれていることに確信が持てます。

=== テストコードのリファクタリング

テストが通ったので、最後にコードの重複部分をまとめておきましょう。同じ@<tt>{network {...\}}が重複しているので、RSpecの@<tt>{around}ブロックを使って1か所にまとめます(@<list>{tdd_refactoring})。

//list[tdd_refactoring][共通部分を@<tt>{around}ブロックに移すことでコードの重複をなくす]{
describe RepeaterHub do
  around do | example |
    # 共通のセットアップをここにまとめる
    network {
      vswitch( "switch" ) { dpid "0xabc" }
      vhost( "host1" ) { promisc "on"; ip "192.168.0.1" }
      vhost( "host2" ) { promisc "on"; ip "192.168.0.2" }
      vhost( "host3" ) { promisc "on"; ip "192.168.0.3" }
      link "switch", "host1"
      link "switch", "host2"
      link "switch", "host3"
    }.run(RepeaterHub) {
      # exampleがシナリオ(itブロック)に相当。
      # 次の行でそれぞれのシナリオを実行する
      example.run  
    }
  end

  it "は、入ってきたパケットを他のすべてのポートに転送する" do
    send_packets "host1", "host2"
    vhost( "host2" ).stats( :rx ).should have( 1 ).packets
    vhost( "host3" ).stats( :rx ).should have( 1 ).packets
  end

  it "は、パケットをばらまくフローエントリをスイッチに追加する" do
    send_packets "host1", "host2"
    vswitch( "switch" ).should have( 1 ).flows
    vswitch( "switch" ).flows.first.actions.should == "FLOOD"
  end
end
//}

== 再びパケットの受信をテスト

いよいよ完成も間近です。テストシナリオ1に戻り、パケットがhost2とhost3に届くことをテストします。さきほどの保留マーク(pending)を消して、再び実行してみましょう。

//cmd{
Failure/Error: vhost( "host2" ).stats( :rx ).should have( 1 ).packets
  expected 1 packets, got 0
//}

失敗してしまいました。host2がパケットを受信できていません。そういえば、Flow Modしただけではパケットは送信されないので、明示的にPacket Outしてやらないといけないのでしたね。というわけでPacket Outを追加します(@<list>{tdd_add_packet_out})。

#@warn(「明示的に Packet Out」する章へのリンク)

//list[tdd_add_packet_out][@<tt>{RepeaterHub}にPacket Outを追加]{
class RepeaterHub < Controller
  def packet_in dpid, message
    send_flow_mod_add(
      dpid,
      :match => ExactMatch.from( message ),
      :actions => ActionOutput.new( OFPP_FLOOD )
    )
    send_packet_out(
      dpid,
      :packet_in => message,
      :actions => ActionOutput.new( OFPP_FLOOD )
    )
  end
end
//}

さっそく実行してみましょう。

//cmd{
RepeaterHub
  は、入ってきたパケットを他のすべてのポートに転送する
  は、パケットをばらまくフローエントリをスイッチに追加する
Finished in 15.66 seconds
2 examples, 0 failures
//}

すべてのテストに通りました！これでリピータハブとテストコード一式が完成です。このテストコードの実行結果は、RepeaterHubの仕様書としても読めますね。

== まとめ

Tremaのユニットテストフレームワークを使ってリピータハブを作り、コントローラをテスト駆動開発する方法を学びました。今回学んだことは次の2つです。

 * TremaはRubyのユニットテストフレームワークRSpecと統合されており、仮想スイッチのフローテーブルや仮想ホストの受信パケット数などについてのテストを書ける
 * テストをGiven、When、Thenの3ステップに分けて分析し設計する方法を学んだ。それぞれのステップをRSpecのテストコードに置き換えることで、テストコードが完成する

Tremaのサンプルディレクトリ(@<tt>{src/examples})の下には他にもテストコードのサンプルがいくつかあります。本格的にテストコードを書く人は参考にしてください。

== 参考文献

: 『テスト駆動開発入門』(Kent Beck著／ピアソン・エデュケーション)
  ケント・ベック自身によるバイブルです。もったいないことに日本語版の訳がまずく、意味の通らないところがたくさんあります。私たちは仲間との勉強会に原著を使いましたが、わかりやすい英語だったので問題ありませんでした。

: 「stackoverflow」(@<href>{http://stackoverflow.com/})
  私はテストに限らずプログラミングでわからないことがあると、このQ&Aサイトを検索します。ユーザーは役に立つ回答には点数をつけることができ、またケント・ベックなど有名人が回答してくれることが多いので、質の高い回答がそろっています。この章で紹介したケント・ベックの投稿にはなんと200ポイント以上の点数がついていました!

: 「Signal vs. Noise」(@<href>{http://37signals.com/svn})
  Ruby on Railsを作った小さな会社37signalsのブログです。この章で紹介したDHHのテスト論など、有名ハッカーの生の声が読めます。記事それぞれが非常に刺激的で(一部)過激な意見にみちあふれています。

: 『リファクタリング』(Martin Fowler著／ピアソン・エデュケーション)
  この本の最大の功績は、コードのまずい兆候(重複するコードがあるとか、長すぎるメソッドなど)を「コードの臭い」と表現したことです。粗相をした赤ちゃんのおむつのように臭うコードには改善が必要で、この本にはそのためのレシピがそろっています。この本はJavaですが、Ruby版(『リファクタリング：Rubyエディション』Jay Fields、Shane Harvie、Martin Fowler、Kent Beck著／アスキー・メディアワークス)もあります。

: 「reek」(@<href>{https://github.com/troessner/reek})
  「コードの臭い」を検知する能力はプログラマの美意識にいくらか依存しますが、ソフトウェアで客観的に検知できるとしたらすばらしいと思いませんか。reekはRubyコードの臭いを自動的に検知して改善すべき場所を教えてくれる便利なツールです。次に紹介する「flog」「flay」とともに、本書のサンプルコードを書く際にとてもお世話になりました。

: 「flog」(@<href>{http://ruby.sadi.st/Flog.html})
  「Rubyサディストのためのツール」と銘打ったこのツールは、すべてのメソッドがどのくらい複雑かを客観的なポイントで表示してくれます(大きいほど複雑でテストしづらい)。Tremaのコードでは目安としてこれが10ポイントをこえないようにしています。

: 「flay」(@<href>{http://ruby.sadi.st/Flay.html})
  この「Rubyサディストのためのツール その2」は、重複するコードを探して容赦なく指摘してくれます。DRY(Don't repeat yourself)を目指すならこのツールを使って重複を完璧になくすべきです。
