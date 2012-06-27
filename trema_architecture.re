= Trema のアーキテクチャ

3 月にしてはあたたかい夜更け、@<ruby>{友太郎,ゆうたろう}君が自宅の書斎
にていつものようにコーディングを楽しんでいたときのこと。ふいに、玄関の
外から聞き覚えのある声。@<br>{}

//noindent
@<em>{謎の声}「もしもし、友太郎君はおるかの？」@<br>{}
@<em>{友太郎君}「あっ! @<ruby>{取間,とれま}先生じゃないですか。こんな夜中
にどうされたんですか？」@<br>{}
@<em>{取間先生}「三軒茶屋で飲んでいたら、愛用のサンダルが無くなってしまっ
てのう。別の客が間違えて履いて行ってしまったようじゃ。そのおかげで終電
まで無くしてしもうたわい! ワッハッハ」@<br>{}

//noindent
このおじいさん、酔っていて陽気ですが足元を見るとたしかに裸足です。それ
に着物のすそがやたらと汚れていて、どうやら酔った勢いだけで友太郎君の住
む武蔵小杉まではるばる歩いてきてしまったようです。@<br>{}

//noindent
@<em>{取間先生}「そこで悪いのじゃが友太郎君、今夜はこのみじめな老人めを
泊めてくれんか？お礼と言ってはなんじゃが、始発まで時間はたっぷりあるか
ら Trema についてまだ話してなかったことをすべて教えよう。友太郎君は
Trema でいろいろとアプリを書いているようだし、いろいろ聞きたい事もある
だろうと思ってな」@<br>{}
@<em>{友太郎君}「ぜひそのお話聞きたいです! さあさどうぞお上がりください」

== @<tt>{trema run} の裏側

//noindent
@<em>{取間先生}「では基本的な話から。Trema でコントローラを動かすには
"@<tt>{trema run}" じゃったな。このコマンド、実は裏でいろんなプロセスを
起動しているんじゃ。具体的に何をやっているか、"@<tt>{trema run}" に
@<tt>{-v} オプションをつけると見ることができるぞ。さっそく、サンプルの
learning-switch (@<chap>{learning_switch}) を試しに起動してみてごらん」

//cmd{
% ./trema run learning-switch.rb -c learning_switch.conf -v
.../switch_manager --daemonize --port=6633 -- \
  port_status::LearningSwitch packet_in::LearningSwitch \
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
@<em>{友太郎君}「うわっ! なんだかいっぱい字が出てきましたね」@<br>{}
@<em>{取間先生}「この出力を見れば、@<tt>{trema} コマンドが裏でどんなこ
とをしてくれているのかスグわかるのじゃ。たとえばログの最初を見ると、
switch_manager というプロセスが起動されておるな？ Trema ではこのプロセ
スがスイッチと最初に接続するんじゃ。」

#@warn(port_status:: とかの引数の説明)

=== スイッチとの接続

Switch Manager はスイッチからの接続要求を待ち受けるデーモンです。スイッ
チとの接続を確立すると、子プロセスとして Switch Daemon プロセスを起動し、
スイッチとの接続をこのプロセスへ引き渡します
(@<img>{switch_manager_daemon})。@<br>{}

//image[switch_manager_daemon][スイッチからの接続の受付]

//noindent
@<em>{友太郎君}「うーむ。いきなりむずかしいです」@<br>{}
@<em>{取間先生}「そんなことはないぞ、ぜんぜん簡単じゃ。Switch Manager
の役割は言わば inetd と同じと考えればよい。つまり Switch Manager は新し
いスイッチが接続してくるのを見張っているだけで、実際のスイッチとのやり
とりはスイッチごとに起動する Switch Daemon プロセスに一切合切まかせてし
まうというわけじゃ」@<br>{}
@<em>{友太郎君}「なるほど! たしかに仕組みが inetd とよく似ていますね」

==== Switch Daemon

Switch Daemon は、Switch Manager が確立したスイッチとの接続を引き継ぎ、
スイッチと Trema 上のアプリケーションプロセスとの間で流れる OpenFlow メッ
セージを仲介します (@<img>{switch_daemon})。

//image[switch_daemon][スイッチと Trema アプリケーションの間で OpenFlow メッセージを仲介する Switch Daemon][scale=0.3]

 * アプリケーションプロセスが生成した OpenFlow メッセージをスイッチへ
   配送する
 * スイッチから受信した OpenFLow メッセージをアプリケーションプロセス
   へ届ける

