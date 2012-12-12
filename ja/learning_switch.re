= すべての基本、ラーニングスイッチ

//lead{
データセンターのような複雑に入り組んだネットワークも、もしケーブルを抜いてバラバラにできたなら、スイッチやサーバなどの意外とシンプルな部品に分解できます。
//}

//indepimage[learn][][width=12cm]

== ラーニングスイッチとは何か？

OpenFlowの世界では、コントローラとしてソフトウェア実装したスイッチをよくラーニングスイッチと呼びます。なぜ、ラーニング(学習)スイッチと呼ぶのでしょうか？それは、スイッチが次のように動くからです。

 1. 学習：ホストから出たパケットに含まれる、ネットワーク上でのホストの位置情報を学習する
 2. 転送：今まで学習してきた位置情報を使って、パケットを宛先のホストまで転送する

この「学習し、転送する」というラーニングスイッチの仕組みは応用が広く効きます。たとえば第III部で紹介するいくつかのデータセンターネットワークも、基本はラーニングスイッチと同じく「学習し、転送する」という動作をします。このように、自宅ネットワークであろうが最新鋭のデータセンターであろうが、その基本的な考えかたは同じなのです。

このおかげで、ラーニングスイッチを作れるようになれば、それを改造することでいろいろなアプリケーションをで作れるようになります。

 * 複数スイッチ対応ラーニングスイッチ(Tremaのサンプルアプリの1つ。@<chap>{openflow_framework_trema}で紹介)
 * トラフィックモニタ(@<chap>{traffic_monitor}で紹介)
 * ルーティングスイッチ(@<chap>{routing_switch}で紹介)
 * スライス機能つきルーティングスイッチ(@<chap>{sliceable_switch}で紹介)

ではさっそく、ネットワークの基本部品であるラーニングスイッチをTremaで実装してみましょう。まずは一般的なハードウェアスイッチの動作原理を理解し、次にOpenFlowでの実現方法を見ていきます。

== スイッチの仕組み

簡単なネットワークを例にしてスイッチの動作を説明します(@<img>{switch_network})。スイッチのポート1番と4番に、ホスト1と2をそれぞれ接続しています。また、それぞれのホストのネットワークカードは図に示したMACアドレスを持つとします。

スイッチはホストから届いたパケットを宛先のホストまで転送するために、スイッチにつながる各ホストの位置情報をためておくデータベースを持っています。これをフォワーディングデータベース(FDB)と呼び、「ホストのMACアドレス+ポート番号」の組を保持します。

//image[switch_network][スイッチ1台とホスト2台からなるネットワークと、スイッチのFDBの内容][width=12cm]

ここでホスト2がホスト1へパケットを送信すると、スイッチは次のようにパケットをホスト1まで転送します(@<img>{host2to1})。

//image[host2to1][FDB の情報からパケットをホスト 1 に届ける][width=12cm]

 1. 届いたパケットの宛先MACアドレス(00:11:11:11:11:11)を見て、このMACアドレスを持つホストがつながるポート番号をFDBから探す
 2. FDBには「MACアドレス00:11:11:11:11:11=ポート1」と学習しているので、ポート1にパケットを出力する

ここまでの仕組みがわかれば、スイッチの機能を実現するコントローラ(ラーニングスイッチ)をOpenFlowで実現するのは簡単です。

== OpenFlow版スイッチ(ラーニングスイッチ)の仕組み

OpenFlowによるスイッチの構成は@<img>{switch_network_openflow}のようになります。一般的なハードウェアスイッチとの違いは次の2つです。

 * FDBをソフトウェアとして実装し、コントローラが管理する
 * パケットの転送は、コントローラがフローテーブルにフローエントリを書き込むことで制御する

//noindent
なお、初期状態でのFDBとフローテーブルの中身はどちらも空です。

//image[switch_network_openflow][OpenFlow によるスイッチ (ラーニングスイッチ) の構成][width=12cm]

=== Packet Inからホストの位置情報を学習

