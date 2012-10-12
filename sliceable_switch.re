= スライス機能つきスイッチ

//lead{
実用的な大規模ネットワークの一例として、IaaS のようなクラウドサービスを実現する仕組みを見て行きましょう。@<chap>{routing_switch}で紹介したルーティングスイッチの応用です。
//}

よく、クラウドサービスの本質は仮想化だと言われます。ユーザーに提供する各種リソースを雲の向こうに仮想化することで、ユーザーから見るとあたかも無限のリソースがいつでも使えるように見せるのです。

クラウドで仮想化されるリソースには主にネットワークとサーバがありますが、このうちネットワークの仮想化は OpenFlow の得意分野です。ネットワークを OpenFlow で仮想化してやることで、ユーザーごとに専用のネットワークをいつでもオンデマンドで提供できるようになります。本章で紹介する「@<em>{スライス機能つきスイッチ}」は、そのような仮想ネットワーク機能を提供するソフトウェア部品の一つです。

== スライスとは何か？

スライスとはひとつの物理ネットワークを論理的なネットワークに分割することで、たくさんのユーザが独立した専用ネットワークを使えるようにするものです (@<img>{slice})。たとえば IaaS のようなたくさんのユーザをなるべく少ない台数の物理サーバに集約するシステムでは、物理サーバを仮想マシンで、またネットワークをスライスで分割することでそれぞれのユーザに仮想的な専用環境 （仮想マシン + 仮想ネットワーク）を提供します。

//image[slice][スライスとはひとつの物理ネットワークをいくつかの独立した仮想ネットワークに分割したもの][scale=0.5]

スライスを実現する代表的な技術として VLAN があります。VLAN はスイッチをポート単位や MAC アドレス単位でスライスに分割できます。また VLAN タグと呼ばれる ID をパケットにつけることでスイッチをまたがったスライスも作れます。

ただし、VLAN にはプロトコル上 4096 個までのスライスしか作れないという制約があります。このため、オフィスなどの中小規模ネットワークではともかく、IaaS のようにユーザ数がゆうに数万を超えるオーダーになる場合には使えません。

一方 OpenFlow でスライスを実装すればこの制約を超えられます。フローによって同じスライス内にあるホスト同志のみが通信できるようにすれば、既存の VLAN の仕組みを使わなくてもフローだけでスライスを実現できるからです。つまり OpenFlow を使えば、「スライス数に制限の無い VLAN」を作れます。

OpenFlow によるスライス実装のひとつが「スライス機能つきスイッチ」です。これは@<chap>{routing_switch}で紹介したルーティングスイッチを改造したもので、スライス数の上限なくたくさんのスライスを作れます。また、実際に OpenStack などのクラウド構築ミドルウェアの一部として使うことも考慮されており、REST API を通じてスライスの作成/削除などの操作ができます。

== スライスによるネットワーク仮想化

スライス機能つきスイッチが、どのようにネットワークを仮想化するかを見てみましょう。

//image[sliceable_switch_overview][スライス機能つきスイッチが作るスライス (仮想ネットワーク)][scale=0.5]

//noindent
@<img>{sliceable_switch_overview} は、3 つの OpenFlow スイッチから成るネットワークを 2 つのスライスに分割した例です。スライスごとにひとつの仮想スイッチが作られ、スライスに属するすべてのホストはこの仮想スイッチに接続します。それぞれの仮想スイッチは独立しているので、同じスライス内のホスト同士はパケットをやりとりできますが，スライスをまたがったパケットのやりとりはできません。

このようにスライスを使うと、ユーザやアプリケーションごとにそれぞれ独立したネットワークを作れます。ユーザの追加やアプリの起動に応じて、オンデマンドで専用ネットワークを作ったり消したりできるのです。もちろん、これは VLAN などの特別な仕組みは使わずに OpenFlow だけで実現しているので、スライスの数は無制限に増やせます。

=== スライスの実現方法

実はこのスライス機能は、@<chap>{routing_switch} で説明したルーティングスイッチへのほんの少しの機能追加だけで実現しています。コントローラと OpenFlow スイッチの視点で見ると、スライス機能つきスイッチは次のように動作します (@<img>{sliceable_switch_internals})。

//image[sliceable_switch_internals][スライス機能つきスイッチの動作][scale=0.5]

 1. パケットの道順を指定するためのトポロジ情報を収集します。
 2. スイッチが受信したパケットを Packet In メッセージで受け取ります。
 3. FDB を検索し、宛先であるホストが接続するスイッチとポート番号を決定します。
 4. パケットを受信したポートと宛先ホストが接続するポートとが同じスライスに属するか判定します。もし同じスライスではない場合にはパケットを捨て、以降の処理は行いません。
 5. Packet In を出したスイッチから出口となるスイッチまでの最短パスをステップ 1 で収集したトポロジ情報を使って計算します。
 6. この最短パスに沿ってパケットが転送されるよう、パス上のスイッチそれぞれに Flow Mod を送りフローを書き込みます。
 7. 最初の Packet In を起こしたパケットも宛先に送るために、出口となるスイッチに Packet Out を送ります。