Switch Daemon の重要な仕事として、OpenFlow メッセージの検査があります。
Switch Daemon は Trema アプリケーションとスイッチの間で交換される
OpenFlow メッセージの中身をすべて検査します。そして、もし不正なメッセー
ジを発見するとエラーを出します。@<br>{}

#@warn(エラーはどこに出すか説明)

//noindent
@<em>{友太郎}「Switch Manager と Switch Daemon プロセスでの役割分担とか、
スイッチごとに Switch Daemon プロセスが起動するところなど、いかにも
UNIX っぽいですね。」@<br>{}
@<em>{取間先生}「うむ。そうしたほうがひとつひとつのデーモンが単純化できるからな」@<br>{}
@<em>{友太郎}「送信するメッセージを厳密にチェックするのはいいと思うんで
すが、受信もチェックするのってやりすぎではないですか？ほら、"送信するも
のは厳密に、受信するものは寛容に" って言うじゃないですか」@<br>{}
@<em>{取間先生}「受信メッセージのチェックをあまり寛容にすると、後々とん
でもないことが起こるのじゃ。Trema の開発者が言っておったが、昔それをやっ
て相互接続テストで死ぬ目に遭ったそうじゃよ。それに、Trema は受信メッセー
ジもちゃんとチェックするようにしたおかげで、実際に助かったことがたくさ
んあったのじゃ。たとえば OpenFlow の標準的なベンチマークツール cbench
のバグを発見したのも Trema チームだし、2012 年の Interop での OpenFlow
相互接続では様々なベンダのスイッチのバグ発見に Trema が一役買ったそうじゃ
(@<img>{trema_interop})」@<br>{}
@<em>{友太郎}「へえー! すごいですね!」

//image[trema_interop][2012 年 Interop で活躍した Trema ベースの OpenFlow コントローラ Rise (NICT 石井秀治さん提供)][scale=0.6]

Switch Daemon のもうひとつの重要な役割がスイッチの仮想化です。

===[column] 友太郎の質問: あれれ、Trema バグってる？

Q. 「OpenFlow の仕様を読んでみたら、Flow Cookie というのがあったので使っ
    てみたんだ。Cookie をフローエントリに指定すると、同じ Cookie を持つ
    フローエントリを 1 つのグループとしてまとめて管理できて便利だと聞い
    て。でもなぜか、ぼくが指定した Cookie の値がスイッチに正しく設定さ
    れてないように見えるんだけど……。Trema バグってない？」

A. それは Switch Daemon が複数のアプリケーション間で競合が起こらないよ
うに仮想化しているからです。たとえば、Trema 上で動くアプリケーション A
と B がたまたま同じ Cookie 値を使おうとするとどうなるでしょうか？そのま
まだと混じってしまい混乱しますよね。

Switch Daemon は、こうした Cookie 値の重複を避けるための変換を行ってい
ます。つまり、アプリケーションからスイッチに指定する Cookie 値と、逆に
スイッチがアプリケーションに通知する Cookie を Switch Daemon が自動的に
変換します (@<img>{switch_daemon_cookie})。この変換のおかげで、アプリケー
ションがほかのアプリケーションとの間で Cookie 値がかぶらないようにする
必要がなくなります。

//image[switch_daemon_cookie][Cookie 値の変換]

このほかにも、メッセージのトランザクション ID についても自動変換を行っ
てくれています。Switch Daemon は、Trema の世界の平和を守る縁の下の力持
ちなのです。

===[/column]


=== Trema C ライブラリの構成

@<em>{友太郎}「あっそういえば、Trema って実は C からも使えるらしいじゃ
ないですか。せっかくだから、どんなライブラリがあるか教えてくださいよ」@<br>{}
@<em>{取間先生}「よかろう。ちょっとわしは飲み足りないので、一緒にビール
でも買いに行かんか？ C ライブラリもなにしろ数がたくさんあるから、歩きな
がら説明することにしよう」@<br>{}
@<em>{友太郎}「いいですね! そうしましょう」

==== OpenFlow Application Interface

OpenFlow メッセージを受信したときにアプリケーションのハンドラを起動した
り、逆にアプリケーションが送信した OpenFlow メッセージを適切なSwitch
Daemon に配送したりするのが OpenFlow Application Interface
(@<tt>{src/lib/openflow_application_interface.c}) です。

