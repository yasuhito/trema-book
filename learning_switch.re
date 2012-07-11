= ラーニングスイッチ

今回は様々なコントローラの基本となるラーニングスイッチ機能を OpenFlowで実現します。ラーニングスイッチの特徴は、パケットを届けるためにホストの MAC アドレスを使うところです。MAC アドレスは OSI 参照モデルのレイヤ 2 に当たるため、レイヤ 2 スイッチとも呼ばれます。ラーニングスイッチをコントローラとして実装できれば、これをベースにさまざまな機能を持ったコントローラに拡張することができます。

#@warn(それぞれのステップに図を入れる)

ラーニングスイッチは次のように動作します。

 1. パケットが到着すると、スイッチはパケットの送信元の MAC アドレスと、
 宛先の MAC アドレス、およびパケットの入ってきたスイッチのポート番号を
 調べる。
 2. 送信元の MAC アドレスが初めての物の場合、スイッチはパケットが上がっ
 てきたスイッチポートと MAC アドレスの組をフォワーディングデータベース
 (FDB) で学習する。
 3. パケットの宛先 MAC アドレスがすでに FDB にあった場合、宛先ホストが
 スイッチのどのポートにつながっているかが FDB よりわかる。この場合、そ
 のポートにパケットを届ける
 4. パケットの宛先 MAC アドレスが FDB でみつからなかった場合、パケット
 が入ってきたポート以外のすべてのポートにパケットをばらまく。

このように、ラーニングスイッチでは FDB にホストの MAC アドレスとスイッチのポート番号の組を学習しつつ、これを使ってパケットを送り届けるわけです。

== LearningSwitch コントローラ

ラーニングスイッチは大きく分けるとラーニングスイッチ本体と FDB の 2 つの部品からなります。まずは今までどおり、ざっとそれぞれのソースコード(@<list>{learning-switch.rb}、@<list>{fdb.rb}) を眺めてみましょう。このソースコードは Trema のサンプルアプリに付属する @<tt>{learning_switch/learning-switch.rb, fdb.rb} でも読むことができます。

//list[learning-switch.rb][ラーニングスイッチ (@<tt>{learning-switch.rb}) のソースコード]{
require "fdb"


class LearningSwitch < Controller
  add_timer_event :age_fdb, 5, :periodic


  def start
    @fdb = FDB.new
  end


  def packet_in datapath_id, message
    @fdb.learn message.macsa, message.in_port
    port_no = @fdb.port_no_of( message.macda )
    if port_no
      flow_mod datapath_id, message, port_no
      packet_out datapath_id, message, port_no
    else
      flood datapath_id, message
    end
  end


  def age_fdb
    @fdb.age
  end


  ##############################################################################
  private
  ##############################################################################


  def flow_mod datapath_id, message, port_no
    send_flow_mod_add(
      datapath_id,
      :match => ExactMatch.from( message ),
      :actions => ActionOutput.new( :port => port_no )
    )
  end


  def packet_out datapath_id, message, port_no
    send_packet_out(
      datapath_id,
      :packet_in => message,
      :actions => ActionOutput.new( :port => port_no )
    )
  end


  def flood datapath_id, message
    packet_out datapath_id, message, OFPP_FLOOD
  end
end
//}

//list[fdb.rb][フォワーディングデータベース (@<tt>{fdb.rb}) のソースコード]{
class ForwardingEntry
  attr_reader :mac
  attr_reader :port_no
  attr_reader :dpid
  attr_writer :age_max


  def initialize mac, port_no, age_max, dpid
    @mac = mac
    @port_no = port_no
    @age_max = age_max
    @dpid = dpid
    @last_update = Time.now
  end


  def update port_no
    @port_no = port_no
    @last_update = Time.now
  end


  def aged_out?
    Time.now - @last_update > @age_max
  end
end


#
# A database that keep pairs of MAC address and port number
#
class FDB
  DEFAULT_AGE_MAX = 300


  def initialize
    @db = {}
  end


  def port_no_of mac
    dest = @db[ mac ]
    if dest
      dest.port_no
    else
      nil
    end
  end


  def lookup mac
    if dest = @db[ mac ]
      [ dest.dpid, dest.port_no ]
    else
      nil
    end
  end


  def learn mac, port_no, dpid = nil
    entry = @db[ mac ]
    if entry
      entry.update port_no
    else
      new_entry = ForwardingEntry.new( mac, port_no, DEFAULT_AGE_MAX, dpid )
      @db[ new_entry.mac ] = new_entry
    end
  end


  def age
    @db.delete_if do | mac, entry |
      entry.aged_out?
    end
  end
end
//}

ラーニングスイッチの本体は @<tt>{LearningSwitch} という名前のクラスです。そして MAC アドレスを管理するのが @<tt>{FDB} クラスです。

=== 実行してみよう

それでは早速実行してみましょう。今回も仮想ネットワークを使って、仮想スイッチ 1 台、仮想ホスト 2 台の構成でラーニングスイッチを起動してみます。次の内容の設定ファイルを @<tt>{learning-switch.conf} として保存してください。

//emlist{
vswitch("lsw") {
  datapath_id "0xabc"
}

vhost ("host1") {
  ip "192.168.0.1"
  netmask "255.255.0.0"
  mac "00:00:00:01:00:01"
}

vhost ("host2") {
  ip "192.168.0.2"
  netmask "255.255.0.0"
  mac "00:00:00:01:00:02"
}

link "lsw", "host1"
link "lsw", "host2"
//}

次のように @<tt>{trema run} の "@<tt>{-c}" オプションにこの設定ファイルを渡してラーニングスイッチを実行します。ラーニングスイッチ自体の出力はありません。

//cmd{
% ./trema run ./learning-switch.rb -c ./learning-switch.conf
//}

別ターミナルを開き、@<tt>{trema send_packets} コマンドを使って host1 と host2 の間で適当にテストパケットを送ってみます。

//cmd{
% ./trema send_packets --source host1 --source host2
% ./trema send_packets --source host2 --source host1
//}

@<tt>{trema show_stats} コマンドで host1 と host2 の受信パケット数をチェックし、それぞれでパケットが受信されていれば成功です。

== LearningSwitch コントローラの仕組み
=== 未知のパケット (packet-in) の処理
=== フローテーブルの書き換え
=== フォワーディングデータベース (FDB)

== ソースコード
=== フォワーディングデータベースクラス
=== ラーニングスイッチクラス
== 実行してみよう
== まとめ/参考文献
