= OpenFlow の開発フレームワーク

//lead{
「さっそく OpenFlow で何かを作ってみたい!」その前に便利な OpenFlow の開発フレームワークを見てみましょう。巨人の肩に乗ってしまえば目的地まではもうすぐです。
//}

今の時代、新しい Web サービスを立ち上げるには Ruby on Rails などの Web アプリケーションフレームワークの利用が常識です。年々複雑化する Web サービスを作るには、Web アプリケーションフレームワークが提供するライブラリやツールが不可欠だからです。書店の技術書コーナーに行くと、さまざまな Web アプリケーションフレームワークの本があふれかえっています。

OpenFlow コントローラもフルスクラッチで作るのは大変です。OpenFlow の標準仕様は C で書いてあるので、まずは C が読めなければなりません。仕様が理解できたら、開発に使うプログラミング言語向けにライブラリを書き、その上にコントローラを構築し…考えただけでひと仕事です。動作テストのためのツール類も自分で準備しなければなりません。

そこで、OpenFlow コントローラフレームワークの出番です。Web 業界ほどではありませんが、世の中にはすでに主要なプログラミング言語向けの OpenFlow コントローラフレームワークがそろいつつあります。これらを使えば効率的にコントローラを開発できます。またいくつかのフレームワークは開発やデバッグに便利なツールをフレームワークの一部として提供しています。使わない手はありませんね。

@<table>{frameworks} に主な OpenFlow コントローラフレームワークを挙げます。いずれもいわゆるオープンソースソフトウェアで、それぞれ対応する言語が異なります。

//table[frameworks][主な OpenFlow コントローラフレームワーク]{
名前		開発言語		開発元										ライセンス
----------------------------------------------------------------------------------
Trema		Ruby			Trema プロジェクト							GPL2
NOX			C++				Nicira, スタンフォード大、UC バークレイ		GPL3
POX			Python			UC バークレイ								GPL3
Floodlight	Java			Big Switch Networks Inc.					Apache
//}

それでは、それぞれの特長を詳しく見ていきましょう。

== Trema

Trema は Ruby 用の OpenFlow コントローラフレームワークです (@<img>{trema})。GPL バージョン 2 ライセンスのフリーソフトウェアです。

//image[trema][Trema のサイト (@<tt>{http://trema.github.com/trema})]

ターゲット言語が Ruby であることからもわかるとおり、Trema の最大の特長は実行速度よりも開発効率に重きを置いていることです。たとえば、Trema を使うと他のフレームワークに比べて大幅に短いコードでコントローラを実装できます。@<list>{trema_hub} は Trema で書いたコントローラの一例ですが、たった 14 行のコードだけでハブとして動作する完全なコントローラが書けます。

//list[trema_hub][Trema で書いたコントローラ (ハブ) の例]{
class RepeaterHub < Controller
  def packet_in datapath_id, message
    send_flow_mod_add(
      datapath_id,
      :match => ExactMatch.from( message ),
      :actions => SendOutPort.new( OFPP_FLOOD )
    )
    send_packet_out(
      datapath_id,
      :packet_in => message,
      :actions => SendOutPort.new( OFPP_FLOOD )
    )
  end
end
//}

開発効率向上のしくみとして、Trema にはコントローラ開発に役立つツールが充実しています。その中でも強力なツール、ネットワークエミュレータはコントローラのテストに便利です。これはノート PC 1 台でコントローラを開発できるというもので、仮想スイッチと仮想ホストを組み合わせた任意の仮想環境上でコントローラを実行できます。もちろん、こうして開発したコントローラは実際のネットワーク上でもそのまま動作します。

== NOX

NOX は OpenFlow の生まれ故郷スタンフォード大で開発されたもっとも古いフレームワークで、C++ に対応しています (@<img>{nox})。ライセンスは GPL バージョン 3 のフリーソフトウェアです。

//image[nox][NOX のサイト (@<tt>{http://www.noxrepo.org/nox/about-nox/})]

NOX の長所はユーザ層の厚さです。OpenFlow の登場直後から開発しており、メーリングリストでは OpenFlow 仕様を作った研究者本人など、SDN の主要な関係者が活発に議論しています。また歴史が古いため、Web で情報を集めやすいという利点もあります。

最後に NOX のサンプルコードとして、Trema と同じくハブを実装した例を紹介します (@<list>{nox_hub})。

