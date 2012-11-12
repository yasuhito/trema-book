= シンプルなルータ (前編)

//lead{
執筆中です
//}

== ルータとスイッチの違いとは？

ルータを実装する前に、ルータとスイッチ (→@<chap>{learning_switch}) の違いを考えてみましょう。これらの違いを一言で言うと、スイッチはパケットの転送に MAC アドレスを使う一方で、ルータは IP アドレスを使うということです。鋭い読者なら、ルータはなぜ IP アドレスを使って転送するのだろう？と思うのが当然です。MAC アドレスだけでパケットが届くなら、わざわざ IP アドレスを使う必要はありません。実はこれらの違いには、ちゃんとした理由があるのです。

MAC アドレスを使ってパケットを転送する LAN をイーサネットと呼びます。ネットワークを実装のレベルに応じて分類すると、イーサネットはハードウェアに近いレベルのネットワークです。どういうことかと言うと、イーサネットはパケットの宛先と送信元情報として MAC アドレス、つまりハードウェアであるネットワークカードに付いた番号を使うのです。この MAC アドレスは、パケットのイーサネットヘッダと呼ばれる部分に格納されます。

実は、イーサネットと同じくハードウェアに近いレベルのパケット転送方式は他にもいくつもあります。つまりイーサネットは、次に挙げるいくつもある転送方式のうちの、たった一つにすぎないのです。

 * 身近なところでは、ADSL や光ファイバーによるインターネット接続に使う PPP (Point to Point Protocol) は、イーサネットと同じレベルの転送方式です。
 * 徐々に利用が減りつつありますが、WAN で使われる ATM (Asynchronous Transfer Mode) やフレームリレーも同じ仲間です。
 * 大昔には、大学の計算機室のワークステーション間をつなげるための FDDI (Fiber-Distributed Data Interface) という規格もありました。

このような異なるネットワーク同士をつなぐために考え出されたのが IP、つまりインターネットプロトコルです。インターネットはその名のとおり、ネットとネットの間 (inter) をつなぐ技術で、イーサネットその他のネットワークの一段上に属します。ルータはインターネットプロトコルでの識別情報である IP アドレスを使って、より低いレベルのネットワーク同士をつなぐのです。

== イーサネットだけならルータは不要？

ほとんどのネットワークがイーサネットなら、わざわざルータで中継するのはなぜ？これが次の疑問です。現に、OpenFlow のマッチングルールで扱えるのは MAC アドレスなどイーサネットの情報だけで、PPP や ATM といった他のネットワークは対象外です。もしイーサネットだけを使うのであれば、ルータは不要なのでしょうか？

しかし、大きなネットワークをイーサネットだけで作るには、次の問題があります。

 * ブロードキャストの問題: ネットワーク上の情報を発見するためにブロードキャストパケットを送ると、ネットワーク上のすべてのホストにパケットがばらまかれます。もし大きいネットワーク内でみんながブロードキャストを送ってしまうと、ネットワークがパンクしてしまいます。
 * セキュリティ上の問題: もし全体がひとつのネットワークでどことでも自由に通信できてしまうと、他人の家や別の部署のホストと簡単に接続できてしまいます。そうなると、プライバシー情報や機密データを守るのは大変です。

そこで、現実的にはイーサネットだけでネットワークを作る場合にも、家や会社の部署といった単位で小さなネットワークを作り、それらをルータでつなぐ場合が大半です。ルータがネットワーク間の「門番」としても働き、実際にパケットを転送するかしないかを制御することで、上で挙げた 2 つの問題を解決するのです。

== ルータの動作

ルータの存在意義がわかったところで、ルータの動作を詳しく見ていきましょう。おおざっぱに言って、ルータは複数のイーサネットをつなぐために、1)イーサネット間でのパケットの転送と、2)正しく届けるのに必要なパケットの書き換え、の 2 つの仕事を行います。

=== パケットを書き換えて転送する

@<img>{forward} のホスト A がホスト B にパケットを送る場合を考えます。このときパケットの中身とルータの動作は次のようになります。

//image[forward][ルータはパケットを転送するために、パケットのイーサネット部分だけを書き換える]

 * イーサネットのレベルでは、送信元の MAC アドレスはホスト A、また宛先の MAC アドレスはルータの MAC アドレスになります。ホスト B に届けるためにはルータに中継してもらわなくてはならないため、宛先にはこのようにルータの MAC アドレスを指定します。
 * IP のレベルでは、送信元の IP アドレスはホスト A、また宛先の IP アドレスはホスト B になります。IP はイーサネットよりも上のレベルなので、IP アドレスはそのまま指定できます。

