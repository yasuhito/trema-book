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

 * routing switch を動かす

//cmd{
$ cd ./trema
$ ./trema run -c ../apps/routing_switch/routing_switch_fullmesh.conf -d
//}

 * 見つけたリンクを表示する

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

 * パケットを送り、フローが設定されているかを確認する
   
//cmd{
$ ./trema send_packets --source host1 --dest host2
$ TREMA_HOME=. ../apps/flow_dumper/flow_dumper
//}

== まとめ/参考文献