//list[nox_hub][NOX で書いたコントローラ (ハブ) の例]{
#include <boost/bind.hpp>
#include <boost/shared_array.hpp>
#include "assert.hh"
#include "component.hh"
#include "flow.hh"
#include "packet-in.hh"
#include "vlog.hh"

#include "netinet++/ethernet.hh"

namespace {

using namespace vigil;
using namespace vigil::container;

Vlog_module lg("hub");

class Hub 
    : public Component 
{
public:
     Hub(const Context* c,
         const json_object*) 
         : Component(c) { }

    void configure(const Configuration*) {
    }

    Disposition handler(const Event& e)
    {
        const Packet_in_event& pi = assert_cast<const Packet_in_event&>(e);
        uint32_t buffer_id = pi.buffer_id;
        Flow flow(pi.in_port, *(pi.get_buffer()));

        if (flow.dl_type == ethernet::LLDP){
            return CONTINUE;
        }

        ofp_flow_mod* ofm;
        size_t size = sizeof *ofm + sizeof(ofp_action_output);
        boost::shared_array<char> raw_of(new char[size]);
        ofm = (ofp_flow_mod*) raw_of.get();

        ofm->header.version = OFP_VERSION;
        ofm->header.type = OFPT_FLOW_MOD;
        ofm->header.length = htons(size);
        ofm->match.wildcards = htonl(0);
        ofm->match.in_port = htons(flow.in_port);
        ofm->match.dl_vlan = flow.dl_vlan;
        ofm->match.dl_vlan_pcp = flow.dl_vlan_pcp;
        memcpy(ofm->match.dl_src, flow.dl_src.octet, sizeof ofm->match.dl_src);
        memcpy(ofm->match.dl_dst, flow.dl_dst.octet, sizeof ofm->match.dl_dst);
        ofm->match.dl_type = flow.dl_type;
        ofm->match.nw_src = flow.nw_src;
        ofm->match.nw_dst = flow.nw_dst;
        ofm->match.nw_proto = flow.nw_proto;
        ofm->match.tp_src = flow.tp_src;
        ofm->match.tp_dst = flow.tp_dst;
        ofm->cookie = htonl(0);
        ofm->command = htons(OFPFC_ADD);
        ofm->buffer_id = htonl(buffer_id);
        ofm->idle_timeout = htons(5);
        ofm->hard_timeout = htons(5);
        ofm->priority = htons(OFP_DEFAULT_PRIORITY);
        ofm->flags = htons(0);
        ofp_action_output& action = *((ofp_action_output*)ofm->actions);
        memset(&action, 0, sizeof(ofp_action_output));
        action.type = htons(OFPAT_OUTPUT);
        action.len = htons(sizeof(ofp_action_output));
        action.port = htons(OFPP_FLOOD);
        action.max_len = htons(0);
        send_openflow_command(pi.datapath_id, &ofm->header, true);
        free(ofm);

        if (buffer_id == UINT32_MAX) {
            size_t data_len = pi.get_buffer()->size();
            size_t total_len = pi.total_len;
            if (total_len == data_len) {
                send_openflow_packet(pi.datapath_id, *pi.get_buffer(), 
                        OFPP_FLOOD, pi.in_port, true);
            }
        }

        return CONTINUE;
    }

    void install()
    {
        register_handler<Packet_in_event>(boost::bind(&Hub::handler, this, _1));
    }
};

REGISTER_COMPONENT(container::Simple_component_factory<Hub>, Hub);

}
//}

== POX

POX は NOX から派生したプロジェクトで、Python でのコントローラ開発に対応したフレームワークです (@<img>{pox})。ライセンスは GPL バージョン 3 のフリーソフトウェアです。

//image[pox][POX のサイト (@<tt>{http://www.noxrepo.org/pox/about-pox/})]

POX の特長は実装が Pure Python であるため、Linux, Mac, Windows のいずれでも OS を問わず動作することです。まだまだ若いプロジェクトであるためサンプルアプリケーションの数は少ないものの、Python プログラマには注目のプロジェクトです。

最後に POX のサンプルコードとして、同じくハブを実装した例を紹介します (@<list>{pox_hub})。

//list[pox_hub][POX で書いたコントローラ (ハブ) の例]{
from pox.core import core
import pox.openflow.libopenflow_01 as of

