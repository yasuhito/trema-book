= OpenFlow の仕組み

//lead{
OpenFlow は大きな仕様です。この章では最低限必要なポイントと OpenFlow 特有の用語をてっとりばやく頭に入れましょう。
//}

OpenFlow は Open Networking Foundation (ONF) という非営利団体によって標準化が進められています。この OpenFlow 標準仕様が主に定義するのは次の 2 つです。

 1. コントローラとスイッチの間の通信プロトコル (セキュアチャンネル)
 2. 設定されたフローに対するスイッチの振る舞い

本章ではこの 2 つの中でも特によく使う機能を重点的に説明します。なお執筆時の OpenFlow 最新バージョンは 1.3 ですが、以降の説明では対応する商用スイッチも出そろっているバージョン 1.0 をベースに説明します。なお、1.0 とそれ以降では考え方やモデルに大きな変更はありませんので、1.0 を理解していれば新しいバージョンも理解しやすいでしょう。

== スイッチとコントローラ間のやりとり

OpenFlow スイッチとコントローラは OpenFlow 仕様で規定されたメッセージをやりとりしながら動作します。ここでは具体的にどのようなメッセージを送受信するかを順を追って説明します。@<chap>{whats_openflow}で見たカスタマーサービスセンターでのやりとりを思い出しながら読んでください。

=== スイッチとコントローラ間の接続を確立する

最初にコントローラとスイッチはセキュアチャンネルの接続を確立します。OpenFlow にはセキュアチャンネルをスイッチとコントローラのどちらが始めるかの規定はありませんが、現状では@<img>{secure_channel} のようにスイッチがコントローラに接続する実装が多いようです。

//image[secure_channel][スイッチがコントローラに接続しセキュアチャンネルを確立]

ちなみにセキュアチャンネルは普通の TCP ソケットですが、よりセキュアな TLS (Transport Layer Security) を使うこともできます。この場合、スイッチの証明書の確認も行われます。

=== バージョンのネゴシエーション

次に使用する OpenFlow プロトコルのバージョンを確認するステップ、いわゆるバージョンネゴシエーションが始まります。セキュアチャンネルを確立すると、スイッチとコントローラはお互いに自分のしゃべれるバージョン番号を乗せた Hello メッセージを出し合います (@<img>{version_negotiation})。

//image[version_negotiation][Hello メッセージを出し合うことで相手の OpenFlow プロトコルバージョンを確認]

相手と同じバージョンを話せるようであればネゴシエーションに成功で、本格的におしゃべりを始められるようになります。

=== スイッチのスペックの確認

次にコントローラは接続したスイッチがどんなスイッチかを確認します。ネゴシエーション成功後はまだバージョンしか確認できていないので、コントローラはスイッチに Features Request というメッセージを送って次の各情報をリクエストします。

 * スイッチのユニーク ID (Datapath ID)
 * 物理ポートの一覧情報
 * サポートする機能の一覧

//image[features_request_reply][Features Request メッセージでスイッチのスペックを確認]

スイッチは Features Reply メッセージでこの情報を返信します。

=== 未知のパケットの受信

スイッチはフローとして登録されていない通信を検知すると、コントローラにそのパケットを通知します。これは Packet In メッセージと呼ばれます。

//image[packet_in][未知のパケットとその情報が Packet In メッセージとしてコントローラに上がる]

=== フローテーブルの更新とパケットの転送

パケットを転送する場合には、まず Packet Out という OpenFlow メッセージによって Packet In を起こしたパケット自体を正しい宛先に流してやります。これをやらないと、Packet In を起こしたパケットがコントローラに残ったままになってしまうからです。そして、同様の通信を今後はスイッチ側だけで処理するために、Flow Mod メッセージによってフローをスイッチのフローテーブルに書き込みます。

//image[flowmod_packetout][Flow Mod によってフローテーブルを更新し、Packet In を起こしたパケットを Packet Out で転送]

=== フローの寿命と統計情報

Flow Mod で打ち込むフローには「寿命」を設定できます。寿命の指定には次の 2 種類があります。

 * アイドルタイムアウト: パケット処理を行っていない時間がこの寿命に逹っするとフローを消す。パケットが到着した時点で 0 秒にリセットされる。
 * ハードタイムアウト: パケット処理の有無を問わずこの寿命に逹っするとフローを消す。

