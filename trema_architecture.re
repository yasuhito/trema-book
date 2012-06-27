= Trema のアーキテクチャ

3 月にしてはあたたかい夜更け、@<ruby>{友太郎,ゆうたろう}君が自宅の書斎
にていつものようにコーディングを楽しんでいたときのこと。ふいに、玄関の
外から聞き覚えのある声。@<br>{}

//noindent
@<em>{謎の声} 「もしもし、友太郎君はおるかの？」@<br>{}
@<em>{友太郎君} 「あっ! @<ruby>{取間,とれま}先生じゃないですか。こんな夜中
にどうされたんですか？」@<br>{}
@<em>{取間先生} 「三軒茶屋で飲んでいたら、愛用のサンダルが無くなってしまっ
てのう。必死に探したんじゃが、どうやら別の客が間違えて履いて行ってしまっ
たようじゃ。そのおかげで終電まで無くしてしもうたわい! ワッハッハ」@<br>{}

//noindent
この取間先生、飲んでいて陽気ですが足元を見るとたしかに裸足です。それに
着物のすそもやたらと汚れています。どうやら酔った勢いだけで、友太郎君の
住む武蔵小杉まではるばる歩いてきてしまったようです。@<br>{}

//noindent
@<em>{取間先生} 「そこで悪いのじゃが友太郎君、今夜はこのみじめな老人を
泊めてくれんか？お礼と言ってはなんじゃが、始発まで時間はたっぷりあるか
ら Trema についてまだ話してなかったことをすべて教えよう。友太郎君は最近
 Trema でいろいろとアプリを書いているようだし、いろいろ聞きたい事もある
だろうと思ってな」@<br>{}
@<em>{友太郎君} 「ぜひそのお話聞きたいです! さあさどうぞお上がりください」

== @<tt>{trema run} の裏側

//noindent
@<em>{取間先生} 「では基本的な話から。Trema でコントローラを動かすには
 @<tt>{trema run} じゃったな (@<chap>{openflow_framework_trema})。この
コマンド、実は裏でいろんなプロセスを起動しているんじゃ。具体的に何を
やっているか、@<tt>{trema run} に@<tt>{-v} オプションをつけると見るこ
とができるぞ。さっそく、サンプルに付いているラーニングスイッチ
 (@<chap>{learning_switch}) を試しに起動してみてごらん」

//cmd{
% ./trema run learning-switch.rb -c learning_switch.conf -v
.../switch_manager --daemonize --port=6633 \
  -- port_status::LearningSwitch packet_in::LearningSwitch \
  state_notify::LearningSwitch vendor::LearningSwitch
sudo ip link delete trema0-0 2>/dev/null
sudo ip link delete trema1-0 2>/dev/null
sudo ip link add name trema0-0 type veth peer name trema0-1
sudo sysctl -w net.ipv6.conf.trema0-0.disable_ipv6=1 >/dev/null 2>&1
sudo sysctl -w net.ipv6.conf.trema0-1.disable_ipv6=1 >/dev/null 2>&1
sudo /sbin/ifconfig trema0-0 up
sudo /sbin/ifconfig trema0-1 up
sudo ip link add name trema1-0 type veth peer name trema1-1
sudo sysctl -w net.ipv6.conf.trema1-0.disable_ipv6=1 >/dev/null 2>&1
sudo sysctl -w net.ipv6.conf.trema1-1.disable_ipv6=1 >/dev/null 2>&1
sudo /sbin/ifconfig trema1-0 up
sudo /sbin/ifconfig trema1-1 up
sudo .../phost/phost -i trema0-1 -p .../trema/tmp/pid -l .../trema/tmp/log -D
sudo .../phost/cli -i trema0-1 set_host_addr --ip_addr 192.168.0.1 \
  --ip_mask 255.255.0.0 --mac_addr 00:00:00:01:00:01
sudo .../phost/phost -i trema1-1 -p .../trema/tmp/pid -l .../trema/tmp/log -D
sudo .../phost/cli -i trema1-1 set_host_addr --ip_addr 192.168.0.2 \
  --ip_mask 255.255.0.0 --mac_addr 00:00:00:01:00:02
sudo .../openvswitch/bin/ovs-openflowd --detach --out-of-band --fail=closed \
  --inactivity-probe=180 --rate-limit=40000 --burst-limit=20000 \
  --pidfile=.../trema/tmp/pid/open_vswitch.lsw.pid --verbose=ANY:file:dbg \
  --verbose=ANY:console:err --log-file=.../trema/tmp/log/openflowd.lsw.log \
  --datapath-id=0000000000000abc --unixctl=.../trema/tmp/sock/ovs-openflowd.lsw.ctl \
   --ports=trema0-0,trema1-0 netdev@vsw_0xabc tcp:127.0.0.1:6633
