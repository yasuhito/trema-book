= すべての基本、ラーニングスイッチ

//lead{
データセンターのような複雑に入り組んだネットワークも、もしケーブルを抜いてバラバラにできたなら、スイッチやサーバなどの意外とシンプルな部品に分解できるでしょう。いわばネットワークを構成する "原子" とも言えるこのスイッチを OpenFlow で作ってみましょう。
//}

OpenFlow の世界では、コントローラとしてソフトウェア実装したスイッチをよくラーニングスイッチと呼びます。なぜ、ラーニング (学習) スイッチと呼ぶのでしょうか？それは、スイッチが次のように動くからです。

 1. 学習: ホストから出たパケットに含まれる、ネットワーク上でのホストの位置情報を学習する
 2. 転送: 今まで学習してきた位置情報を使って、パケットを宛先のホストまで転送する

この「学習し、転送する」というラーニングスイッチの仕組みは応用が広く効きます。たとえば第 III 部で紹介するいくつかのデータセンターネットワークも、基本はラーニングスイッチと同じく「学習し、転送する」という動作をします。このように、自宅ネットワークであろうが最新鋭のデータセンターであろうが、その基本的な考えかたは同じなのです。

このおかげで、ラーニングスイッチを作れるようになれば、いろいろなアプリケーションを Trema で作れるようになります。

 * 複数スイッチ対応ラーニングスイッチ (Trema のサンプルアプリの一つ。@<chap>{openflow_framework_trema}で紹介)
 * トラフィックモニタ (@<chap>{traffic_monitor}で紹介)
 * ルーティングスイッチ (@<chap>{routing_switch}で紹介)
 * スライス機能つきルーティングスイッチ (@<chap>{sliceable_switch}で紹介)

//noindent
ちょうど、和食の基本である@<ruby>{出汁,だし}をマスターすればおいしい吸い物や煮物などを作れるように、ラーニングスイッチをベースに機能を付け加えていけば、さまざまな OpenFlow アプリケーションが出来上がるのです。

というわけでさっそく、ネットワークの基本部品であるラーニングスイッチを Trema で実装してみましょう。まずは一般的なハードウェアスイッチの動作原理を理解し、次にこれを OpenFlow で実現する方法を見ていきます。

== スイッチの仕組み

簡単なネットワークを例にしてスイッチの動作を説明します (@<img>{switch_network})。スイッチのポート 1 番と 4 番に、ホスト 1 と 2 をそれぞれ接続しています。また、それぞれのホストのネットワークカードは図に示した MAC アドレスを持つとします。

//image[switch_network][スイッチ 1 台とホスト 2 台からなるネットワーク][scale=0.5]

=== ホストの位置情報の学習

スイッチは次のようにネットワーク上でのホストの位置情報を学習します (@<img>{host1to2})。

 1. ホスト 1 がホスト 2 へパケットを送信すると、そのパケットはスイッチのポート 1 番に到着します。
 2. スイッチはパケットの送信元 MAC アドレスとパケットの入ってきたポート番号から、「ポート 1 番には MAC アドレスが 00:11:11:11:11:11 のホストがつながっている」と学習します。

//noindent
この学習した位置情報 (「ホストの MAC アドレス + ポート番号」の組) をためておくスイッチ上のデータベースを、フォワーディングデータベース (FDB) と呼びます。この情報は、以降のパケットの転送に使います。

//image[host1to2][パケットの送信元 MAC アドレスとスイッチのポート番号を FDB に学習する][scale=0.5]

=== パケットの転送 (フラッディング)

学習が終わると、スイッチはパケットを宛先のホストに次のように転送します (@<img>{host1to2_flood})。

  1. パケットの宛先 MAC アドレスと FDB から、この MAC アドレスを持つホストがつながるポート番号を探します。しかし、ホスト 2 の MAC アドレスとポート番号はまだ学習していないので分かりません。
  2. しょうがないのでパケットをポート 1 以外のすべてのポートにばらまき、最終的にポート 4 につながるホスト 2 にパケットが届きます。

//noindent
宛先のホストの位置情報をまだ学習していないときには、このようにパケットをばらまきます。これをフラッディングと呼び、余計なトラフィックが発生してしまいます。

