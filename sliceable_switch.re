= スライス機能つきスイッチ

本章では、前章で紹介したルーティングスイッチを発展させた、スライス機能つきスイッチを紹介します。スライス機能つきスイッチは，OpenFlow ネットワーク全体をスライスに分割し，複数の L2 ネットワークを実現します。ちょうど，L2 スイッチを複数の VLAN に分けて使うイメージです。

このスライス機能つきスイッチを使うことで、IaaS 型データセンターのネットワークを実現できます。

== ネットワークを分割する

スライスを 2 つ設定した例を @<img>{sliceable_switch} に示します。同一のスライスに接続されたホスト同士はパケットをやりとりできますが，異なるスライスに接続されたホスト同士ではできません。このようにうまくスライスを設定することで，アプリケーションやグループ別など用途に応じて独立したネットワークを作ることができます。

//image[sliceable_switch][スライスの作成]

=== スライスの実現

== 実行してみよう

=== 準備

スライス機能つきスイッチも、@<chap>{routing_switch} で取り上げたルーティングスイッチと同様に @<tt>{https://github.com/trema/apps/} にてソースコードが公開されています。ソースコードをまだ取得していない場合は、前章を参考に @<tt>{git} で取得しておいてください。ここでは、以下のようなディレクトリ構成になっていることが前提です。

//cmd{
% ls -F
apps/   trema/
//}

スライス機能つきスイッチは、ルーティングスイッチから派生したアプリケーションです。そのため、ルーティングスイッチと同様に @<tt>{topology} と連動して動作します。また、フロー設定を行う @<tt>{flow_manager} とも連動して動作するようになっていますので、これらのモジュールも合わせて @<tt>{make} してください。

//cmd{
% (cd apps/topology/; make)
% (cd apps/flow_manager/; make)
% (cd apps/sliceable_switch; make)
//}

=== 試してみる

それではさっそくスライス機能つきスイッチを動かしてみましょう。今回はエミュレータ機能を使い、図 X のようなネットワークを作ります。@<tt>{network.conf} に示す内容のファイルを、@<tt>{network.conf} という名前で用意してください。

//list[conf][network.conf]{
vswitch {
  datapath_id "0xe0"
}

vhost ("host1") {
  ip "192.168.0.1"
  netmask "255.255.0.0"
  mac "00:00:00:00:00:01"
}

vhost ("host2") {
  ip "192.168.0.2"
  netmask "255.255.0.0"
  mac "00:00:00:00:00:02"
}

vhost ("host3") {
  ip "192.168.0.3"
  netmask "255.255.0.0"
  mac "00:00:00:00:00:03"
}

vhost ("host4") {
  ip "192.168.0.4"
  netmask "255.255.0.0"
  mac "00:00:00:00:00:04"
}

link "0xe0", "host1"
link "0xe0", "host2"
link "0xe0", "host3"
link "0xe0", "host4"


run {
  path "../apps/topology/topology"
}

run {
  path "../apps/topology/topology_discovery"
}

run {
  path "../apps/flow_manager/flow_manager"
}

run {
  path "../apps/sliceable_switch/sliceable_switch"
  options "-s", "../apps/sliceable_switch/slice.db", "-f", "../apps/sliceable_sw
itch/filter.db"
}

event :port_status => "topology", :packet_in => "filter", :state_notify => "topo
logy"
filter :lldp => "topology_discovery", :packet_in => "sliceable_switch"
//}

trema のディレクトリに移動し、スライス機能つきスイッチを起動します。
スライス機能つきスイッチの起動には、ルート権限が必要です。@<tt>{sudo} を使って、以下のように起動してください。

//cmd{
% cd ../../trema
% sudo ./trema run -c ./network.conf
//}

このように起動しただけでは、スライス機能つきスイッチは動作しません。スライスの設定が必要です。今回は図 Y のように二つのスライスを作ってみましょう。

スライスの作成には、@<tt>{sliceable_switch} のディレクトリに用意されている @<tt>{slice} コマンドを使用します。このコマンドを使って、以下のように、二つのスライス @<tt>{slice1, slice2} を作ってみましょう。

//cmd{
% cd ../apps/sliceable_switch
% ./slice create slice1
A new slice is created successfully.
% ./slice create slice2
A new slice is created successfully.
//}

次は作成したそれぞれのスライスにホストを所属させます。その方法には、ホストが接続しているポートの指定と、ホストの MAC アドレスの登録の二通りがあります。今回は後者の方法で試してみましょう。以下のように @<tt>{host1, host2} の MAC アドレスを @<tt>{slice1} に、@<tt>{host3, host4} の MAC アドレスを @<tt>{slice2} に、それぞれ登録を行います。

//cmd{
% ./slice add-mac slice1 00:00:00:00:00:01
A MAC-based binding is added successfully.
% ./slice add-mac slice1 00:00:00:00:00:02
A MAC-based binding is added successfully.
% ./slice add-mac slice2 00:00:00:00:00:03
A MAC-based binding is added successfully.
% ./slice add-mac slice2 00:00:00:00:00:04
A MAC-based binding is added successfully.
//}

ここまでで準備は完了です。それではまず同じスライスに所属するホスト同士が通信できることを確認してみましょう。

MAC アドレスをスライスに登録する方法では、コントローラは起動直後に、登録した MAC アドレスを持つホストがどこにいるのかを知りません。@<tt>{host1} の位置をコントローラに学習させるために、はじめに @<tt>{host1} から @<tt>{host2} へとパケットを送ります。その後、@<tt>{host2} から @<tt>{host1} へパケットを送り、@<tt>{host1} の受信カウンタを見てみます。@<tt>{host2} からのパケットが受信できていることが確認できます。

//cmd{
% cd ../../trema
% ./trema send_packet --source host1 --dest host2
% ./trema send_packet --source host2 --dest host1
% ./trema show_stats host1 --rx
ip_dst,tp_dst,ip_src,tp_src,n_pkts,n_octets
192.168.0.1,1,192.168.0.2,1,1,50
//}

次に、異なるスライスに所属するホスト同士は通信できないことを確認してみます。@<tt>{slice2} に所属する @<tt>{host4} から @<tt>{slice1} に所属する @<tt>{host1} へとパケットを送ってみましょう。@<tt>{host1} の受信カウンタを見ても、@<tt>{host4} からのパケットが届いていないことがわかります。

//cmd{
% ./trema send_packet --source host4 --dest host1
% ./trema show_stats host1 --rx
ip_dst,tp_dst,ip_src,tp_src,n_pkts,n_octets
192.168.0.1,1,192.168.0.2,1,1,50
//}

== REST API で設定する

スライス機能つきスイッチのスライス設定は、前節で紹介したようにコマンドで行うこともできますが、REST ベースの API で行うこともできます。図 Z のように sqlite3 のデータベースがスライス設定を格納しており、@<tt>{sliceable_switch} モジュールはこのデータベースを参照し、スライス設定を取得しています。


REST API を使用するためには、少し準備が必要です。まずは必要なモジュールのインストールを行いましょう。

//cmd{
% sudo apt-get install sqlite3 libdbi-perl libdbd-sqlite3-perl
% sudo apt-get install apache2-mpm-prefork libjson-perl
//}

次に apache の設定を行います。必要な設定ファイル等は同梱されていますので、以下の手順を実施してください。

//cmd{
% cd apps/sliceable_switch
% sudo cp apache/sliceable_switch /etc/apache2/sites-available
% sudo a2enmod rewrite actions
% sudo a2ensite sliceable_switch
//}

次に、スライス機能つきスイッチが参照するデータベースと、CGI 経由でデータベースに設定を書き込むためのスクリプト群を用意し、適切なディレクトリに配置します。そして最後に Apache の再起動を行います。

//cmd{
% ./create_tables.sh
A filter entry is added successfully.
% sudo mkdir -p /home/sliceable_switch/script
% sudo mkdir /home/sliceable_switch/db
% sudo cp Slice.pm Filter.pm config.cgi /home/sliceable_switch/script
% sudo cp *.db /home/sliceable_switch/db
% sudo chown -R www-data.www-data /home/sliceable_switch
% sudo /etc/init.d/apache2 reload
//}

以下のように、各ファイルが適切に配置されていることを確認してください。

//cmd{
% ls /home/sliceable_switch/*
/home/sliceable_switch/db:
filter.db  slice.db

/home/sliceable_switch/script:
Filter.pm  Slice.pm  config.cgi
//}

以上で設定は終了です。ポート 8888 にアクセスすることで、各設定ができるようになっています。

まず、スライスを作ってみましょう。@<tt>{slice1} という ID のスライスを作る場合には、JSON 形式のファイル (@<list>{slice}) を用意します。

//list[slice][slice.json]{
{
  "id" : "slice1",
  "description" : "Trema-team network"
}
//}

@<tt>{/networks} という URI に POST メソッドでアクセスし、slice.json を送ります。

//cmd{
% ./test/rest_if/httpc POST http://127.0.0.1:8888/networks ./slice.json
Status: 202 Accepted
Content:
{"id":"slice1","description":"Trema-team network"}
//}


次にスライスにポートを割り当ててみましょう。URI は @<tt>{/networks/<スライスID>/ports} になります。以下の例では、@<tt>{slice1} というスライスに、データパス ID が @<tt>{0xe0} である OpenFlow スイッチの 33 番目のポートを割り当てています。このポートからパケットを出す際に VLAN tag を付与したい場合には @<tt>{vid} のパラメータにその値を設定します。VLAN tag の設定が不要の場合には、以下の例のように 65535 としてください。

//list[port][port.json]{
{
  "id" : "port0",
  "datapath_id" : "0xe0",
  "port" : 33,
  "vid" : 65535
}
//}

//cmd{
% ./test/rest_if/httpc POST http://127.0.0.1:8888/networks/slice1/ports ./port.json
Status: 202 Accepted
//}

MAC アドレスをスライスに対応させるためには、以下のようにします。URI は、@<tt>{/networks/<スライスID>/attachments} です。

//list[attachment][attachment.json]{
{
  "id" : "attach0",
  "mac" : "01:00:00:00:00:01"
}
//}

//cmd{
% ./test/rest_if/httpc POST http://127.0.0.1:8888/networks/slice1/attachments attachment.json
Status: 202 Accepted
//}

//cmd{
% ./test/rest_if/httpc GET http://127.0.0.1:8888/networks/slice1
Status: 200 OK
Content:
{ "bindings" : 
  [
    {
      "type" : 2,
      "id" : "attach0",
      "mac" : "01:00:00:00:00:01"
    },
    {
      "vid" : 65535,
      "datapath_id" : "224",
      "type" : 1,
      "id" : "port0",
      "port" : 33 
    }
  ],
  "description" : "Trema-team network"
}
//}

ここで紹介した REST API の正式名称は、Sliceable Network Management API です。上記で紹介した以外を、@<table>{API} に示します。

//table[API][Sliceable Network Management API]{
Method	URI	       説明
----------------------------
POST	/networks	スライス作成
GET	/networks	スライス一覧取得
GET	/networks/<スライスID>	スライス詳細取得
DELETE	/networks/<スライスID>	スライス削除
PUT	/networks/<スライスID>	スライス変更
POST	/networks/<スライスID>/ports	ポート作成
GET	/networks/<スライスID>/ports	ポート一覧取得
GET	/networks/<スライスID>/ports/<ポートID>	ポート詳細取得
DELETE	/networks/<スライスID>/ports/<ポートID>	ポート削除
POST	/networks/<スライスID>/attachments	アタッチメント作成
GET	/networks/<スライスID>/attachments	アタッチメント一覧取得
GET	/networks/<スライスID>/attachments/<アタッチメントID>	アタッチメント詳細取得
DELETE	/networks/<スライスID>/attachments/<アタッチメントID>	アタッチメント削除
//}

以下のサイトで仕様が公開されていますので、詳細を確認したい人はこちらをご参照ください、

 * Sliceable Network Management API ( @<tt>{https://github.com/trema/apps/wiki} )

== VLAN (従来技術) との違い

=== 動作を比べてみる
=== 長所・短所

== まとめ/参考文献
