= 2,000 円で OpenFlow スイッチを DIY

//lead{
DIY の本当の意義は、お金を節約できることではなく DIY する対象をより深く知れることにあります。OpenFlow でも、スイッチを自作しとことん使い倒すことでより理解が深まります。
//}

== 一家に一台、OpenFlowスイッチ

あなたも仮想スイッチではそろそろ物足りなくなってきたころではないでしょうか？本格的に OpenFlow プログラミングに取り組むなら、やはり実機の OpenFlow スイッチを自宅に "デン" と置いて心ゆくまで一人占めしたいものです。

筆者も Trema の動作検証用に OpenFlow スイッチの実機がほしくなり、どうにかして安く手に入れられないかと詳しい友人に聞いたことがあります。彼は「自宅ラック友の会」という変わった名前の集まりによく顔を出していて、なんと自宅にサーバラックを置き、個人ではとうてい使わない Catalyst や Summit などの中古品で自宅のネットワークを構築してしまうというその道のマニアです。そんな彼曰く、

//quote{
「よく行く秋葉原の中古ショップにもまだ出てないんだよなあ。OpenFlow スイッチはまだまだ新しい分野の製品だし、中古市場にも出回ってないみたい」
//}

@<table>{openflow_switch} は 2012 年現在の OpenFlow スイッチの価格比較です。これを見ると、安くなってきたとはいえ、最安でも 32 万円。とても個人では手は出ません。やはり、一般市場に降りてくるまであと数年は指をくわえて待つしかないのでしょうか。

//table[openflow_switch][主な OpenFlow スイッチの価格比較 (2012 年現在)]{
ベンダ		価格
--------------------------------------------------------
I 社		430 万円〜
N 社		250 万円〜
H 社 		34 万円〜
P 社		32 万円〜
//}

そんな中、事件は 2012 年の春に起きました。"自宅ラック派" エンジニアの一人、@SRCHACK.ORG 氏@<fn>{srchack}が 2,000 円で買える家庭用無線 LAN ルータ (@<img>{akihabara}) をハックし、OpenFlow スイッチに改造するツール一式を公開したのです。これは劇的な出来事で、OpenFlow を試したいけどもスイッチが高くてどうしようか迷っている人々を電器店に走らせるきっかけとなりました。さらに同氏とその仲間達は秋葉原にて「無線 LAN ルータを OpenFlow 化して、Trema と接続してみよう」勉強会を開催し、集まった 1 人ひとりに OpenFlow スイッチ自作法を伝授したのです。これらの情報は Twitter でも公開され、日本各地からの OpenFlow スイッチ自作レポートが続々と集まりはじめました。

//footnote[srchack][@<href>{http://www.srchack.org/}]

//image[akihabara][OpenFlow 改造が可能な家庭用無線 LAN ルータ。もちろん秋葉原などでこのように普通に売っている][height=8cm]

2012 年の夏にはこの動きがさらに広がりを見せます。幕張メッセ (千葉県千葉市) で開催されるネットワーク業界の最大の展示会、「Interop Tokyo 2012」の併催イベント、「オープンルータ・コンペティション@<fn>{orc}」に、@SRCHACK.ORG 氏による自作 OpenFlow スイッチプロジェクトが参戦したのです。

このオープンルータ・コンペティションとは、次世代のネットワーク技術者の発掘を目的としたコンテストで「ソフトウェア、ハードウェアに関わらずまったく新しいルータを改良、または創造する」という課題で競われます。最終審査会では予選を突破した 10 チームがそれぞれのルータを持ち寄りましたが、@SRCHACK.ORG 氏の自作 OpenFlow スイッチによるメッシュネットワーク (@<img>{orc_srchack}) が見事準グランプリを勝ち取りました。Interop のような業界が注目する場で受賞したことによって、OpenFlow スイッチの自作がますます注目を集めることになったのです。

//footnote[orc][@<href>{http://www.interop.jp/2012/orc/}]

//image[orc_srchack][オープンルータ・コンペティション会場での @SRCHACK.ORG 氏による自作 OpenFlow スイッチを使ったメッシュネットワーク][scale=0.12]

本章ではこの自作 OpenFlow スイッチの概要と、Trema との接続方法を紹介します。あなたも自分専用の OpenFlow スイッチを手に入れ、とことん使い倒すことで OpenFlow への理解を深めてみましょう。

