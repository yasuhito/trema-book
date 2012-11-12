= ルータ (前編)

//lead{
執筆中です
//}

== ルータとスイッチの違いとは？

ルータを実装する前に、ルータとスイッチ (→@<chap>{learning_switch}) の違いを考えてみましょう。これらの違いを一言で言うと、スイッチはパケットの転送に MAC アドレスを使う一方で、ルータは IP アドレスを使うということです。鋭い読者なら、ルータはなぜ IP アドレスを使って転送するのだろう？と思うのが当然です。MAC アドレスだけでパケットが届くなら、わざわざ IP アドレスを使う必要はありません。実はこれらの違いには、ちゃんとした理由があるのです。

MAC アドレスを使ってパケットを転送する LAN をイーサネットと呼びます。ネットワークを実装のレベルに応じて分類すると、イーサネットはハードウェアに近いレベルのネットワークです。どういうことかと言うと、イーサネットはパケットのあて先と送信元情報として MAC アドレス、つまりハードウェアであるネットワークカードに付いた番号を使うのです。この MAC アドレスは、パケットのイーサネットヘッダと呼ばれる部分に格納されます。

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
 * IP のレベルでは、送信元の IP アドレスはホスト A、またあて先の IP アドレスはホスト B になります。IP はイーサネットよりも上のレベルなので、IP アドレスはそのまま指定できます。

ルータは、受け取ったパケットを転送する際に、パケットのイーサネット部分だけを次のように書き換えます。送信元にルータの MAC アドレス、あて先にホスト B の MAC アドレスを指定して、パケットを出力します。

==== ルータの MAC アドレスを応える

ルータがパケットを受け取るためには、パケットの宛先 MAC アドレスはルータのものである必要があります。このためにホスト A は、パケットを送る前にルータの MAC アドレスを ARP リクエストで調べ、これを宛先 MAC アドレスとしてパケットに指定します。ルータは ARP リクエストを受け取ると、自身の MAC アドレスを ARP リプライとして返します (@<img>{arp_reply})。

//image[arp_reply][ルータは ARP リクエストに対し自分の MAC アドレスを応える]

==== あて先ホストの MAC アドレスを調べる

ルータが受け取ったパケットをあて先ホストに転送するためには、あて先ホストの MAC アドレスを調べる必要があります (@<img>{arp_request})。そこでルータは、あて先であるホスト B の MAC アドレスを調べるための ARP リクエストをホスト B へ送ります。

//image[arp_request][あて先ホストの MAC アドレスを問い合わせる]

ARP を使って調べた MAC アドレスは、再利用するためにルータ内の ARP テーブルにキャッシュしておきます。これによって、同じホストに対してパケットを繰り返し送る場合、何度も ARP リクエストを送らなくてもすみます。

=== いくつものルータを経由して転送する

ルータが複数あるネットワークでの転送は、少し複雑になります (@<img>{router_network})。例えば、ホスト A がホスト C にパケットを送るとします。ルータ A は受け取ったパケットを転送する必要がありますが、宛先であるホスト B はルータ A と直接はつながっていません。そのため、ルータ A はまずルータ B にパケットを転送し、ルータ B がそのパケットをホスト B へと転送します。

//image[router_network][(ダミーの図です) ルータが複数あるネットワークでの転送]

このとき、ルータ A の次の転送先となるルータはあて先ごとに異なります。例えばホスト A からホスト D へパケットを送る場合には、ルータ A はそのパケットをルータ C へと転送します。

この動作を行うために、各ルータは、あて先と次の転送先の対応を記録したルーティングテーブルを持っています。例えば、ルータ A の経路表は@<table>{rtable_a} のようになります。

//table[rtable_a][ルータ A のルーティングテーブル]{
宛先			次の転送先
ホスト B		ルータ B
ホスト C		ルータ C
//}

== ソースコード

//list[simple-router.rb][シンプルルータ (@<tt>{simple-router.rb}) のソースコード]{
require "arptable"
require "config"
require "interface"
require "packet-queue"
require "routing-table"
require "utils"


class SimpleRouter < Controller
  include Utils


  add_timer_event :age_arp_table, 5, :periodic


  def start
    @interfaces = Interfaces.new( $interface )
    @arp_table = ARPTable.new
    @routing_table = RoutingTable.new( $route )
  end


  def packet_in( dpid, message )
    return if not @interfaces.ours?( message.in_port, message.macda )

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


  #######
  private
  #######


  def handle_arp_request( dpid, message )
    port = message.in_port
    interface = @interfaces.find_by_port_and_ipaddr( port, message.arp_tpa )
    if interface
      packet = create_arp_reply( message, interface.hwaddr )
      send_packet( dpid, packet, interface )
    end
  end


  def handle_arp_reply( dpid, message )
    @arp_table.update( message.in_port, message.arp_spa, message.arp_sha )
  end


  def handle_ipv4( dpid, message )
    if should_forward?( message )
      forward( dpid, message )
    elsif message.icmpv4_echo_request?
      handle_icmpv4_echo_request( dpid, message )
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
      packet = create_icmpv4_reply( arp_entry, interface, message )
      send_packet( dpid, packet, interface )
    else
      handle_unresolved_packet( dpid, message, interface, saddr )
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
      flow_mod( dpid, message, action )
      packet_out( dpid, message.data, action )
    else
      handle_unresolved_packet( dpid, message, interface, nexthop )
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
    packet_out( dpid, packet, ActionOutput.new( :port => interface.port ) )
  end


  def handle_unresolved_packet( dpid, message, interface, ipaddr )
    packet = create_arp_request( interface, ipaddr )
    send_packet( dpid, packet, interface )
  end


  def age_arp_table
    @arp_table.age
  end
