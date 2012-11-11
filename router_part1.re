= ルータ (前編)

//lead{
執筆中です
//}

== ルータとスイッチの違い

ルータを実装する前に、ルータとスイッチ (→@<chap>{learning_switch}) の違いを考えてみましょう。これらの違いを一言で言うと、スイッチは MAC アドレスを使って、ルータは IP アドレスを使ってパケットを転送するということです。鋭い読者なら、ルータはなぜ MAC アドレスではなく IP アドレスを使って転送するのだろう？と思うのが当然です。MAC アドレスだけでパケットが届くなら、わざわざ IP アドレスを使う必要はありません。実はこれらの違いには、ちゃんとした理由があるのです。

スイッチが使う MAC アドレスは、パケットのイーサネットヘッダと呼ばれる部分に入っています。イーサネットはネットワークのハードウェアに近いレベルの情報を扱います。たとえば、MAC アドレスはネットワークカードというハードウェアを区別するための番号です。OpenFlow のマッチングルールで扱えるイーサネットの情報には、MAC アドレスのほかにも VLAN ID があります。

実は、イーサネットと同じレベルのパケット転送方式は他にもいくつかあります。身近なところでは、ADSL や光ファイバーによるインターネット接続で使う PPP (Point to Point Protocol) があります。他にも、徐々に利用が減りつつありますが、WAN で使われる ATM (Asynchronous Transfer Mode) やフレームリレーもその一種です。大昔には、大学の計算機室のワークステーション間をつなげるための FDDI (Fiber-Distributed Data Interface) という技術もありました。このように、イーサネットはネットワークを作るための一つの選択肢にすぎません。

このような異なるネットワークをつなぐために考え出されたのが IP、つまりインターネットプロトコルです。インターネットはその名のとおり、ネットの間 (inter) をつなぎます。ルータはインターネットプロトコルでの識別情報である IP アドレスを使って、異なるネットワークの間で中継を行います。

== イーサネットだけならルータは不要？

ほとんどのネットワークがイーサネットで作られているなら、わざわざルータで中継するのはなぜ？これが次の疑問です。現に、OpenFlow で扱えるのも MAC アドレスや VLAN ID などイーサネットの情報だけです。もしイーサネットだけを使うのであれば、ルータは不要なのでしょうか？

しかし、大きなネットワークをイーサネットだけで作るには、次の問題があります。

 * ブロードキャストの問題: ネットワーク上の情報を発見するためにブロードキャストパケットを送ると、ネットワーク上のすべてのホストにパケットがばらまかれます。もし大きいネットワーク内でみんながブロードキャストを送ってしまうと、スイッチがパンクしてしまいます。
 * セキュリティ上の問題: もし全体がひとつのネットワークでどことでも自由に通信できてしまうと、他人の家や別の部署のホストと簡単に接続できてしまいます。そうなると、プライバシー情報や機密データを守るのは大変です。

そこで、現実的にはイーサネットだけでネットワークを作る場合にも、家や会社の部署といった単位で小さなネットワークを作り、それらをルータでつなぐ場合が大半です。ルータがネットワーク間の「門番」としても働き、実際にパケットを転送するかしないかを制御することで、上で挙げた 2 つの問題を解決するのです。

== ルータの動作

ルータの存在意義がわかったところで、動作を詳しく見ていきましょう。

=== パケットの転送

ルータは、受け取ったパケットを転送する際に、L2 (Ethernet) のフレームを乗せ換えを行います。このことにより、ルータは L2 ネットワーク同士を繋ぐ役割を果たします。

@<img>{forward} 中のホスト A がホスト B 宛にパケットを送る場合について考えます。その場合、ホスト A は、自身の IP アドレスをパケットの送信元に、またホスト B の IP アドレスを宛先に指定します。

//image[forward][ルータは、IP パケットの転送を行うときに、Ethernet フレームの載せ替えを行う]

しかし、ホスト A からホスト B へパケットを送るためには、ルータに転送してもらわなければいけません。そのため、ホスト A はパケットの宛先 MAC アドレスにルータの MAC アドレスを指定する必要があります。

ホスト A からのパケットを受け取ったルータは、送信元に自分の MAC アドレスを指定し、宛先にホスト B の MAC アドレスを指定して、パケットを出力します。

=== パケットを受け取る

@<img>{forward} で示したように、ルータがパケットを受け取る際には、パケットの宛先 MAC アドレスはルータのものである必要がありました。送信元ホストは、ルータの MAC アドレスを宛先としてパケットに付与して送信を行う必要があります。

送信元ホストは、ARP リクエストパケットを出して、ルータの MAC アドレスの問い合わせを行います。そのリクエストパケットに対して、ルータは自身の MAC アドレスを ARP リプライパケットとして応答を行います(@<img>{arp_reply})。

//image[arp_reply][ルータは、問い合わせに対し、自分の MAC アドレスを回答する]

=== 送り先の MAC アドレスを調べる