== OpenFlow 化のしくみ

この家庭用無線 LAN ルータはどのようにして OpenFlow 化されているのでしょう？その鍵は無線 LAN ルータのファームウェアにあります。最近の無線 LAN ルータは Linux ベースのファームウェアで動作しており、比較的オープンなハードウェア構成を持っています。そこで、そのファームウェアを Linux ベースのオープンなファームウェアと載せ替えることにより、メーカーの保証は受けられませんがさまざまなソフトウェアを動かすことができるようになります。@SRCHACK.ORG 氏の開発した OpenFlow 化ツールは、Linux ベースのファームウェアに OpenFlow ソフトウェアスイッチなどを組み込んだものです。

この OpenFlow 化ファームウェアの現在の対応機種は @<table>{wifirouter} のとおりです。本章では、このうち WHR-G301N を使って OpenFlow スイッチを作る方法を紹介します。

#@warn(表では味気無いので、写真がぜひとも欲しいです)

//table[wifirouter][OpenFlow スイッチに改造が可能な無線 LAN ルータ (2012 年現在)]{
機種名		メーカー	価格
--------------------------------------------------------
WHR-G301N	Buffalo		約 2,000 円
WZR-HP-AG300H	Buffalo		約 6,000 円
//}

===[column] 取間先生曰く: Interop Tokyo 2012 での Trema の活躍

2012 年の Interop Tokyo では、先ほど紹介したオープンルータ・コンペティション以外でも Trema が大活躍しました。

2012 年は OpenFlow 元年ということもありベンダ各社が OpenFlow 対応スイッチを展示していましたが、多くのブースで OpenFlow スイッチと接続した Trema を見ることができました。すでに日本国内では、Trema は OpenFlow コントローラを作る上でのデファクトスタンダードとなっているようです。

また、Interop の華である ShowNet の OpenFlow ネットワークでも Trema が活躍しました。ShowNet とは Interop 開催中に構築されるネットワークインフラで、各社の最新のネットワーク機器を接続したショーケースとしての顔も持っています。ShowNet の OpenFlow ネットワークでは多種多様なベンダの OpenFlow スイッチを相互接続する必要があったのですが、「正しい」OpenFlow プロトコルをしゃべることで定評のある Trema がコントローラ開発プラットフォームに選ばれました。こうして開発された情報通信研究機構 (NICT) による RISE Controller は、グランプリである "Best of Show Award" を受賞しました (@<img>{trema_interop})。

//image[trema_interop][Interop で Best of Show Award を受賞した Trema ベースの RISE Controller。各ベンダの OpenFlow スイッチとの相互接続で大活躍した (提供 : NICT 石井秀治氏)][scale=0.6]

===[/column]

== OpenFlow 化の手順

それでは、さっそく無線 LAN ルータの OpenFlow 化に取り掛かりましょう。手順は次の 3 ステップです。

 1. ファームウェアの入手と準備
 2. ファームウェアの入れ替え
 3. 動作確認

なおこの改造はもちろんメーカーの保証外ですので、くれぐれも自己責任で行ってください。

=== ファームウェアの入手と準備