スライス機能つきスイッチがルーティングスイッチと異なるのは、ステップ 4 が付け加えられている点だけです。ステップ 4 では送信元と宛先ホストがそれぞれ同じスライスに属しているかを判定し、同じスライスに所属している場合のみパケットを転送します。それ以外はルーティングスイッチとまったく同じです。

== 実行してみよう

ではスライス機能つきスイッチを使ってネットワーク仮想化を実際に試してみましょう。スライス機能スイッチもルーティングスイッチと同じく Trema Apps の一部として GitHub で公開されています。まだ Trema Apps のソースコードを取得していない人は、次のようにダウンロードしてください。

//cmd{
% git clone https://github.com/trema/apps.git
//}

スライス機能つきスイッチは次の 4 つのアプリケーションが連携して動作します。トポロジー関連のアプリ (@<tt>{topology}、@<tt>{topology_discovery}) を使うところはルーティングスイッチと同じです。

 * @<tt>{topology}: 検出したトポロジ情報を管理する。
 * @<tt>{topology_discovery}: トポロジ情報を検出する。
 * @<tt>{flow_manager}: 複数スイッチへのフロー書き込み API を提供。
 * @<tt>{sliceable_switch}: ルーティングスイッチ本体。

これらの 4 つをセットアップするには、ダウンロードした Trema Apps の @<tt>{topology}, @<tt>{flow_manager}、そして @<tt>{sliceable_switch} を次のようにコンパイルしてください。

//cmd{
% (cd ./apps/topology/; make)
% (cd ./apps/flow_manager/; make)
% (cd ./apps/sliceable_switch; make)
//}

スライス機能つきスイッチはスライス情報を格納するためのデータベースとして sqlite3 を用います。以下のように @<tt>{apt-get} で splite3 関連のパッケージをインストールし、@<tt>{sliceable_switch} 付属のスクリプトで空のスライスデータベースを作成してください。

//cmd{
% sudo apt-get install sqlite3 libdbi-perl libdbd-sqlite3-perl libwww-Perl
% (cd ./apps/sliceable_switch; ./create_tables.sh)
A filter entry is added successfully.
//}

//noindent
これで準備は完了です。

=== スライス機能つきスイッチを動かす

それでは、スライス機能つきスイッチを動かしてみましょう。Trema のネットワークエミュレータ機能を用いて、@<img>{sliceable_switch_network} のネットワークを作ります。

//image[sliceable_switch_network][スイッチ 1 台、ホスト 4 台からなるネットワーク][scale=0.5]

設定ファイルは@<list>{network.conf}のようになります。

