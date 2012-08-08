= ラーニングスイッチ

データセンターを動かす大規模ネットワークも、実は基本的な部品の組み合わせです。その複雑な構造をほぐしてバラバラにしてみると、たくさんのスイッチとサーバ、そしてそれらをつなぐネットワークケーブルというごくごく単純な要素に分解できます。

ところでこのスイッチ、OpenFlow の世界ではよくラーニングスイッチという名前で呼ばれます。なぜ「ラーニング (学習)」と呼ぶのでしょうか？それは、スイッチがパケットに含まれる情報を学習し、その情報をうまく使うことでパケットを効率的に転送するからです。

高機能なハードウェアスイッチも、基本的な機能はこのラーニングスイッチとまったく変わりません。OpenFlow で実装したラーニングスイッチに機能を追加していけば、高機能なハードウェアスイッチとほぼ同等なものをソフトウェアで実現できます。そして、これをたくさん組み合わせていけばデータセンターの大規模ネットワークも作ることができます。

この章では、このネットワークの基本部品であるラーニングスイッチを Trema で実装してみましょう。まずは一般的なハードウェアスイッチの動作原理を理解し、次にこれを OpenFlow で実装する方法を学びます。

== スイッチの仕組み

簡単なネットワークを例にしてスイッチの動作を説明します。@<img>{switch_network} のネットワークを考えてください。

//image[switch_network][スイッチ一台とホスト 2 台からなるネットワーク][scale=0.5]

スイッチのポート 1 番と 4 番に、ホスト 1 とホスト 2 がそれぞれ接続されています。また、それぞれのホストのネットワークカードには MAC アドレスが割り振られています。ここでホスト 1 とホスト 2 がお互いに通信しようとしたとき、スイッチは次のように動作します。

//indepimage[host1to2][][scale=0.45]

まずホスト 1 がホスト 2 へパケットを送信すると、そのパケットはスイッチのポート 1 番に到着します。パケットが到着すると、スイッチはパケットの送信元 MAC アドレスとパケットの入ってきたポートの番号を調べ、「ポート 1 番には MAC アドレスが 00:11:11:11:11:11 のホストがつながっている」と学習します。この情報は、以降のパケットの転送に使われます。

//indepimage[host1to2_flood][][scale=0.45]

学習が終わるとスイッチはパケットの宛先 MAC アドレスを調べ、送出先のポート番号を探そうとします。しかし、ホスト 2 の MAC アドレスとポート番号はまだ学習していないので分かりません。そこで、スイッチはパケットをポート 1 以外のすべてのポートにばらまき、その結果ポート 4 につながっているホスト 2 にパケットが届けられます。このようにまだ学習が不十分な段階では、無駄なトラフィックが発生してしまいます。

//indepimage[host2to1][][scale=0.45]

この状態で逆にホスト 2 がホスト 1 へパケットを送信すると、スイッチは「ポート 4 番には MAC アドレスが 00:22:22:22:22:22 のホストがつながっている」と学習します。また、パケットの宛先 MAC アドレスと今までの学習データを照らし合わせ、出力先のポート番号を探します。ここで、最初のステップで「ポート 1 = MAC アドレス 00:11:11:11:11:11」と学習しているので、出力ポートは 1 と決定できます。最初のステップとは違いパケットはポート 1 のみに転送されるので、無駄なトラフィックは発生しません。

ここまでの段階で、スイッチはすべてのホストの MAC アドレスとポート番号を学習したことになります。もし、ホスト 1 がホスト 2 へふたたびパケットを送信すると、今度はパケットはばらまかれるのではなくちゃんとポート 4 のみへ出力されます。このように通信が進むにつれて、ちゃんとパケットの転送も最適化されていきます。

スイッチはこのようにホストの MAC アドレスとポート番号をパケット到着のタイミングで学習しつつ、これを使って賢くパケットを送り届けるわけです。この学習した「MAC アドレス + ポート番号」の組をスイッチがためておくデータベースのことを、専門用語でフォワーディングデータベース (FDB) と呼びます。

ここまでの仕組みがわかれば、スイッチの機能を実現するコントローラ (ラーニングスイッチ) を Trema で実装することは簡単です。ではさっそく実装を見て行きましょう。

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
=== Packet Out でパケットを送り出す

Packet Out は OpenFlow で定義されたメッセージの 1 つで、スイッチの指定したポートからパケットを送信させるためのものです。送信するときにはパケットを書き換えることもできます。よく使われる用途として、Packet In でコントローラにパケットが上がってきたときに Packet Out でこのパケットを書き換えてスイッチのポートから送り出す場合があります。

