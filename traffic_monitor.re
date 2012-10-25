= トラフィックモニタ

//lead{
ラーニングスイッチの応用アプリケーション第一段がこのトラフィックモニタです。ちょっとしたコードを足すだけで、簡単に機能を追加できるという OpenFlow のパワーを実感してください。
//}

@<chap>{learning_switch}で作ったラーニングスイッチをベースに、少し高機能なスイッチを作ってみましょう。本章で紹介するトラフィック集計スイッチは，パっと見は普通のラーニングスイッチとして動作します。しかし，裏では各ホストが送信したトラフィックをカウントしており，定期的に集計情報を表示してくれます。これを使えば，ネットワークを無駄に使いすぎているホストを簡単に特定できます。

この章でのポイントは、トラフィックの集計方法です。トラフィックの集計は、@<chap>{openflow_usecases}で紹介したフローでできる 4 つのことの一つです。@<chap>{whats_openflow} で見たように、これを取り出すには、Flow Mod でフローを書き込むときに寿命を設定しておき、フローが削除されたタイミングでコントローラに上がる Flow Removed メッセージに入った統計情報を取得すれば OK です。

(図)

今回はラーニングスイッチの簡単な変形なので、前置きはこのくらいにいきなりソースコードを見てみましょう。

== ソースコード

トラフィックモニターはおおまかに「ラーニングスイッチ機能」と「トラフィックの集計機能」の 2 つの機能から成ります。コードを分かりやすくするため、それぞれを独立したクラスとして @<tt>{.rb} ファイルを分けて実装してあります。

 * @<tt>{TrafficMonitor} クラス: ラーニングスイッチにトラフィック収集機能を追加したもの
 * @<tt>{Counter} クラス: 収集したトラフィック情報を集計するクラス

//noindent
このうち @<tt>{TrafficMonitor} クラスがコントローラの本体で、@<tt>{Counter} クラスを使うという関係になります。

=== @<tt>{TrafficMonitor} クラス

まずは @<tt>{TrafficMonitor} クラスのソースコード (@<list>{traffic_monitor_class}) をざっと眺めてみましょう。とくに、ラーニングスイッチとの差分に注目してください。

//list[traffic_monitor_class][本体 @<tt>{TrafficMonitor} クラス（@<tt>{traffic-monitor.rb}）]{
require "counter"


