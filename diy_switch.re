= 二千円で OpenFlow スイッチを DIY

//lead{
DIY の本当の意義は、お金の節約ではなく DIY する対象をより深く知れることにあります。筆者は 20 年物のぬか床を毎日混ぜていますが、そのおかげで漬物の善し悪しがすぐ分かるようになりました。家庭菜園をやる人が野菜の旬に詳しく、日曜大工をやる人が建材に詳しいのも同様です。OpenFlow でも、スイッチを自分で作ってみればより理解が深まります。
//}

あなたも仮想スイッチではそろそろ物足りなくなってきたころではないでしょうか？本格的に OpenFlow プログラミングに取り組むなら、やはり実機の OpenFlow スイッチを自宅にデンと置き一人占めしたいものです。「でも、中古ハードウェアショップにもまだ出てないんだよなあ」。自宅に機材ラックを置き、高価な Catalyst や Summit などの中古品をマウントして愉しむ機材マニア達にかかっても、まだ実機は入手できていないようです。OpenFlow スイッチはまだまだ新しい分野の製品なので、中古市場には出回っていないからです。

//table[openflow_switch][主な OpenFlow スイッチの価格比較 (2012 年現在)]{
ベンダ		価格
--------------------------------------------------------
I 社		430 万円〜
N 社		250 万円〜
H 社 		34 万円〜
P 社		32 万円〜
//}

@<table>{openflow_switch} は 2012 年現在の OpenFlow スイッチの価格比較です。これを見ると、安くなってきたとは言え最安でも 32 万円。とても個人では手は出ません。やはり、一般市場に降りてくるまであと数年は指をくわえて待つしかないのでしょうか。

そんな中、事件は 2012 年の春に起きました。"自宅ラック派" エンジニアの一人、@SRCHACK 氏@<fn>{srchack}が二千円で買える家庭用無線 LAN ルータをハックし、OpenFlow スイッチに改造するツール一式を公開したのです。これは劇的な出来事で、OpenFlow を試したいけどもスイッチが高くて躊躇している潜在的な OpenFlow プログラマを電器店に走らせるきっかけとなりました。さらに同氏とその仲間達は秋葉原にて「無線 LAN ルータを OpenFlow 化して、Trema と接続してみよう」勉強会を開催し、集まった一人ひとりに OpenFlow スイッチ自作法を伝授したのです。これらの情報は Twitter でも公開され、日本各地からの OpenFlow スイッチ自作レポートが続々と集まりはじめました。

//footnote[srchack][@<tt>{http://www.srchack.org/}]

2012 年の夏にはこの動きがさらに広がりを見せます。幕張で開催されるネットワーク業界の最大の展示会、Interop Tokyo 2012 の併催イベント、オープンルータ・コンペティション@<fn>{orc}に、@SRCHACK 氏による自作 OpenFlow スイッチプロジェクトが参戦したのです。このオープンルータ・コンペティションとは、次世代のネットワーク技術者の発掘を目的としたコンテストで「ソフトウェア、ハードウェアに関わらずまったく新しいルータを改良、または創造する」という課題で競われます。最終審査会では予選を突破した 10 チームがそれぞれのルータを持ち寄りましたが、@SRCHACK 氏の自作 OpenFlow スイッチによるメッシュネットワーク (@<img>{orc_srchack}) が見事準グランプリを勝ち取りました。Interop のような業界が注目する場で受賞したことによって、OpenFlow スイッチの自作がますます注目を集めることになったのです。

//footnote[orc][@<tt>{http://www.interop.jp/2012/orc/}]

//image[orc_srchack][オープンルータ・コンペティション会場での @SRCHACK 氏による自作 OpenFlow スイッチを使ったメッシュネットワーク][scale=0.12]

本章ではこの自作 OpenFlow スイッチの概要と、Trema との接続方法を紹介します。あなたも自分専用 OpenFlow スイッチを手に入れ、OpenFlow への理解を深めてみましょう。

== OpenFlow 化のしくみ

この激安ルータはどのようにして OpenFlow 化されているのでしょう。その鍵は無線 LAN ルータのファームウェアにあります。最近の無線 LAN ルータは Linux で動作しており、メーカーの保証は受けられませんが OpenWRT などのオープンなファームウェアを使えば好きなソフトウェアを無線 LAN ルータの中で動かすことができます。@SRCHACK 氏の開発した OpenFlow 化ツールは、この OpenWRT をベースに Linux 上で動作する OpenFlow ソフトウェアスイッチを組み込んだものです。