どちらのタイムアウトも 0 にするとそのフローは明示的に消さない限りフローテーブルに残ります。

寿命に逹っしたフローはフローテーブルから消され、消されたフローの情報と、フローにしたがって処理されたパケットの統計情報がコントローラに通知されます。これを Flow Removed メッセージと呼びます。これを利用してコントローラはネットワーク全体のトラフィックを集計できます。

//image[flow_removed][フローが寿命で削除されると、フローによって転送されたパケットの統計情報が Flow Removed としてコントローラに上がる]

== フローの中身

@<chap>{whats_openflow}で見たようにフローは次の 3 要素から成ります。

 * マッチングルール
 * アクション
 * 統計情報
 
以下ではそれぞれの中身を少し細かく見ていきます。なお、これらを最初からすべて頭に入れる必要はありません。以降の章を読んでいてわからなくなったらリファレンスとして活用してください。

=== マッチングルール

マッチングルールとは、OpenFlow スイッチがパケットを受け取ったときにアクションを起こすかどうかを決める条件です。たとえば「パケットの宛先が http サーバだったら」とか「パケットの送信元がローカルネットワークだったら」などという条件に適合したパケットにだけ、スイッチがアクションを起こすというわけです。

OpenFlow 1.0 では、@<table>{matching_rules} の 12 種類の条件が使えます。これらの条件はイーサネットや TCP/UDP でよく使われる値です。

//table[matching_rules][マッチングルールで指定できる 12 種類の条件]{
名前				説明				
--------------------------------------------------------------
Ingress Port		スイッチの物理ポート番号
Ether src			送信元 MAC アドレス
Ether dst			宛先 MAC アドレス
Ether type			イーサネットの種別
VLAN id				VLAN ID
VLAN priority		VLAN PCP の値 (CoS)
IP src				送信元 IP アドレス
IP dst				宛先 IP アドレス
IP proto			IP のプロトコル種別
IP ToS bits			IP の ToS 情報
TCP/UDP src port	TCP/UDP の送信元ポート番号
TCP/UDP dst port	TCP/UDP の宛先ポート番号
//}

OpenFlow の世界では、このマッチングルールで指定できる条件を自由に組み合わせて通信を制御します。たとえば、

 * スイッチの物理ポート 1 番から届く、宛先が TCP 80 番 (= HTTP) のパケットを書き換える
 * MAC アドレスが 02:27:e4:fd:a3:5d で宛先の IP アドレスが 192.168.0.0/24 のものは遮断する
   
というふうに自由に複数の条件を組み合わせることができます。

===[column] 取間先生曰く: OSI ネットワークモデルが壊れる？

あるネットワークの経験豊富な若者がこんな事を言っていた「OpenFlow のようにネットワークレイヤをまたがって自由に何でもできるようになると、OSI ネットワークモデルが壊れるんじゃないか？」

その心配は無用じゃ。OSI ネットワークモデルは正確に言うと「OSI 参照モデル」と言って、通信プロトコルを分類して見通しを良くするために定義した "参照用" の階層モデルじゃ。たとえば自分が xyz プロトコルというのを作ったけど人に説明したいというときに、どう説明するか考えてみてほしい。「これはレイヤ 3 のプロトコルで、…」という風に階層を指して (参照して) 説明を始めれば相手に通りがいいじゃろ。つまり、OSI ネットワークモデルはネットワーク屋同士で通じる「語彙」として使える、まことに便利なものじゃ。

でも、これはあくまで「参照」であって「規約」ではないので、すべてのネットワークプロトコル、ネットワーク機器がこれに従わなきゃならん、というものではないぞ。さっき言ったように「この ○○ は、仮に OSI で言うとレイヤ4 にあたる」のように使うのが正しいのじゃ。

そして、OpenFlow はたまたまいくつものレイヤの情報が使える、ただそれだけのことじゃ。

===[/column]

=== アクション

アクションとは、スイッチに入ってきたパケットをどう料理するか、という@<em>{動詞}にあたる部分です。よく「OpenFlow でパケットを書き換えて曲げる」などと言いますが、こうした書き換えなどはすべてアクションで実現できます。それでは、OpenFlow 1.0 ではどんなアクションが定義されているか見てみましょう。

