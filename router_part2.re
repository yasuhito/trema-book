= シンプルなルータ (後編)

//lead{
執筆中です
//}

== ルータの動作 (続き)

経路表は、ルータの転送動作にとって欠かせない存在です。ルータの動作を理解するために、もう少し詳しく見ていきましょう。

=== 複数の宛先ホストをまとめた経路を作る

前章では、次転送先を宛先のホスト毎に決めていました。しかし、これでは、ホストが多いネットワークでは、経路表のエントリ数が膨大になってしまいます。そのため、実際の経路表では、同じ L2 ネットワーク上にあるホストをまとめてひとつの宛先として指定します。この時、宛先として使用するのがネットワークアドレスとネットマスク長です。

//image[router_network][同じ L2 ネットワーク上にあるホストを一つの宛先としてまとめる]

例えば、@<img>{router_network} では、ルータ B の右側のネットワークは、ネットワークアドレスが 192.168.1.0、ネットマスク長が 24 であるとします。このネットワーク中にあるホストには、ネットワークアドレスと上位 24 ビットが同じとなるように IP アドレスを割り当てます。例えば、ホスト X の IP アドレスは 192.168.1.1 ですが、ネットワークアドレス 192.168.1.0 と上位 24 ビット分が同じであることがわかります。ホスト Y, Z についても同様です。

このようにアドレスを振ることで、ルータ A の経路表は、@<img>{router_network} のようにシンプルに書くことができます。この時、例えばホスト X 宛のパケットを受け取ったルータ A は、次のように動作します。ルータ A の経路表には宛先 192.168.1.0/24 というエントリがあります。パケットは、宛先が 192.168.1.1 であるので、上位 24 ビット分を比較すると、このエントリに該当することがわかります。その結果、ルータ A は次にルータ B にパケットを転送すればいいことがわかります。ホスト Y, Z 宛も同様な処理できるため、このエントリ一つで複数の宛先をカバーできます。

=== さらに経路をまとめる

@<img>{router_network} ではホスト毎の経路をまとめましたが、複数のネットワークの経路についても同様にまとめることができます。

//image[aggregate][複数のネットワークへの経路をまとめる]

例えば @<img>{aggregate} のようにルータ B には、172.16.0.0/24, 172.16.1.0/24, 172.16.2.0/24 の三つのネットワークが接続されていたとします。このときルータ A から見ると、この三つのネットワークへの次転送先は、いずれもルータ B です。またこの三つのネットワークのアドレスは、上位 16 ビットの部分が共通です。そのため、これらのネットワークの経路は、172.16.0.0/16 宛という具合に一つにまとめて、ルータ A に登録することができます。

=== 同じ宛先に対して複数の経路がある場合

次に @<img>{longest_match} のルータ A がホスト X にパケットを送る場合について考えてみましょう。この例では 172.16.3.0/24 のネットワークは、他の三つのネットワークと別のルータ C に接続しています。この時、三つのネットワークを 172.16.0.0/16 にまとめると、ルータ A の経路は図のように 2 つになります。ホスト X のアドレスは、172

//image[longest_match][マスク長が一番長い経路を選択する]

このように、複数の経路にマッチする場合には、ネットマスク長が一番長い経路を選択するというルールになっています。このルールのことを、ロンゲストマッチと呼びます。

ロンゲストマッチにより、@<img>{longest_match} の例では、ルータ A は、ホスト X 宛のパケットをルータ C へと転送します。その結果、無事にホスト X までパケットが届きます。

=== デフォルトルート

ロンゲストマッチがあるお陰で、経路をまとめやすくなっています。究極的には、すべての経路をまとめてしまうこともできます。例えば、@<img>{default_route} のように、インターネットにつながっている場合について考えてみましょう。

//image[default_route][どの経路にもマッチしなかった場合、デフォルトルートを使う]

=== 宛先ホストがルータと直接つながっているかを調べる

@<img>{router_network} では、ルータが宛先ホストに直接接続していない場合について説明しましたが、その判断はどのように行なっているのでしょうか？

ルータは、その判断のために、自身のインターフェイスに割り当てられた IP アドレスを使います。そのため、ルータのインターフェイスには、ネットワーク中のホストとネットワークアドレスが同じになるように、IP アドレスを割り当てる必要があります。例えば、@<img>{router_address} で、ルータ B のインターフェイスには、接続するネットワーク中の各ホストの IP アドレスと同じネットワークアドレス 192.168.1.0/24 になるよう、アドレス 192.168.1.254 を割り当てています。

//image[router_address][ルータのインターフェイスには、ネットワーク内のホストとネットワークアドレスが同じとなるよう、IP アドレスを割り当てる]

@<img>{onlink} のネットワーク上でのホスト X 宛のパケットについて考えてみます。

//image[onlink][ルータは、インターフェイスに割り当てられているアドレスを見て、宛先ホストが直接接続しているかの判断する]

== ソースコード

=== 転送部(再録)

//emlist{
  def forward( dpid, message )
    next_hop = resolve_next_hop( message.ipv4_daddr )

    interface = @interfaces.find_by_prefix( next_hop )
    if not interface or interface.port == message.in_port
      return
    end

    arp_entry = @arp_table.lookup( next_hop )
    if arp_entry
      action = create_action_from( interface.hwaddr, arp_entry.hwaddr, interface.port )
      flow_mod dpid, message, action
      packet_out dpid, message.data, action
    else
      handle_unresolved_packet dpid, message, interface, next_hop
    end
  end
//}



//emlist{
  def resolve_next_hop( daddr )
    next_hop = @routing_table.lookup( daddr.value )
    if next_hop
      next_hop
    else
      daddr.value
    end
  end
//}

=== ルーティングテーブルの実装