ファームウェアを公開している @SRCHACK.ORG 氏のサイト (@<href>{http://openflow.inthebox.info/}) から、手持ちの無線 LAN ルータに対応するファームウェアをダウンロードしてください。いくつかのバージョンの OpenFlow プロトコルに対応したファームウェアが用意されていますが、動作確認のためには Trema が対応しておりコントローラ実装も豊富な OpenFlow 1.0 用ファームウェアが良いでしょう。

次にファームウェアをルータに書き込むために、ファームウェアをダウンロードしたホストを無線 LAN ルータに接続します。ホストは無線 LAN ルータの LAN 側ポートに接続してください (@<img>{firmware_update_setup})。4 つある LAN 側ポートのどれかに接続すれば、無線 LAN ルータの DHCP サーバ機能によりホストに自動的に IP アドレスが割り当てられます。これで準備はおしまいです。

//image[firmware_update_setup][ファームウェアをダウンロードしたホストと無線 LAN ルータを接続する][scale=0.3]

=== ファームウェアの入れ替え

次に、無線 LAN ルータのファームウェアを先ほどダウンロードした OpenFlow 対応のものに書き換えます。ファームウェアのアップデート手順は通常のアップデート手順とまったく同じです。ルータに接続したホストからルータの管理画面をブラウザで開き、さきほどダウンロードしたファームウェアを使ってアップデートしてください。"ファームウェアのアップデート中は決して電源を切らない" という約束を守れば、誰でも簡単にアップデートできるはずです。

入れ替えに成功すると、LAN 側と WAN 側ポートの役割がそれぞれ次のように変更されます。

 * LAN 側ポート: OpenFlow スイッチとして動作する
 * WAN 側ポート: コントローラとの接続用。それ以外にも @<tt>{telnet} ログインに使う

ファームウェアのインストール用に使ったホストは、あとでコントローラを動作させるために使います。そのため、このホストを、LAN 側から抜き、WAN 側につなぎなおします。ファームウェアのアップデート直後には、WAN 側ポートに 192.168.1.1/24 というネットワークが設定されています@<fn>{config}。ファームウェアに設定されているコントローラの IP アドレスはデフォルトで 192.168.1.10 であるため、このアドレスを使用しましょう。この WAN 側ポートでは DHCP サーバは動作していないので、この IP アドレスを固定で設定してください。

//footnote[config][無線 LAN ルータの WAN 側ポートの IP アドレスは、@<tt>{/etc/config/network} に記載されています。また OpenFlow スイッチが接続するコントローラの IP アドレスは、@<tt>{/etc/config/openflow} 内で指定されています。設定を変更したい場合は、これらのファイルを書き換えて、無線 LAN ルータを再起動してください。]

=== 動作確認

うまく OpenFlow 化されたかどうかの確認はおなじみ @<tt>{telnet} コマンドで行えます。次のように @<tt>{root} ユーザで無線 LAN ルータに接続してみてください (パスワードは初期状態では設定されていません)。OpenWRT のアスキーアートロゴが次のように表示されれば OpenFlow 化成功です!

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

=== ファームウェアのアップデートに失敗したら

めったにあることではありませんが、ファームウェアのアップデートに失敗するといわゆる "文鎮" 状態になってしまいます。幸い、配布元の @SRCHACK.ORG 氏のサイトに純正ファームウェアに戻す方法が記載されていますので、もし文鎮化した場合にはこちらを参考にしてみてください。

 * @<href>{http://openflow.inthebox.info/Recovery_WHR-G301N}

== Trema とつないでみよう

さていよいよ出来上がった自分専用 OpenFlow スイッチを Trema とつなげてみましょう。以降では、今まで作ってきたコントローラや Trema Apps のアプリケーションを使い OpenFlow スイッチとしての動作をいろいろな角度から検証してみます。

ハードウェアのセットアップとして、コントローラを動かすホスト 1 台とパケット送受信テスト用のホストを 2 台用意し、自作 OpenFlow スイッチに @<img>{diy_switch_network} のように接続してください。DHCP サーバは無効になっていますので、それぞれの IP アドレスを

 * コントローラ用ホスト: 192.168.1.10/24
 * host1: 192.168.2.1/24
 * host2: 192.168.2.2/24

と固定で振ってください。なお以下のコマンド例では、コントローラを動かすホストのプロンプトを @<tt>{controller%}、host1 のプロンプトを @<tt>{host1%} と表示します。

//image[diy_switch_network][Trema との接続][scale=0.3]

また動作検証用ツールをここで準備しておきましょう。Trema Apps はスイッチの動作確認に有益な次の 2 つのツールを提供しています:

 * @<tt>{show_switch_features}: スイッチの詳細情報を表示する
 * @<tt>{flow_dumper}: スイッチのフロー情報を表示する

#@warn(show-switch-features.rb、flow-dumper.rb を Trema Apps に追加し、「Trema のアーキテクチャ」章でも紹介)
#@warn(show_switch_features と flow_dumper はどちらも Ruby でやりましょう)

Trema Apps をまだ取得していない場合、以下のようにコントローラ用ホスト上に @<tt>{git} を使って取得してください。

//cmd{
controller% git clone https://github.com/trema/apps.git
//}

これで動作検証環境のセットアップは完了です。

=== スイッチの情報を取得する

まずは @<tt>{show_switch_features} を使ってスイッチの詳細情報を表示してみましょう。コントローラ用ホスト上でターミナルを開き、@<tt>{trema run} で @<tt>{show-switch-features.rb} を実行すると、スイッチの製造者情報やハードウェア情報が次のように表示されます。

//cmd{
controller% trema run ./apps/show_switch_features/show-switch-features.rb
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

出力の最初の 3 行により、ルータ内で起動している OpenFlow スイッチの実装は、スタンフォード大学で作成されたソフトウェアレファレンススイッチのバージョン 1.0.0 であることが分かります。また、@<tt>{eth0.1} から @<tt>{eth0.4} までの OpenFlow 用ポート 4 つと、ローカルポート @<tt>{tap0} が定義されていることがわかります。なお@<chap>{openflow_spec}でも触れましたがこのローカルポート @<tt>{tap0} は特殊な用途向けですのでユーザが普段使うことはありません。

=== ラーニングスイッチの起動

スイッチの構成がわかったところで、いよいよラーニングスイッチを起動して OpenFlow スイッチとして動かしてみます。次のようにラーニングスイッチを起動します。

//cmd{
controller% trema run ./learning-switch.rb -d
//}

ここで起動時のオプションとして @<tt>{-d} を使っていることに注意してください。このオプションを指定すると、Trema はコントローラをデーモンとしてバックグラウンドで起動します。

次に、OpenFlow スイッチに接続した 2 つのホスト間で通信ができるかを確認してみましょう。@<img>{diy_switch_network} の host1 から host2 に向けて次のように @<tt>{ping} をうってみてください。次のように応答があれば成功です。

//cmd{
host1% ping 192.168.2.2
PING 192.168.2.2 (192.168.2.2) 56(84) bytes of data.
64 bytes from 192.168.2.2: icmp_req=1 ttl=64 time=18.0 ms
64 bytes from 192.168.2.2: icmp_req=2 ttl=64 time=0.182 ms
...
//}

=== フローを表示する

先ほどの @<tt>{ping} によってスイッチに正しくフローが設定されているか確認してみましょう。

Trema Apps の @<tt>{flow_dumper} はスイッチのフローテーブルに書き込まれているフローを表示するコマンドです。次のように実行してみてください。

//cmd{
controller% trema run ./apps/flow_dumper/flow-dumper.rb
[0x00002320698790] priority = 65535, match = [wildcards = 0,	   \
  ...								   \
  nw_proto = 1, nw_src = 192.168.2.1/32, nw_dst = 192.168.2.2/32,  \
  tp_src = 8, tp_dst = 0], actions = [output: port=4 max_len=65535]
[0x00002320698790] priority = 65535, match = [wildcards = 0,	   \
  ... 		   	      	     	     		     	   \
  nw_proto = 1, nw_src = 192.168.2.2/32, nw_dst = 192.168.2.1/32,  \ 
  tp_src = 0, tp_dst = 0], actions = [output: port=1 max_len=65535]
//}

ICMP パケットのプロトコル番号は 1 なので、@<tt>{nw_proto=1} のフローが ping に対応します。host1 (@<tt>{192.168.2.1}) から host2 (@<tt>{192.168.2.2}) 宛の ICMP のフローと、その逆向きのフローがそれぞれ見つかるはずです。正しく表示されたでしょうか？

== 自作 OpenFlow スイッチの制限

こうしてできた自作 OpenFlow スイッチの内部構成は@<img>{switch_internal_vlan}のようになっています。

//image[switch_internal_vlan][自作 OpenFlow スイッチの内部構成][scale=0.3]

スイッチ内で動作するファームウェア (Linux) からは eth0、eth1 の 2 つのネットワークインターフェースが見えており、それぞれ内部スイッチと WAN 側ポートに接続しています。ファームウェア上ではスタンフォード大のソフトウェアレファレンススイッチが動作しており、これが OpenFlow プロトコルをしゃべります。ルータには VLAN に対応した内部スイッチが入っており、これが VLAN タグを付けることによって 4 つの LAN 側ポートからのパケットを区別して eth0 に転送します。このように eth0 を内部スイッチが 4 つのポートに分けることで、仮想的に 4 ポートを使うことができるようになっているわけです。

こうした構成になっていることから、自作 OpenFlow スイッチの利用に関していくつかの点で注意があります。

: 性能が低い
  OpenFlow 対応はすべてファームウェアである Linux 上でのソフトウェア実装のため高性能は期待できません。あくまでお手軽に OpenFlow を楽しめるスイッチと割り切る必要があります。

: OpenFlow 1.0 への準拠度が不明
  すべての動作パターンを検証している訳ではないため OpenFlow 1.0 にどの程度準拠できているかは不明です。スタンフォード大学のレファレンススイッチ実装を利用しているため OpenFlow 1.0 準拠としていますが、思わぬところで実際の OpenFlow スイッチとの動作の違いが出る可能性があります。

: ポートごとのステータスを取ることができない
  自作 OpenFlow スイッチでは 1 つのインターフェースが仮想的に 4 ポートに分かれていますが、実質的にはこれらは 1 つのポートです。このため、@<chap>{routing_switch}で紹介した Trema Apps の Topology など、個々のポートの UP や DOWN 情報を使うアプリケーションは一部正しく動作しません。また内部的にタグ付き VLAN を使っているため、タグ付き VLAN のパケットが入ってきたときの挙動も未検証です。

自作 OpenFlow 用ファームウェアはオープンに開発が行われています (@<href>{http://openflow.inthebox.info/})。もし新しい不具合や改善方法をみつけた場合は、ぜひフィードバックを送ってください。

== まとめ

本章では個人でも手に入る安価な無線 LAN ルータを OpenFlow スイッチに改造しました。また、実際に Trema とつなげて動作確認を行いました。

 * @SRCHACK.ORG 氏公開の OpenWRT ベースのファームウェアを使うと、一部の無線 LAN ルータを OpenFlow スイッチに改造できる
 * @<tt>{show-switch-features.rb} コマンドを使うと、スイッチの詳細情報を表示できる
 * @<tt>{flow-dumper.rb} コマンドを使うと、実際に設定されているフローの中身を確認できる
 * 自作 OpenFlow スイッチにはいくつかの機能的な制限や未検証な動作がある。開発サイト @<href>{http://openflow.inthebox.info/} にフィードバックをして改善していこう
 
さあ、これであなたも OpenFlow スイッチのオーナーです。今まで紹介してきた Trema のコントローラを動かしていろいろ実験してみましょう。お金に余裕があれば複数台買ってより大きなネットワークを作ることも可能です。

== 参考文献

: 「OpenWRT」 (@<href>{https://openwrt.org/})
  無線 LAN ルータ用のフリーなファームウェアを提供する Linux ディストリビューションで、数十名のハッカーから成る OpenWRT Project (@<href>{https://dev.openwrt.org/wiki/people}) が開発しています。OpenWRT はメーカー純正ファームウェアと異なり、普通の Linux ディストリビューションの感覚でソフトウェアをパッケージ単位でルータに追加・削除できます。

: 「自作 OpenFlow スイッチ用ファームウェア」 (@<href>{http://openflow.inthebox.info/})
  日本国内で比較的入手のしやすい、バッファローの無線 LAN ルータ「WHR-G301N」と「WZR-HP-AG300H」に対応した OpenFlow 化ファームウェアの配布サイトです。ファームウェアやドキュメント一式に加えソースコードも公開されていますので、腕に覚えのある方はぜひ開発に参加してみてください。

: 「@SRCHACK.ORG 氏のブログ」 (@<href>{http://www.srchack.org/})
  今回紹介した自作 OpenFlow スイッチ界の中心人物 @SRCHACK.ORG 氏のブログです。OpenFlow 対応ファームウェアを無線 LAN ルータ上で動作させるまでの試行錯誤や、自宅でサーバラックを運用する "自宅ラック友の会" の活動の様子などをブログ形式で紹介しています。

: 「オープンルータ・コンペティションまとめ」 (@<href>{https://sites.google.com/site/orcmatome/})
  「Interop Tokyo 2012」 併催の「オープンルータ・コンペティション」では、計 10 チームからのさまざまなアイデアでの参戦がありました。このうち 4 チームが OpenFlow を活用したアイデアで、この 4 チームすべてが実装に Trema を使っていました。筆者の 1 人が運営するこのまとめサイトには、参加各チームのスライドやソースコードへのリンクがあります。
