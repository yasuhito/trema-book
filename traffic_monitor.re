= トラフィックモニタ

トラフィック集計スイッチは，パっと見は普通のL2スイッチとして動作します。
しかし，裏では各ホストが送信したトラフィックをカウントしており，定期的
に集計情報を表示してくれます。これを使えば，ネットワークを無駄に使いす
ぎているホストを簡単に特定できます。

== トラフィックを集計するには

=== flow_removed メッセージ

=== フローの寿命を設定する
=== タイマで一定時間ごとに処理する
== 「トラフィックモニタ」の仕組み
=== トラフィック情報の集計
=== トラフィック情報の表示

== ソースコード

「L2スイッチ機能」と「トラフィックの集計機能」のためにはどんな部品が必
要でしょうか？ まずは，スイッチに指示を出す上司にあたるコントローラクラ
スが必要です。これを @<tt>{TrafficMonitor} クラスと名付けましょう。また，
パケットを宛先のスイッチポートへ届けるための @<tt>{FDB} クラス（注1），
あとはトラフィックを集計するための @<tt>{Counter} クラスの 3 つが必要で
す。

=== カウンタクラス

@<tt>{Counter} クラス（@<list>{counter_class}）は，ホスト（MACアドレス
で区別します）ごとの送信パケット数およびバイト数をカウントします。また，
カウントした集計情報を表示するためのヘルパメソッド (@<tt>{each_pair})
を提供します。

//list[counter_class][トラフィックを記録し集計する @<tt>{Counter} クラス（@<tt>{counter.rb}）]{
class Counter
  def initialize
    @db = {}  # ホストごとのトラフィック集計情報を記録する連想配列
  end

  def add mac, packet_count, byte_count  # ホスト（MACアドレス＝mac）の送信パケット数、バイト数を追加
    @db[ mac ] ||= { :packet_count => 0, :byte_count => 0 }
    @db[ mac ][ :packet_count ] += packet_count
    @db[ mac ][ :byte_count ] += byte_count
  end

  def each_pair &block  # 集計情報の表示用
    @db.each_pair &block
  end
end
//}

=== トラフィックモニタクラス

@<tt>{TrafficMonitor} クラスはコントローラの本体です (@<list>{traffic_monitor_class})。メインの処理はリスト3①～③の3つになります。

 1. packet_inメッセージが到着したとき，パケットを宛先のスイッチポートに転送し，フローテーブルを更新する部分
 2. flow_removedメッセージが到着したとき，トラフィック集計情報を更新する部分
 3. タイマーで10秒ごとにトラフィックの集計情報を表示する部分

//list[traffic_monitor_class][本体 @<tt>{TrafficMonitor} クラス（@<tt>{traffic-monitor.rb}）]{
require "counter"
require "fdb"

class TrafficMonitor < Controller
  periodic_timer_event :show_counter, 10  # (3)

  def start
    @counter = Counter.new  # Counterオブジェクト
    @fdb = FDB.new  # FDBオブジェクト
  end

  def packet_in datapath_id, message  # (1)
    macsa = message.macsa  # パケットを送信したホストのMACアドレス
    macda = message.macda  # パケットの宛先ホストのMACアドレス

    @fdb.learn macsa, message.in_port
    @counter.add macsa, 1, message.total_len
    out_port = @fdb.lookup( macda )
    if out_port
      packet_out datapath_id, message, out_port
      flow_mod datapath_id, macsa, macda, out_port
    else
      flood datapath_id, message
    end
  end

  def flow_removed datapath_id, message  # (2)
    @counter.add message.match.dl_src,message.packet_count, message.byte_count
  end

  private  # 以下、プライベートメソッド

  def show_counter  # カウンタを表示
    puts Time.now
    @counter.each_pair do | mac, counter |
      puts "#{ mac } #{ counter[ :packet_count ] } packets (#{ counter[ :byte_count ] } bytes)"
    end
  end

  def flow_mod datapath_id, macsa, macda, out_port  # macsaからmacdaへのパケットをout_portへ転送するflow_modを打つ
    send_flow_mod_add(
      datapath_id,
      :hard_timeout => 10,  # flow_modの有効期限は10秒
      :match => Match.new( :dl_src => macsa, :dl_dst => macda ),
      :actions => Trema::ActionOutput.new( out_port )
    )
  end

  def packet_out datapath_id, message, out_port  # packet_inしたメッセージをout_portへ転送
    send_packet_out(
      datapath_id,
      :packet_in => message,
      :actions => Trema::ActionOutput.new( out_port )
    )
  end

  def flood datapath_id, message  # packet_inしたメッセージをin_port以外の全スイッチポートへ転送
    packet_out datapath_id, message, OFPP_FLOOD
  end
end
//}

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

== 実行してみよう

それでは，早速実行してみましょう。@<list>{traffic_monitor_conf} の内容
の仮想ネットワーク設定を @<tt>{traffic-monitor.conf} として保存します。

//list[traffic_monitor_conf][仮想スイッチ 0xabc に仮想ホスト host1，host2 を接続する設定]{
vswitch {  # 仮想スイッチ 0xabc を定義
  datapath_id 0xabc
}

vhost ("host1") {  # 仮想ホスト host1 を定義
  ip "192.168.0.1"
  mac "00:00:00:00:00:01"
}

vhost ("host2") {  # 仮想ホスト host2 を定義
  ip "192.168.0.2"
  mac "00:00:00:00:00:02"
}

link "0xabc", "host1"  # ホスト host1、host2 をスイッチ 0xabc に接続
link "0xabc", "host2"
//}

次のように実行してください。

//cmd{
% ./trema run ./traffic-monitor.rb -c ./traffic-monitor.conf
//}

実行すると，図3に示した仮想ネットワークが構成され，Traffic Monitor コントローラが起動します。

それでは，実際にトラフィックを発生させて集計されるか見てみましょう。
Trema の @<tt>{send_packets} コマンドを使うと，仮想ホスト間で簡単にパケッ
トを送受信できます。別ターミナルを開き，次のコマンドを入力してください。

//cmd{
% ./trema send_packets --source host1 --dest host2 --n_pkts 10 --pps 10  # host1からhost2宛にパケットを10個送る
% ./trema send_packets --source host2 --dest host1 --n_pkts 10 --pps 10  # host2からhost1宛にパケットを10個送る
//}

trema runを実行した元のターミナルに次のような出力が出ていれば成功です（注3）。

//cmd{
……
00:00:00:00:00:01 10 packets (640 bytes)
↑host1からパケットが10個送信された

00:00:00:00:00:02 10 packets (640 bytes)
↑host2からパケットが10個送信された
……
//}


== まとめ/参考文献

「トラフィック集計機能付きスイッチ」を実現するコントローラを書きました。学んだことは次の2つです。

 * トラフィック情報はフローが消えるときに発生する flow_removed メッセー
   ジの中身を調べることで集計できます。フローの寿命はフローを書き込む
   flow_mod メッセージで指定できます。
 * 仮想ネットワークを使ったコントローラの動作テスト方法を学びました。仮
   想スイッチと仮想ホストを起動してつなぎ，send_packetsコマンドを使って
   仮想ホスト間でパケットを送受信することで，コントローラの簡単な動作テ
   ストができます。