==== アクションの種類

アクションは大きく分けて次の 4 種類があります。

 * Forward: パケットを指定したポートから出力する
 * Modify-Field: パケットの中身を書き換える
 * Drop: パケットを捨てる
 * Enqueue: ポートごとに指定されたスイッチのキューに入れる。QoS 用。
 
アクションは動詞と同じく指定した順番に実行されます。「おにぎりを作って、食べて、片付ける」といったふうに。たとえば、パケットを書き換えて指定したポートに出したいときには、

//emlist{
[Modify-Field, Forward]
//}

というアクションのリストを指定します。ここで、アクションは指定された順番に実行されることに注意してください。アクションの順番を変えてしまうと、違う結果が起こります。たとえば「おにぎりを食べてから、おにぎりを作る」と最後におにぎりが残ってしまいます。同様に先ほどの例を逆にしてしまうと、先にパケットがフォワードされてしまうので Modify-Field は実行されなくなります。

//emlist{
# パケットを書き換える前にフォワードされてしまう。
[Forward, Modify-Field]
//}

同じ動詞を複数指定することもできます。

//emlist{
[Modify-Field A, Modify-Field B, Forward A, Forward B]
//}

この場合は、フィールド A と B を書き換えて、ポート A と B へフォワードする、と読めます。このように、複数の箇所を書き換えたり複数のポートからパケットを出したい場合には、アクションを複数連ねて指定します。

Drop は特殊なアクションで、実際には Drop アクションというものが具体的に定義されているわけではありません。アクションのリストに Forward アクションをひとつも入れなかった場合、そのパケットはどこにもフォワードされずに捨てられます。これを Drop アクションと呼んでいるわけです。

それでは、最もよく使われる Forward アクションと Modify-Field アクションで具体的に何が指定できるか見て行きましょう。

===== Forward アクション

Forward アクションでは指定した出力ポートにパケットを転送します。出力ポートはポート番号で指定することもできますが、いくつか論理的なポートが定義されています。

 * ポート番号: パケットを指定した番号のポートに出す。
 * IN_PORT: パケットを入ってきたポートに出す。
 * ALL: パケットを入ってきたポート以外のすべてのポートに出す。
 * FLOOD: パケットをスイッチの作ったスパニングツリーに沿って出す。スイッチが複数台ある場合にはループが起こらないので ALL よりも安全。
 * CONTROLLER: パケットをコントローラに明示的に送り、Packet In を起こす。
 * NORMAL: パケットをスイッチの機能を使って転送する。従来のスイッチの動作と同じ。
 * LOCAL: パケットをスイッチのローカルポートに出す。ローカルポートを監視するアプリケーションに特殊な処理をさせたい場合に使う。あまり使われない。

===== Modify-Field アクション

Modify-Field アクションではパケットの様々な部分を変更することができます。

 * VLAN ヘッダの除去: VLAN のヘッダを除去する。
 * VLAN ID の書き換え: 指定された VLAN ID をセットする、または既存のものがあれば書き換える。
 * VLAN プライオリティの書き換え: 指定された VLAN プライオリティをセットする、または既存のものがあれば書き換える。VLAN ID はゼロになる。
 * 送信元 MAC アドレスの書き換え: 送信元の MAC アドレスを書き換える。
 * 宛先 MAC アドレスの書き換え: 宛先の MAC アドレスを書き換える。
 * 送信元 IP アドレスの書き換え: 送信元の IP アドレスを書き換える。
 * 宛先 IP アドレスの書き換え: 宛先の IP アドレスを書き換える。
 * TCP/UDP 送信元ポートの書き換え: TCP/UDP の送信元ポート番号を書き換える。
 * TCP/UDP 宛先ポートの書き換え: TCP/UDP の宛先ポート番号を書き換える。
 * ToS フィールドの書き換え: IP の ToS フィールドを書き換える。

それぞれのアクションでできることと、代表的な使い途を順番に見ていきましょう。

====== VLAN 系の書き換え

