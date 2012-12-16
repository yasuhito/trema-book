= What's OpenFlow?

//lead{
OpenFlow - one of the hottest topic nowadays - how is it structured and what are the advantages? We'll be using familiar examples rather than using difficult network related jargons to explain what it is.
//}

//indepimage[incredible_machine][][width=8cm]

== Control as you wish, by software

//quote{
Laziness: The quality that makes you go to great effort to reduce overall energy expenditure. It makes you write labor-saving programs that other people will find useful, and document what you wrote so you don't have to answer so many questions about it. Hence, the first great virtue of a programmer. Also hence, this book. - Programming Perl by Larry Wall et al. O'Reilly
//}

Laziness is one of the most important hacker characteristics of a skilled programmer. Mulling over how to get a bag of potato chips without ever leaving the beloved computer might seem like a indolence from ordinary people but it really is a serious matter from the hacker's point of view.

Being lazy by utilizing the software is one area that hackers most specializes in, to exert their creativeness. The following three stories are great examples that utilize high technical capabilities simply because of laziness.

 1. There was a UNIX command in the MIT's AI laboratory (popular for being the hackers' haven), to order pizza online from the computer@<fn>{xpizza}. Order a box of pizza by typing a command when you get hungry from all the hacking you've done. Now how lazy is that?
 2. In the renowned Computer Science Department of Carnegie Mellon University, there's a eccentric vending machine called 'Coke Machine'@<fn>{coke_machine}. It lets you check how cold the coke is, by using a single UNIX command. Not surprisingly, it's to save you the pain of going all the way to the vending machine and get a lukewarm can of coke.
 3. The Hyper Text Coffee Pot Control Protocol is specified in RFC 2324@<fn>{rfc2324}. The interface for remotely monitoring the amount of coffee in the pot and automatically brewing the coffee is defined. It's an April Fools' Day RFC so the whole thing is supposed be a joke but it's surprising that someone had actually implemented it.

//footnote[xpizza][MITの@<tt>{xpizza} command manual: @<href>{http://stuff.mit.edu/afs/sipb/project/lnf/other/CONTRIB/ai-info}]
//footnote[coke_machine][Carnegie Mellon Computer Science Department Coke machine: @<href>{http://www.cs.cmu.edu/~coke/}]
//footnote[rfc2324][RFC 2324: @<href>{http://www.ietf.org/rfc/rfc2324.txt}]

The largest-scaled example of these 'have/hack fun using software' is the state-of-art data center. I'm sure a lot of the readers have heard that the data centers supporting the cloud services are operated by only few engineers and most management process is automated to the utmost limit by the software. From the recreational activities involving pizza, coke, and coffee to the data center which is rather tricky, controlling 'something' just as you wish by software is diverting than anything and actually a worthwhile hack.

== SDN: Let's control the network with software!

その中でもネットワークをハックする技術の1つが、本書で取り上げるOpenFlowです。簡単に言えば、OpenFlowはネットワークスイッチの動作を制御するための標準プロトコルの1つです。OpenFlowを使えばスイッチ1つひとつの動作をソフトウェアから自由に書き換えられるので、究極的にはネットワーク全体の動作をソースコードとして記述できます。これをSoftware Defined Networking（SDN、ソフトウェアで定義されるネットワーク）と呼び、OpenFlowはSDNを実現する代表的な技術として注目を集めています。

OpenFlowの登場によって、これからはネットワークもプログラミングの対象になります。「いまだにネットワークを手で管理してるの？そんなのソフトウェアで横着しようぜ！」こんな声が聞こえてきそうです。たしかに、今までネットワーク管理と言えば専門のオペレータ達による手作業がメインでした。横着できる部分はたくさんあるはずです。

When the creativity as strong as the hackers and the programmable characteristics of the OpenFlow are combined, the following 'ultimate automation' wouldn't feel like a fantasy anymore.

 * A network with a centralized control system by utilizing all sorts of collected data such as faults and traffic information
 * A network that can automatically change the architecture depending on the user or application addition/deletion
 * A network that can optimize the utilization of conventional infrastructure without extra investment 

All three topics will be dealt in this book. We'll be looking at some specific adaptation examples of OpenFlow - from the small and medium-sized network such as home and work to the very-large-scaled data center's network - and answer simple questions such as 'What's OpenFlow and where would it come in handy?'. In addition, for those of you programmers who's thinking 'I want to create a brilliant network with this OpenFlow right away!', we have a lot of practical codes that you can actually run.

