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

無線 LAN ルータには、LAN 側に有線のポートが 4 ポートついており、
L2 スイッチとして動作しています。
この部分を OpenFlow スイッチとして動作させるようにしてみましょう
@<fn>{musen}。

いまどきの無線 LAN ルータの OS は、Linux で動作しています。
Linux 上で動作する OpenFlow スイッチのソフトウェア実装がありますので、
これが無線 LAN ルータ上でも動作させればよさそうです。
しかし、無線 LAN ルータのメーカーは、自由にソフトウェアを
動かす目的で Linux を採用しているわけではありません。

OpenWRT は、そんな制約を超えるために無線 LAN ルータのファームウェアを
入れ替えてしまおうというプロジェクトです。
このファームウェアを使えば、その上で自由にソフトウェアを
動かすことができます。

SRCHACK さんは、この OpenWRT をベースに OpenFlow スイッチの
ソフトウェア実装を組み込んだファームウェアを開発しました。
本章では、このファームウェアを使って、OpenFlow スイッチを作ってみます。

//footnote[musen][無線 LAN ルータですが、無線 (WiFi) のインターフェイスは今回使用しません。]

===[column] オープンルータ・コンペティション

毎年、業界最大の展示会 Interop Tokyo が幕張メッセにて開催されています。
2012 年にはこの Interop と併催で、次世代のインターネットを担う技術者の
発掘を目指したオープンルータ・コンペティションが開催されました。
「ソフトウェア、ハードウェアに関わらず OpenRouter を改良、
または創造し、その成果を発表すること」という課題に対し、
一次選考を通過した 10 チームによりその成果が競われました。

その結果、本章で取り上げた @SRCHACK.ORG さんが、
見事準グランプリ (NEC 賞) を受賞しました (@<img>{orc})。
また他にも 3 チームにて OpenFlow に関連した成果発表が行われ、
これらのチームすべてが Trema を使用していました。

//image[orc][最終審査会場で動作する OpenFlow スイッチ][scale=0.12]

===[/column]

== OpenFlow 化の手順

それでは、無線 LAN ルータの改造に取り掛かりましょう。

事前に @SRCHACK さんのサイトから、手持ちの機種に対応するファームウェアと、
アップデート方法について記載された手順書をダウンロードしておいてください。

まず、ファームウェアを入れたホストを、無線 LAN ルータと接続します。
この時、@<img>{update} (A) のように無線 LAN ルータの LAN 側ポートに接続してください。
LAN 側ポートは 4 ポートありますが、どのポートに接続してもよいです。
ホストの IP アドレスは、無線 LAN ルータの DHCP サーバ機能により、
自動的に割り当てられます。

//image[update][無線 LAN ルータとホストとの接続][scale=0.5]

次に、ファームウェアを OpenFlow 対応のものに書き換えます。
ダウンロードしておいた手順書に従い、アップデートを行なってください。
``ファームウェアのアップデート中は決して電源を切らないでください'' という
お約束を守れば、簡単にアップデートできるはずです。

OpenFlow 対応ファームウェアをいれた無線 LAN ルータでは、
各ポートの役割が変わってしまっていることに注意が必要です。
LAN 側の 4 ポートは OpenFlow スイッチとして動作するポートとなります。
また WAN 側ポートは、コントローラとの接続に用いる管理用のポートになります。
WHR-G301N では Internet と記載されている青いポートが、WAN 側ポートです。
コントローラとの接続以外にも telnet での接続などにも用いられます。

ファームウェアのアップデート直後には、WAN 側ポートに 192.168.1.1/24 という
アドレスが設定されています。
そのため、@<img>{update} (B) のように、ホストを WAN 側ポートに繋ぎ直してください。
この WAN 側ポートでは DHCP サーバ機能は動作していないので、
ホストに固定で IP アドレスを設定してください。
OpenFlow スイッチ側に事前に設定されているコントローラの IP アドレスが
192.168.1.10 であるため、このアドレスを使用しましょう。
ホストにアドレスを設定した後、OpenFlow スイッチに
telnet を使って root で接続してみます。

//cmd{
$ telnet -l root 192.168.1.1 
Trying 192.168.1.1...
Connected to 192.168.1.1.
Escape character is '^]'.
 === IMPORTANT ============================
  Use 'passwd' to set your login password
  this will disable telnet and enable SSH
 ------------------------------------------


BusyBox v1.19.3 (2012-05-20 02:08:52 JST) built-in shell (ash)
Enter 'help' for a list of built-in commands.

  _______                     ________        __
 |       |.-----.-----.-----.|  |  |  |.----.|  |_
 |   -   ||  _  |  -__|     ||  |  |  ||   _||   _|
 |_______||   __|_____|__|__||________||__|  |____|
          |__| W I R E L E S S   F R E E D O M
 ATTITUDE ADJUSTMENT (bleeding edge, r30406) ----------
  * 1/4 oz Vodka      Pour all ingredients into mixing
  * 1/4 oz Gin        tin with ice, strain into glass.
  * 1/4 oz Amaretto
  * 1/4 oz Triple sec
  * 1/4 oz Peach schnapps
  * 1/4 oz Sour mix
  * 1 splash Cranberry juice
 -----------------------------------------------------