ルータは、受け取ったパケットを転送する際に、パケットのイーサネット部分だけを次のように書き換えます。送信元にルータの MAC アドレス、宛先にホスト B の MAC アドレスを指定して、パケットを出力します。

==== ルータの MAC アドレスを応える

ルータがパケットを受け取るためには、パケットの宛先 MAC アドレスはルータのものである必要があります。このためにホスト A は、パケットを送る前にルータの MAC アドレスを ARP リクエストで調べ、これを宛先 MAC アドレスとしてパケットに指定します。ルータは ARP リクエストを受け取ると、自身の MAC アドレスを ARP リプライとして返します (@<img>{arp_reply})。

//image[arp_reply][ルータは ARP リクエストに対し自分の MAC アドレスを応える]

==== 宛先ホストの MAC アドレスを調べる

ルータが受け取ったパケットを宛先ホストに転送するためには、宛先ホストの MAC アドレスを調べる必要があります (@<img>{arp_request})。そこでルータは、宛先であるホスト B の MAC アドレスを調べるための ARP リクエストをホスト B へ送ります。

//image[arp_request][宛先ホストの MAC アドレスを問い合わせる]

ARP を使って調べた MAC アドレスは、再利用するためにルータ内の ARP テーブルにキャッシュしておきます。これによって、同じホストに対してパケットを繰り返し送る場合、何度も ARP リクエストを送らなくてもすみます。

=== いくつものルータを経由して転送する

ルータが複数あるネットワークでの転送は、少し複雑になります (@<img>{router_network})。例えば、ホスト A がホスト C にパケットを送るとします。ルータ A は受け取ったパケットを転送する必要がありますが、宛先であるホスト B はルータ A と直接はつながっていません。そのため、ルータ A はまずルータ B にパケットを転送し、ルータ B がそのパケットをホスト B へと転送します。

//image[router_network][ルータが複数あるネットワークでの転送]

このとき、ルータ A の次の転送先となるルータは宛先ごとに異なります。例えばホスト A からホスト C へパケットを送る場合には、ルータ A はそのパケットをルータ C へと転送します。

この動作を行うために、各ルータは、宛先と次の転送先の対応を記録したルーティングテーブルを持っています。例えば、ルータ A の経路表は@<table>{rtable_a} のようになります。

//table[rtable_a][ルータ A のルーティングテーブル]{
宛先			次の転送先
ホスト B		ルータ B
ホスト C		ルータ C
//}

== ソースコード

シンプルルータはメインのソースコード (@<list>{simple-router.rb}) の他にいくつかのファイルからなります。紙面の都合上、以下ではメインのソースコードを中心に説明します。残りソースコードについては、Trema のサンプルディレクトリにあるソースコード (@<tt>{src/examples/router}) を参照してください。

//list[simple-router.rb][シンプルルータ (@<tt>{simple-router.rb}) のソースコード]{
require "arp-table"
require "interface"
require "router-utils"
require "routing-table"


class SimpleRouter < Controller
  include RouterUtils


  def start
    load "router.conf"
    @interfaces = Interfaces.new( $interface )
    @arp_table = ARPTable.new
    @routing_table = RoutingTable.new( $route )
  end


  def packet_in( dpid, message )
    return if not to_me?( message )

    if message.arp_request?
      handle_arp_request dpid, message
    elsif message.arp_reply?
      handle_arp_reply dpid, message
    elsif message.ipv4?
      handle_ipv4 dpid, message
    else
      # noop.
    end
  end


  private


  def to_me?( message )
    return true if message.macda.broadcast?

    interface = @interfaces.find_by_port( message.in_port )
    if interface and interface.has?( message.macda )
      return true
    end
  end


  def handle_arp_request( dpid, message )
    interface = @interfaces.find_by_port_and_ipaddr( message.in_port, message.arp_tpa )
    if interface
      arp_reply = create_arp_reply( message, interface.hwaddr )
      send_packet dpid, arp_reply, interface
    end
  end


  def handle_arp_reply( dpid, message )
    @arp_table.update message.in_port, message.arp_spa, message.arp_sha
  end


  def handle_ipv4( dpid, message )
    if should_forward?( message )
      forward dpid, message
    elsif message.icmpv4_echo_request?
      handle_icmpv4_echo_request dpid, message
    else
      # noop.
    end
  end


  def should_forward?( message )
    not @interfaces.find_by_ipaddr( message.ipv4_daddr )
  end


  def handle_icmpv4_echo_request( dpid, message )
    interface = @interfaces.find_by_port( message.in_port )
    saddr = message.ipv4_saddr.value
    arp_entry = @arp_table.lookup( saddr )
    if arp_entry
      icmpv4_reply = create_icmpv4_reply( arp_entry, interface, message )
      send_packet dpid, icmpv4_reply, interface
    else
      handle_unresolved_packet dpid, message, interface, saddr
    end
  end


  def forward( dpid, message )
    nexthop = resolve_nexthop( message )

    interface = @interfaces.find_by_prefix( nexthop )
    if not interface or interface.port == message.in_port
      return
    end

    arp_entry = @arp_table.lookup( nexthop )
    if arp_entry
      action = interface.forward_action( arp_entry.hwaddr )
      flow_mod dpid, message, action
      packet_out dpid, message.data, action
    else
      handle_unresolved_packet dpid, message, interface, nexthop
    end
  end


  def resolve_nexthop( message )
    daddr = message.ipv4_daddr.value
    nexthop = @routing_table.lookup( daddr )
    if nexthop
      nexthop
    else
      daddr
    end
  end


  def flow_mod( dpid, message, action )
    send_flow_mod_add(
      dpid,
      :match => ExactMatch.from( message ),
      :actions => action
    )
  end


  def packet_out( dpid, packet, action )
    send_packet_out(
      dpid,
      :data => packet,
      :actions => action
    )
  end


  def send_packet( dpid, packet, interface )
    packet_out dpid, packet, ActionOutput.new( interface.port )
  end


  def handle_unresolved_packet( dpid, message, interface, ipaddr )
    arp_request = create_arp_request( interface, ipaddr )
    send_packet dpid, arp_request, interface
  end
