= ルータ (後編)

//lead{
執筆中です
//}

== ソースコード

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

=== ARP テーブルの実装

//list[arptable.rb][ARP テーブル (@<tt>{arptable.rb}) のソースコード]{
class ARPEntry
  include Trema::Logger


  attr_reader :port
  attr_reader :hwaddr
  attr_writer :age_max


  def initialize port, hwaddr, age_max
    @port = port
    @hwaddr = hwaddr
    @age_max = age_max
    @last_updated = Time.now
    info "New entry: MAC addr = #{ @hwaddr.to_s }, port = #{ @port }"
  end


  def update port, hwaddr
    @port = port
    @hwaddr = hwaddr
    @last_updated = Time.now
    info "Update entry: MAC addr = #{ @hwaddr.to_s }, port = #{ @port }"
  end


  def aged_out?
    aged_out = Time.now - @last_updated > @age_max
    info "Age out: An ARP entry (MAC address = #{ @hwaddr.to_s }, port number = #{ @port }) has been aged-out" if aged_out
    aged_out
  end
end


class ARPTable
  DEFAULT_AGE_MAX = 300


  def initialize
    @db = {}
  end


  def update port, ipaddr, hwaddr
    entry = @db[ ipaddr.to_i ]
    if entry
      entry.update( port, hwaddr )
    else
      new_entry = ARPEntry.new( port, hwaddr, DEFAULT_AGE_MAX )
      @db[ ipaddr.to_i ] = new_entry
    end
  end


  def lookup ipaddr
    @db[ ipaddr.to_i ]
  end


  def age
    @db.delete_if do | ipaddr, entry |
      entry.aged_out?
    end
  end
end
//}

=== Interface クラス

//list[interface.rb][Interface クラス (@<tt>{interface.rb}) のソースコード]{
require "arptable"
require "routing-table"


class Interface
  attr_reader :hwaddr
  attr_reader :ipaddr
  attr_reader :prefixlen
  attr_reader :port


  def initialize options
    @port = options[ :port ]
    @hwaddr = Mac.new( options[ :hwaddr ] )
    @ipaddr = IPAddr.new( options[ :ipaddr ] )
    @prefixlen = options[ :prefixlen ]
  end

  
  def has? mac
    mac == hwaddr
  end


  def forward_action macda
    [
      ActionSetDlSrc.new( hwaddr.to_s ),
      ActionSetDlDst.new( macda.to_s ),
      ActionOutput.new( port )
    ]
  end
end


class Interfaces
  def initialize interfaces = []
    @list = []
    interfaces.each do | each |
      @list << Interface.new( each )
    end
  end

  
  def find_by_port port
    @list.find do | each |
      each.port == port
    end
  end


  def find_by_ipaddr ipaddr
    @list.find do | each |
      each.ipaddr == ipaddr
    end
  end


  def find_by_prefix ipaddr
    @list.find do | each |
      prefixlen = each.prefixlen
      each.ipaddr.mask( prefixlen ) == ipaddr.mask( prefixlen )
    end
  end


  def find_by_port_and_ipaddr port, ipaddr
    @list.find do | each |
      each.port == port and each.ipaddr == ipaddr
    end
  end


  def ours? port, macda
    return true if macda.broadcast?

    interface = find_by_port( port )
    if not interface.nil? and interface.has?( macda )
      return true
    end
  end
end
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
