= シンプルなルータ (後編)

//lead{
前編で省略した、ルーティングテーブルの詳細を見ていきましょう。ルータは実に巧妙な仕組みで転送先の情報を管理します。
//}

== ルーティングテーブルの宛先ホストをまとめる

ルータが管理するルーティングテーブルは、宛先ホストが増えるごとに大きくなります。前編の説明では、ルータは宛先ホスト1つごとにルーティングテーブルのエントリを管理していました。しかしこれでは、たとえばインターネットにホストが加わるごとに、インターネット上のルータはルーティングテーブルを更新する必要があります。しかも、インターネット上のホスト数は直線的に増え続け、2012年現在では10億台を超えると言われています。そうなると、宛先ホストごとにエントリを管理する方法は非現実的です。

これを解決するために、ルータは同じイーサネット上にあるホストをひとつのグループとしてまとめます。そして、ルーティングテーブルの宛先として、ホストではなくこのグループを指定することで、エントリ数を圧縮します。この時、グループ情報として使うのがネットワークアドレスとネットマスク長です。

//image[router_network][同じイーサネット上にあるホストを、一つの宛先としてまとめる]

宛先ホストのグループ化は次のように行います。たとえば、@<img>{router_network}の右側のネットワークは、ネットワークアドレスが192.168.1.0でネットマスク長が24です(これを192.168.1.0/24と表現します)。このネットワーク中にあるホストX,Y,Zには、ネットワークアドレスと上位24ビットが同じとなるように、つまりIPアドレスが192.168.1で始まるようにします。こうすれば、ホストX,Y,Zは同じ1つのグループ192.168.1.0/24に属するとみなせます。

このようにアドレスを振ることで、ルータAのルーティングテーブルは、@<img>{router_network}のようにシンプルに書けます。ホストX,Y,Z宛てのパケットを192.168.1.0/24グループ宛てと表現することで、エントリをひとつにまとめられるのです。

この時、ホストX(192.168.1.1)宛のパケットを受け取ったルータAは次のように動作します。ルーティングテーブルのエントリ192.168.1.0/24と、宛先の192.168.1.1の上位24ビットを比較すると一致します。そこで、ルーティングテーブルから次の転送先はルータBだとわかります。ホストY,Z宛も同様に処理できるので、このエントリ一つでホスト3台分の宛先をカバーできます。

== ネットワーク宛てのエントリをさらにまとめる

複数のホスト宛てエントリをまとめて出来たエントリは、さらにまとめられる場合もあります。

//image[aggregate][複数のネットワークへのルーティング情報をまとめる]

例として、@<img>{aggregate}の3つのネットワークに接続するルータBを考えてみましょう。これら3つのネットワークアドレスは、上位16ビットが172.16.0.0で共通です。ここでルータAから見ると、この三つのネットワークへの次の転送先は、いずれもルータBです。そのため、これら3つのネットワークへのルーティング情報は、172.16.0.0/16宛として1つにまとめられます。

=== 1つの宛先に複数のエントリが該当する場合

パケットの宛先IPアドレスに複数のエントリが該当する場合はどうなるでしょうか？@<img>{longest_match}のルータAがホストX(172.16.3.1)にパケットを送る場合について考えてみましょう。ルータAのルーティングテーブルは図のように2つのエントリを持ち、どちらもパケットの宛先172.16.3.1とマッチします。正しい次の転送先はどちらでしょうか？

//image[longest_match][マスク長が一番長いエントリを選択する]

このように、複数のエントリにマッチする場合には、ネットマスク長が一番長いエントリを選択するというルールがあります。これをロンゲストマッチと呼びます。ロンゲストマッチにより、ルータAは、ホストX宛のパケットをルータCへと転送し、その結果ホストXへとパケットが届きます。

=== デフォルトルート

ロンゲストマッチがあるお陰で、経路をまとめやすくなっています。究極的には、すべての経路をまとめてしまうこともできます。

@<img>{default_route} のように、インターネットにつながっている場合のルータ A のルーティングテーブルについて考えてみましょう。

//image[default_route][どの経路にもマッチしなかった場合、デフォルトルートを使う]

(★頑張って書く)

=== 宛先ホストがルータと直接つながっているかを調べる

@<img>{router_network} では、ルータが宛先ホストに直接接続していない場合について説明しましたが、その判断はどのように行なっているのでしょうか？

ルータは、その判断のために、自身のインターフェイスに割り当てられた IP アドレスを使います。そのため、ルータのインターフェイスには、ネットワーク中のホストとネットワークアドレスが同じになるように、IP アドレスを割り当てる必要があります。例えば、@<img>{router_address} で、ルータ B のインターフェイスには、接続するネットワーク中の各ホストの IP アドレスと同じネットワークアドレス 192.168.1.0/24 になるよう、アドレス 192.168.1.254 を割り当てています。

//image[router_address][ルータのインターフェイスには、ネットワーク内のホストとネットワークアドレスが同じとなるよう、IP アドレスを割り当てる]

@<img>{onlink} のネットワーク上でのホスト X 宛のパケットについて考えてみます。

//image[onlink][ルータは、インターフェイスに割り当てられているアドレスを見て、宛先ホストが直接接続しているかの判断する]

== ソースコード

=== パケットを書き換えて転送する(再)

パケットを書き換えて、転送を行う @<tt>{forward} メソッドを、もう一度見ていきましょう。@<chap>{router_part1} でこのメソッドが行う 5 つの処理について説明しましたが、そのうち、次転送先の決定と、出力インターフェイスの決定について見ていきます。

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

宛先アドレス (@<tt>{message.ipv4_daddr} に対する次転送先の決定は、@<tt>{resolve_next_hop} メソッドで行なっています。このメソッドは、以下のように定義されています。

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

このメソッドでは、まずルーティングテーブル (@<tt>{@routing_table}) を @<tt>{lookup} メソッドで検索します。このときルーティングテーブル中で見つかった次転送先を、戻り値として返します。もし、見つからなかった場合は、宛先アドレス (@<tt>{daddr}) をそのまま返します。これは、@<img>{router_address} で説明したように、宛先ホストとルータが直接接続している可能性があるからです。

//emlist{
    interface = @interfaces.find_by_prefix( next_hop )
    if not interface or interface.port == message.in_port
      return
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

それでは、次にルータをまたいだ二つのホスト間で通信できることを確認してみましょう。ホスト 2 の IP アドレスである 192.168.2.2 に対して、ホスト 1 から @<tt>{ping} を送ります。

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
