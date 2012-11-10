= ルータ (前編)

//lead{
執筆中です
//}


@<chap>{learning_switch} と @<chap>{traffic_monitor} では、L2 スイッチ機能を OpenFlow を使って実現する方法について見てきました。L2 スイッチにおける、パケットの MAC アドレスを見て転送するという動作の実現を行いました。しかし、@<chap>{openflow} で説明したように、OpenFlow でマッチングルールとして扱えるのはレイヤー 2 の情報だけではありません。

本章では、レイヤー 3 の情報を使うルータを OpenFlow で実現する方法を説明します。

== ルータとは

ルータと L2 スイッチは何が違うのでしょうか？パケットを転送する際に見るところが違うだけでしょうか？もちろん、パケットを転送する際に L2 スイッチは宛先 MAC アドレスを、ルータは宛先 IP アドレスを参照しており、それぞれ異なっています。それでは、なぜわざわざ異なるフィールドを参照する必要があるのでしょうか？その理由について、考えてみましょう。

=== L2 ネットワーク同士を接続する

OpenFlow の世界では、L2 といえば、Ethernet です。@<chap>{openflow} で説明したマッチングルールには、MAC アドレスや VLAN ID など、Ethernet のヘッダ中に含まれる情報ゐ使っています。この Ethernet は、現在最もメジャーな L2 技術といっても間違いではないでしょう。データセンター内で、各サーバ間をつなぐネットワークから、家庭内ネットワークまで Ethernet は、いろいろなところで使われています。

Ethernet の他に L2 技術はないのでしょうか？そんなことはありません。身近なところでは、PPP  (Point to Point Protocol) があります。以前 Internet を利用する際のダイアルアップ接続では、この PPP が用いられていました。また現在では ADSL や光ファイバーによるインターネット接続サービスにて PPP が用いられています。他にも、徐々に利用が減りつつありますが、ATM (Asynchronous Transfer Mode) やフレームリレーも L2 技術の一種です。今ほど Ethernet 技術が普及する前には、大学の計算機室のワークステーション間接続には FDDI (Fiber-Distributed Data Interface) という L2 技術が広く使われていました。

異なる L2 技術を採用したホスト間で通信を行うためには、一工夫が必要です。まず、L2 技術が異なれば、通信を行う相手を指定する方法に違いがあるためです。そこで、必要となるのが、L2 技術が異なっていても通信ができるよう共通言語としての L3 技術、Internet Protocol です。この IP を用いて、異なる L2 ネットワーク間の中継を行うのが、ルータの役割です。

=== Ethernet が普及したらルータはいらなくなる？

様々な L2 技術があることを紹介しましたが、OpenFlow で扱うことが出来るのは、そのうちでも Ethernet だけです。L2 技術がすべて Ethernet になれば、ルータはいらなくなるのでしょうか？ルータは、Ethernet でできた複数の L2 ネットワーク同士をつなぐ用途でも実際に用いられています。

しかし Ethernet だけで大きなネットワークを作るのには、以下のような問題があります。

 * ブロードキャストの問題
 * セキュリティ上の問題

Ethernet だけでネットワークを作る場合にも、いくつかの比較的小さな L2 ネットワークを作り、それらをルータでつなぐといった構成は、一般的に用いられています。

== ルータの動作

ルータの動作について見ていきましょう。

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

=== 送り先を決める

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
    # ルータ宛てのパケットかを調べる
    return if not @interfaces.ours?( message.in_port, message.macda )

    # メッセージの種別ごとに処理を振り分け
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

@<tt>{packet_in} は、以下二つの役割を果します。

 1. 受信パケットを処理すべきかを判断
 2. メッセージ種別を判別し、適切な処理を実行

受信パケットを自身が処理すべきどうかの判断は、@<tt>{Interface} クラスの @<tt>{ours?} メソッドで行います。このメソッドは、宛先 MAC アドレス (@<tt>{macda}) がブロードキャストであるか、もしくは受信ポート (@<tt>{port}) に割り当てられている MAC アドレスと同じである場合、自身が処理すべきとして、@<tt>{true} を返します。

//emlist{
  def ours? port, macda
    return true if macda.broadcast?

    interface = find_by_port( port )
    if not interface.nil? and interface.has?( macda )
      return true
    end
  end
//}

メッセージ種別の判別には、Trema の @<tt>{PacketIn} クラスが持つメソッドを使います。

: @<tt>{arp_request?}
  受信パケットが ARP リクエストの場合、true を返します。
: @<tt>{arp_reply?}
  受信パケットが ARP リプライの場合、true を返します。    
: @<tt>{ipv4?}
  受信パケットが IPv4 パケットの場合、true を返します。

== まとめ/参考文献
