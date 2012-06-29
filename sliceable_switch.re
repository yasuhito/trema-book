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

スライス機能つきスイッチも、@<chap>{rouinting_switch} で取り上げた
ルーティングスイッチと同様に @<tt>{https://github.com/trema/apps/} にて
ソースコードが公開されています。ソースコードをまだ取得していない場合は、
前章を参考に @<tt>{git} で取得しておいてください。
ここでは、以下のようなディレクトリ構成になっていることが前提で、
説明します。

//cmd{
$ ls -F
apps/   trema/
//}

スライス機能つきスイッチは、ルーティングスイッチから
派生したアプリケーションです。そのため、ルーティングスイッチと
同様に @<tt>{topology} と連動して動作します。
また、フロー設定を行う @<tt>{flow_manager} とも連動して
動作するようになっていますので、これらのモジュールも合わせて
$<tt>{make} してください。

//cmd{
$ (cd apps/topology/; make)
$ (cd apps/flow_manager/; make)
$ (cd apps/sliceable_switch; make)
//}

=== REST API の設定

//cmd{
$ sudo apt-get install sqlite3 libdbi-perl libdbd-sqlite3-perl
$ sudo apt-get install apache2-mpm-prefork libjson-perl
//}
 
//cmd{
$ cd apps/sliceable_switch
$ ./create_tables.sh
A filter entry is added successfully.
$ sudo cp apache/sliceable_switch /etc/apache2/sites-available
$ sudo a2enmod rewrite actions
$ sudo a2ensite sliceable_switch
$ sudo mkdir -p /home/sliceable_switch/script
$ sudo mkdir /home/sliceable_switch/db
$ sudo cp Slice.pm Filter.pm config.cgi /home/sliceable_switch/script
$ sudo cp *.db /home/sliceable_switch/db
//}

//cmd{
$ ls /home/sliceable_switch/*
/home/sliceable_switch/db:
filter.db  slice.db

/home/sliceable_switch/script:
Filter.pm  Slice.pm  config.cgi
//}

=== 試してみる

== VLAN (従来技術) との違い

=== 動作を比べてみる
=== 長所・短所

== まとめ/参考文献