root@OpenWrt:/# 
//}

上記のように接続ができたでしょうか？ 
接続ができたら、OpenFlow スイッチの設定ファイルを見てみましょう。
設定ファイルは、/etc/config 配下にあります。
IP アドレス等、ネットワーク関連の設定は、network ファイルに
記載されています (@<list>{config_network})。
WAN 側ポートは OpenFlow スイッチ上では eth1 に対応しているので、
IP アドレスを変更したい場合には eth1 の ipaddr の値を変更してください。
変更後の値を反映させるためには、再起動が必要です。

//list[config_network][/etc/config/network ファイル]{
config 'interface' 'wan'
      option 'ifname'         'eth1'
      option 'proto'          'static'
      option 'ipaddr'         '192.168.1.1'
      option 'netmask'        '255.255.255.0'

config 'switch' 'eth0'
      option 'enable'         '1'
      
config 'interface' 'loopback'
      option 'ifname'         'lo'
      option 'proto'          'static'
      option 'ipaddr'         '127.0.0.1'
      option 'netmask'        '255.0.0.0'

config 'switch_vlan'
      option 'device'         'eth0'
      option 'vlan'           '1'
      option 'ports'          '1 0t'

config 'switch_vlan'
      option 'device'         'eth0'
      option 'vlan'           '2'
      option 'ports'          '2 0t'

config 'switch_vlan'
      option 'device'         'eth0'
      option 'vlan'           '3'
      option 'ports'          '3 0t'

config 'switch_vlan'
      option 'device'         'eth0'
      option 'vlan'           '4'
      option 'ports'          '4 0t'

config 'interface'
      option 'ifname'         'eth0.1'
      option 'proto'          'static'
      
config 'interface'
      option 'ifname'         'eth0.2'
      option 'proto'          'static'

config 'interface'
      option 'ifname'         'eth0.3'
      option 'proto'          'static'

config 'interface'
      option 'ifname'         'eth0.4'
      option 'proto'          'static'
//}


OpenFlow 関連の設定は、openflow ファイルに記載されています
(@<list>{config_openflow})。
OpenFlow スイッチのポートとして使用する
LAN 側の 4 ポートはそれぞれ eth0.1, eth0.2, eth0.3, eth0.4 に対応しており、
ofports オプションの部分に指定されます。
また OpenFlow コントローラの IP アドレス、ポート番号は ofctl の値として
指定されるので、変更したい場合はこのオプションの値を変更してください。
変更した設定は、以下のコマンドで反映させることができます。

//cmd{
$ /etc/init.d/openflow restart
//}

//list[config_openflow][/etc/config/openflow ファイル]{
config 'ofswitch'
      option 'dp' 'dp0'
      option 'ofports' 'eth0.1 eth0.2 eth0.3 eth0.4'
      option 'ofctl' 'tcp:192.168.1.10:6633'
      option 'mode'  'outofband'
//}

== Trema とつないでみよう

Trema のサンプルアプリ learning-switch (@<chap>{learning_switch}) を使って、
今回作った OpenFlow スイッチを動かしてみましょう。
コントローラを動かすホストに加え、OpenFlow スイッチを使って
通信を行うホストを二台用意して、@<img>{network} のように接続します。

//image[network][Trema との接続][scale=0.5]

=== Trema の起動

#@# Trema 側の設定 (Learning Switch ？)

まずコントローラとして learning-switch を起動します。

//cmd{
$ cd trema
$ ./trema run ./src/examples/learning_switch/learning-switch.rb -d
//}

以下のコマンドでスイッチとコントローラ間に TCP コネクションが
張られているかを確認しましょう。

//cmd{
$ netstat -an -A inet | grep 6633
//}

TCP コネクションはスイッチ側からコントローラへと接続されます。
もし、接続がされてないければ、OpenFlow スイッチに telnet で
ログインして、openflow 機能の再起動を行ってください。

//cmd{
$ /etc/init.d/openflow restart
//}

=== スイッチの情報を取得する

OpenFlow プロトコルには、スイッチから情報を取得するための
メッセージがいくつか定義されています。
これらのメッセージを使って、スイッチ情報を取得・表示するための
show_description コマンドが Trema Apps に用意されています。
まず、コマンドを使えるように用意します。

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

Manufacturer description から Human readable description of datapath までは、
Stats Request メッセージでタイプに OFPST_DESC を指定すると取得できる
情報です。また、Datapath ID 以降の情報は、Features Request メッセージで
取得できる情報です。