@<img>{forward} で示したように、ルータが受け取ったパケットを送る際には、送り先ホストの MAC アドレスを調べる必要があります(@<img>{arp_request})。そのため、ルータは、送り先ホストの IP アドレスに対応する MAC アドレスが何かを問い合わせる ARP リクエスト作成し、ホストへ送ります。ルータは、ホストからの ARP リプライを受け取ることで、送り先ホストの MAC アドレスが分かります。

//image[arp_request][送り先ホストの MAC アドレスを問い合わせる]

ARP を使って調べた結果は、再利用するためにルータ内の ARP テーブル (@<table>{arptable}) に格納しておきます。このことにより、同じホストに対して MAC アドレスを調べるため、何度も ARP リクエストを送らなくてもすみます。

//table[arptable][ルータの ARP テーブル]{
IP アドレス		   MAC アドレス
192.168.2.2		   00:00:00:02:02:02
//}

=== 次転送先を決める

ここでは @<img>{router_network} のように、複数のルータから構成されるネットワーク上で、ルータがどのような転送動作を行うかについて考えてみます。

//image[router_network][複数のルータから構成されるネットワーク]

@<img>{forward} のようにルータが一台の場合と比べ、複数台から構成されるネットワークでの転送動作は、少し複雑になります。例えば、ホスト A がホスト B 宛にパケットを送るとします。ルータ A は受け取ったパケットを転送する必要がありますが、宛先であるホスト B はルータ A と直接はつながっていません。そのため、ルータ A はルータ B にパケットを転送し、ルータ B がそのパケットをホスト B へと転送します。

次にどのルータに転送するかは、宛先毎に決める必要があります。例えばホスト A から、今度はホスト C へパケットを送る場合には、ルータ A はそのパケットをルータ C へと転送します。

このような動作を行うために、各ルータは、宛先と次転送先の関係を記述したテーブルを持っています。このテーブルを、経路表 @<fn>{routingtable} と呼びます。例えば、ルータ A の経路表は、@<table>{rtable_a} のようになっています。

//table[rtable_a][ルータ A の経路表]{
宛先			次転送先
ホスト B		ルータ B
ホスト C		ルータ C
//}

//footnote[routingtable][ルーティングテーブルと呼ぶこともあります。]

=== 経路表の詳細

経路表は、ルータの転送動作にとって欠かせない存在です。ルータの動作を理解するために、もう少し詳しく見ていきましょう。

@<table>{rtable_a} では、次転送先を宛先のホスト毎に決めていました。しかし、これでは、ホストが多いネットワークでは、経路表のエントリ数が膨大になってしまいます。そのため、実際の経路表では、同じ L2 ネットワーク上にあるホストをまとめてひとつの宛先として指定します。この時、宛先として使用するのがネットワークアドレスとネットマスク長です。

//image[network][同じ L2 ネットワーク上にあるホストを一つの宛先としてまとめる]

例えば、@<img>{network} では、ルータ B の右側のネットワークは、ネットワークアドレスが 192.168.1.0、ネットマスク長が 24 であるとします。このネットワーク中にあるホストには、ネットワークアドレスと上位 24 ビットが同じとなるように IP アドレスを割り当てます。例えば、ホスト X の IP アドレスは 192.168.1.1 ですが、ネットワークアドレス 192.168.1.0 と上位 24 ビット分が同じであることがわかります。ホスト Y, Z についても同様です。

このようにアドレスを振ることで、ルータ A の経路表は、@<img>{network} のようにシンプルに書くことができます。この時、例えばホスト X 宛のパケットを受け取ったルータ A は、次のように動作します。ルータ A の経路表には宛先 192.168.1.0/24 というエントリがあります。パケットは、宛先が 192.168.1.1 であるので、上位 24 ビット分を比較すると、このエントリに該当することがわかります。その結果、ルータ A は次にルータ B にパケットを転送すればいいことがわかります。ホスト Y, Z 宛も同様な処理できるため、このエントリ一つで複数の宛先をカバーできます。

#@warn(デフォルトルートの話を書く)

=== オンリンク

@<img>{router_network} では、ルータが宛先ホストに直接接続していない場合について説明しましたが、その判断はどのように行なっているのでしょうか？

ルータは、その判断のために、自身のインターフェイスに割り当てられた IP アドレスを使います。そのため、ルータのインターフェイスには、ネットワーク中のホストとネットワークアドレスが同じになるように、IP アドレスを割り当てる必要があります。例えば、@<img>{router_address} で、ルータ B のインターフェイスには、接続するネットワーク中の各ホストの IP アドレスと同じネットワークアドレス 192.168.1.0/24 になるよう、アドレス 192.168.1.254 を割り当てています。

//image[router_address][ルータのインターフェイスには、ネットワーク内のホストとネットワークアドレスが同じとなるよう、IP アドレスを割り当てる]

@<img>{onlink} のネットワーク上でのホスト X 宛のパケットについて考えてみます。

//image[onlink][ルータは、インターフェイスに割り当てられているアドレスを見て、宛先ホストが直接接続しているかの判断する]

#@warn(オンリンクの判断を書く)

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
//}

== まとめ/参考文献