//image[host1to2_flood][パケットの宛先 MAC アドレスからスイッチのポート番号が FDB にみつからないため、パケットをばらまく][scale=0.5]

=== ふたたび学習と転送

この状態でホスト 2 がホスト 1 へパケットを送信するとどうなるでしょうか (@<img>{host2to1})

 1.「ポート 4 番には MAC アドレスが 00:22:22:22:22:22 のホストがつながっている」と学習します。
 2. パケットの宛先 MAC アドレスと FDB を照らし合わせ、出力先のポート番号を探します。ここですでに「MAC アドレス 00:11:11:11:11:11 = ポート 1」と学習しているので、ポート 1 にパケットを出力します。

この場合パケットは宛先のポートのみに出力するので、フラッディングと異なり余計なトラフィックが発生しません。

//image[host2to1][同様にホスト 2 の MAC アドレスとポート番号を学習し、FDB の情報からパケットをホスト 1 に届ける][scale=0.5]

ここまでの段階で、スイッチはすべてのホストの MAC アドレスとポート番号を学習したことになります。もし、ホスト 1 がホスト 2 へふたたびパケットを送信すると、今度はパケットをばらまくのではなくちゃんとポート 4 のみへと出力します。このように通信が進むにつれて、FDB が賢くなりパケットを効率的に転送できるようになります。

以上のように、スイッチはホストの MAC アドレスとポート番号をパケット到着のタイミングで学習しつつ、これを使ってパケットを送り届けるわけです。ここまでの仕組みがわかれば、スイッチの機能を実現するコントローラ (ラーニングスイッチ) を OpenFlow で実現するのは簡単です。

== OpenFlow 版スイッチ (ラーニングスイッチ) の仕組み

OpenFlow によるスイッチの構成は @<img>{switch_network_openflow} のようになります。一般的なハードウェアスイッチとの違いは次の 2 つです。

 * FDB をソフトウェアとして実装し、コントローラが管理する。
 * パケットの転送は、コントローラがフローテーブルにフローを書き込むことで制御する。

//noindent
なお、初期状態での FDB とフローテーブルの中身はどちらも空です。

//image[switch_network_openflow][OpenFlow によるスイッチ (ラーニングスイッチ) の構成][scale=0.45]

=== Packet In からホストの位置情報を学習

この状態でホスト 1 がホスト 2 へパケットを送信すると、コントローラは次のようにホスト 1 のネットワーク上での位置情報を学習します (@<img>{host1to2_openflow})。

 1. フローテーブルは空なのでパケットは Packet In としてコントローラまで上がります。
 2. コントローラは Packet In メッセージからパケットの送信元 MAC アドレスとパケットの入ってきたポートの番号を調べ、「ポート 1 番には MAC アドレスが 00:11:11:11:11:11 のホストがつながっている」と FDB に保存します。

//image[host1to2_openflow][Packet In の送信元 MAC アドレスとスイッチのポート番号を FDB に学習する][scale=0.45]

=== Packet Out でパケットを転送 (フラッディング)

学習が終わると次はパケットの転送です。もちろん、パケットの宛先はまだ学習していないので、コントローラは次のようにパケットをフラッディングします (@<img>{host1to2_flood_openflow})。

 1. コントローラは Packet In メッセージの宛先 MAC アドレスを調べ、FDB から送出先のポート番号を探します。しかし、ホスト 2 の MAC アドレスとポート番号はまだ FDB に入っていないので分かりません。
 2. コントローラは Packet Out メッセージ (出力ポート = フラッディング) でパケットをばらまくようにスイッチに指示します。その結果、ポート 4 につながるホスト 2 にパケットが届きます。

//image[host1to2_flood_openflow][パケットの宛先 MAC アドレスからスイッチのポート番号が FDB にみつからないため、Packet Out メッセージ (出力ポート = フラッディング) でパケットをばらまく][scale=0.45]

=== ふたたび学習と転送 (Flow Mod と Packet Out)