You don't need to have an in-depth knowledge on the computer network or programming when you read this book. We'll be explaining one-by-one from the basics and the book is organized for almost anyone - network professionals, programmers, system engineers, sales people and managers - to understand right away if you have an interest in the OpenFlow. Let's begin by understanding the mechanisms of the OpenFlow!

== Mechanism of the OpenFlow

To explain the mechanism of the OpenFlow, we'll be using a little allegory. You've probably used the method before but think of a customer support center. Yes, it's that place with a toll-free number where you call when your TV or computer starts acting weird. Okay but how's that related to the OpenFlow?

Actually the basic mechanism of the OpenFlow is very similar to that of the customer support center. If you can understand the following two stories, it would be virtually the same as understanding the OpenFlow about 95%. So let's welcome three people in the stories; the main actor Yutaro, customer support center's Ms. Aoi, and her boss Mr. Miyasaka!

=== Story 1：Aww my AC isn't working

It's that time of the year when you're glad that you have your AC. However, something seems to be wrong with Yutaro's AC so he decides to dial the customer support center's number listed on the instructions handbook. 
He checks every item in doubt suggested by the automatic support voice but to no avail. In the end, he gives up and decides to consult the operator.

"Hello, customer support center's Aoi. How may I help you?"

Ms.Aoi is a telephone operator working for Yoyodyne Air Conditioner. Her job is to transfer the call from the customer with an AC problem to the engineer. (@<img>{yoyodyne_support})。

//image[yoyodyne_support][Telephone operator transfers the inquiries from the customer to the appropriate engineer][width=12cm]

"Something is wrong with my AC remote control. The temperature screen keeps blinking - how do I make this stop?"

Ms. Aoi reaches for the manual by her side and opens it (@<table>{manual1}). In the manual, source of malfunction and the extension number of engineer who can fix the problem is written. The following figures are the number of inquiries for each malfunction.

//table[manual1][Manual book for telephone operators]{
Source of malfunction		Extension of engineer in charge		Number of inquiries
------------------------------------------------------------------
Remote control			555-2222					8 cases
Interior evaporating unit		555-4444					6 cases
Exterior condensing unit				555-3333					4 cases
//}

The entry was conveniently listed right on the top of the manual.

"I'm sorry for your inconvenience. I'll forward you to the engineer in charge right away."

When the transfer is done, Ms. Aoi updates the number of inquiries on remote control from 8 to 9 (@<table>{manual2}).

//table[manual2][Updating the number of cases]{
Source of Malfunction		Extension of engineer in charge	 Number of inqueries
------------------------------------------------------------------
Remote control			555-2222					@<em>{9 cases}
Interior evaporating unit		555-4444					6 cases
Exterior condensing unit				555-3333					4 cases
//}

//noindent
This way, Ms.Aoi can report how many cases there were for each malfunction and have the development department make use of this feedback for the next product. 

==== The story in the OpenFlow world

In the realm of the OpenFlow, the source host of the packet and the OpenFlow switch that forwards the packet are, customer Yutaro and telephone operator Ms.Aoi, respectively（@<img>{openflow_host_switch}）. When the host sends packets, the OpenFlow switch forwards the packets to the appropriate destination according to the content of the packets. This is just like Ms. Aoi forwarding the call to the engineer in charge according to the inquiries that Yutaro made.

//image[openflow_host_switch][Host = customer, switch = telephone operator, and flow table = manual][width=12cm]

In an OpenFlow switch, this transactions are in a "manual". In the example of the customer support, Ms. Aoi looked up the extension line information in the manual. In an OpenFlow switch, the decision where to forward the packets are made by looking up the database in the switch called flow table. As everything that Ms. Aoi does are manualized, everything that OpenFlow switch does is determined by the content of this flow table.

==== Flow table, where forwarding information is managed

In a flow table, several rules such as 'When this kind of packet arrives, forward it to the port x' are stored. These rules are called flow entries. Flow entries correspond to items in the manual book for instance, 'When there's an inquiry on the malfunctioning remote control, forward to extension 555-2222'.

Let's take a look at an example of a actual flow table. In Table @<table>{story1_openflow}, each entry corresponds to a flow entry composed of three elements; matching rule, action, and statistics.

//table[story1_openflow][Example of flow table and flow entry]{
Matching rule						Action			Statistics
-------------------------------------------------------------------------------
Source IP address: 192.168.1.100			Forward to Port 8		80 packets
Destination IP address: 192.168.10.92			Forward to Port 10	14 packets
Source MAC address: 00:50:56:c0:00:08	Forward to Port 1		24 packets
//}

