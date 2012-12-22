= What's OpenFlow?

//lead{
OpenFlow - one of the hottest topic nowadays - how is it structured and what are the advantages? We'll be using familiar examples rather than the difficult network related jargons to explain what it is.
//}

//indepimage[incredible_machine][][width=8cm]

== Control as you wish, by software

//quote{
Laziness: The quality that makes you go to great effort to reduce overall energy expenditure. It makes you write labor-saving programs that other people will find useful, and document what you wrote so you don't have to answer so many questions about it. Hence, the first great virtue of a programmer. Also hence, this book. - Programming Perl by Larry Wall et al. O'Reilly
//}

Laziness is one of the most important hacker characteristics of a skilled programmer. Mulling over how to get a bag of potato chips without ever leaving the beloved computer might seem like an indolence from the ordinary people but it really is a serious matter from the hacker's point of view.

Being lazy by utilizing the software is one area that hackers most specializes in, to exert their creativeness. The following three stories are great examples that utilize the high technical capabilities simply because of their laziness.

 1. There was once a UNIX command in the MIT's AI laboratory (popular for being the hackers' haven), to order pizza online from the computer@<fn>{xpizza}. Order a box of pizza by typing a command when you get hungry from all the hacking you've done. Now how lazy is that?
 2. In the renowned Computer Science Department of Carnegie Mellon University, there was a eccentric vending machine called 'Coke Machine'@<fn>{coke_machine}. It lets you check how cold the coke is, by using a single UNIX command. Not surprisingly, it's to save you the pain of going all the way to the vending machine and to get a lukewarm can of coke.
 3. The Hyper Text Coffee Pot Control Protocol is specified in RFC 2324@<fn>{rfc2324}. The interface for remotely monitoring the amount of coffee in the pot and automatically brewing the coffee is defined. It's an April Fools' Day RFC so the whole thing is supposed be a joke but it's surprising that someone had actually implemented it.

//footnote[xpizza][MITの@<tt>{xpizza} command manual: @<href>{http://stuff.mit.edu/afs/sipb/project/lnf/other/CONTRIB/ai-info}]
//footnote[coke_machine][Carnegie Mellon Computer Science Department Coke machine: @<href>{http://www.cs.cmu.edu/~coke/}]
//footnote[rfc2324][RFC 2324: @<href>{http://www.ietf.org/rfc/rfc2324.txt}]

The largest-scaled example of these 'have/hack fun using software' is the state-of-art data center. I'm sure a lot of the readers of this book have heard that the data centers supporting the cloud services are operated by only few engineers and most management process is automated to the utmost limit by softwares. From the recreational activities involving pizza, coke, and coffee to the rather tricky data centers, controlling 'something' just as you wish by software is diverting than anything and actually a worthwhile hack.

== SDN: Let's control the network with software!

OpenFlow is one of the technologies that you can use to hack the network. To put it simply, OpenFlow is one of the standard protocols used for controlling the behavior of the network switch. Even the entire network can be defined by a source code, since using the OpenFlow lets you rewrite the behavior of each switch freely with the software. This is called SDN (Software Defined Networking) and OpenFlow is attracting an attention as a typical technology to realized the SDN.

Network is now considered as something programmable by the advent of the OpenFlow. We can almost hear a voice saying 'I can't believe you're still managing the network manually! Just let the software take care of it!'. I'm sure there are lots of places where you can be lazy about. 

When the creativity as strong as the hackers and the programmable characteristics of the OpenFlow are combined, the following 'ultimate automation' wouldn't feel like a fantasy anymore.

 * A network with a centralized control system by utilizing all sorts of collected data such as faults and traffic information
 * A network architecture that can automatically changed depending on the user or application addition/deletion
 * A network that can optimize the utilization of conventional infrastructure without extra investment 

All three topics will be dealt in this book. We'll be looking at some specific adaptation examples of the OpenFlow - from the small and medium-sized network such as home and work to the very-large-scaled data center's network - and answer simple questions such as 'What's OpenFlow and where would it come in handy?'. In addition, for those of you programmers who's thinking 'I want to create a brilliant network with this OpenFlow right away!', we have a lot of practical codes that you can actually run.

You don't need to have an in-depth knowledge on the computer network or programming when you read this book. We'll be explaining one-by-one from the basics and the book is for almost anyone - network professionals, programmers, system engineers, sales people and managers - to understand right away if you have a pinch of an interest in the OpenFlow. Let's begin by understanding the mechanism of the OpenFlow!

== Mechanism of the OpenFlow

To explain the mechanism of the OpenFlow, we'll be using a little allegory. Think of a customer support center. Yes, it's that place with a toll-free number where you call when your TV or computer starts acting weird. Okay, but how's that related to the OpenFlow?

Actually the basic mechanism of the OpenFlow is very similar to that of the customer support center. If you can understand the following two stories, it would be virtually the same as understanding the OpenFlow about 95%. So let's welcome three people; the main actor Yutaro, telephone operator Ms. Aoi, and her boss Mr. Miyasaka!