class TrafficMonitor < Controller
  periodic_timer_event :show_counter, 10


  def start
    @fdb = {}
    @counter = Counter.new
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
    @counter.add message.macsa, 1, message.total_len
  end


  def flow_removed( datapath_id, message )
    @counter.add message.match.dl_src, message.packet_count, message.byte_count
  end


  private


  def show_counter
    puts Time.now
    @counter.each_pair do | mac, counter |
      puts "#{ mac } #{ counter[ :packet_count ] } packets (#{ counter[ :byte_count ] } bytes)"
    end
  end


  def flow_mod( datapath_id, message, port_no )
    send_flow_mod_add(
      datapath_id,
      :hard_timeout => 10,
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

まず、ラーニングスイッチのコードと比べて、新たに 2 つのメソッドが追加されていることが分かるでしょう。それぞれの処理内容はだいたい次のように読めます。

 1. @<tt>{flow_removed} メソッド: Flow Removed メッセージを受け取るハンドラです。Flow Removed メッセージに含まれるトラフィック情報を取り出してカウンタを更新しています。
 2. @<tt>{show_counter} メソッド: トラフィックの集計情報を表示します。冒頭の @<tt>{periodic_timer_event} により、10 秒ごとに情報を表示します。

そのほかのメソッドはラーニングスイッチと変わりませんが、次の 3 つのメソッドで 1 行ずつコードが追加されています。

 1. @<tt>{start} ハンドラで、トラフィック集計用の @<tt>{Counter} オブジェクトを作っています。
 2. @<tt>{packet_in} ハンドラの最後で、このカウンタを更新するコードが一行追加されています。
 3. @<tt>{flow_mod} メソッドで、Flow Mod メッセージのパラメータに @<tt>{:hard_timeout} オプションが追加されています。これがフローの寿命を設定し Flow Removed を起こすための仕掛けです。

メイン部分とのラーニングスイッチとの差分は、たったこれだけです。

=== @<tt>{Counter} クラス

@<tt>{Counter} クラス（@<list>{counter_class}）はトラフィックモニタのために新たに導入したクラスで、@<tt>{TrafficMonitor} クラスが収集したトラフィック情報を集計します。

//list[counter_class][トラフィックを記録し集計する @<tt>{Counter} クラス（@<tt>{counter.rb}）]{
class Counter
  def initialize
    @db = {}
  end


  def add( mac, packet_count, byte_count )
    @db[ mac ] ||= { :packet_count => 0, :byte_count => 0 }
    @db[ mac ][ :packet_count ] += packet_count
    @db[ mac ][ :byte_count ] += byte_count
  end


  def each_pair( &block )
    @db.each_pair &block
  end
end
//}

このクラスは次の 2 つのパブリックメソッドを持ちます:

 * @<tt>{add} メソッド: 収集したトラフィック情報を記録します。ホストごと (MAC アドレスで区別) に、送信パケット数とバイト数を記録しています。
 * @<tt>{each_pair} メソッド: 集計情報をホストごとに処理するときに使う、一種のループです。ここでは見慣れない文法を使っていますので、詳しくは後の「即席 Ruby 入門」で見ていくことにしましょう。

それでは動作のイメージをつかむため、細かい実装の解説は後回しにしてさっそく実行してみましょう。

== 実行してみよう

今回も仮想ネットワークを使って、仮想スイッチ 1 台、仮想ホスト 2 台の構成でトラフィックモニタを起動してみます。@<list>{traffic_monitor_conf} の内容の仮想ネットワーク設定を @<tt>{traffic-monitor.conf} として保存します。

//list[traffic_monitor_conf][仮想スイッチ 0xabc に仮想ホスト host1，host2 を接続する設定]{
vswitch {
  datapath_id "0xabc"
}

vhost ("host1") {
  ip "192.168.0.1"
  mac "00:00:00:00:00:01"
}

vhost ("host2") {
  ip "192.168.0.2"
  mac "00:00:00:00:00:02"
}

link "0xabc", "host1"
link "0xabc", "host2"
//}

次のように @<tt>{trema run} の "@<tt>{-c}" オプションにこの設定ファイルを渡してトラフィックモニタを実行します。トラフィックモニタはスイッチにつながったホストからのトラフィックを検知し、ホストごとに集計して表示します。ただし起動直後ではまだトラフィックは発生していないので、画面には何も出力されません。

//cmd{
% trema run ./traffic-monitor.rb -c ./traffic-monitor.conf
//}

それでは，実際にトラフィックを発生させて集計されるか見てみましょう。@<tt>{send_packets} コマンドを使って host1 と host2 の間でテストパケットを送ってみます。

//cmd{
% trema send_packets --source host1 --dest host2 --n_pkts 10 --pps 10
% trema send_packets --source host2 --dest host1 --n_pkts 10 --pps 10
//}

@<tt>{trema run} を実行した元のターミナルに、次のような出力が出ていれば成功です。

//cmd{
% trema run ./traffic-monitor.rb -c ./traffic-monitor.conf
……
00:00:00:00:00:01 10 packets (640 bytes)
00:00:00:00:00:02 10 packets (640 bytes)
……
//}

//noindent
host1 と host2 ごとに 10 パケットずつ送信したことが、コントローラからきちんと検知されています。

トラフィックモニタの動作イメージがわかったところで、ソースコードの解説に移りましょう。まずはこの章で新しく登場した Ruby の構文を紹介します。

=== ソースコード解説

それでは，とくに重要な 1 の処理を詳しく見ていきましょう。以下の説明では
@<img>{traffic_monitor_config} に示すホスト 2 台＋スイッチ 1 台からなる
ネットワーク構成を使います。

//image[traffic_monitor_config][TrafficMonitor を動作させるネットワーク構成の例]

host1 から host2 にパケットを送信したときの動作シーケンスは
@<img>{sequence_diagram1} のようになります。

//image[sequence_diagram1][host1からhost2宛にパケットを送信したときの動作シーケンス]

 1. host1からhost2を宛先としてパケットを送信すると，まずはスイッチにパ
 ケットが届く
 2. スイッチのフローテーブルは最初はまっさらで，どう処理すればよいかわ
 からない状態なので，コントローラであるTrafficMonitorにpacket_inメッセー
 ジを送る
 3. TrafficMonitorのpacket_inメッセージハンドラでは，packet_inメッセー
 ジのin_port（host1のつながるスイッチポート）とhost1のMACアドレスをFDB
 に記録する
 4. また，Counterに記録されたhost1の送信トラフィックを1パケット分増やす
 5. packet_inメッセージの宛先MACアドレスから転送先のスイッチポート番号
 をFDBに問い合わせる。この時点ではhost2のスイッチポートは学習していない
 ので，結果は「不明」
 6. そこで，パケットをin_port以外のすべてのスイッチポートに出力する
 packet_outメッセージ（FLOODと呼ばれる）をスイッチに送り，host2が受信し
 てくれることを期待する
 7. スイッチは，パケットをin_port以外のすべてのポートに出す

これで，最終的にhost2がパケットを受信できます。逆に，この状態でhost1を
宛先としてhost2からパケットを送信したときの動作シーケンスは次のとおりに
なります（@<img>{sequence_diagram2}）。

//image[sequence_diagram2][host1からhost2宛にパケットを送信したときの動作シーケンス]

4 までの動作は@<img>{sequence_diagram1}と同じですが，5 からの動作が次の
ように異なります。

 5. packet_inメッセージの宛先MACアドレスから，転送先のスイッチポート番
 号をFDBに問い合わせる。これは，先ほどhost1からhost2にパケットを送った
 時点でFDBに学習させているので，送信先はスイッチポート1番ということがわ
 かる
 6. そこで，TrafficMonitorはパケットをスイッチポート1番へ出力する
 packet_outメッセージをスイッチに送る。スイッチはこれを受け取ると，パケッ
 トをスイッチポート1番に出し，最終的にhost1がパケットを受信する
 7. 「送信元=00:00:00:00:00:02，送信先=00:00:00:00:00:01となるパケット
 はスイッチポート1番に転送せよ」というflow_modメッセージをスイッチに送
 信する

最後の 7 によって，以降のhost2からhost1へのパケットはすべてスイッチ側だ
けで処理されるようになります。

== まとめ/参考文献

「トラフィック集計機能付きスイッチ」を実現するコントローラを書きました。学んだことは次の 1 つです。

 * トラフィック情報はフローが消えるときに発生する flow_removed メッセージの中身を調べることで集計できます。フローの寿命はフローを書き込む Flow Mod メッセージで指定できます。