OpenFlow Application Interface は、受信した OpenFlow メッセージをアプリ
ケーションが扱いやすいに変換してハンドラに渡します。たとえば、次の処理
を行います。

 * OpenFlow メッセージのマッチングルール部をホストバイトオーダへ変換
 * 可変長部分をリストに変換
 * packet_in メッセージに含まれるイーサネットフレームを解析し、フレーム
   と解析結果 (パケットの種類など) を組にしてハンドラへ提供

//noindent
@<em>{友太郎}「うーん、ちょっとむずかしいけど、要はプログラムから扱いや
すいように OpenFlow メッセージを変換したり、あらかじめ解析しておいてく
れたりするってことですか？」@<br>{}
@<em>{取間先生}「そうそう、そういうことじゃ。友太郎君冴えておるぞ」

==== OpenFlow Messages

アプリケーションが OpenFlow メッセージを生成する際に利用するのが、
OpenFlow Messages (@<tt>{src/lib/openflow_message.c}) です。メッセージ
受信の場合とは逆に、

 * ホストバイトオーダで指定された値をネットワークバイトオーダで変換し、
   OpenFlow メッセージのヘッダへ格納
 * リストの形で与えられた値を可変長ヘッダへ格納  

などを行います。また、不正なメッセージの生成を防ぐため、値の範囲やビッ
トの検査も行います。OpenFlow Messages によって生成したメッセージは、
OpenFlow Application Interface が提供するメッセージ送信 API を用いて、
Switch Daemon を介してスイッチへ送信します。@<br>{}

//noindent
@<em>{友太郎}「いろいろと気が効いてますね」@<br>{}
@<em>{取間先生}「そうじゃろうそうじゃろう。まあふつうプログラミングフレーム
ワークと言ったらバイトオーダの自動変換や可変長ヘッダのリストへの変換く
らいやってくれてもいいと思うんじゃが。しかし、他のフレームワークではやっ
てくれないことがほとんどなのじゃ。。。」@<br>{}
@<em>{友太郎}「そうなんですね! Trema って便利だなあ」

==== パケットパーサ

packet_in メッセージに含まれるイーサネットフレームを解析したり、解析結
果を参照する API を提供したりするのが、パケットパーサ
(@<tt>{src/lib/packet_parser.c}) です。パケットが TCP なのか UDP なのか
といったパケットの種類の判別や、MAC や IP アドレスといったヘッダフィー
ルド値の参照を用意にしています。

==== プロセス間通信

Switch Daemon とユーザのアプリケーション間の OpenFlow メッセージのやり
とりなど、プロセス間の通信には @<tt>{src/lib/messenger.c} で定義される
プロセス間通信 API が使われます。

==== 基本データ構造

その他、C ライブラリでは基本的なデータ構造を提供しています。たとえば、
可変長バッファ (@<tt>{src/lib/buffer.c})、連結リスト
(@<tt>{linked_list.c, doubly_linked_list.c})、ハッシュテーブル
(@<tt>{src/lib/hash_table.c}) などです。

=== 仮想ネットワークの仕組み

//noindent
@<em>{友太郎}「Trema でなにがうれしいかって、仮想ネットワークの機能です
よね! おかげでノート PC 1 台で開発できるからすっごく楽なんですけど、あ
れってどういうしくみなんですか？」@<br>{}
@<em>{取間先生}「むずかしいことはしておらん。Linux カーネルの標準機能を
使って、仮想スイッチプロセスと仮想ホストプロセスをつなげているだけじゃ」@<br>{}
@<em>{友太郎}「？」

==== 仮想スイッチ