//list[network.conf][network.conf]{
vswitch {
  datapath_id "0x1"
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

link "0x1", "host1"
link "0x1", "host2"
link "0x1", "host3"
link "0x1", "host4"


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

スライス機能つきスイッチを起動するには、次のように @<tt>{sudo} を使って root 権限で起動してください。

//cmd{
% sudo trema run -c ./network.conf
//}

//noindent
それでは起動したスライス機能つきスイッチを使って、さっそくいくつかスライスを作ってみましょう。

=== スライスを作る

Trema Apps の @<tt>{sliceable_switch} ディレクトリには、スライスを作成するコマンド @<tt>{slice} が用意されています。このコマンドを使って@<img>{creating_slices}のような 2 枚のスライス @<tt>{slice1, slice2} を作ってみましょう。

//image[creating_slices][スライスを 2 枚作る][scale=0.5]

//cmd{
% cd apps/sliceable_switch
% ./slice create slice1
A new slice is created successfully.
% ./slice create slice2
A new slice is created successfully.
//}

スライスができたらスライスにホストを追加します。以下のように @<tt>{host1, host2} の MAC アドレスを @<tt>{slice1} に、@<tt>{host3, host4} の MAC アドレスを @<tt>{slice2} に、それぞれ登録します。

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

//noindent
とても簡単にスライスを作れました。それではさっそくきちんとネットワークが分割できているか確認してみましょう。

=== スライスによるネットワーク分割を確認する

作ったスライスが正しく動作しているか確認するためには、次の 2 つを試してみれば良さそうです。

 1. 同じスライスに属するホスト同士で通信できること
 2. 異なるスライスに属するホスト間で通信できないこと

//noindent
これは今までやってきた通り、@<tt>{trema send_packet} コマンドと @<tt>{trema show_stats} を使えば簡単に確認できます。たとえば次のようにすると、同じスライス @<tt>{slice1} に属するホスト @<tt>{host1} と @<tt>{host2} で通信できていることがわかります。

//cmd{
% trema send_packet --source host1 --dest host2
% trema send_packet --source host2 --dest host1
% trema show_stats host1 --rx
ip_dst,tp_dst,ip_src,tp_src,n_pkts,n_octets
192.168.0.1,1,192.168.0.2,1,1,50
//}

異なるスライス間での通信はどうでしょう。これも次のように簡単にテストできます。

//cmd{
% trema reset_stats host1
% trema send_packet --source host4 --dest host1
% trema show_stats host1 --rx
ip_dst,tp_dst,ip_src,tp_src,n_pkts,n_octets
//}

//noindent
たしかに、@<tt>{slice1} に属する @<tt>{host1} から @<tt>{slice2} に属する @<tt>{host4} へのパケットは届いていません。以上で、ひとつのネットワークが 2 つの独立したスライスにうまく分割できていることを確認できました。

== REST API を使う

スライス機能つきスイッチは OpenStack などのクラウド構築ミドルウェアと連携するための REST API を提供しています。スライスの作成や削除を REST と JSON による Web サービスとして提供することで、さまざまなプログラミング言語から仮想ネットワーク機能を使えます。これによって、仮想ネットワーク機能を必要とするいろいろなミドルウェアからの利用がしやすくなります。

スライス機能つきスイッチの REST API は、Apache 上で動作する CGI として実現しています (@<img>{rest_overview})。クラウド構築ミドルウェアなどから HTTP でアクセスすると、スライスの変更をスライス DB へと反映し、スライス機能つきスイッチはこの内容を実際のスライス構成に反映します。

//image[rest_overview][スライス機能つきスイッチの REST API 構成][scale=0.5]

//noindent
では、さっそく REST API をセットアップして使ってみましょう。

=== セットアップ

まずは REST API の動作に必要ないくつかのパッケージをインストールしましょう。

//cmd{
% sudo apt-get install apache2-mpm-prefork libjson-perl
//}

次は CGI の動作に必要な Apache の設定です。必要な設定ファイル等はすべて Trema/Apps の @<tt>{sliceable_switch} ディレクトリに入っていますので、以下の手順でコピーし Apache の設定に反映してください。

//cmd{
% cd apps/sliceable_switch
% sudo cp apache/sliceable_switch /etc/apache2/sites-available
% sudo a2enmod rewrite actions
% sudo a2ensite sliceable_switch
//}

次に CGI 本体とスライスデータベース、そしてデータベースを操作するための各種スクリプトを次の手順で配置します。最後に Apache を再起動し準備完了です。

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

正しくセットアップするとファイル構成は次のようになります。

//cmd{
% ls /home/sliceable_switch/*
/home/sliceable_switch/db:
filter.db  slice.db

/home/sliceable_switch/script:
Filter.pm  Slice.pm  config.cgi
//}

=== REST API でスライスを作る

REST API 経由でスライスを作るには、スライスの情報を書いた JSON 形式のファイルを作り、これを HTTP で REST API の CGI に送ってやります。たとえば @<tt>{slice_yutaro} という名前のスライスを作るには、次の内容のファイル (@<list>{slice.json}) を用意します。

//list[slice.json][slice.json]{
{
  "id" : "slice_yutaro",
  "description" : "Yutaro's Network"
}
//}

次にこの JSON 形式のファイルを @<tt>{/networks} という URI に POST メソッドで送ります。Trema/Apps の @<tt>{sliceable_switch/test/rest_if/} ディレクトリには @<tt>{httpc} という簡単な HTTP クライアントが用意されていますので、これを使ってみましょう。Apache の待ち受けポートは 8888 に設定されていますので、以下のように実行します。

//cmd{
% cd ./test/rest_if
% ./httpc POST http://127.0.0.1:8888/networks ./slice.json
//}

実行すると次のように実行結果と作成したスライスの情報が表示されます。

//cmd{
Status: 202 Accepted
Content:
{"id":"slice_yutaro","description":"Yutaro's Network"}
//}

=== スライスにホストを追加する

作ったスライスにはホストを追加できます。次のように追加したいホストの MAC アドレスを記述した JSON 形式のファイルを用意します (@<list>{attachment.json})。

//list[attachment.json][attachment.json]{
{
  "id" : "yutaro_desktop",
  "mac" : "01:00:00:01:00:01"
}
//}

ホスト追加の URI は @<tt>{/networks/<スライスの名前>/attachments} です。作った JSON ファイルをこの URI に POST メソッドで送ってください。

//cmd{
% ./httpc POST http://127.0.0.1:8888/networks/slice_yutaro/attachments attachment.json
Status: 202 Accepted
//}

スライスに追加するホストの指定には、ホストの MAC アドレスだけでなくホストが接続するスイッチのポート番号も使えます。次のようにホストが接続するスイッチのデータパス ID と、接続するポート番号を記述した JSON 形式のファイルを用意してください (@<list>{port.json})。もし、このポートから出るパケットに VLAN タグを付与したい場合には @<tt>{vid} にその値を設定します。不要な場合には 65535 としてください。

//list[port.json][port.json]{
{
  "id" : "port0",
  "datapath_id" : "0x1",
  "port" : 33,
  "vid" : 65535
}
//}

ポート番号を指定してホストを追加するには、@<tt>{/networks/<スライスの名前>/ports} という URI を使います。今までと同じく、作った JSON ファイルを POST してみましょう。

//cmd{
% ./httpc POST http://127.0.0.1:8888/networks/slice_yutaro/ports ./port.json
Status: 202 Accepted
//}

=== スライスの構成を見る

これまでの設定がきちんと行われているかを確認してみましょう。@<tt>{/networks/<スライスの名前>} に GET メソッドでアクセスすることで、スライスに関する情報を取得できます。先ほど作った @<tt>{slice_yutaro} スライスに関する情報を取得してみましょう。

//cmd{
% ./httpc GET http://127.0.0.1:8888/networks/slice_yutaro
Status: 200 OK
Content:
{ "bindings" :
  [
    {
      "type" : 2,
      "id" : "yutaro_desktop",
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
  "description" : "Yutaro's Network"
}
//}

作ったスライスとスライスに属するホスト情報が JSON 形式で出力されます。なお、この出力結果は見やすいように改行しインデントしていますが、実際には改行やインデントなしで表示されることに注意してください。

=== REST API 一覧

REST API は今回紹介した以外にもいくつかの便利な API を提供しています (@<table>{API})。JSON ファイルの書式などこの API の詳しい仕様は @<tt>{https://github.com/trema/apps/wiki} で公開していますので、本格的に使いたい人はこちらも参照してください。

//table[API][REST API 一覧]{
動作						Method	URI
------------------------------------------------------------------------------------------
スライス作成				POST	/networks
スライス一覧				GET		/networks
スライス詳細				GET		/networks/<スライスの名前>
スライス削除				DELETE	/networks/<スライスの名前>
スライス変更				PUT		/networks/<スライスの名前>
ホスト追加 (ポート指定)		POST	/networks/<スライスの名前>/ports
ホスト一覧 (ポート指定)		GET		/networks/<スライスの名前>/ports
ホスト詳細 (ポート指定)		GET		/networks/<スライスの名前>/ports/<ポートの名前>
ホスト削除 (ポート指定)		DELETE	/networks/<スライスの名前>/ports/<ポートの名前>
ホスト追加 (MAC 指定)		POST	/networks/<スライスの名前>/attachments
ホスト一覧 (MAC 指定)		GET		/networks/<スライスの名前>/attachments
ホスト詳細 (MAC 指定)		GET		/networks/<スライスの名前>/attachments/<ホストの名前>
ホスト削除 (MAC 指定)		DELETE	/networks/<スライスの名前>/attachments/<ホストの名前>
//}

== OpenStack と連携する

スライス機能つきスイッチの OpenStack 用プラグインを使うと、OpenStack で仮想ネットワークまでを含めた IaaS を構築できます。このプラグインは OpenStack のネットワークコントロール機能である Quantum にスライス機能を追加します。

OpenStack Quantum の詳細やセットアップ方法は本書の範囲を超えるので省きますが、利用に必要なすべての情報がまとまった Web サイトを紹介しておきます。

 * OpenStack プラグインのページ: @<tt>{https://github.com/nec-openstack/quantum-openflow-plugin}
 * OpenStack のプラグイン解説ページ: @<tt>{http://wiki.openstack.org/Quantum-NEC-OpenFlow-Plugin}

== まとめ

Hello Trema から始めた Trema プログラミングも、いつの間にか本格的なクラウドを作れるまでになりました!

 * ネットワークを仮想的なスライスに分割して使える、スライス機能つきスイッチがどのように動作するかを見てきました。また実際に動作させ、同一のスライス内のみ通信が許可される仕組みを学びました。
 * スライス機能つきスイッチを利用するための REST API を使って、スライスを設定する方法を学びました。

次章では Trema を使った商用 IaaS の一つである Wakame-vdc のアーキテクチャを紹介します。本章で解説したスライス機能つきスイッチとはまったく異なる「分散 Trema」とも言えるスライスの実現方法は、商用クラウドの作り方として参考になります。