sudo .../phost/cli -i trema0-1 add_arp_entry --ip_addr 192.168.0.2 \
  --mac_addr 00:00:00:01:00:02
sudo .../phost/cli -i trema1-1 add_arp_entry --ip_addr 192.168.0.1 \
  --mac_addr 00:00:00:01:00:01
//}

//noindent
@<em>{友太郎君} 「うわっ! なんだかいっぱい字が出てきましたね」@<br>{}
@<em>{取間先生} 「この出力を見れば、@<tt>{trema run} コマンドが裏でどんなこ
とをしてくれているのかスグわかるのじゃ。たとえばログの最初を見ると、
@<tt>{switch_manager} というプロセスが起動されておるな？ Trema ではこの
プロセスがスイッチと最初に接続するんじゃ。」

#@warn(port_status:: とかの引数が Switch Manager に渡ることの説明)

== Switch Manager

Switch Manager (@<tt>{[trema]/src/switch_manager/}) はスイッチからの接
続要求を待ち受けるデーモンです。スイッチとの接続を確立すると、子プロセ
スとして Switch Daemon プロセスを起動し、スイッチとの接続をこのプロセス
へ引き渡 します (@<img>{switch_manager_daemon})。@<br>{}

//image[switch_manager_daemon][スイッチからの接続の受付]

//noindent
@<em>{友太郎君}「うーむ。いきなりむずかしいです」@<br>{}
@<em>{取間先生}「そんなことはないぞ、ぜんぜん簡単じゃ。Switch Manager
の役割は言わば inetd と同じと考えればよい。つまり Switch Manager は新し
いスイッチが接続してくるのを見張っているだけで、実際のスイッチとのやり
とりはスイッチごとに起動する Switch Daemon プロセスに一切合切まかせてし
まうというわけじゃ」@<br>{}
@<em>{友太郎君}「なるほど! たしかに仕組みが inetd とよく似ていますね」

== Switch Daemon

Switch Daemon (@<tt>{[trema]/src/switch_manager/}) は、Switch Manager
が確立したスイッチとの接続を引き継ぎ、スイッチと Trema 上のアプリケーショ
ンプロセスとの間で流れる OpenFlow メッセージを仲介します
(@<img>{switch_daemon})。

//image[switch_daemon][スイッチと Trema アプリケーションの間で OpenFlow メッセージを仲介する Switch Daemon][scale=0.3]

 * アプリケーションプロセスが生成した OpenFlow メッセージをスイッチへ
   配送する
 * スイッチから受信した OpenFlow メッセージをアプリケーションプロセス
   へ届ける

=== OpenFlow メッセージの検査

Switch Daemon の重要な仕事として、OpenFlow メッセージの検査があります。
Switch Daemon は Trema アプリケーションとスイッチの間で交換される
OpenFlow メッセージの中身をすべて検査します。そして、もし不正なメッセー
ジを発見するとログファイル
(@<tt>{[trema]/tmp/log/switch.[接続しているスイッチの Datapath ID].log})
にエラーを出します。@<br>{}

