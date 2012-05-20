= ルーティングスイッチ
== 複数のスイッチを扱う
=== 最短パスを計算する
=== トポロジーを検出する

 * スイッチ間のリンクを見つける
 * LLDP 

=== フローの設定
 
== 実行してみよう

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
$ ./trema/trema run -c ./apps/routing_switch/routing_switch.conf -d
//}

 * 見つけたリンクを表示する

//cmd{
$ TREMA_HOME=trema/ ./apps/topology/show_topology -D
//}

 * パケットを送り、フローが設定されているかを確認する
   
//cmd{
$ ./trema/trema send_packets --source host1 --dest host2
$ ./trema/trema dump_flows
//}

== まとめ/参考文献