このファームウェアの対応機種は @<table>{wifirouter} のとおりです。本章では、このうち WHR-G301N を使って OpenFlow スイッチを作ってみます。

//table[wifirouter][OpenFlow スイッチに改造可能な無線 LAN ルータ (2012 年現在)]{
機種名		メーカー	価格
--------------------------------------------------------
WHR-G301N	Buffalo		約二千円
WZR-HP-AG300H	Buffalo		約六千円
//}

===[column] Interop Tokyo 2012 での Trema の活躍

2012 年の Interop Tokyo では、先ほどのオープンルータ・コンペティション以外でも Trema が大活躍しました。

2012 年は OpenFlow 元年ということもありベンダ各社が OpenFlow 対応スイッチを展示していましたが、さまざまなブースで OpenFlow スイッチと接続した Trema を見ることができました。

また、Interop の華である Shownet (Interop 開催中のネットワークインフラ。各社の最新のネットワーク機器を接続しショーケースとしても展示される) の OpenFlow ネットワークは Trema で構築されました。この情報通信研究機構 (NICT) による Trema ベースの RISE Controller は Best of Show Award (ShowNet プロダクト部門) のグランプリを受賞しました (@<img>{trema_interop})。

//image[trema_interop][Interop で Best of Show Award を受賞した Trema ベースの RISE Controller。各ベンダの OpenFlow スイッチとの相互接続で大活躍した。(NICT 石井秀治さん提供)][scale=0.6]

===[/column]

== OpenFlow 化の手順

それでは、無線 LAN ルータの改造に取り掛かりましょう。無線 LAN ルータを OpenFlow 化は次の 3 ステップで実行できます。

 1. ファームウェアの入手と準備
 2. ファームウェアの入れ替え
 3. 動作確認

なおこの改造はメーカーの保証外ですので、自己責任で行ってください。

=== ファームウェアの入手と準備