//noindent
@<em>{友太郎} 「Switch Manager と Switch Daemon プロセスでの役割分担とか、
スイッチごとに Switch Daemon プロセスが起動するところなど、いかにも
UNIX っぽいですね。」@<br>{}
@<em>{取間先生} 「うむ。そうしたほうがひとつひとつのデーモンが単純化でき
て、実装も簡単になるからな」@<br>{}
@<em>{友太郎} 「送信するメッセージを厳密にチェックするのはいいと思うん
ですが、受信もチェックするのってやりすぎではないですか？ほら、"送信する
ものに関しては厳密に、受信するものに関しては寛容に@<fn>{postel}" って言う
じゃないですか」@<br>{}
@<em>{取間先生} 「受信メッセージのチェックをあまり寛容にしすぎると、後々
とんでもないことが起こるのじゃ。Trema の開発者が言っておったが、昔ある
ネットワーク機器の開発でそれをやって相互接続テストでひどい目に遭ったそ
うじゃよ。それに、Trema は受信メッセージもちゃんとチェックするようにし
たおかげで、実際に助かったことがたくさんあったのじゃ。たとえば
OpenFlow の標準的なベンチマークツール cbench
(@<tt>{http://www.openflow.org/wk/index.php/Oflops}) のバグを発見したの
も Trema チームだし、2012 年の Interop での OpenFlow 相互接続では様々な
ベンダのスイッチのバグ発見に Trema が一役買っ たそうじゃ
(@<img>{trema_interop})」@<br>{}
@<em>{友太郎} 「へえー! すごいですね!」

//footnote[postel][TCP を規定した RFC 793 において、ジョン・ポステルが "相互運用性を確保するために TCP の実装が持つべき性質" として要約した、いわゆる堅牢さ原則のこと。]

//image[trema_interop][2012 年 Interop で活躍した Trema ベースの RISE Controller。各ベンダの OpenFlow スイッチとの相性問題をたくさん解決した。(NICT 石井秀治さん提供)][scale=0.6]

#@warn(パディングを 0 で埋めている話を、取間先生コラムとしてここに追加)

===[column] 取間先生曰く: きれいな OpenFlow メッセージを作る

OpenFlow メッセージフォーマットは C の構造体で定義されていて、ところど
ころパディング (詰め物) 領域がある。このパディング領域はまったく使われ
ないので、実際は何が入っていても動く。しかしデバッグで OpenFlow メッセー
ジを 16 進ダンプしたときなど、ここにゴミが入っていると大変見づらいものじゃ。

そこで Switch Daemon は送信・受信する OpenFlow メッセージのパディング部
分をすべてゼロで埋め、きれいな OpenFlow メッセージにして送ってあげると
いう地道なことをやっておる。これに気付く者は少ないが、こういう見えない
工夫こそが Trema の品質なのじゃ。

===[/column]

=== スイッチの仮想化

Switch Daemon のもうひとつの重要な役割がスイッチの仮想化です。実は、
Trema ではひとつのスイッチにいくつものアプリケーションをつなげることが
できます。このとき、アプリケーションの間でスイッチ制御を調停し、競合が
起きないようにするのも Switch Daemon の大きな役割です
(@<img>{switch_virtualization})。

//image[switch_virtualization][Switch Daemon はスイッチ制御を調停することでアプリ間の競合を防ぐ]

ひとつ例を挙げましょう。OpenFlow の仕様に Flow Cookie という便利な機能
があります。この Cookie を Flow Mod を打つときのパラメータとして指定す
ると、同じ Cookie を持つフローエントリどうしを 1 つのグループとしてまと
めて管理できます。Trema でこの Cookie を使った場合、よく調べるとアプリ
ケーションから見た Cookie 値と、スイッチのフローテーブルに実際に設定さ
れた Cookie 値が異なることがわかります。

これは Switch Daemon がアプリケーション間で Cookie 値の競合が起こらない
ように調停しているからです。たとえば、Trema 上で動くアプリケーション A
と B がたまたま同じ Cookie 値を使おうとするとどうなるでしょうか。そのま
まだと、両者が混じってしまい混乱します。

Switch Daemon は、こうした Cookie 値の重複を避けるための変換を行います。
つまり、アプリケーションからスイッチに指定する Cookie 値と、逆にスイッ
チがアプリケーションに通知する Cookie がぶつからないように自動変換しま
す (@<img>{flow_cookie_virtualization})。この変換のおかげで、プログラマ
は ほかのアプリケーションと Cookie 値がかぶらないように注意する必要がな
くなります。

//image[flow_cookie_virtualization][Cookie 値を自動変換してアプリ間の競合を防ぐ]

このほかにも、Switch Daemon は OpenFlow メッセージのトランザクション
ID も自動で変換します。まさに Trema の世界の平和を守る縁の下の力持ちと
言えます。

== 仮想ネットワーク

//noindent
@<em>{友太郎}「Trema でなにがうれしいかって、仮想ネットワークの機能です
よね! おかげでノート PC 1 台で開発できるからすっごく楽なんですけど、あ
れってどういうしくみなんですか？」@<br>{}
@<em>{取間先生}「むずかしいことはしておらん。Linux カーネルの標準機能を
使って、仮想スイッチプロセスと仮想ホストプロセスをつなげているだけじゃ」@<br>{}
@<em>{友太郎}「？」

=== 仮想スイッチ

仮想スイッチの実体は、フリーの OpenFlow スイッチ実装である Open
vSwitch (@<tt>{http://openvswitch.org/}) です。@<tt>{trema run} コマン
ドに与えられた仮想ネットワーク設定ファイル中の仮想スイッチ定義
(@<list>{vswitch}) に従って、Trema はスイッチプロセスを必要な数だけ起動
します。

//list[vswitch][仮想ネットワーク設定ファイル中の仮想スイッチ定義例]{
vswitch("lsw") {
  datapath_id "0xabc"
}
//}

これに対応する @<tt>{trema run} のログはこうなります。

//cmd{
% ./trema run learning-switch.rb -c learning_switch.conf -v
  ...
sudo .../openvswitch/bin/ovs-openflowd --detach --out-of-band --fail=closed \
  --inactivity-probe=180 --rate-limit=40000 --burst-limit=20000 \
  --pidfile=.../trema/tmp/pid/open_vswitch.lsw.pid --verbose=ANY:file:dbg \
  --verbose=ANY:console:err --log-file=.../trema/tmp/log/openflowd.lsw.log \
  --datapath-id=0000000000000abc --unixctl=.../trema/tmp/sock/ovs-openflowd.lsw.ctl \
   --ports=trema0-0,trema1-0 netdev@vsw_0xabc tcp:127.0.0.1:6633
  ...
//}

=== 仮想ホスト

仮想ホストの実体は、phost と呼ばれるユーザレベルプロセスです
(@<tt>{vendor/phost/})。これは、任意のイーサネットフレーム・UDP/IP パケッ
トを送受信できます。@<tt>{trema run} コマンドに与えられた設定ファイル中
の仮想ホスト定義 (@<list>{vswitch}) に従って、Trema は必要な数のphost
プロセスを起動します。

//list[vhost][仮想ネットワーク設定ファイル中の仮想ホスト定義例]{
vhost("host1") {
  ip "192.168.0.1"
  netmask "255.255.0.0"
  mac "00:00:00:01:00:01"
}

vhost("host2") {
  ip "192.168.0.2"
  netmask "255.255.0.0"
  mac "00:00:00:01:00:02"
}
//}

これに対応する @<tt>{trema run} のログはこうなります。

//cmd{
% ./trema run learning-switch.rb -c learning_switch.conf -v
  ...
sudo .../phost/phost -i trema0-1 -p .../trema/tmp/pid -l .../trema/tmp/log -D
sudo .../phost/cli -i trema0-1 set_host_addr --ip_addr 192.168.0.1 \
  --ip_mask 255.255.0.0 --mac_addr 00:00:00:01:00:01
sudo .../phost/phost -i trema1-1 -p .../trema/tmp/pid -l .../trema/tmp/log -D
sudo .../phost/cli -i trema1-1 set_host_addr --ip_addr 192.168.0.2 \
  --ip_mask 255.255.0.0 --mac_addr 00:00:00:01:00:02
  ...
sudo .../phost/cli -i trema0-1 add_arp_entry --ip_addr 192.168.0.2 \
  --mac_addr 00:00:00:01:00:02
sudo .../phost/cli -i trema1-1 add_arp_entry --ip_addr 192.168.0.1 \
  --mac_addr 00:00:00:01:00:01
//}

=== 仮想リンク

仮想スイッチと仮想ホストを接続する仮想リンクの実体は、Linux が標準で提
供する Virtual Ethernet Device です。これは、Point-to-Point のイーサネッ
トリンクを仮想的に構成してくれるものです。@<tt>{trema run} コマンドに与
えられた仮想ネットワーク設定ファイル中の仮想リンク定義
(@<list>{vlink}) に従って、Trema は必要な数の仮想リンクを作ります。

//list[vlink][仮想ネットワーク設定ファイル中の仮想リンク定義例]{
link "lsw", "host1"
link "lsw", "host2"
//}

これに対応する @<tt>{trema run} のログはこうなります。

//cmd{
% ./trema run learning-switch.rb -c learning_switch.conf -v
  ...
sudo ip link delete trema0-0 2>/dev/null
sudo ip link delete trema1-0 2>/dev/null
sudo ip link add name trema0-0 type veth peer name trema0-1
sudo sysctl -w net.ipv6.conf.trema0-0.disable_ipv6=1 >/dev/null 2>&1
sudo sysctl -w net.ipv6.conf.trema0-1.disable_ipv6=1 >/dev/null 2>&1
sudo /sbin/ifconfig trema0-0 up
sudo /sbin/ifconfig trema0-1 up
sudo ip link add name trema1-0 type veth peer name trema1-1
sudo sysctl -w net.ipv6.conf.trema1-0.disable_ipv6=1 >/dev/null 2>&1
sudo sysctl -w net.ipv6.conf.trema1-1.disable_ipv6=1 >/dev/null 2>&1
sudo /sbin/ifconfig trema1-0 up
sudo /sbin/ifconfig trema1-1 up
  ...
//}

== Trema C ライブラリ

@<em>{友太郎} 「そういえば、Trema って実は C からも使えるらしいじゃない
ですか。せっかくだから、どんなライブラリがあるか教えてくれますか」@<br>{}
@<em>{取間先生} 「よかろう。しかしわしはちょっと飲み足りないので、一緒に
ビールでも買いに行かんか？ C ライブラリもなにしろ数がたくさんあるから、
歩きながらひとつずつ説明することにしよう」@<br>{}
@<em>{友太郎} 「まだ飲むんだ…」@<br>{}
@<em>{取間先生} 「なんじゃとお!」@<br>{}

酔っ払いに怒られることほどみじめなことはありません。でも友太郎君は取間
先生の話が聞きたいので仕方無くコンビニまでつきあいます。

=== OpenFlow Application Interface

OpenFlow メッセージを受信したときにアプリケーションのハンドラを起動した
り、逆にアプリケーションが送信した OpenFlow メッセージを適切なSwitch
Daemon に配送したりするのが OpenFlow Application Interface
(@<tt>{[trema]/src/lib/openflow_application_interface.c}) です。

OpenFlow Application Interface は、アプリケーションが受信した OpenFlow
メッセージをプログラマが扱いやすいに変換してハンドラに渡します。たとえ
ば、次の処理を行います。

 * OpenFlow メッセージのマッチングルール部をホストバイトオーダへ変換
 * アクションなどの可変長部分をリストに変換
 * Packet In メッセージに含まれるイーサネットフレームを解析し、フレーム
   と解析結果 (パケットの種類など) を組にしてハンドラへ渡す

//noindent
@<em>{友太郎} 「なるほど。Trema の OpenFlow API が使いやすい理由って、裏
でこういう変換をたくさんやってくれているからなんですね。たしかにバイト
オーダ変換とかを意識しないで書けるのってすごく楽です」@<br>{}
@<em>{取間先生} 「そうそう。他の OpenFlow フレームワークでここまで親切な
ものは無いぞ。Trema はこうやってプログラマの凡ミスによるバグを減らしてい
るんじゃ」

=== OpenFlow Messages

アプリケーションが OpenFlow メッセージを生成するときに使うのが、
OpenFlow Messages (@<tt>{[trema]/src/lib/openflow_message.c}) です。メッセージ
受信の場合とは逆に、

 * ホストバイトオーダで指定された値をネットワークバイトオーダで変換し、
   OpenFlow メッセージのヘッダへ格納
 * リストの形で与えられた値を可変長ヘッダへ格納  

などを行います。

また、不正なメッセージの生成を防ぐため、パラメータ値の範囲検査やフラグ
の検査など OpenFlow 仕様と照らし合わせた厳密なチェックをここでやります。
生成したメッセージは、OpenFlow Application Interface が提供するメッセー
ジ送信 API を用いて、Switch Daemon を介してスイッチへ送信します。@<br>{}

//noindent
@<em>{友太郎} 「チェックが厳しいですね!」@<br>{}
@<em>{取間先生} 「これはさっきも言ったように、Trema をいろいろなベンダの
スイッチと相互接続したときに相手に迷惑をかけないための最低限の礼儀じゃ。
逆に言うと、Trema と問題無くつながるスイッチは正しい OpenFlow 1.0 をしゃ
べることができる、とも言えるな」@<br>{}
@<em>{友太郎} 「かっこいい!」

=== パケットパーサ

Packet In メッセージに含まれるイーサネットフレームを解析したり、解析結
果を参照する API を提供したりするのが、パケットパーサ
(@<tt>{[trema]/src/lib/packet_parser.c}) です。パケットが TCP なのか UDP なのか
といったパケットの種類の判別や、MAC や IP アドレスといったヘッダフィー
ルド値の参照を容易にしています。

=== プロセス間通信

Switch Daemon とユーザのアプリケーション間の OpenFlow メッセージのやり
とりなど、プロセス間の通信には @<tt>{[trema]/src/lib/messenger.c} で定
義されるプロセス間通信 API が使われます。

=== 基本データ構造

その他、C ライブラリでは基本的なデータ構造を提供しています。たとえば、
イーサネットフレームを扱うための可変長バッファ
(@<tt>{[trema]/src/lib/buffer.c})、アクションリストなどを入れるための連
結リスト (@<tt>{[trema]/src/lib/{linked_list.c,doubly_linked_list.c}})、
FDB などに使うハッシュテーブル (@<tt>{[trema]/src/lib/hash_table.c}) な
どです。ただし、Ruby には標準でこれらのデータ構造があるため、Ruby ライ
ブラリでは使われません。@<br>{}

//noindent
@<em>{取間先生} 「Trema の Ruby ライブラリも、この堅牢な C ライブラリの
上に構築されておる。@<tt>{[trema]/ruby/} ディレクトリの中を見てみるとわ
かるが、Ruby ライブラリの大半は C で書かれておるのだ。そのお影でこうし
た各種チェックが Ruby からも利用できる」@<br>{}
@<em>{友太郎} 「へー! あ、家に着きましたね」

== 低レベルデバッグツール Tremashark

コンビニから帰ってきましたが、深夜の二人のTrema 談義はさらに続きます。@<br>{}

//noindent
@<em>{取間先生} 「こうして見ると Trema って意外と複雑じゃろう。もし友太
郎君がさらに Trema をハックしてみたいとして、ツールの手助け無しにやるの
はちょっとたいへんだと思うから、いいツールを紹介してあげよう。これは
Tremashark と言って、Trema の内部動作を可視化してくれるありがたいツール
じゃ。これを使うと、アプリケーションと Switch Daemon の間でやりとりされ
るメッセージの中身など、いろんなものが Wireshark の GUI で見られて便利じゃぞ」@<br>{}
@<em>{友太郎} 「おお! そんなものがあるんですね!」

=== Tremashark の強力な機能

Tremashark は Trema の内部動作と関連するさまざまな情報を可視化するツール
で、具体的には次の情報を収集・解析し、表示する機能を持ちます (@<img>{tremashark_overview})。

 1. Trema 内部やアプリケーション間の通信 (IPC) イベント
 2. セキュアチャネル、および任意のネットワークインタフェース上を流れるメッセージ
 3. スイッチやホストなどから送信された Syslog メッセージ
 4. スイッチの CLI 出力など、任意の文字列

//image[tremashark_overview][Tremashark の概要]

各種情報の収集を行うのが Tremashark のイベントコレクタです。これは、
Trema 内部や外部プロセス・ネットワーク装置などから情報を収集し、時系列
順に整列します。整列した情報は、ファイルに保存したりユーザインタフェー
ス上でリアルタイムに表示したりできます。

Tremashark のユーザインタフェースは Wireshark と Trema プラグインからな
ります。イベントコレクタによって収集した情報はこのプラグインが解析し、
Wireshark の GUI もしくは CUI 上に表示します。

=== 動かしてみよう

イベントコレクタは、Trema をビルドする際に自動的にビルドされます。しか
し、Trema プラグイン標準ではビルドされませんので、利用するには次の準備
が必要です。

==== Wireshark のインストール

Tremashark のユーザインタフェースは Wireshark を利用していますので、ま
ずは Wireshark のインストールが必要です。Ubuntu Linux や Debian
GNU/Linux での手順は次のようになります。

//cmd{
% sudo apt-get install wireshark
//}

==== Trema プラグインのインストール

次に、Wireshark の Trema プラグインをビルドしてインストールします。
Ubuntu Linux 11.10 の場合の手順は次のとおりです。

//cmd{
% cd /tmp
% apt-get source wireshark
% sudo apt-get build-dep wireshark
% cd [trema ディレクトリ]/src/tremashark/plugin
% ln -s /tmp/wireshark-(バージョン番号) wireshark
% cd wireshark
% ./configure
% cd ../packet-trema
% make
% mkdir -p ~/.wireshark/plugins
% cp packet-trema.so ~/.wireshark/plugins
% cp ../user_dlts ~/.wireshark
//}

==== OpenFlow プラグインのインストール

Trema のモジュール間で交換される OpenFlow メッセージを解析し表示するに
は、Trema プラグインに加えて OpenFlow プロトコルのプラグインも必要です。
OpenFlow プロトコルのプラグインは OpenFlow のリファレンス実装とともに配
布されており、次の手順でインストールできます。

//cmd{
% git clone git://gitosis.stanford.edu/openflow.git
% cd openflow/utilities/wireshark_dissectors/openflow
% patch < [trema ディレクトリ]/vendor/packet-openflow.patch
% cd ..
% make
% cp openflow/pcacket-openflow.so ~/.wireshark/plugins
//}

==== 実行してみよう

いよいよ、Tremashark を使って Trema のモジュール間通信を覗いてみましょ
う。例として Trema サンプルアプリケーションのひとつ、ラーニングスイッチ
と Switch Daemon 間の通信を見てみることにします。

まず、次のコマンドでラーニングスイッチを起動してください。ここでオプショ
ンに @<tt>{-s} を指定することで、Tremashark のイベントコレクタとユーザ
インタフェースが起動します。

//cmd{
% ./trema run learning-switch.rb -c learning_switch.conf -s -d
//}

ラーニンスイッチの起動後、イベントコレクタへのイベント通知を有効にする
必要があります。これは、イベントを収集したいプロセスに USR2 シグナルを
送ることで有効にできます。シグナルを送るための各プロセスの PID は、
Trema のディレクトリの下の @<tt>{tmp/pid} 以下のファイルに保存されてい
ます。たとえば、Ruby で書かれたアプリケーションの PID は、"[コントロー
ラのクラス名].pid" という名前のファイルに保存されます。また Switch
Daemon の PID は、"switch.[管理するスイッチの Datapath ID].pid" という
名前のファイルに保存されます。

今回の例では、ラーニングスイッチと Switch Daemon のイベントを見るのです
から、次のように @<tt>{kill} コマンドを使って各プロセスへ USR2 シグナル
を送ります。

//cmd{
% kill -USR2 `cat tmp/pid/LearningSwitch.pid`
% kill -USR2 `cat tmp/pid/switch.0x1.pid`
//}

これで、プロセス間の IPC イベントを覗く準備ができました。ではイベントを
発生させるために、以下のようにスイッチに接続されたホスト間でパケットを
交換してみましょう。

//cmd{
% ./trema send_packets --source host1 --dest host2
% ./trema send_packets --source host2 --dest host1
//}

すると、@<img>{tremashark_gui} に示すようにモジュール間の通信をリアルタ
イムに観測できます。これによって、アプリケーションがどのような
OpenFlow メッセージを送受信しているかなどを知ることができます。

//image[tremashark_gui][Tremashark ユーザインタフェース]

たとえば、@<img>{tremashark_gui} の一連の解析結果 (7, 8, 9, 10 番のメッ
セージ) により、Packet In メッセージをトリガとしてラーニングスイッチが
Flow Mod メッセージをスイッチ 0x1 に対して送信していることがわかります。
また、下半分のペインには送信した Flow Mod メッセージの各フィールドの値
が表示されています (@<img>{trema_internal_with_tremashark})。

//image[trema_internal_with_tremashark][Tremashark による解析結果 (@<img>{tremashark_gui})]

//noindent
@<em>{友太郎}「もし原因不明なバグに遭遇したときは、こうやってメッセージ
の中身まで追える Tremashark を使えばいいわけですね。」@<br>{}
@<em>{取間先生}「ここまでわかっていれば、本格的な実用コントローラを作る
のも難しくはないぞ。そういえば Trema はサンプルとは別に Trema Apps とい
う実用アプリも公開しておる。何か大きなアプリケーションを作るときに役立
つと思うから、友太郎君のためについでに紹介しておこうかの」@<br>{}

== Trema Apps

Trema Apps (@<tt>{http://github.com/trema/apps}) は、Trema を使った実用
的・実験的な少し大きめのアプリケーションを集めたリポジトリです。Trema
本体と同様に GitHub 上で公開されており、次の手順でダウンロードできます。

//cmd{
% git clone https://github.com/trema/apps.git
//}

実はすでに第 III 部でルーティングスイッチ (@<chap>{routing_switch})、ト
ポロジ (@<chap>{routing_switch})、そしてスライス対応スイッチ
(@<chap>{sliceable_switch}) を紹介してきました。本節では、このほかにも
Trema Apps の中でとくに実用的なアプリを中心に解説していきます。

=== リダイレクト機能付きルーティングスイッチ

ルーティングスイッチ (@<chap>{routing_switch}) の亜種で、ユーザ認証とパ
ケットのリダイレクト機能を付け加えたものです。基本的な動作はルーティン
グスイッチと同じですが、認証されていないホストからのパケットをほかのサー
バに強制的にリダイレクトします。このしくみを使えばたとえば、認証してい
ないユーザの HTTP セッションを強制的に特定のサイトへ飛ばすなどといった
ことが簡単にできます。

=== memcached 版ラーニングスイッチ

サンプルプログラムのラーニングスイッチ (@<chap>{learning_switch}) と同
じ機能を持ちますが、FDB の実装に memcached
(@<tt>{http://memcached.org/}) を用いています。Ruby のmemcached ライブ
ラリを使うことで、オリジナルのラーニングスイッチにほとんど変更を加えず
に memcached 対応できているところがポイントです。また、マルチラーニング
スイッチ (@<chap>{openflow_framework_trema}) の memcached 版もあります。

=== シンプルマルチキャスト

IPTV サービスで使われているマルチキャスト転送を実現するコントローラです。
配信サーバから送られたパケットを OpenFlow スイッチがコピーして、
すべての視聴者へと届けます。

=== フローダンパー

OpenFlow スイッチのフローテーブルを取得するためのユーティリティです。デ
バッグツールとしても便利です。@<chap>{diy_switch} で使いかたを紹介して
います。

=== Packet In ディスパッチャ

Packet In メッセージを複数の Trema アプリケーションに振り分けるサンプル
です。物理アドレスから、ユニキャストかブロードキャストかを判断します。

=== ブロードキャストヘルパ

ブロードキャストやマルチキャストなど、コントローラに負荷の大きいトラ
フィックを分離して、別の独立したコントローラで処理させるためのアプリで
す。

=== フローマネージャ

フローエントリとパス情報を管理するアプリケーションおよび API を提供しま
す。かなり実験的な実装なので、API は変更する可能性があります。

== まとめ

そうこうしている間に夜は白み、取間先生はゆっくりと帰り支度を始めました。@<br>{}

//noindent
@<em>{取間先生}「友太郎君どうもありがとう。今夜はひさびさに若者と話せて
楽しかったよ。わしが教えられることはすべて教えたし、これで君も立派な
OpenFlow プログラマじゃ」@<br>{}
@<em>{友太郎}「こちらこそありがとうございました。なにかいいアプリケーショ
ンができたら、先生にも教えますね!」@<br>{}

今回は、Trema の構成や内部動作について学びました。学んだことは、次の 4 点です。

 * Trema で開発した OpenFlow コントローラは、Trema が提供するSwitch
   Manager、Switch Daemon とアプリケーションプロセスから構成されます。
 * コントローラの動作に必要なプロセスの起動や停止などの管理は trema コ
   マンドにより行われます。
 * Tremashark により、Trema の内部動作を可視化することができます。これ
   は、アプリケーションや Trema の動作を詳細に確認したい場合に役立ちます。
 * Trema Apps には、大きめの Trema アプリケーションが公開されており、自
   分で実用アプリケーションを作る際の参考になります。

さて、本書はこれでおしまいです。Trema を使えば OpenFlow コントローラが
簡単に開発できることを実感していただけたのではないでしょうか？Trema を
使って、ぜひあなたも便利な OpenFlow コントローラを開発してみてください。

== 参考文献

ここでは第 II 部プログラミング編のまとめとして、開発全般で私達が役に立
つと思った本を紹介します。

: XPエクストリーム・プログラミング導入編 (ロン・ジェフリーズら著、ピアソン・エデュケーション)
  友太郎君は夜中のプログラミングが好きなようですが、高品質なコードは昼
  作られるものです (そして、Trema のコードは昼に書かれています)。"残業
  無し、週 40 時間労働" や "継続的インテグレーション" など、XP には今で
  も使えるすぐれたアイデアがたくさんあります。

: 「週 4 時間」だけ働く。(ティモシー・フェリス著、青志社)
  実はプログラミングで最も難しいのはプログラミングのためにまとまった時
  間を捻出することなのではないかと思っています。XP の "週 40 時間" より
  も過激な "週 4 時間" を提唱するこの本は、プログラミングと関係の無い雑
  用をいかにうまくかわすかに 600 ページ超を割いています。とくに雑用のア
  ウトソーシングのテクニックの数々は参考になります。

: レボリューション・イン・ザ・バレー (アンディ・ハーツフェルド著、オライリー)
  週 90 時間労働という極限状態でオリジナル Macintosh を作った人々のストー
  リー。これはもちろん真似してはいけませんが、スティーブ・ジョブズを中
  心としたチームの結束力と強いこだわりを見ると誰でもやる気が出ます。開
  発当時のメモや写真がたくさん載っており、よくできたドキュメンタリー映
  画のように当時の熱気が伝わってきます。

: ワインバーグの文章読本 (ジェラルド・M・ワインバーグ著、翔泳社)
  ライター必読の書。この本自体を執筆するのにも役立っただけでなく、コー
  ディングのような「プログラム執筆」にも役立つ本です。ぜひ、この本が勧
  める執筆日記をつけてみてください。無駄な時間の使い方に気付くようにな
  るだけでなく、作業と作業の間のコンテキストスイッチも軽くなり、何より
  毎日の作業が楽しくなります。