: Matching rule
Matching rule is used as a 'condition' to find out how the arrived packet should be processed from the flow table. As the forwarding destination was chosen based on the inquiry on 'something is wrong with the remote control', the process method, i.e. the action, is determined based on the matching rule that fits the trait of the packet.

: Action
Action corresponds to a 'process method', that is, how to handle the arrived packet. As 'Transfer to extension 555-4444', the action is something like 'Forward to port number 8 of the switch'. 
However, action isn't limited to simple forwarding. It can be about rewriting or dropping the packet.

: Statistics
Statistics is a recording of how many packets were processed for each flow entry. As '9 inquiries were related to the remote control' was noted in the manual, information such as '80 packets were forwarded based on this flow entry' is stored.

What did you think of that? Isn't the example of customer support center and the OpenFlow really similar? The mechanism of the OpenFlow is very simple and easily comprehensible.

=== Story 2：The AC isn't working again!

The AC was working okay for a while but a month later, it's acting weird again. Yutaro dials the customer center once again.

"The drain hose gets clogged really easily"

Ms. Aoi flips through the manual pages but there aren't any sections on the drain hose! Apparently, it seems to be a whole new kind of problem.

"I'm sorry sir, could you hold on for a moment? I'll see if I can reach the engineer who can guide you"

Then there's the moment of waiting with please-hold-on-message and a pleasant background music.

//image[yoyodyne_support_miyasaka][When you can't find what you're looking for in the manual, ask the boss][width=12cm]

At a time like this, it's Mr. Miyasaka who Ms. Aoi always leans to (@<img>{yoyodyne_support_miyasaka}). 

"Mr.Miyasaka, there's an inquiry on the drain hose. To whom should I forward the call to?"

"Oh, in that case, Mr. Yamamoto should be the perfect person to ask"

Ms. Aoi returns to the call with the information. 

"Thanks for waiting, I'll forward your call to the engineer in charge"

This took more time compared to the very first inquiry on the remote control but at last the case is closed. Furthermore Ms. Aoi adds the number of Mr. Yamamoto's extension number informed by Mr. Miyasaka in the manual(@<table>{manual3}). When there's an inquiry on the duct hose in the future she can answer quickly on the matter. 

//table[manual3][Updates the manual by adding a new source of malfunction and the contact information]{
Source of Malfunction				Extension of engineer in charge		Number of inquiries
----------------------------------------------------------------------
Remote control				555-2222						9 cases
Interior evaporating unit			555-4444						6 cases
Exterior condensing unit					555-3333						4 cases
@<em>{Duct hose}		@<em>{555-5555}					@<em>{1 case}
//}

==== The story in the OpenFlow world

In OpenFlow, the boss is a software called 'controller' (@<img>{openflow_host_switch_controller}). When programming network with OpenFlow, it's the controller part that the programmers write. You can control the network freely by coding the controller inside your brain to a software.

//image[openflow_host_switch_controller][When the packet entry is not in the flow table, ask the controller][width=12cm]

The packets specified in the flow table are forwarded by the switch at speed but there are packets without such instructions, making the switch lost in the dark. In this case, the switch asks the controller what to do with the packet. The controller inspects the content of the packet and gives an instruction what to do, i.e. writes the flow entry in the flow table. 

When the packet that doesn't have any forwarding information in the flow table arrives, the forwarding speed becomes slow since the switch has to ask the controller. However, the process is fast just with the switch if the controller writes the necessary flow entries in advance when the switch starts up.

=====[column] Question from Yutaro: How slow is querying the controller?

What happens if the controller gives out instructions every time without using the flow table? The answer - a LOT slower, like several manyfold. To test, we wrote a short program in our testing environment and compared forwarding by the software switch and processing everything by the controller. Guess what, 5 times slower!! Of course this is just a rough estimation but it should give you an rough idea about how slow it gets. In addition, if a hardware switch was used instead of the software switch in the comparison, the difference would have been larger.  

=====[/column]

== The Joy of OpenFlow

OpenFlowの仕組みの大枠は理解できたと思います。それでは最も肝心な部分、「OpenFlowって何がうれしいの？」を掘り下げてみましょう。

=== 自動化やシステム連携がしやすい

カスタマーサポートセンターでは、あらかじめ適切なマニュアルを作っておけば業務はすべて電話オペレータが自動的にやってくれます。これによって、全体の監督は管理職で実務は電話オペレータ、というふうにきっちりと分業できるようになります。たとえば電話オペレータが実務をやってくれている間、管理職は他の部署との協業や調整に集中できます。