この状態でホスト 2 がホスト 1 へパケットを送信すると次のようになります (@<img>{host2to1_openflow})。

 1. フローテーブルが空なためコントローラまでふたたび Packet In メッセージが上がります。
 2. コントローラはこの Packet In メッセージから「ポート 4 番には MAC アドレスが 00:22:22:22:22:22 のホストがつながっている」と FDB に保存します。
 3. Packet In の宛先 MAC アドレスと FDB を照らし合わせ、出力先のポート番号を探します。ここですでに「ポート 1 = MAC アドレス 00:11:11:11:11:11」と学習しているので、出力ポートは 1 と決定できます。
 4. 「ホスト 2 からホスト 1 へのパケットはポート 1 へ出力せよ」というフローを Flow Mod メッセージでフローテーブルに書き込みます。加えて、Packet Out メッセージ (出力ポート = 1 )で Packet In を起こしたパケットをポート 1 へ出力します。

//image[host2to1_openflow][同様にホスト 2 の MAC アドレスとポート番号を FDB に学習し、フローを書き込むとともにパケットをホスト 1 に届ける][scale=0.45]

さて、ここまでの段階でフローテーブルには「ホスト 2 からホスト 1 へのパケットはポート 1 へ出力せよ」というフローが入りました。もし、ホスト 2 がホスト 1 へふたたびパケットを送信すると、今度は Packet In がコントローラまで上がることはなく、スイッチ側だけでパケットを転送します。

残りのホスト 1 からホスト 2 へのフローはどうでしょう。すでに FDB はすべてのホストの MAC アドレスとポート番号を学習してあります。もし、ふたたびホスト 1 からホスト 2 ヘパケットを送信すると、@<img>{host2to1_openflow}と同様にコントローラが「ホスト 1 からホスト 2 へのパケットはポート 4 へ出力せよ」というフローを書き込みます。もちろん、それ以降の通信では Packet In はまったく上がらずにすべてスイッチ側だけでパケットを処理します。

ちょっと長くなってしまいましたが OpenFlow によるスイッチの実現方法がわかりました。いよいよ Trema での実装に移ります。

== LearningSwitch コントローラ

まずはラーニングスイッチのソースコード (@<list>{learning-switch.rb}) をざっと眺めてみましょう。とくに、@<tt>{private} の行よりも上のパブリックなメソッドに注目してください。

//list[learning-switch.rb][ラーニングスイッチ (@<tt>{learning-switch.rb}) のソースコード]{
class LearningSwitch < Controller
  def start
    @fdb = {}
  end


  def packet_in datapath_id, message
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


  def flow_mod datapath_id, message, port_no
    send_flow_mod_add(
      datapath_id,
      :match => ExactMatch.from( message ),
      :actions => SendOutPort.new( port_no )
    )
  end


  def packet_out datapath_id, message, port_no
    send_packet_out(
      datapath_id,
      :packet_in => message,
      :actions => SendOutPort.new( port_no )
    )
  end


  def flood datapath_id, message
    packet_out datapath_id, message, OFPP_FLOOD
  end
end
//}

今までの知識だけでもこれだけたくさんのことがわかります。

  * ラーニングスイッチの本体は LearningSwitch という名前のクラスです。
  * 起動時に呼ばれる @<tt>{start} ハンドラで FDB のインスタンス変数を作っています。@<tt>{{\}} という文法は見慣れませんが、これに MAC アドレスとポート番号の組を保存するのでしょう。
  * 見慣れないハンドラ @<tt>{packet_in} が登場しました。これは、ご想像のとおり Packet In を捕捉するためのハンドラです。スイッチのフローにマッチしないパケットがコントローラに上がってくると、このハンドラが呼ばれます。
  * @<tt>{packet_in} ハンドラの中では、パケットの宛先 MAC アドレスから FDB でポート番号を調べています。もし宛先のポート番号がみつかった場合には、Flow Mod でフローを打ち込み Packet Out でパケットを送信しているようです。もしポート番号がみつからなかった場合は、flood というメソッドを呼んでいます。これは先程の説明であった「パケットをばらまく (フラッディング)」という処理でしょう。

いかがでしょうか。ラーニングスイッチの心臓部は @<tt>{packet_in} ハンドラだけで、その中身もやっていることはなんとなくわかると思います。細かい実装の解説は後回しにして、さっそく実行してみましょう。

