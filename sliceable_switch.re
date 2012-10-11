= スライス機能つきスイッチ

//lead{
実用的な大規模ネットワークの一例として、IaaS のようなクラウドサービスを実現する仕組みを見て行きましょう。@<chap>{routing_switch}で紹介したルーティングスイッチの応用です。
//}

よく、クラウドサービスの本質は仮想化だと言われます。ユーザーに提供する各種リソースを雲の向こうに仮想化することで、ユーザーから見るとあたかも無限のリソースがいつでも使えるように見せるのです。

クラウドで仮想化されるリソースには主にネットワークとサーバがありますが、このうちネットワークの仮想化は OpenFlow の得意分野です。ネットワークを OpenFlow で仮想化してやることで、ユーザーごとに専用のネットワークをいつでもオンデマンドで提供できるようになります。本章で紹介する「@<em>{スライス機能つきスイッチ}」は、そのような仮想ネットワーク機能を提供するソフトウェア部品の一つです。

== スライスとは何か？

スライスとはひとつの物理ネットワークを論理的なネットワークに分割することで、たくさんのユーザが独立した専用ネットワークを使えるようにするものです (@<img>{slice})。たとえば IaaS のようなたくさんのユーザを限られた台数の物理サーバに集約するシステムでは、物理サーバを仮想マシンで、またネットワークをスライスで分割することでそれぞれのユーザに仮想的な専用環境 （仮想マシン＋仮想ネットワーク）を提供します。

//image[slice][スライスとはひとつの物理ネットワークをそれぞれ独立した仮想ネットワークに分割したもの][scale=0.5]

スライスを実現する代表的な技術として VLAN があります。VLAN はスイッチをポート単位や MAC アドレス単位でスライスに分割できます。また VLAN タグと呼ばれる ID をパケットにつけることでスイッチをまたがったスライスも作れます。

ただし、VLAN にはプロトコル上 4096 個までのスライスしか作れないという制約があります。このため、オフィスなどの中小規模ネットワークではともかく、IaaS のようにユーザ数がゆうに数万を超えるオーダーになる場合には使えません。

一方 OpenFlow でスライスを実装すればこの制約を超えられます。フローによって同じスライス内にあるホスト同志のみが通信できるようにすれば、既存の VLAN の仕組みを使わなくてもフローだけでスライスを実現できるからです。つまり OpenFlow を使えば、「スライス数に制限の無い VLAN」を作れます。

OpenFlow によるスライス実装のひとつが「スライス機能つきスイッチ」です。これは@<chap>{routing_switch}で紹介したルーティングスイッチを改造したもので、スライス数の上限なくたくさんのスライスを作れます。また、実際に OpenStack など IaaS 構築用ミドルウェアの一部として使うことも考慮されており、REST API を通じてスライスの作成/削除などの操作ができます。

== スライスによるネットワーク仮想化

スライス機能つきスイッチが、どのようにネットワークを仮想化するかを見てみましょう。

//image[sliceable_switch_overview][スライス機能つきスイッチが作るスライス (仮想ネットワーク)][scale=0.5]

//noindent
@<img>{sliceable_switch_overview} は、3 つの OpenFlow スイッチから成るネットワークを 2 つのスライスに分割した例です。スライスごとにひとつの仮想スイッチが作られ、スライスに属するすべてのホストはこの仮想スイッチに接続します。それぞれの仮想スイッチは独立しているので、同じスライス内のホスト同士はパケットをやりとりできますが，スライスをまたがったパケットのやりとりはできません。

このようにスライスを使うと、ユーザやアプリケーションごとにそれぞれ独立したネットワークを作れます。ユーザの追加やアプリの起動に応じて、オンデマンドで専用ネットワークを作ったり消したりできるのです。もちろん、これは VLAN などの特別な仕組みは使わずに OpenFlow だけで実現しているので、スライスの数は無制限に増やせます。

=== スライスの実現

スライス機能つきスイッチは次のように動作します (@<img>{sliceable_switch})。

//image[sliceable_switch][OpenFlow ネットワークを 2 つのスライスに分割する]

 1. スライス機能つきスイッチは、 スイッチが受信したパケットを Packet In メッセージで受け取ります。
 2. 次に FDB を検索し、宛先であるホストが接続されているスイッチ、ポートを検索します。
 3. はじめに送信元からのパケットを受信したポートと、宛先となるホストが接続しているポートとが、同じスライスに所属しているかを判定します。もし、同じスライスではない場合には、以降の処理は行われません。
 4. Packet In を送ってきたスイッチから出口となるスイッチまでの最短パスを計算します。
 5. 最短パスに沿ってパケットが転送されるよう、各スイッチそれぞれに Flow Mod メッセージを送り、フローを設定します。
 6. その後、Packet In でコントローラに送られたパケットを 2 で決定したスイッチのポートから出力するよう、Packet Out メッセージを送ります。