OpenFlow はネットワーク仮想化の手段の一つである VLAN のパケットを扱うことができます。VLAN (仮想 LAN) は一枚のネットワーク (ブロードキャストが届く範囲) を薄くスライスして何枚かの仮想的な層に分離するための仕組みです。OpenFlow では仮想ネットワークを作る手段としてこの VLAN の仕組みをそのまま使うことができます。あるいは VLAN を使わずに別のネットワーク仮想化方式を使う場合でも、既存のネットワークから流れてくるかもしれない VLAN パケットをいったん素のパケットに戻す必要があります。いずれの場合でも OpenFlow スイッチは VLAN パケットを書き換える必要があります。Modify-Field アクションはこうした処理に必要なアクションを 3 種類用意しています。

//image[strip_vlan][VLAN 情報を書き換えるアクションの使い途][scale=0.3]

: VLAN ヘッダの除去
  VLAN を流れる VLAN ヘッダ付きパケットを普通のパケットに戻すアクションです。これによって、たとえば VLAN から流れてきたパケットを普通の仮想化されていないネットワークに流したり、別の方式で作った仮想ネットワークに流すための前処理として使えます。

: VLAN ID の書き換え
  VLAN パケットが属する仮想 LAN の ID を書き換えます。たとえば仮想 LAN 1 番 (ID = 1 番) を流れるパケットを仮想 LAN 3 番に流すときなどに使います。

: VLAN プライオリティの書き換え
  VLAN 上でのパケットを転送する優先度を変更します。このプライオリティはトラフィックの種類 (データ、音声、動画など) に応じて指定することができます。指定できる値は 0 (最低) から 7 (最高) までです。

====== MAC アドレスの書き換え

MAC アドレス書き換えの代表的な例がルータです。OpenFlow はルータの実装に必要な、送信元と宛先 MAC アドレスの書き換えアクションをサポートしています。

//image[rewrite_mac][ルータでの送信元と宛先 MAC アドレスの書き換え][scale=0.3]

ルータは 2 つのネットワークの間で動作し、ネットワーク間で行き交うパケットの交通整理を行います。ホスト A が異なるネットワークに属するホスト B にパケットを送ると、ルータはそのパケットを受け取りその宛先 IP アドレスから転送先のネットワークを決定します。そして、パケットに記述された宛先 MAC アドレスを次に送るべきホストの MAC アドレスに、送信元を自分の MAC アドレスに書き換えてデータを転送します。

====== IP アドレスの書き換え

IP アドレス書き換えの代表的な例が NAT (Network Address Transition) です。OpenFlow は NAT の実装に必要な、送信元と宛先 IP アドレスの書き換えアクションをサポートしています。

//image[rewrite_ip_address][NAT での送信元と宛先 IP アドレスの書き換え][scale=0.3]

ゲートウェイなどのインターネットと接続するルータでは、プライベート/グローバルネットワーク間での通信を通すために IP アドレスを次のように変換します。プライベートネットワーク内のクライアントホストからインターネット上のサーバに通信をする場合、ゲートウェイはプライベートネットワークから届いたパケットの送信元 IP アドレスを自分のグローバルな IP アドレスに変換して送信します。逆にサーバからの返信は逆の書き換えを行うことによりプライベートネットワーク内のクライアントホストに届けます。

====== TCP/UDP ポート番号の書き換え

TCP/UDP ポート番号書き換えの代表的な例が IP マスカレードです。OpenFlow は IP マスカレードの実装に必要な、送信元と宛先の TCP/UDP ポート番号の書き換えアクションをサポートしています。

//image[rewrite_port][IP マスカレードでの送信元と宛先 TCP/UDP ポート番号の書き換え][scale=0.3]

ブロードバンドルータなど 1 つのグローバルアドレスで複数のホストが同時に通信を行う環境では、NAT だけだと TCP/UDP のポート番号が重複する可能性があります。そこで、IP マスカレードではプライベートネットワーク側のポート番号をホストごとに適当に割り当て、通信のつどポート番号を変換することで解決します。

====== ToS フィールドの書き換え

ToS フィールドは通信のサービス品質 (QoS) を制御する目的でパケットを受け取ったルーターに対して処理の優先度を指定するために使われます。OpenFlow はこの ToS フィールドの書き換えアクションをサポートしています。

=== 統計情報

OpenFlow 1.0 ではフローごとに次の統計情報を取得できます。

 * 受信パケット数
 * 受信バイト数
 * フローが作られてからの経過時間 (秒)
 * フローが作られてからの経過時間 (ナノ秒)

== まとめ

 * OpenFlow の仕様書