ファームウェアを公開している @SRCHACK 氏のサイト (@<tt>{http://www.srchack.org/orc12/}) から、手持ちの無線 LAN ルータに対応するファームウェアとをダウンロードしてください。異なるいくつかのバージョンの OpenFlow プロトコルに対応したファームウェアが用意されていますが、動作確認のためには Trema が対応している OpenFlow 1.0 用ファームウェアが良いでしょう。

次にファームウェアをダウンロードしたホストを無線 LAN ルータに接続します。ホストはかならず無線 LAN ルータの LAN 側ポートに接続してください (@<img>{firmware_update_setup})。4 つある LAN 側ポートのどれかに接続すれば、無線 LAN ルータの DHCP サーバ機能によりホストに自動的に IP アドレスが割り当てられます。これで準備はおしまいです。

//image[firmware_update_setup][無線 LAN ルータとファームウェアを置いたホストとの接続][scale=0.3]

=== ファームウェアの入れ替え

次に、ファームウェアを OpenFlow 対応のものに書き換えます。ファームウェアのアップデート手順は通常のアップデート手順とまったく同じです。ルータの管理画面を開き、さきほどダウンロードしたファームウェアを使ってアップデートしてください。"ファームウェアのアップデート中は決して電源を切らない" という約束を守れば、簡単にアップデートできるはずです。

入れ替えに成功すると、LAN 側と WAN 側ポートの役割がそれぞれ次のように変更されます。

 * LAN 側ポート: OpenFlow スイッチとして動作するポート。
 * WAN 側ポート: コントローラが接続する管理用ポート。コントローラとの接続以外にも @<tt>{telnet} ログインに用いられる。

ここで、ファームウェアのインストール用に使ったホストを LAN 側から抜き、WAN 側につなぎなおします。ファームウェアのアップデート直後には、WAN 側ポートに 192.168.1.1/24 というネットワークが設定されています。ファームウェアに設定されているコントローラの IP アドレスはデフォルトで 192.168.1.10 であるため、このアドレスを使用しましょう。この WAN 側ポートでは DHCP サーバは動作していないので、固定で IP アドレスを設定してください。

=== 動作確認

動作確認はおなじみ @<tt>{telnet} で行うことができます。次のように @<tt>{root} ユーザで OpenFlow スイッチに接続してみてください。OpenWRT のアスキーアートロゴが次のように表示されれば OpenFlow 化成功です。

//cmd{
% telnet -l root 192.168.1.1 
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

== OpenFlow スイッチの内部構成

こうしてできた OpenFlow スイッチの内部構成は次のようになっています。スイッチ内で動作する Linux からは eth0、eth1 の 2 つのネットワークインタフェースが見えており、それぞれ内部スイッチと WAN 側ポートに接続されています。内部スイッチは VLAN に対応したレイヤ 2 スイッチで、タグを付けることによって 4 つの LAN 側ポートからのパケットを区別します。このように eth0 を 4 つの論理ポートに分けることで、OpenFlow から LAN 側の 4 ポートを区別して使うことができます。

//image[switch_internal_vlan][OpenFlow スイッチの内部構成][scale=0.3]

=== VLAN の設定

VLAN の設定ファイルは、@<tt>{/etc/config/network} に記載されています (@<list>{config_network})。この設定ファイル中の 4 つの @<tt>{config 'switch_vlan'} セクションにより、LAN 側ポートごとに VLAN が切られていて eth0 とつながっているということがわかると思います。

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

=== OpenFlow 関連の設定

OpenFlow 関連の設定は、@<tt>{/etc/config/openflow} ファイルに記載されています (@<list>{config_openflow})。@<tt>{ofports} オプションの項目より、OpenFlow スイッチのポートとして使用する LAN 側の 4 ポートはそれぞれ @<tt>{eth0.1, eth0.2, eth0.3, eth0.4} に対応していることがなんとなくわかります。

//list[config_openflow][/etc/config/openflow ファイル]{
config 'ofswitch'
      option 'dp' 'dp0'
      option 'ofports' 'eth0.1 eth0.2 eth0.3 eth0.4'
      option 'ofctl' 'tcp:192.168.1.10:6633'
      option 'mode'  'outofband'
//}

== Trema とつないでみよう

今回作った OpenFlow スイッチをラーニングスイッチ (@<chap>{learning_switch}) を使って、動かしてみましょう。コントローラを動かすホストに加え、OpenFlow スイッチを使って通信を行うホストを二台用意して、@<img>{network} のように接続します。

//image[network][Trema との接続][scale=0.5]

=== Trema の起動

まずコントローラとして WAN 側につないだホスト上でラーニングスイッチを起動します。

//cmd{
% cd trema
% ./trema run ./src/examples/learning_switch/learning-switch.rb -d
//}

次に、スイッチとコントローラ間に TCP コネクションが張られているかを確認しましょう。TCP コネクションを確認するためには、@<tt>{netstat} コマンドを使います。OpenFlow プロトコルではポート 6633 が使用されますので、@<tt>{grep} を使い OpenFlow のコネクションだけを表示します。

//cmd{
% netstat -an -A inet | grep 6633
tcp        0      0 0.0.0.0:6633            0.0.0.0:*               LISTEN     
tcp        0      0 192.168.11.10:6633      192.168.11.1:60246      ESTABLISHED
//}

このように表示されていれば成功です。1 行目の表示は、Switch Manager (@<chap>{trema_architecture} で紹介) が、ポート 6633 でスイッチからの接続要求を待ち受けていることを表しています。2 行目の表示で、192.168.11.1 の OpenFlow スイッチとの間で TCP コネクションが張られていることがわかります

もし 2 行目の表示がされない場合、スイッチ側の OpenFlow サービスが、なんらかの理由で有効になっていない可能性があります。この場合、スイッチの OpenFlow サービスを再起動する必要があります。OpenFlow スイッチに @<tt>{telnet} でログインし、以下のようにして、サービスの再起動を行います。

//cmd{
% /etc/init.d/openflow restart
//}

=== スイッチの情報を取得する



OpenFlow プロトコルには、スイッチから情報を取得するためのメッセージがいくつか定義されています。これらのメッセージを使って、スイッチ情報を取得・表示するための @<tt>{show_description} コマンドが Trema Apps に用意されています。まず、コマンドを使えるように用意します。

//cmd{
% git clone https://github.com/trema/apps.git
% (cd ./apps/show_description/; make)
//}

作成したコマンドを使って、スイッチの情報を取得してみましょう。

//cmd{
% TREMA_HOME=trema/ ./apps/show_description/show_desctiption
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

@<tt>{Manufacturer description} から @<tt>{Human readable description of datapath} までは、Stats Request メッセージでタイプに @<tt>{OFPST_DESC} を指定すると取得できる情報です。また、@<tt>{Datapath ID} 以降の情報は、Features Request メッセージで取得できる情報です。

今回の OpenFlow スイッチの実装として、Stanford 大学で作成されたリファレンススイッチが使用されていることが分かります。また、OpenFlow スイッチとして動作するポート @<tt>{eth0.1} から @<tt>{eth0.4} までと@<tt>{tap0} @<fn>{tap0} が定義されています。

//footnote[tap0][@<tt>{tap0} は、内部的に使われるポートであり、ユーザが直接使うことはありません。]

=== フローを表示する

それでは、OpenFlow スイッチに接続する二つのホスト間で通信ができるかを確認してみましょう。@<img>{network} のホスト 1 からホスト 2 に向けて@<tt>{ping} をうってみてください。

//cmd{
% ping 192.168.2.2
PING 192.168.2.2 (192.168.2.2) 56(84) bytes of data.
64 bytes from 192.168.2.2: icmp_req=1 ttl=64 time=18.0 ms
64 bytes from 192.168.2.2: icmp_req=2 ttl=64 time=0.182 ms
...
//}

実際にフローが設定されているか、確認してみましょう。OpenFlow スイッチに telnet でログインして、以下のコマンドを実行します。

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

ホスト 1 (@<tt>{nw_src=192.168.2.1}) からホスト 2 (@<tt>{nw_dst=192.168.2.2}) 宛の ICMP エコー要求 (@<tt>{nw_proto=1, icmp_type=8}) 用のフローと、送信元と宛先が入れ替わった ICMP エコー応答 (@<tt>{nw_proto=1, icmp_type=0}) 用のフローが設定されていることが確認できるはずです。タイミングによっては、これ以外に ARP 用のフローが表示されるかもしれません。

OpenFlow プロトコルには、スイッチ側のフローエントリをコントローラ側から取得するためのメッセージが存在します。このメッセージを使って取得したフローエントリを表示する @<tt>{flow_dumper} コマンドが、Trema Apps に用意されています。

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

OpenFlow スイッチ側で確認したエントリが取得できていることが確認できたでしょうか？もしかしたら、@<tt>{ping} 以外にも OS が独自にだしているパケットによりフローが出来ているかもしれません。その場合はもう一度 OpenFlow スイッチ側のエントリも確認してみてください。

== まとめ

この章では個人でも手に入る安価な無線 LAN ルータを OpenFlow スイッチに改造しました。また、実際に Trema とつなげて動作確認を行いました。

 * 二千円の無線 LAN ルータを OpenFlow スイッチに改造し、ラーニングスイッチを使って動作確認しました。
 * @<tt>{show_description} コマンドを用意して、スイッチの情報を取得する方法について学びました。
 * @<tt>{flow_dumper} コマンドを使い、実際に設定されているフローの確認を行いました。

さあ、これであなたも OpenFlow スイッチのオーナーです。お金に余裕があれば @SRCHACK 氏のように複数台買ってネットワークを拡張することも可能です。今回は動作確認にラーニングスイッチを使いましたが、スイッチが増えたときには@<chap>{routing_switch}で紹介したルーティングスイッチを使うのがおすすめです。

== 参考文献

: OpenWRT (@<tt>{https://openwrt.org/})
  無線 LAN ルータで Linux を動かせれば、もっといろいろなことが出来るんじゃないか？では、やってみよう！そんな風に思った人が、始めたプロジェクトです。いまでは、市販品には搭載されていないさまざまな機能が提供されています。

: @SRCHACK 氏のブログ (@<tt>{http://www.srchack.org/})
  今回紹介した激安 OpenFlow スイッチを作るための無線 LAN 対応ファームウェアの作者 @SRCHACK 氏のブログです。改造ファームウェアを完成させるまでの試行錯誤の様子などが紹介されています。