Trema の Packet Out API は @<tt>{Controller#send_packet_out} メソッドで定義されています。なお @<tt>{Controller} クラスはすべてのコントローラの親クラスなので、コントローラはこの @<tt>{send_packet_out} メソッドをクラス内で直接呼び出すことができます。それでは、API 定義を見ていきましょう。

==== API 定義

@<tt>{send_packet_out} メソッドは次の 2 つの引数を取ります。

//emlist{
send_packet_out( datapath_id, options )
//}

それぞれの引数の意味は次のとおりです。

 * datapath_id: Packet Out の届け先となるスイッチの Datapath ID です。
 * options: Packet Out メッセージの中身を決めるためのオプションで、アクションによるパケットの書き換えや出力するポートの指定が行われます。これは @<tt>{Hash} で定義されていて、必要なオプションのみを指定すればいいことになっています。

具体的な利用例は次のとおりです。

==== パケットを作って出す

任意のパケットを作ってスイッチの特定のポートに出したい場合、次のように @<tt>{:data} オプションにパケットの中身を指定してスイッチの @<tt>{port_number} 番ポートへと出力します。この呼び出しはコントローラのコードのどこからでもできます。

//emlist{
send_packet_out(
  0x1,
  :data => packet_data,
  :actions => ActionOutput.new( port_number )
)
//}

パケットを送り出すときには、ポートへの出力だけでなく Modify-Field タイプのアクションを指定して書き換えることもできます。

==== packet_in ハンドラで使う

@<tt>{packet_in} ハンドラから使う場合、Packet In メッセージとして入ってきたパケットの内容をそのままスイッチのポートから送り出す場合がほとんどです。この場合、パケットの送信にスイッチのバッファを使う場合と、バッファを使わずにコントローラからパケットを送る場合で呼び出しかたが変わります。

===== スイッチのバッファを使って Packet Out する場合

通信量が少なくパケットがスイッチのバッファに乗っていることが期待できる場合には、次のように @<tt>{:buffer_id} オプションを指定してやることでバッファに乗っているパケットデータを ID で指定して Packet Out できます。この場合コントローラからスイッチへのパケットデータのコピーが起こらないため、若干のスピードアップが期待できます。ただし、バッファがすでに消されている場合にはエラーが返ります。

#@warn(エラーハンドリングの説明をどこかでやる)

//emlist{
def packet_in datapath_id, message
  # ...

  send_packet_out(
    0x1,
    :buffer_id => message.buffer_id,
    :data => message.data,
    :actions => ActionOutput.new( port_number )
  )
//}

これは次のように @<tt>{:packet_in} オプションを使って短く書くこともできます。この場合、@<tt>{:packet_in} オプションは @<tt>{:buffer_id}, @<tt>{:data} オプションを @<tt>{PacketIn} オブジェクトを使って自動的にセットします。

//emlist{
def packet_in datapath_id, message
  # ...

  send_packet_out(
    0x1,
    :packet_in => message,
    :actions => ActionOutput.new( port_number )
  )
//}

===== スイッチのバッファを使わずに Packet Out する場合

スイッチのバッファを使わずに Packet Out する場合、次のように @<tt>{:data} オプションを指定する必要があります。バッファに乗っているかいないかにかかわらず Packet Out できるので、若干遅くなりますが安全です。

//emlist{
def packet_in datapath_id, message
  # ...

  send_packet_out(
    0x1,
    :data => message.data,
    :actions => ActionOutput.new( port_number )
  )
//}

==== フローテーブルでパケットを制御する

パケットの出力や書き換えをスイッチのフローテーブルにまかせたい場合には、@<tt>{ActionOutput} の出力先ポート番号として特殊なポート番号である @<tt>{OFPP_TABLE} を指定することができます。この場合、フローテーブルでの検索に使う入力ポートは、@<tt>{:in_port} オプションを使って次のように指定できます。

//emlist{
def packet_in datapath_id, message
  # ...

  send_packet_out(
    0x1,
    :in_port => message.in_port,
    :data => message.data,
    :actions => ActionOutput.new( OFPP_TABLE )
  )
//}

このコードも、@<tt>{:packet_in} オプションを使って次のように短く書けます。@<tt>{:packet_in} オプションは @<tt>{:buffer_id}, @<tt>{:data}, @<tt>{:in_port} オプションを @<tt>{PacketIn} オブジェクトを使って自動的にセットします。

//emlist{
def packet_in datapath_id, message
  # ...

  send_packet_out(
    0x1,
    :packet_in => message,
    :actions => ActionOutput.new( OFPP_TABLE )
  )
//}

==== オプション一覧

次が options に指定できるオプション一覧です。

: @<tt>{:buffer_id}
  スイッチでバッファされているパケットの ID を指定します。この値を使うと、スイッチでバッファされているパケットを指定して Packet Out できるので効率が良くなります。ただし、スイッチにバッファされていない時はエラーになります。この値を @<tt>{0xffffffff} に指定した場合、バッファされているパケットは使われず @<tt>{:data} オプションに指定した値を Packet Out することになります。デフォルトは @<tt>{0xffffffff} です。

: @<tt>{:in_port}
  @<tt>{:actions} オプションに @<tt>{OFPP_TABLE} ポートへのアウトプットが指定されている場合にのみ有効です。指定されている場合、フローテーブルのルックアップにはこの値が使われます。デフォルトは @<tt>{OFPP_NONE} です。

: @<tt>{:data}
  Packet Out するパケットの中身を指定します。もし @<tt>{:buffer_id} オプションが指定されておりスイッチにバッファされたパケットを Packet Out する場合、この値は使われません。@<tt>{:buffer_id} オプションが @<tt>{0xfffffff} のときは、@<tt>{:data} オプションに指定された値を使うので指定する必要があります。デフォルトで @<tt>{nil} です。

: @<tt>{:packet_in}
  @<tt>{:in_port}, @<tt>{:buffer_id}, @<tt>{:data} オプションを指定するためのショートカットです。@<tt>{packet_in} ハンドラの引数として渡される @<tt>{PacketIn} メッセージを指定します。

: @<tt>{:actions}
  Packet Out のときに実行したいアクションの配列を指定します。アクションが一つの場合は配列でなくてかまいません。

== 実行してみよう
== まとめ/参考文献
