= OpenFlow のプロジェクト

//lead{
「さっそく OpenFlow で何かを作ってみよう!」その前に先人たちの所産を見てみましょう。巨人の肩に乗れば目的地まではあっという間です。
//}

独自の OpenFlow コントローラをフルスクラッチで作るのは大変です。OpenFlow の標準仕様は C で書いてあるので、まずは C が読めなければなりません。仕様が理解できたら、開発に使うプログラミング言語向けにライブラリを書き、その上にコントローラを構築し…考えただけでひと仕事です。動作テストのためのツール類も自分で準備しなければなりません。

そこで、OpenFlow ライブラリやフレームワークの出番です。世の中にはすでに主要なプログラミング言語向けの OpenFlow ライブラリがそろいつつあるので、それを使えば効率的にコントローラを作れます。またいくつかのフレームワークは開発やデバッグに便利なツールをフレームワークの一部として提供しています。これらを使わない手はありませんね。

@<table>{frameworks} に主な OpenFlow 開発フレームワークを挙げます。いずれもライセンスは異なりますがいわゆるオープンソースソフトウェアで、それぞれ対応する言語が異なります。

//table[frameworks][主な OpenFlow 開発フレームワーク]{
名前		開発言語		開発元										ライセンス
----------------------------------------------------------------------------------
Trema		Ruby			Trema プロジェクト							GPL2
NOX			C++				Nicira, スタンフォード大、UC バークレイ		GPL3
POX			Python			UC バークレイ								GPL3
Floodlight	Java			Big Switch									Apache
Nettle		Haskell			イエール大									BSD3
//}

それでは、それぞれの特長を詳しく見ていきましょう。

== Trema

Trema は Ruby 用の OpenFlow 開発フレームワークです (@<img>{trema})。GPL バージョン 2 ライセンスのフリーソフトウェアです。

//image[trema][Trema のサイト (@<tt>{http://trema.github.com/trema})]

ターゲット言語が Ruby であることからもわかるとおり、Trema の最大の特長は実行速度よりも開発効率に重きを置いていることです。たとえば、Trema を使うと他のフレームワークに比べて大幅に短いコードでコントローラを実装できます。@<list>{repeater_hub} は Trema で書いたコントローラの一例ですが、たった 14 行のコードだけでハブとして動作する完全なコントローラが書けます。

//list[repeater_hub][Trema で書いたコントローラの例]{
class RepeaterHub < Controller
  def packet_in datapath_id, message
    send_flow_mod_add(
      datapath_id,
      :match => ExactMatch.from( message ),
      :actions => ActionOutput.new( OFPP_FLOOD )
    )
    send_packet_out(
      datapath_id,
      :packet_in => message,
      :actions => ActionOutput.new( OFPP_FLOOD )
    )
  end
end
//}

開発効率のためのもうひとつの特長は、コントローラ開発に役立つツールが充実していることです。その中でも強力なツール、ネットワークエミュレータはコントローラのテストに便利です。これは実機のスイッチやホストを準備しなくともノート PC 1 台でコントローラの開発から動作確認までができるというもので、開発マシン内に作った仮想ネットワーク上でコントローラを実行できます。もちろん、こうして開発したコントローラは実際のネットワークでもそのまま動作します。

このネットワークエミュレータは、仮想スイッチと仮想ホストを組み合わせた任意のトポロジを構築できます。たとえば @<img>{sample_topology} はスイッチ 4 台、ホスト 4 台からなる簡単なトポロジですが、Trema のネットワーク設定ファイルを使うと@<list>{network_emulator_conf} のように記述できます。これをコントローラの実行時に指定することで、仮想ネットワーク上にコントローラが起動します。

//image[sample_topology][スイッチ 4 台、ホスト 4 台のトポロジ][scale=0.3]

//list[network_emulator_conf][ネットワーク設定ファイルの例]{
vswitch("switch1") { datapath_id "0x1" }
vswitch("switch2") { datapath_id "0x2" }
vswitch("switch3") { datapath_id "0x3" }
vswitch("switch4") { datapath_id "0x4" }

vhost("host1")
vhost("host2")
vhost("host3")
vhost("host4")

link "switch1", "host1"
link "switch2", "host2"
link "switch3", "host3"
link "switch4", "host4"
link "switch1", "switch2"
link "switch2", "switch3"
link "switch3", "switch4"
//}

== NOX

NOX は C++ と Python に対応したフレームワークで、OpenFlow の登場から比
較的早い時期に公開されたため他のフレームワークと比べて広く使われていま
す。しかし NOX 自体の開発は終了し、Python 部分が POX プロジェクトとして
分離し、一からの作り直しへと移行しています。

== POX

== Floodlight

Floodlight は Java 向けのフ
レームワークであり、機能拡張をするためのモジュール化されたアーキテクチャ
を採用しています。

== どれを選べばいい？

では、このたくさんあるフレームワークのうちどれを使えばいいでしょうか？
まっとうな答は「あなたが慣れた言語をサポートするフレームワークを使え」
です。つまり、あなたが C++ プログラマであれば NOX 一択ですし、Ruby プロ
グラマなら Trema 一択ということです。

これを裏付けるものとして「コード・コンプリート (Steve McConell 著、日経
BP ソフトプレス)」に説得力のあるデータがあります。

//quote{
プログラマの生産性は、使い慣れた言語を使用したときの方が、そうでない言
語を使用したときよりも向上する。COCOMO II という見積もりモデルがはじき
出したデータによると、3 年以上使っている言語で作業しているプログラマの
生産性は、ほぼ同じ経験を持つプログラマが始めての言語を使っている場合の
生産性を、約 30% 上回る (Boehm et al.2000)。これに先立って行われた IBM
の調査では、あるプログラミング言語での経験が豊富なプログラマは、その言
語にほとんど経験のないプログラマの 3 倍以上の生産性があることがわかって
いる (Walston and Felix 1977)。
//}