class RepeaterHub (object):
  def __init__ (self, connection):
    self.connection = connection
    connection.addListeners(self)

  def send_packet (self, buffer_id, raw_data, out_port, in_port):
    msg = of.ofp_packet_out()
    msg.in_port = in_port
    if buffer_id != -1 and buffer_id is not None:
      msg.buffer_id = buffer_id
    else:
      if raw_data is None:
        return
      msg.data = raw_data
    action = of.ofp_action_output(port = out_port)
    msg.actions.append(action)
    self.connection.send(msg)

  def act_like_hub (self, packet, packet_in):
    self.send_packet(packet_in.buffer_id, packet_in.data,
                     of.OFPP_FLOOD, packet_in.in_port)

  def _handle_PacketIn (self, event):
    packet = event.parsed
    if not packet.parsed:
      return
    packet_in = event.ofp # The actual ofp_packet_in message.
    self.act_like_hub(packet, packet_in)

def launch ():
  def start_switch (event):
    RepeaterHub(event.connection)
  core.openflow.addListenerByName("ConnectionUp", start_switch)
//}

== Floodlight

Floodlight は Java 用のフレームワークです (@<img>{floodlight})。ライセンスは Apache のフリーソフトウェアです。

//image[floodlight][Floodlight のサイト (@<tt>{http://www.noxrepo.org/pox/about-pox/})]

Floodlight の特長はずばり、プログラマ人口の多い Java を採用していることです。最近は大学のカリキュラムで最初に Java を学ぶことが多いため、大部分の人にとって最もとっつきやすいでしょう。また実装が Pure Java であるため、POX と同じく OS を問わず動作するという利点もあります。

最後に Floodlight のサンプルコードとして、同じくハブを実装した例を紹介します (@<list>{floodlight_hub})。

//list[floodlight_hub][Floodlight で書いたコントローラ (ハブ) の例]{
package net.floodlightcontroller.hub;

import java.io.IOException;
import java.util.ArrayList;
import java.util.Collection;
import java.util.Collections;
import java.util.Map;
import net.floodlightcontroller.core.FloodlightContext;
import net.floodlightcontroller.core.IFloodlightProviderService;
import net.floodlightcontroller.core.IOFMessageListener;
import net.floodlightcontroller.core.IOFSwitch;
import net.floodlightcontroller.core.module.FloodlightModuleContext;
import net.floodlightcontroller.core.module.FloodlightModuleException;
import net.floodlightcontroller.core.module.IFloodlightModule;
import net.floodlightcontroller.core.module.IFloodlightService;
import org.openflow.protocol.OFMessage;
import org.openflow.protocol.OFPacketIn;
import org.openflow.protocol.OFPacketOut;
import org.openflow.protocol.OFPort;
import org.openflow.protocol.OFType;
import org.openflow.protocol.action.OFAction;
import org.openflow.protocol.action.OFActionOutput;
import org.openflow.util.U16;
import org.slf4j.Logger;
import org.slf4j.LoggerFactory;

public class Hub implements IFloodlightModule, IOFMessageListener {
    protected static Logger log = LoggerFactory.getLogger(Hub.class);
    protected IFloodlightProviderService floodlightProvider;

    public void setFloodlightProvider(IFloodlightProviderService floodlightProvider) {
        this.floodlightProvider = floodlightProvider;
    }

    @Override
    public String getName() {
        return Hub.class.getPackage().getName();
    }

    public Command receive(IOFSwitch sw, OFMessage msg, FloodlightContext cntx) {
        OFPacketIn pi = (OFPacketIn) msg;
        OFPacketOut po = (OFPacketOut) floodlightProvider.getOFMessageFactory()
                .getMessage(OFType.PACKET_OUT);
        po.setBufferId(pi.getBufferId())
            .setInPort(pi.getInPort());

        OFActionOutput action = new OFActionOutput()
            .setPort((short) OFPort.OFPP_FLOOD.getValue());
        po.setActions(Collections.singletonList((OFAction)action));
        po.setActionsLength((short) OFActionOutput.MINIMUM_LENGTH);

        if (pi.getBufferId() == 0xffffffff) {
            byte[] packetData = pi.getPacketData();
            po.setLength(U16.t(OFPacketOut.MINIMUM_LENGTH
                    + po.getActionsLength() + packetData.length));
            po.setPacketData(packetData);
        } else {
            po.setLength(U16.t(OFPacketOut.MINIMUM_LENGTH
                    + po.getActionsLength()));
        }
        try {
            sw.write(po, cntx);
        } catch (IOException e) {
            log.error("Failure writing PacketOut", e);
        }

        return Command.CONTINUE;
    }

