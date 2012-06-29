= スライス機能つきスイッチ

== ネットワークを分割する

スライス機能つきスイッチは，OpenFlowネットワーク全体をスライスに分割し，
複数の L2 スイッチに仮想化します。
ちょうど，L2 スイッチを複数の VLAN に分けて使うイメージです。

スライスを 2 つ設定した例を @<img>{sliceable_switch} に示します。
同一のスライスに接続されたホスト同士はパケットをやりとりできますが，
異なるスライスに接続されたホスト同士ではできません。
このようにうまくスライスを設定することで，
アプリケーションやグループ別など用途に応じて独立した
ネットワークを作ることができます。

//image[sliceable_switch][スライスの作成]

=== スライスの実現

== 実行してみよう

=== 準備

スライス機能つきスイッチも、@<chap>{routing_switch} で取り上げた
ルーティングスイッチと同様に @<tt>{https://github.com/trema/apps/} にて
ソースコードが公開されています。ソースコードをまだ取得していない場合は、
前章を参考に @<tt>{git} で取得しておいてください。
ここでは、以下のようなディレクトリ構成になっていることが前提で、
説明します。

//cmd{
% ls -F
apps/   trema/
//}

スライス機能つきスイッチは、ルーティングスイッチから
派生したアプリケーションです。そのため、ルーティングスイッチと
同様に @<tt>{topology} と連動して動作します。
また、フロー設定を行う @<tt>{flow_manager} とも連動して
動作するようになっていますので、これらのモジュールも合わせて
@<tt>{make} してください。

//cmd{
% (cd apps/topology/; make)
% (cd apps/flow_manager/; make)
% (cd apps/sliceable_switch; make)
//}

=== REST API の設定

//cmd{
% sudo apt-get install sqlite3 libdbi-perl libdbd-sqlite3-perl
% sudo apt-get install apache2-mpm-prefork libjson-perl
//}
 
//cmd{
% cd apps/sliceable_switch
% ./create_tables.sh
A filter entry is added successfully.
% sudo cp apache/sliceable_switch /etc/apache2/sites-available
% sudo a2enmod rewrite actions
% sudo a2ensite sliceable_switch
% sudo mkdir -p /home/sliceable_switch/script
% sudo mkdir /home/sliceable_switch/db
% sudo cp Slice.pm Filter.pm config.cgi /home/sliceable_switch/script
% sudo cp *.db /home/sliceable_switch/db
//}

//cmd{
% ls /home/sliceable_switch/*
/home/sliceable_switch/db:
filter.db  slice.db

/home/sliceable_switch/script:
Filter.pm  Slice.pm  config.cgi
//}

=== 試してみる

それでは試してみましょう。

//cmd{
% cd ../../trema
% sudo ./trema run -c ../apps/sliceable_switch/sliceable_switch_external.conf
//}

次に REST API を使ってスライスの設定を行ってみます。
スライスとポートを対応付けるためには、OpenFlow スイッチ内部で
管理されているポート番号 (Port no) が必要です。
以下のコマンドで取得してみましょう。

//cmd{
% (cd ../apps/show_description; make)
% TREMA_HOME=. sudo -E ../apps/show_description/show_description
Manufacturer description: Nicira Networks, Inc.
Hardware description: Open vSwitch
Software description: 1.2.2
Serial number: None
Human readable description of datapath: None
Datapath ID: 0xe0
Port no: 3(0x3)(Port up)
  Hardware address: xx:xx:xx:xx:xx:03
  Port name: eth3
Port no: 2(0x2)(Port up)
  Hardware address: xx:xx:xx:xx:xx:02
  Port name: eth1
Port no: 4(0x4)(Port up)
  Hardware address: xx:xx:xx:xx:xx:04
  Port name: eth2
Port no: 65534(0xfffe:Local)(Port down)
  Hardware address: xx:xx:xx:xx:xx:ff
  Port name: vsw_0xe0
Port no: 1(0x1)(Port up)
  Hardware address: xx:xx:xx:xx:xx:01
  Port name: eth0
//}

今回は、@<tt>{eth0} から @<tt>{eth3} までのポートの番号は、
OpenFlow スイッチ内部ではそれぞれ 1 から 4 と認識されているのが
分かります。お手持ちの環境での結果が異なる場合、
以降の手順は、適宜読み替えを行って、試してみてください。

//list[network1][create_network1.json]{
{ "id": "slice1", "description": "Slice 1" }
//}

//list[port1][create_port1.json]{
{ "id": "port_eth0", "datapath_id": "0xe0", "port": 1, "vid": 65535 }
//}

//list[port2][create_port2.json]{
{ "id": "port_eth1", "datapath_id": "0xe0", "port": 2, "vid": 65535 }
//}

//list[network2][create_network2.json]{
{ "id": "slice2", "description": "Slice 2" }
//}

//list[port3][create_port3.json]{
{ "id": "port_eth2", "datapath_id": "0xe2", "port": 1, "vid": 65535 }
//}

//list[port4][create_port4.json]{
{ "id": "port_eth3", "datapath_id": "0xe2", "port": 2, "vid": 65535 }
//}

== VLAN (従来技術) との違い

=== 動作を比べてみる
=== 長所・短所

== まとめ/参考文献