=== 実行してみよう

今回も仮想ネットワークを使って、仮想スイッチ 1 台、仮想ホスト 2 台の構成でラーニングスイッチを起動してみます。次の内容の設定ファイルを @<tt>{learning-switch.conf} として保存してください。

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
% trema run ./learning-switch.rb -c ./learning-switch.conf
//}

別ターミナルを開き、@<tt>{trema send_packets} コマンドを使って host1 と host2 の間でテストパケットを送ってみます。

//cmd{
% trema send_packets --source host1 --dest host2
% trema send_packets --source host2 --dest host1
//}

@<tt>{trema show_stats} コマンドで host1 と host2 の受信パケット数をチェックし、それぞれでパケットが受信されていれば成功です。

//cmd{
% trema show_stats host1 --rx
ip_dst,tp_dst,ip_src,tp_src,n_pkts,n_octets
192.168.0.1,1,192.168.0.2,1,1,50
% trema show_stats host2 --rx
ip_dst,tp_dst,ip_src,tp_src,n_pkts,n_octets
192.168.0.2,1,192.168.0.1,1,1,50
//}

ラーニングスイッチの動作イメージがわかったところで、ソースコードの解説に移りましょう。まずはこの章で新しく登場した Ruby の構文を紹介します。

== 即席 Ruby 入門

ラーニングスイッチのソースコード (@<list>{learning-switch.rb}) で新しく登場した構文はたった一つだけです。@<tt>{start} ハンドラの中で登場した FDB の本体 @<tt>{{\}} がそれですが、カンの良い読者ならこれはハッシュテーブルでは？と気付いているかもしれません。

=== ハッシュテーブル

ハッシュテーブルは中カッコで囲まれた辞書です。辞書とは「言葉をその定義に対応させたデータベース」です@<fn>{python_dictionary}。Ruby では、この対応を "@<tt>{=>}" という矢印で次のように表します。

//footnote[python_dictionary][Python の人々はハッシュテーブルのことをズバリ辞書と呼んでいますが、そちらのほうが分かりやすいと私は感じます。]