いっぽうで、プログラマがいくつもの言語に習熟していた場合、それらの言語
の間に明らかな生産性の差が出てくるのも事実です。C や C++ のような明示的
にメモリ管理が必要な低水準言語と、これにガベージ・コレクションを付け加
えた Java や C# のような言語、また最近の Ruby や Python のように、さら
に高レベルで記述できるスクリプティング言語では、生産性と品質に何十倍も
の差が出ます。さきほどの「コード・コンプリート」をふたたび引きましょう。

//quote{
高級言語を使って作業するプログラマの生産性と品質は、低水準言語を使用す
るプログラマより高い。... C 言語のように、ステートメントが仕様どおりに
動いたからといって、いちいち祝杯をあげる必要がなければ、時間が節約でき
るものというものだ。そのうえ、高級言語は低水準言語よりも表現力が豊かで
ある。つまり、1 行のコードでより多くの命令を伝えることができる。
//}

== その他のツール

=== Oflops

Oflops は OpenFlow コントローラとスイッチのためのマイクロベンチマークです。コントローラ用のベンチマーク Cbench とスイッチ用のベンチマーク OFlops を提供します。スイッチを作る機会はめったにないのでここではコントローラのベンチマークである Cbench について説明します。

Cbench は「1 秒あたりにコントローラが出せる Flow Mod の数」を計測します。Cbench はスイッチのふりをしてコントローラに接続し、コントローラに Packet In を送ります。これに反応したコントローラからの Flow Mod の数をカウントし、スコアとします。このスコアが大きいコントローラほど「速い」とみなすのです。

Cbench は次の 2 種類のベンチマークをサポートします。

//noindent
@<em>{レイテンシモード}

 1. Packet In をコントローラに送り、
 2. コントローラから Flow Mod が帰ってくるのを待ち
 3. それを繰り返す。

//noindent
@<em>{スループットモード}

 1. Flow Mod を待たずに Packet In を送信し続け、
 2. Flow Mod が返信されたらカウントする。


=== 実行のしかた (Trema の場合)   

Trema は Cbench および Cbench と接続できるコントローラを含むので、この 2 つのベンチマークを簡単に実行できます。次のコマンドは、Cbench をレイテンシモードとスループットモードで実行し結果を表示します。

//cmd{
% ./build.rb cbench
./trema run src/examples/cbench_switch/cbench-switch.rb -d
/home/yasuhito/play/trema/objects/oflops/bin/cbench --switches 1 --loops 10 --delay 1000
cbench: controller benchmarking tool
   running in mode 'latency'
   connecting to controller at localhost:6633 
   faking 1 switches :: 10 tests each; 1000 ms per test
   with 100000 unique source MACs per switch
   starting test with 1000 ms delay after features_reply
   ignoring first 1 "warmup" and last 0 "cooldown" loops
   debugging info is off
1   switches: fmods/sec:  10353   total = 10.352990 per ms 
1   switches: fmods/sec:  10142   total = 10.141990 per ms 
1   switches: fmods/sec:  10260   total = 10.259990 per ms 
1   switches: fmods/sec:  10736   total = 10.734497 per ms 
1   switches: fmods/sec:  10884   total = 10.883989 per ms 
1   switches: fmods/sec:  10752   total = 10.751989 per ms 
1   switches: fmods/sec:  10743   total = 10.742989 per ms 
1   switches: fmods/sec:  10828   total = 10.827989 per ms 
1   switches: fmods/sec:  10454   total = 10.453990 per ms 
1   switches: fmods/sec:  10642   total = 10.641989 per ms 
RESULT: 1 switches 9 tests min/max/avg/stdev = 10141.99/10883.99/10604.38/245.53 responses/s
./trema killall
./trema run src/examples/cbench_switch/cbench-switch.rb -d
/home/yasuhito/play/trema/objects/oflops/bin/cbench --switches 1 --loops 10 --delay 1000 --throughput
cbench: controller benchmarking tool
   running in mode 'throughput'
   connecting to controller at localhost:6633 
   faking 1 switches :: 10 tests each; 1000 ms per test
   with 100000 unique source MACs per switch
   starting test with 1000 ms delay after features_reply
   ignoring first 1 "warmup" and last 0 "cooldown" loops
   debugging info is off
1   switches: fmods/sec:  36883   total = 36.761283 per ms 
1   switches: fmods/sec:  36421   total = 36.398433 per ms 
1   switches: fmods/sec:  37286   total = 37.174106 per ms 
1   switches: fmods/sec:  36559   total = 36.526637 per ms 
1   switches: fmods/sec:  36072   total = 36.007331 per ms 
1   switches: fmods/sec:  34130   total = 33.993855 per ms 
1   switches: fmods/sec:  32119   total = 32.086016 per ms 
1   switches: fmods/sec:  33733   total = 33.533876 per ms 
1   switches: fmods/sec:  33270   total = 33.262582 per ms 
1   switches: fmods/sec:  32119   total = 32.107056 per ms 
RESULT: 1 switches 9 tests min/max/avg/stdev = 32086.02/37174.11/34565.54/1866.96 responses/s
./trema killall
//}

=== Cbench の使いみち

Cbench はマイクロベンチマークですのでスコアを盲信しないように注意してください。どんなマイクロベンチマークでもそうですが、測定対象が何でその数字にはどんな意味があるか？を理解しないと針小棒大な結論を招きます。Cbench の使いみちとしては、自作 OpenFlow ライブラリやフレームワークのテストに使うと良いでしょう。

#@warn(「cbench は無意味」コラムを取間先生で)

== まとめ/参考文献