=== Story 1：Aww my AC isn't working

It's that time of the year when you're glad that you have your AC. However, something seems wrong with Yutaro's AC so he decides to dial the customer support center's number listed on the instructions handbook. 
He checks every item in doubt suggested by the automatic support voice but to no avail. In the end, he gives up and decides to talk to the operator.

"Hello, customer support center's Aoi. How may I help you?"

Ms. Aoi is a telephone operator working for Yoyodyne Air Conditioner. Her job is to transfer the call from the customer with an AC problem to the engineer. (@<img>{yoyodyne_support})。

//image[yoyodyne_support][Telephone operator transfers the inquiries from the customer to the appropriate engineer][width=12cm]

"Something is wrong with my AC remote control. The temperature screen keeps blinking - how do I make this stop?"

Ms. Aoi reaches for the manual by her side and opens it (@<table>{manual1}). In the manual, the source of malfunction and the extension number of the engineer who can fix the problem is written. The following figures are the number of inquiries for each malfunction.

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
This way, Ms. Aoi can report how many cases there were for each malfunction and have the development department make use of this feedback for the next season's product. 

==== The story in the OpenFlow world

In the realm of the OpenFlow, the source host of the packet and the OpenFlow switch that forwards the packet are, the customer Yutaro and the telephone operator Ms. Aoi, respectively (@<img>{openflow_host_switch}). When the host sends packets, the OpenFlow switch forwards the packets to the appropriate destination according to the content of the packets. This is just like Ms. Aoi forwarding the call to the engineer in charge according to the inquiries that Yutaro made.

//image[openflow_host_switch][Host = customer, switch = telephone operator, and flow table = manual][width=12cm]

In an OpenFlow switch, this transactions are in the 'manual'. In the example of the customer support, Ms. Aoi looked up the extension line information in the manual. In an OpenFlow switch, the decision where to forward the packets are made by looking up the database in the switch called 'flow table'. As everything that Ms. Aoi does are manualized, everything that the OpenFlow switch does is determined by the content of this flow table.

==== Flow table, where forwarding information is managed

In a flow table, rules such as 'When this kind of packet arrives, forward it to the port x' are stored. These rules are called flow entries. Flow entries correspond to the items in the manual book for instance, 'When there's an inquiry on the malfunctioning remote control, forward to extension 555-2222'.

Let's take a look at an example of an actual flow table. In Table @<table>{story1_openflow}, each entry corresponds to a flow entry composed of three elements; matching rule, action, and statistics.

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
Action corresponds to a 'process method', that is, how to handle the arrived packet. Just like 'Transfer to extension 555-4444', the action is something like 'Forward to port number 8 of the switch'. However, the action isn't limited to simple forwarding. It can be about rewriting or dropping the packet.

: Statistics
Statistics is a recording of how many packets were processed for each flow entry. As '9 inquiries were related to the remote control' was noted in the manual, information such as '80 packets were forwarded based on this flow entry' is stored.

What did you think of that? Isn't the customer support center example and the OpenFlow really similar? The mechanism of the OpenFlow is very simple and easily comprehensible.

=== Story 2：The AC isn't working again!

The AC was working okay for a while but a month later, it's acting weird again. Yutaro dials the customer center once again.

"The drain hose gets clogged really easily"

Ms. Aoi flips through the manual pages but there aren't any sections on the drain hose! Apparently, it seems to be a whole new kind of problem.

"I'm sorry sir, could you hold on for a moment? I'll see if I can reach the engineer who can guide you."

Then there's the moment of waiting with please-hold-on-message and a pleasant music in the background.

//image[yoyodyne_support_miyasaka][When you can't find what you're looking for in the manual, ask the boss][width=12cm]

At a time like this, it's Mr. Miyasaka who Ms. Aoi always leans to (@<img>{yoyodyne_support_miyasaka}). 

"Mr. Miyasaka, there's an inquiry on the drain hose. To whom should I forward the call to?"

"Oh, in that case, Mr. Yamamoto should be the perfect person to ask"

Ms. Aoi returns to the call with the information. 

"Thanks for waiting, I'll forward your call to the engineer in charge"

This took more time compared to the very first inquiry on the remote control but at last the case is closed. Furthermore Ms. Aoi adds the number of Mr. Yamamoto's extension number informed by Mr. Miyasaka in the manual(@<table>{manual3}). When there's an inquiry on the duct hose in the future, she can answer quickly on the matter. 

//table[manual3][Updates the manual by adding a new source of malfunction and the contact information]{
Source of Malfunction				Extension of engineer in charge		Number of inquiries
----------------------------------------------------------------------
Remote control				555-2222						9 cases
Interior evaporating unit			555-4444						6 cases
Exterior condensing unit					555-3333						4 cases
@<em>{Duct hose}		@<em>{555-5555}					@<em>{1 case}
//}

==== The story in the OpenFlow world

In the OpenFlow, the boss is a software called 'controller' (@<img>{openflow_host_switch_controller}). When programming the network with OpenFlow, it's the controller part that the programmers code. You can control the network freely by coding the controller inside your brain to a software.

