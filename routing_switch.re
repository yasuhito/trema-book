= ルーティングスイッチ

多数のスイッチ、ルータから構成される大規模なネットワークの構成図を見ると、
ネットワークが好きな人は心が踊ります。
``どんなプロトコルで動いているのか?'' を知りたくなったり、
``いつか自分でも動かしてみたい!'' と思ったりするものです。
ここまでの章では、主に一台の OpenFlow スイッチを制御する方法について学んできました。
もちろん、OpenFlow は、複数台のスイッチからなる大規模なネットワークでも
使えます。
スイッチを自由に操ることのできる OpenFlow を使えば、さまざまな制御方法を試せそうです。
本書で Trema の使い方をマスターすれば、オリジナルの制御方法を作り出せるかもしれません。

そうは言ってもオリジナルの方法を一から考えだすのは、少々骨が折れます。
何か参考にしたいものです。
Trema Apps には、複数台の OpenFlow スイッチをまとめて制御する
ルーティングスイッチというアプリケーションが用意されています。
本章では、大規模ネットワーク制御の一例として、ルーティングスイッチが
どのように動作するかについて紹介を行います。

#@warn(Trema Apps の記述について全体と整合を取る。ここが初出ならもっと詳しく) 

== 複数のスイッチを扱う

ルーティングスイッチは、@<chap>{learning_switch} で扱ったラーニングスイッチと同様に
普通の L2 スイッチとして動作します。
ラーニングスイッチは、1 台の OpenFlow スイッチが 1 台の L2 スイッチとして
動作するよう制御するコントローラでした。
それに対してルーティングスイッチを使えば、複数台の OpenFlow スイッチが連動して、
一つの大きな L2 スイッチとして動作します (@<img>{routing_switch})。

//image[routing_switch][ルーティングスイッチ]

=== ルーティングスイッチの動作

#@warn(ラーニングスイッチの章と整合を取るように。比較して理解しやすいように)

まずは、ルーティングスイッチがどのように動作するかを、順に見ていきましょう。

@<img>{flow_mod} のネットワーク中で、
ホスト 4 宛のパケットをホスト 1 から受け取ったスイッチ 1 は、
受信したパケットを Packet In でコントローラへと送ります。
ホスト 4 は、スイッチ 6 に接続していますので、
@<img>{flow_mod} のように、
スイッチ 1 → スイッチ 5 → スイッチ 6 の順に転送が行われるよう
フローを設定すればよさそうです。

//image[flow_mod][フローの設定]

コントローラは、スイッチ 1, 5, 6 それぞれに Flow Mod メッセージを送り、
フローを設定します。
その後、Packet In でコントローラに送られたパケットを、
Packet Out メッセージを用いてスイッチ 6 へと送り、ホスト 4 へ向けて送出します。

基本的な動作はラーニングスイッチと同じですが、
パケットを受信したスイッチと違うスイッチまでパケットを届けなければ
ならない点が異なっています。
そのためには、入口となるスイッチから出口となるスイッチまでの
最短パスを見つける必要があります。

=== 最短パスを計算する

スイッチ 1 からスイッチ 6 までのパスは、どのように計算すればよいでしょうか？
ルーティングスイッチでは、ダイクストラ法というアルゴリズムを用いて、
二つのスイッチ間の最短パスを求めています。
ダイクストラ法は、ネットワークの世界では非常にポピュラーなアルゴリズムで、
OSPF や IS-IS 等の L3 の経路制御プロトコルで用いられています。
ここでは、ダイクストラ法を用いて最短パスを見つける方法について、
簡単な説明を@<img>{dijkstra}を用いて行います。

//image[dijkstra][最短パスの計算]

まず始点となるスイッチ 1 に着目します (@<img>{dijkstra} (a))。

初めに、スイッチ 1 から 1 ホップで行けるスイッチを見つけ出します。
これはスイッチ 1 に接続するリンクの先に繋がっているスイッチを、
すべてピックアップすれば見つけ出せます。
その結果、スイッチ 2 とスイッチ 5 が見つけ出せます (@<img>{dijkstra} (b))。

同じようにして、今度はスイッチ 1 から 2 ホップで行けるスイッチを見つけ出します。
これは、今見つけたスイッチ 2 とスイッチ 5 から 1 ホップで行けるスイッチから
見つけ出せばよさそうです。このようにして、スイッチ 3, 4, 6 が見つかります 
(@<img>{dijkstra} (c))。

スイッチ 6 はスイッチ 5 の先で見つかりましたので、
最終的にスイッチ 1 → スイッチ 5 → スイッチ 6 というパスが最短パスであることが
わかります。

=== トポロジーを検出する

ネットワークトポロジの検出には，OpenFlow で標準的な
Link Layer Discovery Protocol（LLDP）を用います（@<img>{lldp}）。
Topology から packet_out により送信された LLDP パケットは，
LLDP が到達した隣のスイッチから packet_in で Topology へと戻ります。
LLDP パケット中には，それが経由したスイッチとポート情報などが含まれています。
そのため，これを繰り返すことでネットワーク中のすべてのスイッチ間の
接続関係を知ることができます。