end
//}

=== Packet In ハンドラ

シンプルルータの Packet In ハンドラ (@<tt>{packet_in}) の中身を見ていきます。

//emlist{
  def packet_in( dpid, message )
    # 自身宛てのパケットかを調べる (処理 1)
    return if not @interfaces.ours?( message.in_port, message.macda )

    # メッセージの種別ごとに処理を振り分け (処理 2)
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

@<tt>{packet_in} は、受信パケットを処理すべきかの判断 (処理 1) を行い、メッセージ種別毎の処理への振り分け (処理 2) の二つの処理を行います。

一つ目の処理である、受信パケットを自身が処理すべきどうかの判断は、@<tt>{Interface} クラスの @<tt>{ours?} メソッドで行います。このメソッドは、宛先 MAC アドレス (@<tt>{macda}) がブロードキャストであるか、もしくは受信ポート (@<tt>{port}) に割り当てられている MAC アドレスと同じである場合、自身が処理すべきとして、@<tt>{true} を返します。

//emlist{
  def ours? port, macda
    return true if macda.broadcast?

    interface = find_by_port( port )
    if not interface.nil? and interface.has?( macda )
      return true
    end
  end
//}

二つ目の処理であるメッセージ種別の判別には、Trema の @<tt>{PacketIn} クラスに用意されている、パケット種別判定のためのメソッドを使います。さまざまなパケット種別を判定するためのメソッドが用意されていますが、上記の @<tt>{packet_in} ハンドラ中では、以下のメソッドを使っています。

: @<tt>{arp_request?}
  受信パケットが ARP リクエストの場合、true を返します。
: @<tt>{arp_reply?}
  受信パケットが ARP リプライの場合、true を返します。
: @<tt>{ipv4?}
  受信パケットが IPv4 パケットの場合、true を返します。

受信パケットが ARP リクエストであった場合、@<tt>{handle_arp_request} メソッドが呼ばれます。ARP リクエストに応答するために作成した ARP リプライメッセージを、Packet Out を用いて出力します。

//emlist{
  def handle_arp_request( dpid, message )
    port = message.in_port
    interface = @interfaces.find_by_port_and_ipaddr( port, message.arp_tpa )
    if interface
      packet = create_arp_reply( message, interface.hwaddr )
      send_packet( dpid, packet, interface )
    end
  end
//}

受信パケットが ARP リプライであった場合、自身の ARP テーブルにその結果を格納します。

//emlist{
  def handle_arp_reply( dpid, message )
    @arp_table.update( message.in_port, message.arp_spa, message.arp_sha )
  end
//}

@<tt>{handle_arp_request} と @<tt>{handle_arp_reply} では、受信した ARP パケットに格納されている IP アドレスや MAC アドレスを取得するために、Trema の @<tt>{Packet In} クラスに用意されているアクセサメソッドを用いています。様々な種別のパケット中に格納されている値を取得するアクセサメソッドを用意していますが、上記では以下を用いています。

: @<tt>{arp_tpa} 
  ARP パケット中の宛先 IP アドレス
: @<tt>{arp_tha}
  ARP パケット中の宛先 MAC アドレス
: @<tt>{arp_spa}
  ARP パケット中の送信元 IP アドレス
: @<tt>{arp_sha}
  ARP パケット中の送信元 MAC アドレス

=== IPv4 パケット受信時の処理

packet_in ハンドラ中で受信パケットが IPv4 であると判断された場合、@<tt>{handle_ipv4} メソッドが呼び出されます。

//emlist{
  def handle_ipv4( dpid, message )
    if should_forward?( message )      # パケットを転送すべきかを判断する
      forward( dpid, message )
    elsif message.icmpv4_echo_request? # ICMP Echo リクエストパケットかを判断する 
      handle_icmpv4_echo_request( dpid, message )
    else
      # noop.
    end
  end
//}

@<tt>{handle_ipv4} ではまず、@<tt>{should_forward?} メソッドを用いて、パケットを転送すべきかどうかを判断します。転送すべきと判断された場合、@<tt>{forward} メソッドでパケットの転送を行います。

//emlist{
  def should_forward?( message )
    not @interfaces.find_by_ipaddr( message.ipv4_daddr )
  end
//}

@<tt>{should_forward?} では、パケットの宛先 IPv4 アドレスを参照し、自身のインターフェイスに割り当てられているものと同じかどうかを調べます。宛先 IPv4 アドレスが自身に割り当てられたものでない場合、@<tt>{forward} メソッドを呼び出し、パケットを転送します。

パケットの宛先が自身である場合、ルータが処理を行う必要があります。今回実装したシンプルルータでは、ICMP Echo リクエストに対する応答機能だけ実装しています。そのため @<tt>{PacketIn} クラスのメソッドである @<tt>{icmpv4_echo_request?} でパケット種別の判定を行い、ICMP Echo リクエストである場合のみ @<tt>{handle_icmpv4_echo_request} を呼び出し、応答を行います。

=== パケットの転送

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
