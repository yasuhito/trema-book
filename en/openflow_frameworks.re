= OpenFlow Development Framework

//lead{
Okay, so let's make something with OpenFlow already! But before that, let's take a look at some convenient OpenFlow development frameworks. Standing on the shoulders of giants will take you to your destination in a breeze.
//} 

//indepimage[robot][][width=10cm]

== Let's make use of the development framework

Nowadays, Web application frameworks such as Ruby on Rails@<fn>{rails> is indispensable when launching a new Web service. Without such framework's help, constructing the Web site from scratch will not only result in tremendous amount of codings but also in reinventing the wheel. You could easily find books on various frameworks for programmers who want to effectively create Web sites in book stores' technology section. So one can say that using a framework to build a Web service is already a common sense. 

//footnote[rails][@<href>{http://rubyonrails.org/}]

It's also a lot of work to build a OpenFlow controller from scratch. The standard specification of OpenFlow is written in C so it's essential that you can understand the language. After comprehending the specification, you have to write a library for the programming language that's used in the development, 
construct the controller on top of that, and … it's quite a work just by thinking. On top of that, you would need to prepare some testing tools on your own as well.

This is where the OpenFlow controller frameworks come in. They don't go as far as the ones in the Web industry, but there are already several OpenFlow controller frameworks for major programming languages ready to be used out there. Some of the frameworks provide convenient tools for developing and debugging as a part of the framework. There are no reason not to use them. 

Major OpenFlow controller frameworks are listed in @<table>{frameworks}. They are all so-called open source software and the difference is the languages they are developed in.

//table[frameworks][Major OpenFlow controller framework]{
Name		Language		Developed by										License
----------------------------------------------------------------------------------
Trema		Ruby			Trema Project						GPL2
NOX			C++				Nicira, Stanford University, UC Berkeley		GPL3
POX			Python			UC Berkeley								GPL3
Floodlight	Java			Big Switch Networks Inc.					Apache
//}

Now let's take a look at their details.

== Trema

Trema is an OpenFlow controller framework in Ruby(@<img>{trema}) and it's a free software with GPL version 2 license. 

//image[trema][Trema Website (@<href>{http://trema.github.com/trema})][width=12cm]

The most distinctive feature of Trema is that it puts emphasis on the development efficiency than the execution speed, as you can see from the fact that it targets Ruby. For example, implementing a controller by using Trema drastically reduces the code length compared to other frameworks. @<list>{trema_hub} is an example of a controller written by Trema and by only 14 lines, one can write a full controller that works as a hub. 

//list[trema_hub][Example of a controller (hub) written by Trema]{
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

Trema has a plenty of tools that helps developing the controller, and one of the most powerful tool is the network emulator which is handy when testing the controller. This means that controllers can be developed with just a notebook PC and controllers can be implemented on arbitrary virtual environment by combining virtual switches and virtual hosts. Of course, controllers developed in this way works the same in the real network. 

== NOX

NOX is the oldest framework developed in Stanford University where OpenFlow was born, using C++(@<img>{nox}). It is a free software with GPL version 3 license. 

//image[nox][NOX Website (@<href>{http://www.noxrepo.org/nox/about-nox/})][width=12cm]

NOX is known for its large number of users. It's been developed since the birth of the OpenFlow and the key persons of SDN including the researchers who devised the OpenFlow specification are engaged in a lively discussion over the mailing list. In addition, the accumulated information over the past years assists developers in need.

Below is a sample NOX code for implementing the same hub that was shown with Trema(@<list>{nox_hub}).

//list[nox_hub][Example of a controller (hub) written by NOX]{
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

POX is a project that was derived from NOX and a framework for developing controllers in Python(@<img>{pox}). It's a free software with GPL version 3 license. 

//image[pox][POX Website (@<href>{http://www.noxrepo.org/pox/about-pox/})][width=12cm]

POX works on regardless of the OS whether it's Linux, Mac, or Windows since POX is implemented with Pure Python. There aren't a lot of sample application since it's rather a 'young' project but it's getting attentions from Python programmers. 

Below is a sample POX code for implementing the same hub (@<list>{pox_hub}).

//list[pox_hub][Example of a controller (hub) written by POX]{
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

Floodlight is a framework in Java(@<img>{floodlight}) with the Apache license. 

//image[floodlight][Floodlight Website (@<href>{http://www.noxrepo.org/pox/about-pox/})][width=12cm]

The attribute of Floodlight boils down to adopting Java which has a large population of programmers. Java is one of the first programming languages taught in universities these days so it should be easily accessible for most people. In addition, the implementation is in Pure Java so it works on regardless of the OS like POX.

Below is a sample Floodlight code for implementing the same hub (@<list>{floodlight_hub}).

//list[floodlight_hub][Example of a controller (hub) written by Floodlight]{
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

== Which one should you use?

So, how does one choose a framework? A proper answer would be, "Use a framework that supports the language that the developers are used to". In other words, if you have a team of programmers who are Ruby users, Trema would be the only choice and for C++ programmers, NOX would be the answer.

To back up this statement, there's a convincing data in the great book,『Code Complete: A Practical Handbook of Software Construction, Second Edition』@<fn>{codecomplete}.

//footnote[codecomplete][Steve McConell/Microsoft Press]

//quote{
Programmers are more productive using a familiar language than an unfamiliar one. Data from the Cocomo II estimation model shows that programmers working in a language they've used for three years or more are about 30 percent more productive than programmers with equivalent experience who are new to a language (Boehm et al. 2000). An earlier study at IBM found that programmers who had extensive experience with a programming language were more than three times as productive as those with minimal experience (Walston and Felix 1977)
//}

This is just a matter-of-fact-principle but it's often ignored in programming fields. People would say "Because I was told by my boss", "Everybody seems to be using it", and etc as a reason for choosing a framework only to find out that the project is on the death march. Programmers must use the programming language that they are familiar with. 

On the other hand, when a programmer is proficient in several languages, it's a fact that there's an apparent difference in productivity among the languages. The productivity and the quality is some ten times different between the low-level languages that need explicit memory management such as C and C++, languages that append garbage collection such as Java and C#, and high-level scripting languages such as Ruby and Python. Let us refer to 『Code Complete』 again.

//quote{
Programmers working with high-level languages achieve better productivity and quality than those working with lower-level languages. (...) You save time when you don't need to have an awards ceremony every time a C statement does what it's supposed to. Moreover, higher-level languages are more expressive than lower-level languages. Each line of code says more.
//}

This is obvious when you compare the number of code lines for implementing the hubs(@<img>{comparison}).

//image[comparison][Comparison of number of code lines to implement hubs by major frameworks][width=12cm]

//noindent
Frameworks that adopt scripting languages such as Trema (Ruby) and POX (Python) can be implemented with a small number of code lines but those with conventional languages such as NOX (C++) or Floodlight (Java) require much more lines. Especially for Floodlight, 111 lines are needed whereas only 14 lines suffice for Trema. It's hard to say the number code lines is directly proportional to the programming effort but Trema is about 1/8 of Floodlight. 

So, which framework would you choose?

===[column] Mr. Torema says: どうなる！？OpenFlowコントローラ開発の今後

私の予想では、OpenFlowコントローラフレームワークはかつてのWebアプリケーションフレームワークと同じ道をたどるのではと思っています。歴史をさかのぼると、1990年代〜2000年代初頭はJava用フレームワーク全盛期でした。無数のJava用フレームワークが雨後の竹の子のように登場し、Java EE、JSP、JSFなど新しい技術も次々と出てきました。IDEが自動生成する長いコードやXMLファイルと格闘しながら、次々と登場する新しい仕様を理解してWebアプリケーションを書くのは至難の業でした。しかし2004年、RubyのWebアプリケーションフレームワークであるRuby on Railsの登場によってWeb業界は一変します。Javaによる鈍重な実装は避け、なるべく短いコードで書こうという考え方がWeb業界を席巻したのです。この流れは、「コードが長くなるフレームワーク」の代名詞であったJavaの世界にも取り入れられ、最近のDjangoやPlayなど近代的なフレームワークを産んできました。

OpenFlowコントローラフレームワークはまだまだ黎明期にあります。TremaやPOXのように最近の考えかたを取り入れたフレームワークはありますが、とくに海外ではNOXやFloodlightなど旧来的なフレームワークが主流を占めています。しかし、ネットワーク業界でもスクリプティング言語を使えるプログラマが増えれば、古い設計のフレームワークを使うプログラマよりも何倍もの生産性をあげることができるようになるでしょう。そしてこの考え方が順調に浸透していけば、さまざまな言語で生産性の高いフレームワークが登場するはずです。

===[/column]

== Other tools (Oflops)

OflopsはOpenFlowコントローラとスイッチのためのマイクロベンチマークです。コントローラ用のベンチマークCbenchとスイッチ用のベンチマークOFlopsを提供します。スイッチを作る機会はめったにないのでここではコントローラのベンチマークであるCbenchについて説明します。

Cbenchは「1秒あたりにコントローラが出せるFlow Modの数」を計測します。Cbenchはスイッチのふりをしてコントローラに接続し、コントローラにPacket Inを送ります。これに反応したコントローラからのFlow Modの数をカウントし、スコアとします。このスコアが大きいコントローラほど「速い」とみなすのです。

Cbench supports two kinds of benchmarks.

//noindent
@<em>{Latency Mode}

 1. Send Packet In to controller
 2. Wait for Flow Mod to return from the controller
 3. Repeat step 1 and 2

//noindent
@<em>{Throughput Mode}

 1. Send Packet In without waiting for Flow Mod
 2. Count when Flow Mod is received


=== Cbenchの実行例 (Trema)   

TremaはCbenchおよびCbenchと接続できるコントローラを含むので、この2つのベンチマークを簡単に実行できます。次のコマンドは、Cbenchをレイテンシモードとスループットモードで実行し結果を表示します(Tremaのインストール方法は続く@<chap>{openflow_framework_trema}で説明します)。

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

====[column] Mr. Torema says: Cbenchの注意点

Cbench のスコアを盲信しないようにしてください。現在、いくつかの OpenFlow コントローラフレームワークは Cbench のスコアだけを競っているように見えます。たとえば Floodlight は 1 秒間に 100 万発の Flow Mod を打てると宣伝しています。これはなかなかすごい数字です。きちんと計算したわけではないですが、スレッドを駆使してめいっぱい I/O を使い切るようにしなければなかなかこの数字は出ません。とにかくすごい。でも、この数字にはまったく意味がありません。

Flow Mod を一秒間に 100 万発打たなければならない状況を考えてみてください。それは、Packet In が一秒間に 100 万発起こる状況ということになります。Packet In が一秒間に 100 万発起こるとはどういうことでしょうか? スイッチに何らかのフローが設定されているが入ってきたパケットがまったくそれにマッチせず、どうしたらいいかわからないパケットがすべてコントローラへやってくる、これが一秒間に 100 万回起こるということです。何かがまちがっていると思えないでしょうか？

コントローラが Packet In を何発さばけるかという性能は、極端に遅くない限りは重要ではありません。データセンターのように、どこにどんなマシンがありどういう通信をするか把握できている場合は、フローをちゃんと設計していれば Packet In はそんなに起こらないからです。力技で Packet In をさばくよりも、いかに Packet In が起こらないネットワーク設計やフロー設計をするかの方がずっと大事です。

Cbench のようなマイクロベンチマークでは、測定対象が何でその結果にはどんな意味があるか？を理解しないと針小棒大な結論を招きます。Cbench のスコアは参考程度にとどめましょう。

====[/column]

== Wrap-up

本章では現在利用できる主なOpenFlowコントローラフレームワークを紹介しました。すでに主要な言語のフレームワークがそろっているので、自分の使う言語に合わせてフレームワークを選択できます。

もし生産性の高いフレームワークをお望みであればTremaかPOXを選択してください。流れの速いSDN業界では、実行効率よりも「いかに早くサービスインできるか」という生産性の方がずっと重要だからです。

続く第II部では、Tremaを使ったOpenFlowプログラミングを学習します。Rubyの基礎から解説しますので、Rubyが初めてのプログラマでも読み進められるようにしてあります。