    @Override
    public boolean isCallbackOrderingPrereq(OFType type, String name) {
        return false;
    }

    @Override
    public boolean isCallbackOrderingPostreq(OFType type, String name) {
        return false;
    }

    @Override
    public Collection<Class<? extends IFloodlightService>> getModuleServices() {
        return null;
    }

    @Override
    public Map<Class<? extends IFloodlightService>, IFloodlightService>
            getServiceImpls() {
        return null;
    }

    @Override
    public Collection<Class<? extends IFloodlightService>>
            getModuleDependencies() {
        Collection<Class<? extends IFloodlightService>> l = 
                new ArrayList<Class<? extends IFloodlightService>>();
        l.add(IFloodlightProviderService.class);
        return l;
    }

    @Override
    public void init(FloodlightModuleContext context)
            throws FloodlightModuleException {
        floodlightProvider =
                context.getServiceImpl(IFloodlightProviderService.class);
    }

    @Override
    public void startUp(FloodlightModuleContext context) {
        floodlightProvider.addOFMessageListener(OFType.PACKET_IN, this);
    }
}
//}

== どれを選べばいい？

では、いくつもあるフレームワークのうちどれを使えばいいでしょうか？まっとうな答は「自分が使い慣れた言語をサポートするフレームワークを使え」です。つまり、あなたが Ruby プログラマであれば Trema 一択ですし、C++ プログラマでなら NOX 一択ということです。

これを裏付けるものとして、名著「コード・コンプリート (Steve McConell 著、日経BP ソフトプレス)」に説得力のあるデータがあります。

//quote{
プログラマの生産性は、使い慣れた言語を使用したときの方が、そうでない言語を使用したときよりも向上する。COCOMO II という見積もりモデルがはじき出したデータによると、3 年以上使っている言語で作業しているプログラマの生産性は、ほぼ同じ経験を持つプログラマが始めての言語を使っている場合の生産性を、約 30% 上回る (Boehm et al. 2000)。これに先立って行われた IBMの調査では、あるプログラミング言語での経験が豊富なプログラマは、その言語にほとんど経験のないプログラマの 3 倍以上の生産性があることがわかっている (Walston and Felix 1977)。
//}

//noindent
これはごくあたりまえの原則ですが、プログラミングの現場では無視されていることが少なくありません。「上司が使えと言ったから」「流行っているらしいから」という理由でなんとなくフレームワークを選び、そしてプロジェクトが炎上するというケースが後をたちません。かならず、プログラマ自身が慣れたプログラミング言語で作ることが重要です。

いっぽうで、プログラマがいくつもの言語に習熟していた場合、それらの言語の間に明らかな生産性の差が出てくるのも事実です。C や C++ のような明示的にメモリ管理が必要な低水準言語と、これにガベージ・コレクションを付け加えた Java や C# のような言語、また最近の Ruby や Python のように、さらに高レベルで記述できるスクリプティング言語では、生産性と品質に何十倍もの差が出ます。さきほどの「コード・コンプリート」をふたたび引きましょう。

//quote{
高級言語を使って作業するプログラマの生産性と品質は、低水準言語を使用するプログラマより高い。... C 言語のように、ステートメントが仕様どおりに動いたからといって、いちいち祝杯をあげる必要がなければ、時間が節約できるものというものだ。そのうえ、高級言語は低水準言語よりも表現力が豊かである。つまり、1 行のコードでより多くの命令を伝えることができる。
//}

このことは、今まで見てきたハブ実装のコード行数を比べても明らかです (@<img>{comparison})。

//image[comparison][主なフレームワークでハブを実装したときのコード行数を比較]

//noindent
Trema (Ruby) や POX (Python) などスクリプティング言語を採用するフレームワークでは短い行数で実装できていますが、NOX (C++) や Floodlight (Java) など従来の言語を採用するフレームワークでは一気に行数がはねあがっています。とくに、最も短い Trema (14 行) と最も長い Floodlight (111 行) を比べるとその差は 8 倍にもなります。単純には言えませんが、行数だけで見ると Trema は Floodlight の 1/8 の労力で同じ機能を実装できるのです。