//image[lldp][トポロジーの検出]


== 実行してみよう


=== 準備

ルーティングスイッチのソースコードは、Trema Apps にて公開されています。
まず Trema Apps を @<tt>{git} を使って取得しましょう。

//cmd{
$ git clone https://github.com/trema/apps.git
//}

Trema Apps にはさまざまなアプリケーションが含まれていますが、
今回使用するのは @<tt>{topology} と @<tt>{routing_switch} です。
この二つを順に @<tt>{make} します。

//cmd{
$ (cd apps/topology/; make)
$ (cd apps/routing_switch; make)
//}

=== ルーティングスイッチを動かす

それでは、ルーティングスイッチを動かしてみましょう。
ソースコード一式の中に @<tt>{routing_switch_fullmesh.conf} という
ファイルが含まれています。

//list[conf][@<tt>{routing_switch_fullmesh.conf}]{
vswitch {
  datapath_id "0xe0"
}

vswitch {
  datapath_id "0xe1"
}

vswitch {
  datapath_id "0xe2"
}

vswitch {
  datapath_id "0xe3"
}

vhost ("host1") {
  ip "192.168.0.1"
  netmask "255.255.0.0"
  mac "00:00:00:01:00:01"
}

vhost ("host2") {
  ip "192.168.0.2"
  netmask "255.255.0.0"
  mac "00:00:00:01:00:02"
}

vhost ("host3") {
  ip "192.168.0.3"
  netmask "255.255.0.0"
  mac "00:00:00:01:00:03"
}

vhost ("host4") {
  ip "192.168.0.4"
  netmask "255.255.0.0"
  mac "00:00:00:01:00:04"
}

link "0xe0", "host1"
link "0xe1", "host2"
link "0xe2", "host3"
link "0xe3", "host4"
link "0xe0", "0xe1"
link "0xe0", "0xe2"
link "0xe0", "0xe3"
link "0xe1", "0xe2"
link "0xe1", "0xe3"
link "0xe2", "0xe3"

run {
  path "../apps/topology/topology"
}

run {
  path "../apps/topology/topology_discovery"
}

run {
  path "../apps/routing_switch/routing_switch"
}

event :port_status => "topology", :packet_in => "filter", :state_notify => "topology"
filter :lldp => "topology_discovery", :packet_in => "routing_switch"
//}

今回はこのファイルを使って、以下のように起動してください。

//cmd{
$ cd ./trema
$ ./trema run -c ../apps/routing_switch/routing_switch_fullmesh.conf -d
//}

@<img>{fullmesh} のようなネットワークが構成されます。

//image[fullmesh][ネットワーク構成]

=== 見つけたリンクを表示する

@<tt>{topology} モジュールには、検出したリンクを表示するコマンドが
用意されていますので、使ってみましょう。
以下のように実行してください。

//cmd{
$ TREMA_HOME=. ../apps/topology/show_topology -D
vswitch {
  datapath_id "0xe0"
}

vswitch {
  datapath_id "0xe2"
}

vswitch {
  datapath_id "0xe3"
}

vswitch {
  datapath_id "0xe1"
}

link "0xe3", "0xe2"
link "0xe1", "0xe0"
link "0xe3", "0xe0"
link "0xe2", "0xe1"
link "0xe2", "0xe0"
link "0xe3", "0xe1"
//}

ルーティングスイッチの起動時に指定した設定ファイル (@<list>{conf}) や
ネットワーク構成 (@<img>{fullmesh}) と
比較してみましょう。@<tt>{topology_discovery} モジュールが検出できるのは、
スイッチ間のリンクのみです。
仮想ホストとスイッチ間のリンクは検出できないため、@<tt>{show_topology} の
検出結果には表示されないことに注意しましょう。

=== パケットを送り、フローが設定されているかを確認する
   
次に、仮想ホストからパケットを送り、フローが設定されることを確認しましょう。

//cmd{
$ ./trema send_packets --source host1 --dest host2
$ ./trema send_packets --source host2 --dest host1
//}

//cmd{
$ TREMA_HOME=. ../apps/flow_dumper/flow_dumper
[0x000000000000e1] table_id = 0, priority = 65535, cookie = 0xbd100000000000e,\
  ...									      \
  dl_src = 00:00:00:01:00:02, dl_dst = 00:00:00:01:00:01, dl_vlan = 65535,    \
  ...	   		      	       			  	    	      \
  actions = [output: port=3 max_len=65535]
[0x000000000000e0] table_id = 0, priority = 65535, cookie = 0xbd100000000000d,\
  ...									      \
  dl_src = 00:00:00:01:00:02, dl_dst = 00:00:00:01:00:01, dl_vlan = 65535,    \
  ...									      \
  actions = [output: port=3 max_len=65535]
//}

== まとめ/参考文献