//image[openflow_host_switch_controller][When the packet entry is not in the flow table, ask the controller][width=12cm]

The packets specified in the flow table are forwarded by the switch at speed but there are packets without such instructions, making the switch lost in the dark. In this case, the switch asks the controller what to do with the packet. The controller inspects the content of the packet and gives an instruction what to do, i.e. writes the flow entry in the flow table. 

When the packet that doesn't have any forwarding information in the flow table arrives, the forwarding speed becomes slow since the switch has to ask the controller. However, the process is fast just with the switch if the controller writes the necessary flow entries in advance when the switch starts up.

=====[column] Question from Yutaro: How slow is querying the controller?

What happens if the controller gives out instructions every time without using the flow table? The answer - a LOT slower, like several manyfold. To test, we wrote a short program in our testing environment and compared the forwarding by the software switch and processing everything by the controller. Guess what, 5 times slower!! Of course this is just a rough estimation but it should give you a rough idea about how slow it gets. In addition, if a hardware switch was used instead of the software switch in the comparison, the difference would have been larger.  

=====[/column]

== The Joy of OpenFlow

I'm sure you now understand the nuts and bolts of the OpenFlow. So let's get to the bottom line, how is OpenFlow going to make my life happy?

=== Automation and system collaboration is easy

In a customer support center, everything is taken care automatically by the telephone operators if there's an appropriate manual made in advance. A perfect division of labor - overall monitoring done by the manager and the actual work done by the telephone operators. This way the manager can focus on collaborating/coordinating with other departments while the telephone operator does the actual job. 

In a similar way, automating the network operation is easy since controlling the OpenFlow switch is implemented by the 100% software controller. In addition, if the controller is written by well known languages such as Ruby, Python, and Java, collaborating with conventional systems and services is convenient. For example, an advanced automation is possible if the network configuration can be modified by triggers such as problem alert, application request, and business policy update. 

=== Centralized control of network traffic is convenient 

The estimation of congestion level or controlling the traffic is an easy task since every information of the number of inquiries is reported to the managerial level in the customer support center. The manager can instruct the operators through the manual book to have the calls shared among the engineers, if a specific engineer is bombarded with inquiries. On the other hand, if each and every operator makes a separate decision, they might end up forwarding the calls to the same engineer. 

In the OpenFlow, the traffic can be optimized with all things considered since the traffic information is reported to the controller. The controller can collect all sorts of traffic data and its statistical information from the whole network. By updating the flow table of each switch based on the information, the optimal route of the packet can be estimated. Conversely, if each and every switch makes a separate decision, the traffic can't be distributed evenly.

=== Techniques and tools for developing software can be used

The controller is a kind of a software. Therefore, many years of software development's various techniques and tools can be applied to constructing the network.

 * If the controller is developed using the agile development which is the recent mainstream, the functions can be added repetitively. The network can be gradually constructed, getting version upgrades with the feedbacks.
 * The whole network can be tested automatically by writing controller's unit test or acceptance test. The output of the test result becomes a part of the specifications and there's no need to manage a separate Excel or Word file.
 * If the controller's source code or related data are managed by version control tools such as git, managing the entire network's version, checking the difference of two versions, and reverting may be possible.

====[column] Mr. Torema says：OpenFlow is a conveyor belt sushi!?

Conventional routers and switches demands you to use their vendor-defined functions. For example, even when you want to use only 10 functions, you have to buy the router that comes with 100 pre-defined features. This is like going to a French restaurant where you can only order full course cuisine! Even if you are using only a few features, there are millions of point of failures and it isn't always easy to pinpoint the cause or debug.
OpenFlow is like a conveyor belt sushi. It might be difficult to mimic a dish from a fancy French restaurant but if you choose only the necessary functions and implement those, you might end up with something just as you intended. 

====[/column]

== Some things you should be careful about OpenFlow

Of course, not everything about OpenFlow makes you happy. You should be careful about the scalability issue when there's an increase in the number of the switch, since the controller does all the controlling in the OpenFlow. In the worst case, the controller might just stop working if too many packets that are not on the flow entry arrive at the controller at the same time. 

Also, you have to be extra careful about where to use the OpenFlow and the remaining flow table space. For example, if you connect the OpenFlow switch to the Internet where various kinds of packets exist, the controller will soon crash with all the inquiries that fills up the flow table. However in a closed environment such as data centers, you can pretty much guess the traffic characteristics and types of the packets in advance. That way, the scalability can be achieved even when the number of switches increase, by constructing the network and the flow entry so that only the minimal packets can reach the controller.

== Wrap-up

In this section, we gave an illustration on the OpenFlow that can realize SDN. OpenFlow consists of switches that have flow tables and a software controller that does the centralized control of the flow tables. By making the network control a software, you can get all the benefits such as automation, collaboration with various systems, traffic control, and application of software technology.

In the next section, we'll be covering some specific use cases of OpenFlow.