筆者の予想では、OpenFlow コントローラフレームワークはかつての Web アプリケーションフレームワークと同じ道をたどるのではと思っています。歴史をさかのぼると、1990 年代〜 2000 年代初頭は Java 用フレームワーク全盛期でした。無数の Java 用フレームワークが雨後の竹の子のように登場し、Java EE、JSP、JSF など新しい技術も次々と出てきました。IDE が自動生成する長いコードや XML ファイルと格闘しながら、次々と登場する新しい仕様を理解して Web アプリケーションを書くのは至難の業でした。しかし 2004 年、Ruby の Web アプリケーションフレームワークである Ruby on Rails の登場によって Web 業界は一変します。Java による鈍重な実装は避け、なるべく短いコードで書こうという考え方が Web 業界を席巻したのです。この流れは、「コードが長くなるフレームワーク」の代名詞であった Java の世界にも取り入れられ、最近の Django や Play など近代的なフレームワークを産んできました。

OpenFlow コントローラフレームワークはまだまだ黎明期にあります。Trema のように最近の考えかたを取り入れたフレームワークはありますが、とくに海外では NOX や Floodlight など旧来的なフレームワークが主流を占めています。しかし、ネットワーク業界でもスクリプティング言語を使えるプログラマが増えれば、古い設計のフレームワークを使うプログラマよりも何倍もの生産性をあげることができるようになるでしょう。そしてこの考え方が順調に浸透していけば、さまざまな言語で生産性の高いフレームワークが登場するはずです。

== その他のツール (Oflops ベンチマーク)

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


=== Cbench の実行例 (Trema の場合)   

Trema は Cbench および Cbench と接続できるコントローラを含むので、この 2 つのベンチマークを簡単に実行できます。次のコマンドは、Cbench をレイテンシモードとスループットモードで実行し結果を表示します (Trema のインストール方法は続く@<chap>{openflow_framework_trema}で説明します)。

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

=== Cbench の注意点

Cbench のスコアを盲信しないようにしてください。現在、いくつかの OpenFlow コントローラフレームワークは Cbench のスコアだけを競っているように見えます。たとえば Floodlight は 1 秒間に 100 万発の Flow Mod を打てると宣伝しています。これはなかなかすごい数字です。きちんと計算したわけではないですが、スレッドを駆使してめいっぱい I/O を使い切るようにしなければなかなかこの数字は出ません。とにかくすごい。でも、この数字にはまったく意味がありません。

Flow Mod を一秒間に 100 万発打たなければならない状況を考えてみてください。それは、Packet In が一秒間に 100 万発起こる状況ということになります。Packet In が一秒間に 100 万発起こるとはどういうことでしょうか? スイッチに何らかのフローが設定されているが入ってきたパケットがまったくそれにマッチせず、どうしたらいいかわからないパケットがすべてコントローラへやってくる、これが一秒間に 100 万回起こるということです。何かがまちがっていると思えないでしょうか？

コントローラが Packet In を何発さばけるかという性能は、極端に遅くない限りは重要ではありません。データセンターのように、どこにどんなマシンがありどういう通信をするか把握できている場合は、フローをちゃんと設計していれば Packet In はそんなに起こらないからです。力技で Packet In をさばくよりも、いかに Packet In が起こらないネットワーク設計やフロー設計をするかの方がずっと大事です。

Cbench のようなマイクロベンチマークでは、測定対象が何でその結果にはどんな意味があるか？を理解しないと針小棒大な結論を招きます。Cbench のスコアは参考程度にとどめましょう。

== まとめ

本章では現在利用できる主な OpenFlow コントローラフレームワークを紹介しました。すでに主要な言語のフレームワークがそろっているので、自分の使う言語に合わせてフレームワークを選択できます。

もし生産性の高いフレームワークをお望みであれば Trema か POX を選択してください。流れの速い SDN 業界では、実行効率よりも「いかに早くサービスインできるか」という生産性の方がずっと重要だからです。続く第 II 部では、Trema を使った OpenFlow プログラミングを学習します。Ruby の基礎から解説しますので、Ruby が初めてのプログラマでも読み進められるようにしてあります。
