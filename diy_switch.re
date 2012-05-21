= 数千円で OpenFlow スイッチを DIY

これまで、Trema を使ってコントローラを作る方法を学んできました。
またケーススタディを通して、OpenFlow を活用する方法も身につけました。
そろそろ、実物の OpenFlow スイッチを使ってみたいと思いませんか？
しかし市販の OpenFlow スイッチはとても高価で、個人で手が届く価格では
ありません。手に入れることは、無理なのでしょうか？

あきらめるのはもう少し待ってください。この章では、
市販の無線 LAN ルータを OpenFlow スイッチに改造する
方法を紹介します。この章を読めば、
数千円で OpenFlow スイッチを手に入れることができます。

== 無線 LAN ルータを OpenFlow 化しよう

 * OpenWRT の話
 * SRCHACK さんの改造ファームの話

== OpenFlow 化の手順

それでは、無線 LAN ルータの改造に取り掛かりましょう。

 1. G301N の LAN 側に PC を接続し、DHCP でアドレスをもらう
 2. G301N の管理 Web 画面経由でファームを流し込む
 3. PC を WAN 側に差し替え、G301 にログイン

== Trema とつないでみよう

=== Trema の起動

#@# Trema 側の設定 (Learning Switch ？)

//cmd{
$ cd trema
$ ./trema run ./src/examples/learning_switch/learning-switch.rb -d
//}

以下のコマンドでスイッチとコントローラ間に TCP コネクションが
はられているかを確認しましょう。

//cmd{
$ netstat -an -A inet | grep 6633

//}

=== スイッチの情報を取得する

#@# show_description

//cmd{
$ git clone https://github.com/trema/apps.git
$ (cd ./apps/show_description/; make)
//}

作成したコマンドを使って、スイッチの情報を取得してみましょう。

//cmd{
$ TREMA_HOME=trema/ ./apps/show_description/show_desctiption
Manufacturer description: Stanford University
Hardware description: Reference Userspace Switch
Software description: 1.0.0
Serial number: None
Human readable description of datapath: OpenWrt pid=1909
Datapath ID: 0x2320477671
Port no: 1(0x1)(Port up)
  Hardware address: 10:6f:3f:07:d3:6f
  Port name: eth0.1
Port no: 2(0x2)(Port up)
  Hardware address: 10:6f:3f:07:d3:6f
  Port name: eth0.2
Port no: 3(0x3)(Port up)
  Hardware address: 10:6f:3f:07:d3:6f
  Port name: eth0.3
Port no: 4(0x4)(Port up)
  Hardware address: 10:6f:3f:07:d3:6f
  Port name: eth0.4
Port no: 65534(0xfffe:Local)(Port up)
  Hardware address: 00:23:20:47:76:71
  Port name: tap0
//}

=== フローを表示する

#@# ping を打つ説明

実際にフローが設定されているか、確認してみましょう。
OpenFlow スイッチに ssh でログインして、以下のコマンドをうってみてください。

//cmd{
root@OpenWrt:~# dpctl dump-flows unix:/var/run/dp0.sock 
stats_reply (xid=0x8e5d6e05): flags=none type=1(flow)
  cookie=38, duration_sec=338s, duration_nsec=858000000s, table_id=0, priority=65535, n_packets=339, n_bytes=25086, idle_timeout=61,hard_timeout=0,icmp,in_port=1,dl_vlan=0xffff,dl_vlan_pcp=0x00,dl_src=00:24:81:5d:6b:f1,dl_dst=c4:2c:03:1d:91:e4,nw_src=192.168.11.3,nw_dst=192.168.11.2,nw_tos=0x00,icmp_type=8,icmp_code=0,actions=output:4
  cookie=40, duration_sec=338s, duration_nsec=855000000s, table_id=0, priority=65535, n_packets=339, n_bytes=25086, idle_timeout=61,hard_timeout=0,icmp,in_port=4,dl_vlan=0xffff,dl_vlan_pcp=0x00,dl_src=c4:2c:03:1d:91:e4,dl_dst=00:24:81:5d:6b:f1,nw_src=192.168.11.2,nw_dst=192.168.11.3,nw_tos=0x00,icmp_type=0,icmp_code=0,actions=output:1
//}

OpenFlow プロトコルには、スイッチ側のフローエントリをコントローラ側から
取得するためのメッセージが存在します。
このメッセージを使って取得したフローエントリを表示するコマンドが、
Trema apps に用意されています。

//cmd{
# (cd apps/flow_dumper; make)
# TREMA_HOME=./trema apps/flow_dumper/flow_dumper
[0x00002320698790] priority = 65535, match = [wildcards = 0, in_port = 1, dl_src = 00:24:81:5d:6b:f1, dl_dst = c4:2c:03:1d:91:e4, dl_vlan = 65535, dl_vlan_pcp = 0, dl_type = 0x800, nw_tos = 0, nw_proto = 1, nw_src = 192.168.11.3/32, nw_dst = 192.168.11.2/32, tp_src = 8, tp_dst = 0], actions = [output: port=4 max_len=65535]
[0x00002320698790] priority = 65535, match = [wildcards = 0, in_port = 4, dl_src = c4:2c:03:1d:91:e4, dl_dst = 00:24:81:5d:6b:f1, dl_vlan = 65535, dl_vlan_pcp = 0, dl_type = 0x800, nw_tos = 0, nw_proto = 1, nw_src = 192.168.11.2/32, nw_dst = 192.168.11.3/32, tp_src = 0, tp_dst = 0], actions = [output: port=1 max_len=65535]
//}

OpenFlow スイッチ側で確認したエントリが取得できていることが確認できたでしょうか？
もしかしたら、ping 以外にも OS が独自にだしているパケットによりフローが出来ているかも
しれません。その場合はもう一度 OpenFlow スイッチ側のエントリも確認してみてください。

== まとめ/参考文献

本章で学んだことは、以下の二つです。

 * 数千円の無線 LAN ルータを、OpenFlow スイッチに
   改造しました。
 * その OpenFlow スイッチを Trema と接続し、
   動作確認を行いました。

参考文献

 * OpenWRT ( @<href>{https://openwrt.org/} )

 * OpenFlow 対応ファームウェア作者のSRCHACK 氏の Blog
   ( @<href>{http://www.srchack.org/} )

 * WHR-G301N の OpenFlow 化手順
   ( @<href>{http://www.srchack.org/article.php?story=20120324164358634} )