この状態でホスト1がホスト2へパケットを送信すると、コントローラは次のようにホスト1のネットワーク上での位置情報を学習します(@<img>{host1to2_openflow})。

 1. フローテーブルは空なのでパケットはPacket Inとしてコントローラまで上がる
 2. コントローラはPacket Inメッセージからパケットの送信元MACアドレスとパケットの入ってきたポートの番号を調べ、「ポート1番にはMACアドレスが00:11:11:11:11:11のホストがつながっている」とFDBに保存する

//image[host1to2_openflow][Packet Inの送信元MACアドレスとスイッチのポート番号をFDBに学習する][width=12cm]

=== Packet Outでパケットを転送(フラッディング)

学習が終わると次はパケットの転送です。もちろん、パケットの宛先はまだ学習していないので、コントローラは次のようにパケットをばらまくことで宛先まで届けます。このばらまく処理をフラッディングと呼びます(@<img>{host1to2_flood_openflow})。

 1. コントローラはPacket Inメッセージの宛先MACアドレスを調べ、FDBから送出先のポート番号を探す。しかし、ホスト2のMACアドレスとポート番号はまだFDBに入っていないのでわからない
 2. コントローラはPacket Outメッセージ(出力ポート=フラッディング)でパケットをばらまくようにスイッチに指示する。その結果、ポート4につながるホスト2にパケットが届く

//image[host1to2_flood_openflow][パケットの宛先MACアドレスからスイッチのポート番号がFDBにみつからないため、Packet Outメッセージ(出力ポート=フラッディング)でパケットをばらまく][width=12cm]

=== ふたたび学習と転送(Flow ModとPacket Out)

この状態でホスト2がホスト1へパケットを送信すると次のようになります(@<img>{host2to1_openflow})。

 1. フローテーブルが空なためコントローラまでふたたびPacket Inメッセージが上がる
 2. コントローラはこのPacket Inメッセージから「ポート4番にはMACアドレスが00:22:22:22:22:22のホストがつながっている」とFDBに保存する
 3. Packet Inの宛先MACアドレスとFDBを照らし合わせ、出力先のポート番号を探す。ここですでに「ポート1=MACアドレス00:11:11:11:11:11」と学習しているので、出力ポートは1と決定できる
 4. 「ホスト2からホスト1へのパケットはポート1へ出力せよ」というフローエントリをFlow Modメッセージでフローテーブルに書き込む。加えて、Packet Outメッセージ(出力ポート=1)でPacket Inを起こしたパケットをポート1へ出力する

//image[host2to1_openflow][同様にホスト2のMACアドレスとポート番号をFDBに学習し、フローエントリを書き込むとともにパケットをホスト1に届ける][width=12cm]

さて、ここまでの段階でフローテーブルには「ホスト2からホスト1へのパケットはポート1へ出力せよ」というフローエントリが入りました。もし、ホスト2がホスト1へふたたびパケットを送信すると、今度はPacket Inがコントローラまで上がることはなく、スイッチ側だけでパケットを転送します。

残りのホスト1からホスト2へのフローエントリはどうでしょう。すでにFDBはすべてのホストのMACアドレスとポート番号を学習してあります。もし、ふたたびホスト1からホスト2ヘパケットを送信すると、@<img>{host2to1_openflow}と同様にコントローラが「ホスト1からホスト2へのパケットはポート4へ出力せよ」というフローエントリを書き込みます。もちろん、それ以降の通信ではPacket Inはまったく上がらずにすべてスイッチ側だけでパケットを処理します。

ちょっと長くなってしまいましたがOpenFlowによるスイッチの実現方法がわかりました。いよいよTremaでの実装に移ります。

== LearningSwitchコントローラ

まずはラーニングスイッチのソースコード(@<list>{learning-switch.rb})をざっと眺めてみましょう。とくに、@<tt>{private}の行よりも上のパブリックなメソッドに注目してください。