同様に、OpenFlowスイッチの制御はすべてソフトウェアであるコントローラで実現しているので、ネットワーク管理の自動化が容易です。さらにコントローラがRubyやPython、Javaなどよく知られた汎用言語で書いてあれば、既存のシステムやサービスなどとの連携も簡単です。たとえば、アプリケーションからの要求やビジネスポリシーの変更、問題発生などさまざまなトリガーに応じてネットワークの設定を変更するといった、一歩進んだ自動化もできます。

=== ネットワークトラフィックを集中制御しやすい

カスタマーサポートセンターでは問い合わせ件数の情報はすべて管理職に上がってくるため、混み具合の把握や全体の交通整理が楽です。もし特定のエンジニアに問い合わせが集中しても、問い合わせがうまくばらけるようにマニュアルを通じて電話オペレータの全員に指示できます。反対にもし各オペレータが個々に判断してしまうと、おなじエンジニアに問い合わせが偏ることは避けられません。

OpenFlowでもすべてのトラフィック情報はコントローラに上がってくるため、全体を見たトラフィックの最適化が可能です。各種統計情報を集計することで、コントローラはネットワーク全体のトラフィックデータを集められます。そしてその情報をもとに各スイッチのフローテーブルを更新することで、全体的に見て最適となるパケットの通り道を引けます。反対にもし個々のスイッチが判断してしまうと、うまくトラフィックを分散できません。

=== ソフトウェア開発のテクニックやツールが使える

コントローラはソフトウェアの一種なので、ソフトウェア開発で長年培われているさまざまなテクニックやツールをネットワーク構築に応用できます。

 * 近年主流のアジャイル開発手法でコントローラを開発すれば、反復的な機能追加が可能。フィードバックを受けながら少しずつバージョンアップしていくことで、ネットワークを段階的に構築できる
 * コントローラのユニットテストや受け入れテストを書くことで、ネットワーク全体を自動的にテストできる。テスト結果の出力は、そのまま仕様書の一部になる。ExcelやWordで書いた仕様書を別個に管理する必要はない
 * コントローラのソースコードや関連データをgitなどのバージョン管理ツールで管理すれば、ネットワーク全体のバージョン管理やバージョン間の差分のチェック、および巻き戻しも可能だろう

====[column] Mr. Trema says：OpenFlow is a conveyor belt sushi!?

Conventional routers and switches demands you to use their vendor-defined functions. For example, even when you want to use only 10 functions, you have to buy the router that comes with 100 pre-defined features. This is like going to a French restaurant where you can only order full course cuisine! Even if you are using only a few features, there are millions of point of failures and it isn't always easy to pinpoint the cause or debug.
OpenFlow is like a conveyor belt sushi. It might be difficult to mimic a dish from a fancy French restaurant but if you choose only the necessary functions and implement, you might end up with something just as you intended. 

====[/column]

== Some things you should be careful about OpenFlow

Of course, not everything about OpenFlow is wonderful.コントローラで制御を一手に引き受けるというモデルになっているため、スイッチの台数が増えたときのスケーラビリティに気をつける必要があります。もし、フローテーブルに載っていないパケットが一気にコントローラへ到着すると、最悪の場合コントローラが停止してしまいます。

そこで、OpenFlowの使いどころやフローテーブルの残り容量には特に注意する必要があります。たとえばOpenFlowスイッチをインターネットのような多種多様のパケットが流れる環境につなげると、すぐにコントローラへの問い合わせが殺到しフローテーブルがいっぱいになって破綻してしまいます。しかしデータセンターなどの閉じた環境では、トラフィックの特徴や流れるパケットの種類はあらかじめ見当を付けておけます。そこで最低限のパケットのみがコントローラへ上がってくるようにうまくネットワークとフローエントリを設計することで、スイッチが増えてもスケールさせることができます。

== Wrap-up

本章ではSDNを実現するための部品であるOpenFlowを解説しました。OpenFlowはフローテーブルを持つスイッチと、フローテーブルの内容を集中制御するソフトウェアであるコントローラから成ります。ネットワークの制御をソフトウェア化することによって、自動化やさまざまなシステムとの連携、トラフィック制御のしやすさ、ソフトウェア技術の応用などなどさまざまな恩恵があります。

In the next section, we'll be covering some specific use cases of OpenFlow.