end
//}

それでは、シンプルルータのソースコードの重要な部分を見ていきましょう。

=== Packet In ハンドラ

シンプルルータの動作の中心は Packet In ハンドラです。ルータはパケットを受け取って転送するのが仕事なので、メインのハンドラは次の @<tt>{packet_in} になります。

//emlist{
  def packet_in( dpid, message )
    return if not to_me?( message )

    if message.arp_request?
      handle_arp_request( dpid, message )
    elsif message.arp_reply?
      handle_arp_reply( dpid, message )
    elsif message.ipv4?
      handle_ipv4( dpid, message )
    else
      # noop.
    end
  end
//}

==== 自分宛のパケットかを判定する

ルータはいくつものネットワークにつながっているので、Packet In メッセージが上がってきたときには、まずそのパケットが自分宛かどうかを判断します (@<tt>{to_me?} メソッド)。もし自分宛でない場合にはパケットを破棄します。

//emlist{
  def to_me?( message )
    return true if message.macda.broadcast?

    interface = @interfaces.find_by_port( message.in_port )
    if interface and interface.has?( message.macda )
      return true
    end
  end
//}

この @<tt>{to_me?} メソッドの動作は次のようになっています。まず、宛先 MAC アドレス (@<tt>{macda}) がブロードキャストである場合には自分宛と判断します。次に、宛先 MAC アドレスが受信ポート (@<tt>{message.in_port}) に割り当てられている MAC アドレスと同じである場合にも、自身宛と判断します。

==== パケットの種類によって処理を切り替え

自分宛のパケットだと分かった場合、次にパケットの種類を判別します。シンプルルータが処理するパケットは、ARP のリクエストとリプライ、および IPv4 のパケットの 3 種類です。@<tt>{PacketIn} クラスに用意されている次のメソッドを使って、パケットの種類によって処理を切り替えます。

: @<tt>{arp_request?}
  受信パケットが ARP リクエストの場合、@<tt>{true} を返す。
: @<tt>{arp_reply?}
  受信パケットが ARP リプライの場合、@<tt>{true} を返す。
: @<tt>{ipv4?}
  受信パケットが IPv4 パケットの場合、@<tt>{true} を返す。

==== ARP リクエストのハンドル

受信パケットが ARP リクエストであった場合、次の@<tt>{handle_arp_request} メソッドが呼ばれます。ここでは、ARP リプライメッセージを作って Packet Out で ARP リクエストが届いたポートに出力します。

//emlist{
  def handle_arp_request( dpid, message )
    port = message.in_port
    interface = @interfaces.find_by_port_and_ipaddr( port, message.arp_tpa )
    if interface
      packet = create_arp_reply( message, interface.hwaddr )
      send_packet dpid, packet, interface
    end
  end
//}

なお、このハンドラ中で使っている @<tt>{message.arp_tpa} は @<tt>{PacketIn} クラスで定義されたメソッドで、ARP パケット中の宛先 IP アドレスを返します。

==== ARP リプライのハンドル