//emlist{
animals = { "armadillo" => "アルマジロ", "boar" => "イノシシ }
//}

たとえば "boar" を日本語で言うと何だろう？と辞書で調べたくなったら、次のようにして辞書を引きます。

//emlist{
animals[ "boar" ] #=> "イノシシ"
//}

この辞書を引くときに使う言葉 (この場合、"boar") を@<em>{キー}と言います。そして、みつかった定義 (この場合、"イノシシ") を@<em>{バリュー}と言います。

新しい動物を辞書に加えるのも簡単です。

//emlist{
animals[ "cow" ] = "ウシ"
//}

Ruby のハッシュテーブルはとても高性能なので、文字列だけでなく好きなオブジェクトを格納できます。たとえば、FDB では MAC アドレスをキーとして、ポート番号をバリューにします。

//emlist{
fdb[ "00:11:11:11:11:11" ] = 1
//}

もちろん @<tt>{{\}} は空のハッシュテーブルです。FDB は最初は空なので、@<tt>{{\}} に初期化されていました。

//emlist{
def start
  @fdb = {}
end
//}

実は、すでにいろんなところでハッシュテーブルを使ってきました。@<tt>{send_flow_mod_add} などの省略可能なオプションは、矢印 (@<tt>{=>}) を使っていることからも分かるように実はハッシュテーブルなのです。Ruby では、引数の最後がハッシュテーブルである場合、その中カッコを次のように省略できます。

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

== ソースコード

それではラーニングスイッチのソースコードを読み解いていきましょう。今回の肝となるのは、Packet In ハンドラでの次の処理です。

 * FDB の更新とポート番号の検索
 * ポート番号がみつかった場合の、Flow Mod と Packet Out 処理
 * ポート番号がみつからなかった場合のフラッディング処理

それでは、最初に Packet In ハンドラの定義方法から見ていきましょう。

=== 未知のパケット (Packet In) の処理

コントローラに上がってくる未知のパケットを拾うには、Packet In ハンドラをコントローラクラスに実装します。典型的な Packet In ハンドラは次のように実装されます (@<list>{learning-switch.rb}より抜粋)。

//emlist{
class LearningSwitch < Controller
  # ...

  def packet_in datapath_id, message
    # ...
  end

  # ...
//}

最初の引数 @<tt>{datapath_id} は、Packet In を上げたスイッチの Datapath ID です。二番目の引数 @<tt>{message} は @<tt>{PacketIn} クラスのインスタンスで、Packet In メッセージをオブジェクトとしてラップしたものです。この @<tt>{PacketIn} クラスには主に次の 3 種類のメソッドが定義されています。

 * Packet In を起こしたパケットのデータやその長さ、およびパケットが入ってきたスイッチのポート番号など OpenFlow メッセージ固有の情報を返すメソッド
 * Packet In を起こしたパケットの種別 (TCP か UDP か？ また VLAN タグの有無など) を判定するための、"@<tt>{?}" で終わるメソッド
 * 送信元や宛先の MAC アドレス、IP アドレスなど、パケットの各フィールドを調べるためのアクセサメソッド

@<tt>{PacketIn} クラスは非常に多くのメソッドを持っており、また Trema のバージョンアップごとにその数も増え続けているためすべては紹介しきれません。そのかわり、代表的でよく使うものを以下に紹介します。

: @<tt>{:data}
  パケットのデータ全体をバイナリ文字列で返します。

: @<tt>{:in_port}
  パケットが入ってきたスイッチのポート番号を返します。

: @<tt>{:total_len}
  パケットのデータ長を返します。

: @<tt>{:buffered?}
  Packet In を起こしたパケットがスイッチにバッファされているかどうかを返します。

: @<tt>{:macsa}
  パケットの送信元 MAC アドレスを返します。

: @<tt>{:macda}
  パケットの宛先 MAC アドレスを返します。

: @<tt>{:ipv4?}
  パケットが IPv4 である場合 true を返します。

: @<tt>{:ipv4_saddr}
  パケットの送信元 IP アドレスを返します。

: @<tt>{:ipv4_daddr}
  パケットの宛先 IP アドレスを返します。

: @<tt>{:tcp?}
  パケットが TCP である場合 true を返します。

: @<tt>{:tcp_src_port}
  パケットの TCP の送信元ポート番号を返します。

: @<tt>{:tcp_dst_port}
  パケットの TCP 宛先ポート番号を返します。

: @<tt>{:udp?}
  パケットが UDP である場合 true を返します。

: @<tt>{:udp_src_port}
  パケットの UDP の送信元ポート番号を返します。

: @<tt>{:udp_dst_port}
  パケットの UDP の宛先ポート番号を返します。

: @<tt>{:vtag?}
  パケットに VLAN ヘッダが付いている場合 true を返します。

: @<tt>{:vlan_vid}
  VLAN の VID を返します。

このようなメソッドは他にもたくさんあります。メソッドの完全なリストや詳しい情報を知りたい場合には、@<chap>{openflow_framework_trema} で紹介した @<tt>{trema run} コマンドで最新の Trema API ドキュメントを参照してください。

=== FDB の更新とポート番号の検索

知らないパケットが Packet In として入ってきたとき、ラーニングスイッチは次のように FDB にホストの位置情報を学習し、宛先のポート番号を調べます。

 1. パケットの送信元 MAC アドレスとパケットが入ってきたポート番号を Packet In メッセージから取り出し、FDB (@<tt>{@fdb}) に保存します。
 2. パケットの宛先 MAC アドレスと FDB から、パケットを出力するポート番号を調べます。

//noindent
FDB の実装は単純にハッシュテーブルを使っているだけなので、ひっかかる箇所は無いと思います。

//emlist{
class LearningSwitch < Controller
  # ...

  def packet_in datapath_id, message
    @fdb[ message.macsa ] = message.in_port
    port_no = @fdb[ message.macda ]

    # ...
  end

  # ...
end
//}

=== 宛先ポート番号がみつかった場合 (Flow Mod と Packet Out)

もし宛先ポートがみつかった場合、以降は同じパケットは同様に転送せよ、というフローをスイッチに書き込みます (@<tt>{flow_mod} メソッド)。また、Packet In を起こしたパケットも忘れずにそのポートへ出力します (@<tt>{packet_out} メソッド)。

//emlist{
def packet_in datapath_id, message
  # ...
  port_no = @fdb[ message.macda ]
  if port_no
    flow_mod datapath_id, message, port_no
    packet_out datapath_id, message, port_no
  else

  # ...
//}

この @<tt>{flow_mod} メソッドと @<tt>{packet_out} メソッドはそれぞれ @<tt>{Controller} クラスの @<tt>{send_flow_mod_add} (@<chap>{patch_panel}で紹介) および @<tt>{send_packet_out} (Packet Out の送信) メソッドを次のように呼び出します。

//emlist{
  # ...

  private


  def flow_mod datapath_id, message, port_no
    send_flow_mod_add(
      datapath_id,
      :match => ExactMatch.from( message ),
      :actions => SendOutPort.new( port_no )
    )
  end


  def packet_out datapath_id, message, port_no
    send_packet_out(
      datapath_id,
      :packet_in => message,
      :actions => SendOutPort.new( port_no )
    )
  end

  # ...
//}

ここでいくつか見慣れない要素が登場しています。

 * @<tt>{send_flow_mod_add} の中では、マッチングルールに @<tt>{ExactMatch.from} の返り値を指定しています。
 * Packet Out の送信用メソッド (@<tt>{send_packet_out}) は初登場です。

それでは、それぞれの詳細を見て行きましょう。

==== Exact Match の作り方

マッチングルールの中でもすべての条件を指定したものを Exact Match と呼びます。たとえば Packet In としてコントローラに入ってきたパケットと、

 * パケットが入ってきたスイッチのポート番号
 * 送信元 MAC アドレス
 * 宛先 MAC アドレス
 * ...

//noindent
などなどマッチングルールが定義する 12 個の条件 (詳しくは@<chap>{openflow}を参照) がすべてまったく同じ、というのが Exact Match です。

もし、Exact Match を普通に作るとどうなるでしょうか？

//emlist{
def packet_in datapath_id, message
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
うーん。やりたいことは分かりますが、マッチングルールを 1 つ作るだけでこのようにいちいち 10 行以上も書いていたらしんどいですよね。

そこで、Trema ではこのような Exact Match を楽に欠ける次のショートカットを用意しています。

//emlist{
def packet_in datapath_id, message
  # ...

  send_flow_mod_add(
    datapath_id,
    :match => ExactMatch.from( message ),
    # ...
//}

//noindent
なんとたった 1 行で書けました! Trema ではこのようにプログラムを短く書ける工夫がたくさんありますので、そのつど説明して行きましょう。

ところで今回のラーニングスイッチの例では、スイッチに書き込まれるフローは次の 2 パターンだけです。

 1. ホスト 1 からホスト 2
 2. ホスト 2 からホスト 1

//noindent
今回は流れるパケットの種類も限られていますので、フローのマッチングルールを楽に作るために Exact Macth を使っています。

==== Packet Out API

Packet Out は OpenFlow で定義されたメッセージの 1 つで、スイッチの指定したポートからパケットを送信させるためのものです。Trema で Packet Out を送るためのメソッド、@<tt>{send_packet_out} は次の 2 つの引数を取ります。

//emlist{
send_packet_out( datapath_id, options )
//}

それぞれの引数の意味は次のとおりです。

 * datapath_id: Packet Out の届け先となるスイッチの Datapath ID です。
 * options: Packet Out メッセージの中身を決めるためのオプションで、アクションによるパケットの書き換えや出力するポートの指定が行われます。これはハッシュテーブルで定義されていて、必要なオプションのみを指定すればいいことになっています。

Packet Out の使い道は、Packet In メッセージとして入ってきたパケットをそのままスイッチのポートから送り出す場合がほとんどです。この場合、パケットの送信にスイッチのバッファを使う場合と使わない場合とで呼び出しかたが変わります。

===== スイッチのバッファを使って Packet Out する場合

パケットのデータがスイッチのバッファに乗っていることが期待できる場合には、次のように @<tt>{:buffer_id} オプションでバッファに乗っているパケットデータの ID を指定してやることで Packet Out できます。

//emlist{
def packet_in datapath_id, message
  # ...

  send_packet_out(
    datapath_id,
    :buffer_id => message.buffer_id,
    :data => message.data,
    :actions => SendOutPort.new( port_number )
  )
//}

//noindent
この場合コントローラからスイッチへのパケットデータのコピーが起こらないため、若干のスピードアップが期待できます。ただし、@<chap>{openflow}のコラムで説明したとおり、バッファの中身は予測不能でいつデータが消えるかわからないため、この方法は推奨しません。

===== スイッチのバッファを使わずに Packet Out する場合

スイッチのバッファを使わずに Packet Out する場合、次のように @<tt>{:data} オプションでパケットのデータを指定する必要があります。バッファに乗っているかいないかにかかわらず Packet Out できるので、若干遅くなりますが安全です。

//emlist{
def packet_in datapath_id, message
  # ...

  send_packet_out(
    datapath_id,
    :data => message.data,
    :actions => SendOutPort.new( port_number )
  )
//}

//noindent
これは、次のように @<tt>{:packet_in} オプションを使うことで若干短くできます (@<tt>{.data} を書かなくてよくなります)。

//emlist{
def packet_in datapath_id, message
  # ...

  send_packet_out(
    datapath_id,
    :packet_in => message,
    :actions => SendOutPort.new( port_number )
  )
//}

===== 主なオプション一覧

@<tt>{options} に指定できる主なオプションは次のとおりです。

: @<tt>{:buffer_id}
  スイッチでバッファされているパケットの ID を指定します。この値を使うと、スイッチでバッファされているパケットを指定して Packet Out できるので効率が良くなります。ただし、スイッチにバッファされていない時はエラーになります。

: @<tt>{:data}
  Packet Out するパケットの中身を指定します。もし @<tt>{:buffer_id} オプションが指定されておりスイッチにバッファされたパケットを Packet Out する場合、この値は使われません。

: @<tt>{:packet_in}
  @<tt>{:data} および @<tt>{:in_port} オプションを指定するためのショートカットです@<fn>{in_port_option}。@<tt>{packet_in} ハンドラの引数として渡される @<tt>{PacketIn} メッセージを指定します。

: @<tt>{:actions}
  Packet Out のときに実行したいアクションの配列を指定します。アクションが一つの場合は配列でなくてかまいません。

//footnote[in_port_option][@<tt>{:in_port} オプションは出力ポートに @<tt>{OFPP_TABLE} という特殊なポートを指定したときに使うオプションです。通常はまったく使いませんが、もし知りたい方は Trema Ruby API を参照してください。]

=== 宛先ポート番号がみつからなかった場合 (フラッディング)

もし宛先ポートがみつからなかった場合、コントローラは Packet In したメッセージをフラッディングしてばらまきます。これをやっているのが @<tt>{flood} メソッドで、実体は @<tt>{packet_out} メソッドのポート番号に仮想ポート番号 @<tt>{OFPP_FLOOD} を指定しているだけです。これが指定された Packet Out メッセージをスイッチが受け取ると、指定されたパケットをフラッディングします。

//emlist{
def packet_in datapath_id, message
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

def flood datapath_id, message
  packet_out datapath_id, message, OFPP_FLOOD
end
//}

== まとめ

さまざまな OpenFlow アプリケーションのベースとなるラーニングスイッチの動作と作り方を学びました。

 * コントローラは、Packet In メッセージから送信元ホストの MAC アドレスとホストのつながるスイッチポート番号を FDB に学習します。
 * Packet In の転送先が FDB から分かる場合、Flow Mod で以降の転送情報をスイッチに書き込み Packet Out します。FDB で決定できない場合は、入力ポート以外のすべてのポートに Packet Out でフラッディングします。

続く章ではさっそくこのラーニングスイッチを少し改造してトラフィック集計機能を加えます。@<chap>{openflow_usecases}で紹介したフローでできる 4 つのことのうち、"流量を調べる" の実装例です。
