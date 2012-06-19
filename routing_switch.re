= ルーティングスイッチ

ここまでの章では、主に一台の OpenFlow スイッチを制御する方法について
学んできました。本章では、複数台の OpenFlow スイッチをまとめて制御する
アプリケーションであるルーティングスイッチを紹介します。

== 複数のスイッチを扱う

ルーティングスイッチは、X 章で扱ったラーニングスイッチと同様に
普通の L2 スイッチとして動作します。
ルーティングスイッチは、複数台の OpenFlow スイッチが連動して、
L2 スイッチ機能を提供します(@<img>{routing_switch})。

=== 最短パスを計算する

@<img>{shortestpath}

=== トポロジーを検出する

@<img>{lldp}

 * スイッチ間のリンクを見つける
 * LLDP 

=== フローの設定
 
== 実行してみよう

ルーティングスイッチのソースコードは、Trema Apps にて公開されています。

 * 準備

//cmd{
$ git clone https://github.com/trema/trema.git
$ git clone https://github.com/trema/apps.git
$ (cd trema/; ./build.rb)
$ (cd apps/topology/; make)
$ (cd apps/routing_switch; make)
//}

 * ルーティングスイッチを動かす

それでは、ルーティングスイッチを動かしてみましょう。
ソースコード一式の中に routing_switch_fullmesh.conf という
設定ファイルが同梱されています。
今回はこのファイルを使って、以下のように起動してください。

//cmd{
$ cd ./trema
$ ./trema run -c ../apps/routing_switch/routing_switch_fullmesh.conf -d
//}

 * 見つけたリンクを表示する

topology モジュールには、検出したリンクを表示するコマンドが
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

ルーティングスイッチの起動時に指定した routing_switch_fullmesh.conf と
比較してみましょう。topology_discovery モジュールが検出できるのは、
スイッチ間のリンクのみです。
仮想ホストとスイッチ間のリンクは検出できないため、show_topology の
検出結果には表示されないことに注意しましょう。

 * パケットを送り、フローが設定されているかを確認する
   
次に、仮想ホストからパケットを送り、フローが設定されることを確認しましょう。

//cmd{
$ ./trema send_packets --source host1 --dest host2
$ ./trema send_packets --source host2 --dest host1
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