//list[learning-switch.rb][ラーニングスイッチ(@<tt>{learning-switch.rb})のソースコード]{
class LearningSwitch < Controller
  def start
    @fdb = {}
  end


  def packet_in( datapath_id, message )
    @fdb[ message.macsa ] = message.in_port
    port_no = @fdb[ message.macda ]
    if port_no
      flow_mod datapath_id, message, port_no
      packet_out datapath_id, message, port_no
    else
      flood datapath_id, message
    end
  end


  private


  def flow_mod( datapath_id, message, port_no )
    send_flow_mod_add(
      datapath_id,
      :match => ExactMatch.from( message ),
      :actions => SendOutPort.new( port_no )
    )
  end


  def packet_out( datapath_id, message, port_no )
    send_packet_out(
      datapath_id,
      :packet_in => message,
      :actions => SendOutPort.new( port_no )
    )
  end


  def flood( datapath_id, message )
    packet_out datapath_id, message, OFPP_FLOOD
  end
end
//}

今までの知識だけでもこれだけたくさんのことがわかります。

  * ラーニングスイッチの本体は@<tt>{LearningSwitch}という名前のクラス
  * 起動時に呼ばれる@<tt>{start}ハンドラでFDBのインスタンス変数を作っている。@<tt>{{\}}という文法は見慣れないが、これにMACアドレスとポート番号の組を保存するらしい
  * 見慣れない@<tt>{packet_in}メソッドはPacket Inを捕捉するためのハンドラ。スイッチのフローエントリにマッチしないパケットがコントローラに上がってくると、このハンドラが呼ばれる
  * @<tt>{packet_in}ハンドラの中では、パケットの宛先MACアドレスからFDBでポート番号を調べている。もし宛先のポート番号がみつかった場合には、Flow Modでフローエントリを打ち込みPacket Outでパケットを送信している。もしポート番号がみつからなかった場合は、@<tt>{flood}というメソッドを呼んでいる。これは先述した「パケットをばらまく(フラッディング)」の処理

いかがでしょうか。ラーニングスイッチの心臓部は @<tt>{packet_in} ハンドラだけで、その中身もやっていることはなんとなくわかると思います。細かい実装の解説は後回しにして、さっそく実行してみましょう。

=== 実行してみよう

今回も仮想ネットワークを使って、仮想スイッチ1台、仮想ホスト2台の構成でラーニングスイッチを起動してみます。次の内容の設定ファイルを@<tt>{learning-switch.conf}として保存してください。

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

@<tt>{vhost}は仮想ホストの設定です。この設定ファイルでは@<tt>{host1}、@<tt>{host2}という名前の2つの仮想ホストを起動します。また、@<tt>{link}は仮想リンクで、指定した仮想スイッチと仮想ホストを接続できます。ここでは、仮想スイッチ@<tt>{lsw}と、仮想ホスト@<tt>{host1}、@<tt>{host2}をそれぞれ仮想リンクで接続しています。

次のように@<tt>{trema run}の@<tt>{-c}オプションにこの設定ファイルを渡してラーニングスイッチを実行します。ラーニングスイッチ自体の出力はありません。

//cmd{
% trema run ./learning-switch.rb -c ./learning-switch.conf
//}

別ターミナルを開き、@<tt>{trema send_packets}コマンドを使ってhost1とhost2の間でテストパケットを送ってみます。

//cmd{
% trema send_packets --source host1 --dest host2
% trema send_packets --source host2 --dest host1
//}

@<tt>{trema show_stats}コマンドでhost1 と host2の受信パケット数をチェックし、それぞれでパケットを受信していれば成功です。

//cmd{
% trema show_stats host1 --rx
ip_dst,tp_dst,ip_src,tp_src,n_pkts,n_octets
192.168.0.1,1,192.168.0.2,1,1,50
% trema show_stats host2 --rx
ip_dst,tp_dst,ip_src,tp_src,n_pkts,n_octets
192.168.0.2,1,192.168.0.1,1,1,50
//}

ラーニングスイッチの動作イメージがわかったところで、ソースコードの解説に移りましょう。まずはこの章で新しく登場したRubyの構文を紹介します。