受信パケットが ARP リプライであった場合、ARP テーブル (@<tt>{@arp_table}) に MAC アドレスを格納します。

//emlist{
  def handle_arp_reply( dpid, message )
    @arp_table.update message.in_port, message.arp_spa, message.arp_sha
  end
//}

なお、ここでも同様に @<tt>{PacketIn} クラスに定義された以下のメソッドを使って ARP パケットからさまざまな情報を取り出しています。

: @<tt>{arp_spa}
  ARP パケット中の送信元 IP アドレスを返す
: @<tt>{arp_sha}
  ARP パケット中の送信元 MAC アドレスを返す

==== IPv4 パケットのハンドル

受信パケットが IPv4 であった場合、@<tt>{handle_ipv4} メソッドが呼ばれます。ルータに届く IPv4 パケットには、次の 3 種類があり、それぞれによって処理を切り替えます。

 1. パケットを転送する場合。つまり、パケットが自分宛でなかった場合。
 2. パケットが自分宛だった場合。
 3. それ以外だった場合 (IPv6 など)。この場合はパケットを破棄します。

//emlist{
  def handle_ipv4( dpid, message )
    if should_forward?( message )
      forward dpid, message
    elsif message.icmpv4_echo_request?
      handle_icmpv4_echo_request dpid, message
    else
      # noop.
    end
  end
//}

パケットを転送するかどうかを判定するのが次の @<tt>{should_forward?} メソッドです。パケットを転送する場合とはつまり、パケットの宛先 IPv4 アドレスが、ルータのインターフェイスに割り当てらている IPv4 アドレスと異なる場合です。

//emlist{
  def should_forward?( message )
    not @interfaces.find_by_ipaddr( message.ipv4_daddr )
  end
//}

パケットの宛先がルータである場合、ルータ自身が応答する必要があります。今回実装したシンプルルータでは、ICMP Echo リクエスト (ping) に応答する機能だけ実装しています。詳細は省きますが、(☆ここ説明よろしくおねがいします☆)。

//emlist{
  def handle_icmpv4_echo_request( dpid, message )
    interface = @interfaces.find_by_port( message.in_port )
    saddr = message.ipv4_saddr.value
    arp_entry = @arp_table.lookup( saddr )
    if arp_entry
      icmpv4_reply = create_icmpv4_reply( arp_entry, interface, message )
      send_packet dpid, icmpv4_reply, interface
    else
      handle_unresolved_packet dpid, message, interface, saddr
    end
  end
//}

=== パケットを転送する

パケット転送の役目を担うのが、@<tt>{forward} メソッドです。

//emlist{
  def forward( dpid, message )
    # 1. 経路表を参照し、次転送先を決定する
    nexthop = resolve_nexthop( message )

    # 2. 出力インターフェイスを決定する
    interface = @interfaces.find_by_prefix( nexthop )
    if not interface or interface.port == message.in_port
      return
    end

    # 3. ARP テーブルを検索する
    arp_entry = @arp_table.lookup( nexthop )
    if arp_entry
      # 4. 転送用のフローエントリを設定し、受信パケットを Packet Out する
      action = interface.forward_action( arp_entry.hwaddr )
      flow_mod( dpid, message, action )
      packet_out( dpid, message.data, action )
    else
      # 5. MAC アドレスを問い合わせるための ARP リクエストを出す
      handle_unresolved_packet( dpid, message, interface, nexthop )
    end
  end
//}

このメソッドでは、主に以下の 5 つの処理を行なっています。

 1. 経路表を参照し、次転送先を決定する
 2. 次転送先に送るための、出力インターフェイスを決定する
 3. ARP テーブルを検索する
 4. 3 で ARP エントリが見つかった場合、転送用のフローエントリを設定し、受信パケットを Packet Out する
 5. 3 で ARP エントリが見つからなかった場合、MAC アドレスを問い合わせるための ARP リクエストを作り、Packet Out する

4 の処理では、@<tt>{Interface} クラスの @<tt>{forward_action} メソッドを用いて、フローエントリのアクションを作成しています。@<img>{forward} で説明したように、ルータは MAC アドレスを書き換えてから出力する必要があります。そのため、@<tt>{forward_action} は、送信元 MAC アドレスの書き換え、宛先 MAC アドレスの書き換え、該当するポートからの出力という三つのアクションを含む配列を作成します。

//emlist{
  def forward_action macda
    [
      SetEthSrcAddr.new( hwaddr.to_s ),
      SetEthDstAddr.new( macda.to_s ),
      SendOutPort.new( port )
    ]
  end
//}

== まとめ/参考文献