ソフトウェア実装の OpenFlow スイッチです。Trema では、フリーの
OpenFlow スイッチ実装である Open vSwitch
(@<tt>{http://openvswitch.org/}) を利用しています。"trema run" コマン
ドに与えられた仮想ネットワーク設定ファイル中の仮想スイッチ定義
(@<tt>{vswitch} で始まる行) に従って、スイッチプロセスを必要な数だけ起
動します。

==== 仮想ホスト

仮想ホストの実態は、任意のイーサネットインタフェースから任意のイーサネッ
トフレーム・UDP/IP パケットを送受信できる phost と呼ばれるソフトウェア
です (@<tt>{vendor/phost/})。"trema run" コマンドに与えられた設定ファイ
ル中の仮想ホスト定義 (@<tt>{vhost} で始まる行) に従って、必要な数のホス
トプロセスを起動します。

==== 仮想リンク

仮想スイッチと仮想ホストを接続するため、Linux が標準で提供する Virtual
Ethernet Device を仕様しています。これは、Point-to-Point のイーサネット
リンクを仮想的に構成してくれるものです。"trema run コマンドに与えられた
仮想ネットワーク設定ファイル中の仮想リンク定義 (@<tt>{link} で始まる行)
に従って、必要な数の仮想リンクが設定されます。

#@warn(今まで説明してきたこのあたり、trema run -v の出力と対応させたいな)

== 低レベルデバッグツール Tremashark

//noindent
@<em>{取間先生}「こうして見ると Trema って意外と複雑じゃろう。もし友太
郎君がさらに Trema をハックしてみたいとして、ふつうにやるのはちょっとた
いへんだと思うから、いいツールを紹介してあげよう。これは Tremashark と
言って、Trema の内部動作を可視化してくれるありがたいツールじゃ。これを
使うと、アプリケーションと Switch Daemon の間でやりとりされるメッセージ
など、いろんなものが Wireshark の GUI で見られて便利じゃぞ!」@<br>{}
@<em>{友太郎}「おお! ぜひ教えてください!」

=== Tremashark の強力な機能

Tremashark は Trema の内部動作と関連するさまざまな情報を可視化するもの
で、具体的には次の情報を収集/解析/表示する機能を持ちます (@<img>{tremashark_overview})。

 1. Trema 内部・アプリケーションのモジュール間通信 (IPC) イベント
 2. セキュアチャネル、および任意のネットワークインタフェース上を流れるメッセージ
 3. スイッチなどから送信された Syslog メッセージ
 4. スイッチの CLI 出力など、任意テキスト文字列

//image[tremashark_overview][Tremashark の概要]

情報の収集を行うのが Tremashark イベントコレクタと呼ばれるモジュールで
す。これは、Trema 内部、もしくは外部プロセス・ネットワーク装置から情報
を収集して時系列に整列します。整列した情報は、ファイルに保存したりユー
ザインタフェース上でリアルタイムに表示したりできます。

Tremashark のユーザインタフェースは Wireshark と Trema 用プラグインから
構成されています。Tremashark イベントコレクタによって収集した情報はこの
プラグインが解析し、GUI もしくは CUI 上に表示されます。

=== 動かしてみよう

Tremashark イベントコレクタは、Trema をビルドする際に自動的にビルドされ
ます。しかし、ユーザインタフェースは標準ではビルドされていませんので、
利用するには次の準備が必要です。

==== Wireshark のインストール

Tremashark のユーザインタフェースは Wireshark を利用していますので、
Wireshark のインストールが必要です。Ubuntu Linux や Debian GNU/Linux で
は、次のようにインストールできます。

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

Trema のモジュール間で交換される OpenFlow メッセージを解析・表示するに
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

いよいよ、Tremashark を使ってみましょう。ここでは、Trema のモジュール間
通信を覗いてみます。Trema サンプルアプリケーションとして提供されている
Learning Switch と Switch Daemon 間の通信を見てみることにします。

次のコマンドで Learning Switch を起動してください。オプションに -s を指
定することで、Tremashark のイベントコレクタとユーザインタフェースが起動
します。

//cmd{
% ./trema run src/examples/learning_switch/learning-switch.rb \\
  -c src/examples/learning_switch/learning_switch.conf -s -d
//}

Learning Switch の起動後、Tremashark イベントコレクタへの IPC イベント
通知を有効にします。これは、モジュールのプロセスに USR2 シグナルを送る
ことで有効にできます。シグナルを送るための各モジュールのプロセスの PID
は、Trema のディレクトリの下の @<tt>{tmp/pid} 以下のファイルに保存され
ています。Ruby で書かれたアプリケーションの PID は、
[コントローラのクラス名].pid という名前のファイルに保存されます。
Switch Daemon の PID は、switch.[管理するスイッチの Datapath ID].pid と
いう名前のファイルに保存されます。

今回の例では、次のようにアプリケーションと Switch Daemon に対してシグナ
ルを送りましょう。

//cmd{
% kill -USR2 `cat tmp/pid/LearningSwitch.pid`
% kill -USR2 `cat tmp/pid/switch.0x1.pid`
//}

これで、モジュール間の IPC イベントを除く準備ができました。たとえば、以
下のようにスイッチに接続されたホスト間でパケットを交換してみましょう。

//cmd{
% ./trema send_packets --source host1 --dest host2
% ./trema send_packets --source host2 --dest host1
//}

すると、@<img>{tremashark_gui} に示すようにモジュール間の通信をリアルタ
イムに観測できます。これによって、モジュール間で正常に情報交換が行われ
ていることや、アプリケーションが OpenFlow メッセージの送受信を正常に行っ
ているかどうかなどを知ることができます。

//image[tremashark_gui][Tremashark ユーザインタフェース]

たとえば、@<img>{tremashark_gui} の No.7 〜 No.10 のメッセージにより、
packet_in メッセージをトリガとして、Learning Switch が Flow Mod メッセー
ジをスイッチに対して送信していることがわかります
(@<img>{trema_internal_by_tremashark})。

//image[trema_internal_by_tremashark][@<img>{tremashark_gui} からわかる Trema の内部動作]

== Trema Apps

//noindent
@<em>{友太郎}「今夜はいろいろとためになるなあ」@<br>{}
@<em>{取間先生}「どうじゃ？ここまでわかっていれば、本格的な実用コントロー
ラを作るのも難しくはないぞ。そういえば Trema はサンプルとは別に Trema
Apps という実用アプリも公開しておる。何か大きなアプリケーションを作ると
きに役立つと思うから、友太郎君のために紹介しておこうかの」@<br>{}

Trema Apps (@<tt>{http://github.com/trema/apps}) は、Trema を使った実用
的・実験的なアプリケーションを集めたリポジトリです。Trema と同様に
GitHub 上で公開されており、次の手順でダウンロードできます。

//cmd{
% git clone https://github.com/trema/apps.git
//}

実はすでに Routing Switch (@<chap>{routing_switch})、Topology
(@<chap>{routing_switch})、そして Sliceable Switch
(@<chap>{sliceable_switch}) は紹介してきました。このほかに Trema Apps
の中でもとくに実用的なアプリを中心に簡単に解説していきます。

=== Redirectable Routing Switch

Routing Switch (@<chap>{routing_switch})の亜種で、ユーザ認証とパケット
のリダイレクト機能を付け加えたものです。認証されていないホストからのパ
ケットをほかのサーバに強制的にリダイレクトします。このしくみを使えばた
とえば、認証していないユーザの HTTP セッションを強制的に特定のサイトへ
飛ばすといったことが簡単にできます。

=== Learning switch with memcached

サンプルプログラムの learning_switch (@<chap>{learning_switch}) と同じ
機能を持ちますが、FDB の実装に memcached を用いています。Ruby の
memcached ライブラリを使うことで、オリジナルの learning_switch にほとん
ど変更を加えずに memcached 対応できているところがポイントです。

=== Multi learning switch with memcached

サンプルプログラムの multi_learning_switch
(@<chap>{openflow_framework_trema}) を memcached 対応したものです。

=== Flow dumper

OpenFlow スイッチのフローテーブルを取得するためのユーティリティです。デ
バッグツールとしても便利です。@<chap>{diy_switch} で使いかたを紹介して
います。

=== Packetin dispatcher

packet_in メッセージを複数の Trema アプリケーションに振り分けるサンプル
です。物理アドレスから、ユニキャストかブロードキャストかを判断します。

=== Broadcast helper

ブロードキャストやマルチキャストなど、コントローラに負荷の大きいトラ
フィックを分離して、別の独立したコントローラで処理させるためのアプリで
す。

=== Flow manager

フローエントリとパス情報を管理するアプリケーションおよび API を提供しま
す。かなり実験的な実装なので、API は変更する可能性があります。

== まとめ

そうこうしている間に夜は白み、取間先生は帰り支度を始めました。

//noindent
@<em>{取間先生}「友太郎君どうもありがとう。今夜はひさびさに若者と話せて
楽しかったよ。これで君も立派な OpenFlow プログラマじゃ」@<br>{}
@<em>{友太郎}「こちらこそありがとうございました。なにかいいアプリケーショ
ンができたら、先生にも教えますね!」@<br>{}

今回は、Trema の構成、内部動作について学びました。学んだことは、次の 3 点です。

 * Trema を利用して開発した OpenFlow コントローラは、Trema が提供する
   Switch Manager、Switch Daemon とアプリケーションプロセスから構成され
   ます。
 * Trema は複数プロセスから構成されるシステムであり、必要なプロセスの起
   動・停止・管理は trema コマンドにより行われます。
 * Tremashark により、Trema の内部動作を可視化することができます。これ
   は、アプリケーションや Trema の動作を詳細に確認したい場合に役立ちま
   す。
   
本書はこれでおしまいです。Trema を使えば OpenFlow コントローラが簡単に
開発できることを実感していただけたのではないでしょうか？Trema を使って、
ぜひあなたも OpenFlow コントローラを開発してみてください。

== 参考文献