@<chap>{routing_switch} で説明したルーティングスイッチの動作と見比べてみましょう。基本的な動きは、ほぼ同じであることが分かるでしょう。

スライス機能つきスイッチが、ルーティングスイッチと異なる点は、上記のステップ 3 の処理が付け加えられている点です。スライス機能つきスイッチは、送信元と宛先ホストがそれぞれ同じスライスに所属しているかを判定します。両者が同じスライスに所属している場合のみ、フローの設定等の以降の処理を行います。この動作により、異なるスライスに所属するホスト間における通信を禁止します。

== 実行してみよう

=== 準備

スライス機能つきスイッチも、ルーティングスイッチと同様に @<tt>{https://github.com/trema/apps/} にてソースコードが公開されています。ソースコードをまだ取得していない場合は、前章を参考に @<tt>{git} で取得しておいてください。

スライス機能つきスイッチは、ルーティングスイッチから派生したアプリケーションです。そのため、ルーティングスイッチと同様に @<tt>{topology} と連動して動作します。また、フロー設定を行う @<tt>{flow_manager} とも連動して動作するようになっていますので、これらのモジュールも合わせて @<tt>{make} してください。

//cmd{
% (cd ./apps/topology/; make)
% (cd ./apps/flow_manager/; make)
% (cd ./apps/sliceable_switch; make)
//}

スライス機能つきスイッチでは、スライスに関する情報を格納するためのデータベースとして、sqlite3 を用いています。以下のようにして、@<tt>{apt-get} で関連するモジュールをインストールした後、データベースファイルを作成してください。

//cmd{
% sudo apt-get install sqlite3 libdbi-perl libdbd-sqlite3-perl libwww-Perl
% (cd ./apps/sliceable_switch; ./create_tables.sh)
A filter entry is added successfully.
//}

=== スライス機能つきスイッチを実行する

//image[slicing][スライス機能つきスイッチで制御するネットワーク構成]

それではさっそくスライス機能つきスイッチを動かしてみましょう。今回はエミュレータ機能を使い、@<img>{slicing} のようなネットワークを作ります。@<list>{conf} に示す内容のファイルを、@<tt>{network.conf} という名前で用意してください。

//list[conf][network.conf]{
vswitch {
  datapath_id "0xe0"
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
link "0xe0", "host2"
link "0xe0", "host3"
link "0xe0", "host4"


run {
  path "./apps/topology/topology"
}

run {
  path "./apps/topology/topology_discovery"
}

run {
  path "./apps/flow_manager/flow_manager"
}

run {
  path "./apps/sliceable_switch/sliceable_switch"
  options "-s", "./apps/sliceable_switch/slice.db", "-a", "./apps/sliceable_switch/filter.db"
}

event :port_status => "topology", :packet_in => "filter", :state_notify => "topology"
filter :lldp => "topology_discovery", :packet_in => "sliceable_switch"
//}

trema のディレクトリに移動し、スライス機能つきスイッチを起動します。
スライス機能つきスイッチの起動には、ルート権限が必要です。@<tt>{sudo} を使って、以下のように起動してください。

//cmd{
% sudo trema run -c ./network.conf
//}

このように起動しただけでは、スライス機能つきスイッチは動作しません。スライスの設定が必要です。今回は @<img>{slicing} のように二つのスライスを作ってみましょう。

スライスの作成には、@<tt>{sliceable_switch} のディレクトリに用意されている @<tt>{slice} コマンドを使用します。このコマンドを使って、以下のように、二つのスライス @<tt>{slice1, slice2} を作ってみましょう。

//cmd{
% cd apps/sliceable_switch
% ./slice create slice1
A new slice is created successfully.
% ./slice create slice2
A new slice is created successfully.
//}

次は作成したそれぞれのスライスにホストを所属させます。その方法には、ホストが接続しているポートの指定と、ホストの MAC アドレスの登録の二通りがあります。今回は後者の方法で試してみましょう。以下のように @<tt>{host1, host2} の MAC アドレスを @<tt>{slice1} に、@<tt>{host3, host4} の MAC アドレスを @<tt>{slice2} に、それぞれ登録します。

//cmd{
% ./slice add-mac slice1 00:00:00:01:00:01
A MAC-based binding is added successfully.
% ./slice add-mac slice1 00:00:00:01:00:02
A MAC-based binding is added successfully.
% ./slice add-mac slice2 00:00:00:01:00:03
A MAC-based binding is added successfully.
% ./slice add-mac slice2 00:00:00:01:00:04
A MAC-based binding is added successfully.
//}

ここまでで準備は完了です。それではまず同じスライスに所属するホスト同士が通信できることを確認してみましょう。

MAC アドレスをスライスに登録する方法では、コントローラは起動直後に、登録した MAC アドレスを持つホストがどこにいるのかを知りません。@<tt>{host1} の位置をコントローラに学習させるために、はじめに @<tt>{host1} から @<tt>{host2} へとパケットを送ります。その後、@<tt>{host2} から @<tt>{host1} へパケットを送り、@<tt>{host1} の受信カウンタを見てみます。@<tt>{host2} からのパケットが受信できていることが確認できます。

//cmd{
% trema send_packet --source host1 --dest host2
% trema send_packet --source host2 --dest host1
% trema show_stats host1 --rx
ip_dst,tp_dst,ip_src,tp_src,n_pkts,n_octets
192.168.0.1,1,192.168.0.2,1,1,50
//}

次に、異なるスライスに所属するホスト同士は通信できないことを確認してみます。@<tt>{slice2} に所属する @<tt>{host4} から @<tt>{slice1} に所属する @<tt>{host1} へとパケットを送ってみましょう。@<tt>{host1} の受信カウンタを見ても、@<tt>{host4} からのパケットが届いていないことがわかります。

//cmd{
% trema send_packet --source host4 --dest host1
% trema show_stats host1 --rx
ip_dst,tp_dst,ip_src,tp_src,n_pkts,n_octets
192.168.0.1,1,192.168.0.2,1,1,50
//}

== 設定の Web サービス化

スライス機能つきスイッチは、スライス設定・管理を Web サービスとして提供するしくみを持っています。

//image[rest][Web サービスを使用する場合のモジュール間の関係][scale=0.5]

@<img>{rest} に示すように、スライスの設定は、@<tt>{slice.db} という名前の sqlite3 のデータベースに格納されています。@<tt>{sliceable_switch} モジュールはこのデータベースから、スライス設定を取得しています。先ほど紹介した @<tt>{slice} コマンドは、スライスの設定をこのデータベースに書き込むために用います。

スライス機能つきスイッチの Web サービスは、Apache 上で動作する CGI で実現しています。HTTP クライアントからアクセスすると、@<tt>{config.cgi} が呼び出され、パースした結果を @<tt>{slice.db} へと書き込みます。

=== Web サービス化のための準備

Web サービス化のためには、少し準備が必要です。まずは必要なモジュールのインストールを行いましょう。

//cmd{
% sudo apt-get install apache2-mpm-prefork libjson-perl
//}

次に Apache を設定します。必要な設定ファイル等は同梱されていますので、以下の手順を実施してください。

//cmd{
% cd apps/sliceable_switch
% sudo cp apache/sliceable_switch /etc/apache2/sites-available
% sudo a2enmod rewrite actions
% sudo a2ensite sliceable_switch
//}

次に、スライス機能つきスイッチが参照するデータベースと、CGI 経由でデータベースに書き込むためのスクリプト群を用意し、適切なディレクトリに配置します。そして最後に Apache を再起動します。

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

これで設定は終了です。最後に 各ファイルが適切に配置されていることを確認してください。

//cmd{
% ls /home/sliceable_switch/*
/home/sliceable_switch/db:
filter.db  slice.db

/home/sliceable_switch/script:
Filter.pm  Slice.pm  config.cgi
//}

=== Web サービスを使ってみる

まず、スライスを作ってみましょう。@<tt>{slice1} という ID のスライスを作る場合には、JSON 形式のファイル (@<list>{slice.json}) を用意します。

//list[slice.json][slice.json]{
{
  "id" : "slice1",
  "description" : "Trema-team network"
}
//}

この JSON 形式のファイルを、@<tt>{/networks} という URI に POST メソッドで送ることで、スライスを作ることができます。

@<tt>{httpc} というテスト用の HTTP クライアントが、@<tt>{sliceable_switch} 内の @<tt>{test/rest_if/} ディレクトリ配下に用意されていますので、これを用います。Apache の待ち受けポートは 8888 に設定されていますので、以下のように実行してみましょう。

//cmd{
% cd ./test/rest_if
% ./httpc POST http://127.0.0.1:8888/networks ./slice.json
Status: 202 Accepted
Content:
{"id":"slice1","description":"Trema-team network"}
//}

成功すれば、上記のように表示されるはずです。

次に、MAC アドレスをスライスに対応させてみましょう。MAC アドレスとスライスの対応のことをアタッチメントと呼びます。アタッチメントを作る場合、割り当てる MAC アドレスを指定した JSON 形式のファイルを用意します(@<list>{attachment.json})。

//list[attachment.json][attachment.json]{
{
  "id" : "attach0",
  "mac" : "01:00:00:01:00:01"
}
//}

アタッチメントを作成する際に使用する URI は、@<tt>{/networks/<スライスID>/attachments} です。以下のように @<tt>{attachment.json} を @<tt>{slice1} に割り当てます。

//cmd{
% ./httpc POST http://127.0.0.1:8888/networks/slice1/attachments attachment.json
Status: 202 Accepted
//}

今度は、ポートをスライスに割り当てる方法を見ていきましょう。これまでと同様に、JSON 形式で、割り当てるポートに関する情報を記載したファイルを用意します(@<list>{port.json})。ここでは、データパス ID が @<tt>{0xe0} である OpenFlow スイッチの 33 番目のポートを指定しています。このポートからパケットを出す際に VLAN タグを付与したい場合には @<tt>{vid} のパラメータにその値を設定します。VLAN タグの設定が不要の場合には、この例のように 65535 としてください。

//list[port.json][port.json]{
{
  "id" : "port0",
  "datapath_id" : "0xe0",
  "port" : 33,
  "vid" : 65535
}
//}

このとき使用する URI は @<tt>{/networks/<スライスID>/ports} になります。以下のようにして、@<list>{port.json} で指定したポートを @<tt>{slice1} というスライスにを割り当てます。

//cmd{
% ./httpc POST http://127.0.0.1:8888/networks/slice1/ports ./port.json
Status: 202 Accepted
//}

これまでの設定がきちんと行われているかを確認してみましょう。@<tt>{/networks/<スライスID>} に GET メソッドでアクセスすることで、スライスに関する情報を取得できます。@<tt>{slice1} に関する情報を取得してみましょう。

//cmd{
% ./httpc GET http://127.0.0.1:8888/networks/slice1
Status: 200 OK
Content:
{ "bindings" :
  [
    {
      "type" : 2,
      "id" : "attach0",
      "mac" : "01:00:00:01:00:01"
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

この出力結果は見やすいようにインデント表示にしていますが、実際には改行なしで表示されます。先に設定した内容が、きちんと反映されているかの確認できます。

=== Web サービスを使いこなす

今回紹介した以外にも、@<table>{API} にあるような API を用意しています。

//table[API][REST API 一覧]{
動作		Method	URI
----------------------------
スライス作成	POST	/networks
スライス一覧取得	GET	/networks
スライス詳細取得	GET	/networks/<スライスID>
スライス削除	DELETE	/networks/<スライスID>
スライス変更	PUT	/networks/<スライスID>
ポート作成	POST	/networks/<スライスID>/ports
ポート一覧取得	GET	/networks/<スライスID>/ports
ポート詳細取得	GET	/networks/<スライスID>/ports/<ポートID>
ポート削除	DELETE	/networks/<スライスID>/ports/<ポートID>
アタッチメント作成	POST	/networks/<スライスID>/attachments
アタッチメント一覧取得	GET	/networks/<スライスID>/attachments
アタッチメント詳細取得	GET	/networks/<スライスID>/attachments/<アタッチメントID>
アタッチメント削除	DELETE	/networks/<スライスID>/attachments/<アタッチメントID>
//}

この API の仕様は、以下のサイトで公開していますので、詳細を確認したい人はこちらをご参照ください。

 * Sliceable Network Management API ( @<tt>{https://github.com/trema/apps/wiki} )

== まとめ/参考文献

本章で学んだことは、次の 2 つです。

 * 大きなネットワークを複数にスライスして使用することができる、スライス機能つきスイッチがどのように動作するかを見てみました。また、スライス機能つきスイッチを実際に動作させ、ホストが同一のスライスに所属している場合のみ通信が許可される仕組みを学びました。
 * スライス機能つきスイッチを設定するための Web サービス API を使って、スライスを設定する方法について学びました。

本章で紹介した Web サービス API を使うことで、他のシステムとの連携ができます。次の章では、スライス機能つきスイッチを使った OpenFlow ネットワークとクラウド管理システムとの連携についても紹介します。

