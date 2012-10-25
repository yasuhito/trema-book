= ルータ (後編)

== ソースコード
=== パケットを調べる

 * PacketIn クラスの各種アクセサ関数
 * 自分宛かを調べる
 * パケットの種類を調べる

=== ARP に応答する
=== ARP でアドレスを問い合わせる
=== ICMP に応答する

== 実行してみよう

 * 準備と実行
 * ICMP を送ってみる

=== 経路を探す
=== パケット転送のためのフローを作る
== 実行してみよう

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