今回の OpenFlow スイッチの実装として、Stanford 大学で作成された
リファレンススイッチが使用されていることが分かります。
また、OpenFlow スイッチとして動作するポート eth0.1 から eth0.4 までと
tap0 @<fn>{tap0} が定義されています。

\footnote[tap0][tap0 は、内部的に使われるポートであり、ユーザが直接使うことはありません。]

=== フローを表示する

それでは、OpenFlow スイッチに接続する二つのホスト間で通信ができるかを
確認してみましょう。
192.168.2.1 のホストから 192.168.2.2 のホストに向けて
ping をうってみてください。

\\cmd{
$ ping 192.168.2.2
PING 192.168.2.2 (192.168.2.2) 56(84) bytes of data.
64 bytes from 192.168.2.2: icmp_req=1 ttl=64 time=18.0 ms
64 bytes from 192.168.2.2: icmp_req=2 ttl=64 time=0.182 ms
...
\\}

実際にフローが設定されているか、確認してみましょう。
OpenFlow スイッチに telnet でログインして、以下のコマンドを実行します。

//cmd{
root@OpenWrt:~# dpctl dump-flows unix:/var/run/dp0.sock 
stats_reply (xid=0x8e5d6e05): flags=none type=1(flow)
  cookie=38, duration_sec=338s, duration_nsec=858000000s,		\
  ...									\
  nw_proto = 1, nw_src=192.168.2.1,nw_dst=192.168.2.2,nw_tos=0x00,	\
  icmp_type=8,icmp_code=0,actions=output:4
  cookie=40, duration_sec=338s, duration_nsec=855000000s,		\
  ...									\ 
  nw_proto = 1, nw_src=192.168.2.2,nw_dst=192.168.2.1,nw_tos=0x00,	\
  icmp_type=0,icmp_code=0,actions=output:1
//}

OpenFlow プロトコルには、スイッチ側のフローエントリをコントローラ側から
取得するためのメッセージが存在します。
このメッセージを使って取得したフローエントリを表示する flow_dumper コマンドが、
Trema apps に用意されています。

//cmd{
# (cd apps/flow_dumper; make)
# TREMA_HOME=./trema apps/flow_dumper/flow_dumper
[0x00002320698790] priority = 65535, match = [wildcards = 0, in_port = 1, \
  ... 		   	      	     	     		     	       	  \
  nw_proto = 1, nw_src = 192.168.2.1/32, nw_dst = 192.168.2.2/32, 	  \ 
  tp_src = 8, tp_dst = 0], actions = [output: port=4 max_len=65535]
[0x00002320698790] priority = 65535, match = [wildcards = 0, in_port = 4, \ 
  ... 		   	      	     	     		     	       	  \
  nw_proto = 1, nw_src = 192.168.2.2/32, nw_dst = 192.168.2.1/32, 	  \ 
  tp_src = 0, tp_dst = 0], actions = [output: port=1 max_len=65535]
//}

OpenFlow スイッチ側で確認したエントリが取得できていることが確認できたでしょうか？
もしかしたら、ping 以外にも OS が独自にだしているパケットによりフローが出来ているかも
しれません。その場合はもう一度 OpenFlow スイッチ側のエントリも確認してみてください。

== まとめ/参考文献

本章で学んだことは、以下の三つです。

 * 数千円の無線 LAN ルータを OpenFlow スイッチに改造し、
   learning-switch を使った動作確認を行いました。
 * show_description コマンドを用意して、
   スイッチの情報を取得する方法について学びました。
 * flow_dumper コマンドを使い、実際に設定されているフローの
   確認を行いました。

今回は learning-switch を使いましたが、他のアプリケーションを
用いることももちろん可能です。もし、今回の OpenFlow スイッチを
二台以上用意できるなら、routing-switch (@<chap>{routing_switch}) を
使うことがお勧めです。

: OpenWRT (@<tt>{https://openwrt.org/})
  無線 LAN ルータで Linux を動かせれば、もっといろいろなことが
  出来るんじゃないか？では、やってみよう！そんな風に思った人が、
  始めたプロジェクトです。いまでは、市販品には搭載されていないさまざまな機能が
  提供されています。

: OpenFlow 対応ファームウェア作者のSRCHACK 氏の Blog (@<tt>{http://www.srchack.org/})
  無線 LAN ルータで Linux が動くのであれば、OpenFlow も動くんじゃないか？
  今回紹介したプロジェクトを始めたとき、SRCHACK さんはそんな風に思ったに
  違いありません。

: WHR-G301N の OpenFlow 化手順
   ( @<tt>{http://www.srchack.org/article.php?story=20120324164358634} )

: オープンルータ・コンペティション ( @<tt>{http://www.interop.jp/2012/orc/} )
  
  