== 即席Ruby入門

ラーニングスイッチのソースコード(@<list>{learning-switch.rb})で新しく登場した構文はたった1つだけです。@<tt>{start}ハンドラの中で登場したFDBの本体@<tt>{{\}}がそれですが、カンの良い読者ならこれはハッシュテーブルでは？と気付いているでしょう。

=== ハッシュテーブル

ハッシュテーブルは中カッコで囲まれた辞書です。辞書とは「言葉をその定義に対応させたデータベース」です@<fn>{python_dictionary}。Rubyでは、この対応を"@<tt>{=>}"という矢印で次のように表します。

//footnote[python_dictionary][Pythonの人々はハッシュテーブルのことをズバリ辞書と呼んでいますが、そちらのほうが分かりやすいと筆者は感じます。]

//emlist{
animals = { "armadillo" => "アルマジロ", "boar" => "イノシシ }
//}

たとえば"boar"を日本語で言うと何だろう？と辞書で調べたくなったら、次のようにして辞書を引きます。

//emlist{
animals[ "boar" ] #=> "イノシシ"
//}

この辞書を引くときに使う言葉(この場合は boar)を@<em>{キー}と言います。そして、みつかった定義(この場合はイノシシ)を@<em>{バリュー}と言います。

新しい動物を辞書に加えるのも簡単です。

//emlist{
animals[ "cow" ] = "ウシ"
//}

Rubyのハッシュテーブルはとても高機能なので、文字列だけでなく好きなオブジェクトを格納できます。たとえば、FDBではMACアドレスをキーとして、ポート番号をバリューにします。

//emlist{
fdb[ "00:11:11:11:11:11" ] = 1
//}

もちろん@<tt>{{\}}は空のハッシュテーブルです。FDBは最初は空なので、@<tt>{{\}}に初期化されていました。

//emlist{
def start
  @fdb = {}
end
//}

実は、すでにいろんなところでハッシュテーブルを使ってきました。@<tt>{send_flow_mod_add}などの省略可能なオプションは、矢印(@<tt>{=>})を使っていることからも分かるように実はハッシュテーブルなのです。Ruby では、引数の最後がハッシュテーブルである場合、その中カッコを次のように省略できます。

//emlist{
send_flow_mod_add(
  datapath_id,
  :match => ExactMatch.from( message ),
  :actions => SendOutPort.new( port_no )
)

# これと同じ
send_flow_mod_add(
  datapath_id,
  {
    :match => ExactMatch.from( message ),
    :actions => SendOutPort.new( port_no )
  }
)
//}

== LearningSwitchのソースコード

それではラーニングスイッチのソースコードを読み解いていきましょう。今回の肝となるのは、Packet Inハンドラでの次の処理です。

 * FDBの更新とポート番号の検索
 * ポート番号がみつかった場合の、Flow ModとPacket Out処理
 * ポート番号がみつからなかった場合のフラッディング処理

それでは、最初にPacket Inハンドラの定義方法から見ていきましょう。

=== 未知のパケット(Packet In)の処理

コントローラに上がってくる未知のパケットを拾うには、Packet Inハンドラをコントローラクラスに実装します。典型的なPacket Inハンドラは次のようになります(@<list>{learning-switch.rb}より抜粋)。

//emlist{
class LearningSwitch < Controller
  # ...

  def packet_in( datapath_id, message )
    # ...
  end

  # ...
//}

最初の引数@<tt>{datapath_id}は、Packet Inを上げたスイッチのDatapath IDです。二番目の引数@<tt>{message}は@<tt>{PacketIn}クラスのインスタンスで、Packet Inメッセージをオブジェクトとしてラップしたものです。この@<tt>{PacketIn}クラスには主に次の3種類のメソッドが定義されています。

 * Packet Inを起こしたパケットのデータやその長さ、およびパケットが入ってきたスイッチのポート番号などOpenFlowメッセージ固有の情報を返すメソッド
 * Packet Inを起こしたパケットの種別(TCPかUDPか？またVLAN タグの有無など)を判定するための"@<tt>{?}"で終わるメソッド
 * 送信元や宛先のMACアドレス、IPアドレスなど、パケットの各フィールドを調べるためのアクセサメソッド

@<tt>{PacketIn}クラスは非常に多くのメソッドを持っており、またTremaのバージョンアップごとにその数も増え続けているためすべては紹介しきれません。そのかわり、代表的でよく使うものを以下に紹介します。

: @<tt>{:data}
  パケットのデータ全体をバイナリ文字列で返す

: @<tt>{:in_port}
  パケットが入ってきたスイッチのポート番号を返す

: @<tt>{:total_len}
  パケットのデータ長を返す

: @<tt>{:buffered?}
  Packet Inを起こしたパケットがスイッチにバッファされているかどうかを返す

: @<tt>{:macsa}
  パケットの送信元MACアドレスを返す

: @<tt>{:macda}
  パケットの宛先MACアドレスを返す

: @<tt>{:ipv4?}
  パケットがIPv4である場合@<tt>{true}を返す

: @<tt>{:ipv4_saddr}
  パケットの送信元IPアドレスを返す

: @<tt>{:ipv4_daddr}
  パケットの宛先IPアドレスを返す

: @<tt>{:tcp?}
  パケットがTCPである場合@<tt>{true}を返す

: @<tt>{:tcp_src_port}
  パケットのTCPの送信元ポート番号を返す

: @<tt>{:tcp_dst_port}
  パケットのTCP宛先ポート番号を返す

: @<tt>{:udp?}
  パケットがUDPである場合@<tt>{true}を返す

: @<tt>{:udp_src_port}
  パケットのUDPの送信元ポート番号を返す

: @<tt>{:udp_dst_port}
  パケットのUDPの宛先ポート番号を返す

: @<tt>{:vtag?}
  パケットにVLANヘッダが付いている場合@<tt>{true}を返す

: @<tt>{:vlan_vid}
  VLANのVIDを返す

このようなメソッドは他にもたくさんあります。メソッドの完全なリストや詳しい情報を知りたい場合には、@<chap>{openflow_framework_trema}で紹介した@<tt>{trema ruby}コマンドで最新のTrema APIドキュメントを参照してください。

=== FDBの更新とポート番号の検索

知らないパケットがPacket Inとして入ってきたとき、ラーニングスイッチは次のようにFDBにホストの位置情報を学習し、宛先のポート番号を調べます。

 1. パケットの送信元MACアドレスとパケットが入ってきたポート番号をPacket Inメッセージから取り出し、FDB(@<tt>{@fdb})に保存する
 2. パケットの宛先MACアドレスとFDBから、パケットを出力するポート番号を調べる

//noindent
FDBの実装は単純にハッシュテーブルを使っているだけなので、ひっかかる箇所は無いと思います。

//emlist{
class LearningSwitch < Controller
  # ...

  def packet_in( datapath_id, message )
    @fdb[ message.macsa ] = message.in_port
    port_no = @fdb[ message.macda ]

    # ...
  end

  # ...
end
//}

=== 宛先ポート番号がみつかった場合(Flow ModとPacket Out)

もし宛先ポートがみつかった場合、以降は同じパケットは同様に転送せよ、というフローエントリをスイッチに書き込みます(@<tt>{flow_mod}メソッド)。また、Packet Inを起こしたパケットも忘れずにそのポートへ出力します(@<tt>{packet_out}メソッド)。

//emlist{
def packet_in( datapath_id, message )
  # ...
  port_no = @fdb[ message.macda ]
  if port_no
    flow_mod datapath_id, message, port_no
    packet_out datapath_id, message, port_no
  else

  # ...
//}

この@<tt>{flow_mod}メソッドと@<tt>{packet_out}メソッドはそれぞれ@<tt>{Controller}クラスの@<tt>{send_flow_mod_add}(@<chap>{patch_panel}で紹介)および@<tt>{send_packet_out}(Packet Outの送信)メソッドを次のように呼び出します。

//emlist{
  # ...

  private


  def flow_mod( datapath_id, message, port_no )
    send_flow_mod_add(
      datapath_id,
      :match => ExactMatch.from( message ),
      :actions => SendOutPort.new( port_no )
    )
  end


  def packet_out( datapath_id, message, port_no )
    send_packet_out(
      datapath_id,
      :packet_in => message,
      :actions => SendOutPort.new( port_no )
    )
  end

  # ...
//}

ここでいくつか見慣れない要素が登場しています。

 * @<tt>{send_flow_mod_add}の中では、マッチングルールに@<tt>{ExactMatch.from}の返り値を指定している
 * Packet Outの送信用メソッド (@<tt>{send_packet_out})

それでは、それぞれの詳細を見て行きましょう。

==== Exact Matchの作り方

マッチングルールの中でもすべての条件を指定したものをExact Matchと呼びます。たとえばPacket Inとしてコントローラに入ってきたパケットと、

 * パケットが入ってきたスイッチのポート番号
 * 送信元 MAC アドレス
 * 宛先 MAC アドレス
 * ...

//noindent
などなどマッチングルールが定義する12個の条件(詳しくは@<chap>{openflow_spec}を参照)がすべてまったく同じ、というのがExact Matchです。

もし、Exact Matchを普通に作るとどうなるでしょうか？

//emlist{
def packet_in( datapath_id, message )
  # ...

  send_flow_mod_add(
    datapath_id,
    :match => Match.new(
      :in_port => message.in_port
      :dl_src => message.macsa,
      :dl_dst => message.macda,
      # ...
//}

//noindent
うーん。やりたいことは分かりますが、マッチングルールを1つ作るだけでこのようにいちいち10行以上も書いていたらしんどいですよね。

そこで、TremaではこのようなExact Matchを楽に書ける次のショートカットを用意しています。

//emlist{
def packet_in( datapath_id, message )
  # ...

  send_flow_mod_add(
    datapath_id,
    :match => ExactMatch.from( message ),
    # ...
//}

//noindent
なんとたった1行で書けました! Tremaではこのようにプログラムを短く書ける工夫がたくさんあります。これはそのつど紹介していくことにしましょう。

ところで今回のラーニングスイッチの例では、スイッチに書き込まれるフローエントリは次の2パターンだけです。

 1. ホスト1からホスト2
 2. ホスト2からホスト1

//noindent
今回は流れるパケットの種類も限られていますので、フローエントリのマッチングルールを楽に作るためにExact Macthを使っています。

==== Packet Out API

Packet OutはOpenFlowで定義されたメッセージの1つで、スイッチの指定したポートからパケットを送信させるためのものです。TremaでPacket Outを送るためのメソッド、@<tt>{send_packet_out}は次の2つの引数を取ります。

//emlist{
send_packet_out( datapath_id, options )
//}

それぞれの引数の意味は次のとおりです。

 * @<tt>{datapath_id}：Packet Outの届け先となるスイッチのDatapath ID
 * @<tt>{options}：Packet Outメッセージの中身を決めるためのオプションで、アクションによるパケットの書き換えや出力するポートを指定する。これはハッシュテーブルで定義されていて、必要なオプションのみを指定すればいいことになっている

Packet Outの使い道は、Packet Inメッセージとして入ってきたパケットをそのままスイッチのポートから送り出す場合がほとんどです。この場合、パケットの送信にスイッチのバッファを使う場合と使わない場合とで呼び出しかたが変わります。

===== スイッチのバッファを使ってPacket Outする場合

パケットのデータがスイッチのバッファに乗っていることが期待できる場合には、次のように@<tt>{:buffer_id}オプションでバッファに乗っているパケットデータのIDを指定してやることでPacket Outできます。

//emlist{
def packet_in( datapath_id, message )
  # ...

  send_packet_out(
    datapath_id,
    :buffer_id => message.buffer_id,
    :data => message.data,
    :actions => SendOutPort.new( port_number )
  )
//}

//noindent
この場合コントローラからスイッチへのパケットデータのコピーが起こらないため、若干のスピードアップが期待できます。ただし、@<chap>{openflow_spec}のコラムで説明したとおり、バッファの中身は予測不能でいつデータが消えるかわからないため、この方法は推奨しません。

===== スイッチのバッファを使わずに Packet Out する場合

スイッチのバッファを使わずにPacket Outする場合、次のように@<tt>{:data}オプションでパケットのデータを指定する必要があります。バッファに乗っているかいないかにかかわらずPacket Outできるので、若干遅くなりますが安全です。

//emlist{
def packet_in( datapath_id, message )
  # ...

  send_packet_out(
    datapath_id,
    :data => message.data,
    :actions => SendOutPort.new( port_number )
  )
//}

//noindent
これは、次のように@<tt>{:packet_in}オプションを使うことで若干短くできます(@<tt>{.data}を書かなくてよくなります)。

//emlist{
def packet_in( datapath_id, message )
  # ...

  send_packet_out(
    datapath_id,
    :packet_in => message,
    :actions => SendOutPort.new( port_number )
  )
//}

===== 主なオプション一覧

@<tt>{options}に指定できる主なオプションは次のとおりです。

: @<tt>{:buffer_id}
  スイッチでバッファされているパケットのIDを指定する。この値を使うと、スイッチでバッファされているパケットを指定してPacket Outできるので効率が良くなる(ただし、スイッチにバッファされていない時はエラーになる)

: @<tt>{:data}
  Packet Outするパケットの中身を指定する。もし@<tt>{:buffer_id}オプションが指定されておりスイッチにバッファされたパケットをPacket Outする場合、この値は使われない

: @<tt>{:packet_in}
  @<tt>{:data}および@<tt>{:in_port} オプションを指定するためのショートカット@<fn>{in_port_option}。@<tt>{packet_in}ハンドラの引数として渡される@<tt>{PacketIn}メッセージを指定する

: @<tt>{:actions}
  Packet Outのときに実行したいアクションの配列を指定する。アクションが1つの場合は配列でなくてかまわない

//footnote[in_port_option][@<tt>{:in_port}オプションは出力ポートに@<tt>{OFPP_TABLE}という特殊なポートを指定したときに使うオプションです。通常はまったく使いませんが、もし知りたい方はTrema Ruby APIを参照してください。]

=== 宛先ポート番号がみつからなかった場合(フラッディング)

もし宛先ポートがみつからなかった場合、コントローラはPacket Inしたメッセージをフラッディングしてばらまきます。これをやっているのが@<tt>{flood}メソッドで、実体は@<tt>{packet_out}メソッドのポート番号に仮想ポート番号@<tt>{OFPP_FLOOD}を指定しているだけです。これが指定されたPacket Outメッセージをスイッチが受け取ると、指定されたパケットをフラッディングします。

//emlist{
def packet_in( datapath_id, message )
  # ...
  port_no = @fdb[ message.macda ]
  if port_no
    # ...
  else
    flood datapath_id, message
  end
  # ...
end

private

# ...

def flood( datapath_id, message )
  packet_out datapath_id, message, OFPP_FLOOD
end
//}

== まとめ

さまざまなOpenFlowアプリケーションのベースとなるラーニングスイッチの動作と作り方を学びました。

 * コントローラは、Packet Inメッセージから送信元ホストのMACアドレスとホストのつながるスイッチポート番号をFDBに学習する
 * Packet Inの転送先がFDBから分かる場合、Flow Modで以降の転送情報をスイッチに書き込みPacket Outする。FDBで決定できない場合は、入力ポート以外のすべてのポートにPacket Outでフラッディングする。

続く章ではさっそくこのラーニングスイッチを少し改造してトラフィック集計機能を加えます。@<chap>{openflow_usecases}で紹介したフローでできる4つのことのうち、"流量を調べる"の実装例です。
