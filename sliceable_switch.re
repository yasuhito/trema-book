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

それではさっそくスライス機能つきスイッチを動かしてみましょう。スライス機能つきスイッチの起動には、ルート権限が必要です。@<tt>{sudo} を使って、以下のように起動してください。

//cmd{
% cd ../../trema
% sudo ./trema run -c ../apps/sliceable_switch/sliceable_switch_external.conf
//}

このように起動しただけでは、スライス機能つきスイッチはなにも動作しません。スライスの設定が必要になります。

== REST API で設定する

まずは必要なモジュールのインストールを行います。

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

次に、スライス機能つきスイッチが参照するデータベースと、CGI 経由でデータベースに設定を書き込むためのスクリプト群を用意し、適切なディレクトリに配置します。

//cmd{
% ./create_tables.sh
A filter entry is added successfully.
% sudo mkdir -p /home/sliceable_switch/script
% sudo mkdir /home/sliceable_switch/db
% sudo cp Slice.pm Filter.pm config.cgi /home/sliceable_switch/script
% sudo cp *.db /home/sliceable_switch/db
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

まず、スライスを作ってみましょう。@<tt>{/networks} という URI に POST メソッドでアクセスし、json 形式で記載されたパラメータを送ります。@<tt>{slice1} という ID のスライスを作る場合には、以下のようにします。

//cmd{
% telnet localhost 8888
POST /networks HTTP/1.1
Content-type: application/json
{
  "id" : "slice1",
  "description" : "Trema-team's network"
}
//}

次にスライスにポートを割り当ててみましょう。URI は @<tt>{/networks/<スライスID>/ports} になります。以下の例では、@<tt>{slice1} というスライスに、データパス ID が @<tt>{0xe0} である OpenFlow スイッチの 33 番目のポートを割り当てています。このポートからパケットを出す際に VLAN tag を付与したい場合には @<tt>{vid} のパラメータにその値を設定します。VLAN tag の設定が不要の場合には、以下の例のように 65535 としてください。

//cmd{
% telnet localhost 8888
POST /network/slice1/ports HTTP/1.1
Content-type: application/json
{
  "id" : "port0",
  "datapath_id" : "0xe0",
  "port" : 33,
  "vid" : 65535
}
//}

MAC アドレスをスライスに対応させるためには、以下のようにします。URI は、@<tt>{/networks/<スライスID>/attachments} です。

//cmd{
% telnet localhost 8888
POST /network/slice1/attachments HTTP/1.1
Content-type: application/json
{
  "id" : "attach0",
  "mac" : "01:00:00:00:00:01"
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