//list[routing-table.rb][ルーティングテーブルのソースコード]{
require "ipaddr"


class RoutingTable
  ADDR_LEN = 32


  def initialize route = []
    @db = Array.new( ADDR_LEN + 1 ) { Hash.new }
    route.each do | each |
      add( each )
    end
  end


  def add options
    dest = IPAddr.new( options[ :destination ] )
    prefixlen = options[ :prefixlen ]
    prefix = dest.mask( prefixlen )
    @db[ prefixlen ][ prefix.to_i ] = IPAddr.new( options[ :gateway ] )
  end


  def delete options
    dest = IPAddr.new( options[ :destination ] )
    prefixlen = options[ :prefixlen ]
    prefix = dest.mask( prefixlen )
    @db[ prefixlen ].delete( prefix.to_i )
  end


  def lookup dest
    ( 0..ADDR_LEN ).reverse_each do | prefixlen |
      prefix = dest.mask( prefixlen )
      entry = @db[ prefixlen ][ prefix.to_i ]
      return entry if entry
    end
    nil
  end
end
//}

=== コンフィグ

//emlist{
$interface = [
  { 
    :port => 3, 
    :hwaddr => "54:00:00:01:01:01",
    :ipaddr => "192.168.11.1",
    :prefixlen => 24
  }, 
  {
    :port => 2,
    :hwaddr => "54:00:00:02:02:02",
    :ipaddr => "192.168.12.1",
    :prefixlen => 24
  },
  { 
    :port => 1, 
    :hwaddr => "54:00:00:04:04:04",
    :ipaddr => "192.168.13.1",
    :prefixlen => 24
  } 
]

$route = [
  {
    :destination => "192.168.14.0", 
    :prefixlen => 24, 
    :gateway => "192.168.13.2" 
  }
]
//}


== 実行してみよう

=== 準備と実行

それでは、ルータを起動してみましょう。パケットの送受信を行うために、今回は仮想ホストではなく、実際のホストを使用します。ルータとして動作させるホストも含めて、合計で 3 台のホストを用意して、@<img>{network} のように接続してください。

//image[network][ルータを動作させる構成]

次に、ホスト 1 にログインし、以下のように IP アドレスとデフォルトルートを設定してください。IP アドレスは 192.168.1.2/24、デフォルトルートは 192.168.1.1 になります。

//cmd{
% sudo ifconfig eth0 192.168.1.2/24
% sudo route add -net default gw 192.168.1.1
//}

ホスト 2 についても同様です。こちらは、IP アドレスを 192.168.2.2/24 に、デフォルトルートを 192.168.2.1 に設定します。

//cmd{
% sudo ifconfig eth0 192.168.2.2/24
% sudo route add -net default gw 192.168.2.1
//}

ルータ用のホスト上では、仮想ネットワーク機能を使い、仮想スイッチを動作させます。次の内容の設定ファイルを @<tt>{simple-router.conf} として用意します。

//emlist{
vswitch("sw") {
  datapath_id "0x1"
}

link "sw", "eth0"
link "sw", "eth1"
//}

次のようにして、この設定ファイルを使い、今回作成した @<tt>{simple-router.rb} を実行します。

//cmd{
% trema run ./simple-router.rb -c ./simple-router.conf
//}

=== ping を送る

それではシンプルルータが正しく動作しているかを順に確認して行きましょう。まずは、ルータが @<tt>{ping} に応答するかを確認してみましょう。ホスト 1 にログインし、ルータの IP アドレス 192.168.1.1 に @<tt>{ping} を打ってみます。

//cmd{
% ping 192.168.1.1
PING 192.168.1.1 (192.168.1.1): 56 data bytes
Request timeout for icmp_seq 0
Request timeout for icmp_seq 1
64 bytes from 192.168.1.1: icmp_seq=2 ttl=64 time=1.806 ms
64 bytes from 192.168.1.1: icmp_seq=3 ttl=64 time=3.629 ms
64 bytes from 192.168.1.1: icmp_seq=4 ttl=64 time=1.475 ms
...
//}

はじめの何回かは、@<tt>{ping} の応答が帰って来ません。実際のルータでは、ARP 未解決のパケットを受信した時に、そのパケットをキューに入れておき、ARP が解決した後転送を行います。しかし、今回作ったシンプルルータの実装では、このキューを実装していないため、このように応答が帰ってこないパケットが発生します。ただし、一度 ARP が解決されてしまえば、後の通信は通常のルータのように行うことができます。

それでは、次にルータをまたいだ二つのホスト間で通信できることを確認してみましょう。ホスト 2 の IP アドレスである 192.168.2.2 に対して、ホスト 1 から <tt>{ping} を送ります。

//cmd{
% ping 192.168.2.2
PING 192.168.2.2 (192.168.2.2): 56 data bytes
64 bytes from 192.168.2.2: icmp_seq=0 ttl=64 time=3.715 ms
64 bytes from 192.168.2.2: icmp_seq=1 ttl=64 time=3.271 ms
64 bytes from 192.168.2.2: icmp_seq=2 ttl=64 time=3.367 ms
64 bytes from 192.168.2.2: icmp_seq=3 ttl=64 time=3.545 ms
64 bytes from 192.168.2.2: icmp_seq=4 ttl=64 time=2.153 ms
^C
--- 192.168.2.2 ping statistics ---
5 packets transmitted, 5 packets received, 0.0% packet loss
round-trip min/avg/max/stddev = 2.153/3.210/3.715/0.550 ms
//}

@<tt>{ping} が通ることが確認できたでしょうか？もちろん、@<tt>{ping} 以外も通りますので、他の通信もいろいろ試してみるとよいでしょう。

== まとめ/参考文献
