= Mechanism of OpenFlow

//lead{
Now that we acknowledge the concept of the OpenFlow, let's proceed to a bit more detailed specification. 
We're going to put an emphasis on the practicality and grasp the points of the OpenFlow specification and frequently used words.
//}

//indepimage[torema][][width=10cm]

== Standard specification of OpenFlow

OpenFlow's standard specification mainly defines the following two items.

 1. Communication protocol between the controller and switch (secure channel)
 2. Switch's behavior on configured flow entry

//noindent
In this section, we'll focus only on the more used function among the two.

The latest version of the OpenFlow specification was 1.3 when this book was written, but the later descriptions are based on 1.0 since the corresponding commercial switches are also based on 1.0. There aren't radical changes on the idea or the model from 1.0 and above so if you can understand 1.0, it should be easy to understand the later versions as well.

== Exchange between the switch and the controller

OpenFlow switches and controllers operate by exchanging messages defined by the OpenFlow specification. Here we explain what kind of messages are sent and received in a sequential order. Recall the story of the customer support center in @<chap>{whats_openflow} when you read.

=== Establish connection between the switch and the controller

The first thing that the controller and switch do is to establish the secure channel connection. There aren't any rules on which one should initiate the connection first but it seems to be from the switch to the controller in most of the recent implementations as shown in @<img>{secure_channel}.

//image[secure_channel][Switch connects to the controller and establishes secure channel][width=12cm] 

Secure channel is a regular TCP connection. When both the controller and the switch are OpenFlow compatible, a more secure TLS (Transport Layer Security) can also be used although the performance may marginally degrade. 

=== Negotiate version

The next step is confirming the OpenFlow protocol version, the so-called version negotiation. After establishing the secure channel, the switch and the controller exchange Hello messages with their own version numbers (@<img>{version_negotiation}).

//image[version_negotiation][Confirming the other side's OpenFlow protocol version by exchanging the Hello message][width=12cm]

The negotiation is successful if the same version is spoken, and then the real communication begins.

=== Confirm switch's specification

Next, the controller confirms the connected switch's specification. The controller sends a 'Features Request' message to the switch and asks for the following information since only the version can be confirmed right after the negotiation.

 * Unique ID of switch (Data path ID)
 * List of physical ports
 * List of supported functions

//image[features_request_reply][Confirm switch specification by 'Features Request' message][width=12cm]

The switch replies by sending 'Features Reply' message.

=== Receive unknown packets

When the switch detects communication that is not registered in the flow table, it notifies the controller of that packet and related information. This is called 'Packet In' message.

//image[packet_in][Unknown packet and related information is sent to the controller as 'Packet In' message][width=12cm]

=== Update flow table and forward packet

When forwarding the packets, a flow entry is written to the flow table in the switch, to forward the packets by 'Flow Mod' message. Then the packets that caused 'Packet In' are sent to the appropriate destination by 'Packet Out' message. Without this, the data of the packets that evoked 'Packet In' ends up remaining in the controller, never to be sent to the destination (@<img>{flowmod_packetout}).

//image[flowmod_packetout][Update the flow table by 'Flow Mod' and forward packets that caused 'Packet In' by 'Packet Out'][width=12cm]

====[column] Mr. Torema says: 'Flow Mod' and 'Packet Out' at the same time?

Actually, there's a way to do 'Flow Mod' and 'Packet Out' at the same time. Although it's written in the OpenFlow specification, it's rather a dangerous programming style.

When the switch receives 'Packet In', the switch buffers the contents of the packet that evoked 'Packet In' and the ID of the buffer (Buffer ID) is notified to the 'Packet In' message that is sent to the controller. If this is set at the timing of 'Flow Mod', the switch performs 'Packet Out' (@<img>{buffer_id}).

//image[buffer_id]['Flow Mod' and 'Packet Out' at the same time by designating Buffer ID][width=12cm]

However, this is a @<em>{taboo}. Here's why.

 * It's hit or miss whether the packets designated by the Buffer ID are still in the switch's buffer or not since there's no way to observe from outside of the switch
 * Even if you know for sure that the packets are still in the switch's buffer, they might disappear as soon as you type in 'Flow Mod'
 * Cheap switches might not have a buffer from the beginning

So it's safe to send 'Packet Out' and 'Flow Mod' commands independently as explained in this book.

Just for the record when 'Flow Mod' and 'Packet Out' is done at the same time, here's a pseudo code that works even if there's nothing in the buffer.

//emlist{
begin
  flow_mod( Buffer-ID = packet_in.buffer_id )  # Designation of Buffer ID
rescue
  packet_out( packet_in )  # If Flow Mod fails, explicitly Packet Out
end
//}

//noindent
As you can see, you have to write 'Packet Out' as an exception handling in case there's nothing in the buffer. The code gets longer as a result.

Let's look at the correct way:

//emlist{
flow_mod # No designation of Buffer ID 
packet_out( packet_in )
//}

//noindent
It's much shorter this way and it works just right as well.

====[/column]

=== Lifespan of flow table and statistical information

A 'lifespan' can be set to the flow entry that is created by 'Flow Mod'. There are two types of lifespan.

 * Idle time out: When the non-referred time reaches this lifespan, the flow entry is deleted. Once the packets arrive and the flow entry is referred, the time is reset to 0 second.
 * Hard time out: The flow entry is deleted when this lifespan is reached (counted from the moment when the flow entry was written) regardless of the referred status.

When these time outs are set to 0, the flow entry remains in the flow table unless it is explicitly deleted.
 
When the flow entry is deleted, the information of the deleted flow entry and the statistical information of the packets processed according to that flow entry are notified to the controller. This is called 'Flow Removed' message and it's used for collecting the network traffic.

//image[flow_removed][When the flow entry is deleted by time out, the statistical information of the forwarded packet is sent to the controller as 'Flow Removed'][width=12cm]

== Inside the flow entry

A flow entry is composed of the following 3 components as shown in @<chap>{whats_openflow}.

 * Matching rule
 * Action
 * Statistical information

We'll be looking at these components in detail but you don't have to remember everything from the beginning. Please use the following subsections as a reference when you get lost in the following chapters.

=== Matching rule

Matching rule is a condition that makes the OpenFlow switch to decide whether it should take an action or not when it receives packets. For example, the switch takes an action only on the packets that match the given rules such as 'If the packet's destination is a http server' or 'If the packet's destination is a broadcast address'.

12 types of rules (@<table>{matching_rules}) can be used in OpenFlow 1.0. These rules are values well used in ethernet and TCP/UDP.

====[column] Mr. Torema says: Another name of matching rule

There're actually other ways to call 'matching rule': 'OpenFlow 12 tuple' and 'header field'. Which is quite confusing so it's just 'matching rule' in this book. It's because the name expresses the role of 'When packet arrives, match according to the rules' straightforwardly and the most easily comprehensible.

"OpenFlow 12 tuples" becomes 15 tuples in OpenFlow 1.1, and who knows how many there'll be in the future? In other words, on one knows till when this name is available. Also the name 'header field' doesn't really tell what it does and seems a bit difficult in a strange way.


====[/column]

//table[matching_rules][12 kinds of conditions that can be set by matching rule]{
Name				Description
--------------------------------------------------------------
Ingress Port		Physical port number of switch
Ether src		Source MAC address
Ether dst			Destination MAC address
Ether type			Ethernet type
IP src				Source IP address
IP dst				 Destination IP address
IP proto			 IP protocol type
IP ToS bits			IP ToS information 
TCP/UDP src port	Source port number of TCP/UDP
TCP/UDP dst port	Destination port number of TCP/UDP
VLAN id				VLAN ID
VLAN priority		VLAN PCP value (CoS)
//}

In the world of the OpenFlow, the communication is controlled by freely combining the conditions that can be set by these matching rules. Example conditions follow. 

 * Rewrite packet that arrives at switch's physical port #1 with destination of TCP 80 (= HTTP) 
 * Deny packet with MAC address of 02:27:e4:fd:a3:5d and destination IP address of 192.168.0.0/24

These are called 'Wildcard Match' since only partial conditions are specified. Of course, it's possible to create a matching rule that specifies all 12 types of conditions, which is called 'Exact Match'.'Exact Match' is used when a particular packet needs to be matched by the flow entry and 'Wildcard Match' is used when a wide range of types of packets needs to matched by a single flow entry.

====[column] Mr. Torema says: OSI network model falls apart?

A lad with a rich experience in network once said,

'If OpenFlow can do anything across the layers, wouldn't OSI network model@<fn>{OSImodel} fall apart?'

//footnote[OSImodel][It's those Layer 2 and Layer 3 stuff. To be more correct, it's a construction policy of the network architecture defined by ISO to realize data communication between dissimilar devices.]

There's no need for such worries. OSI network model is a 'OSI reference model' and it's a model to 'reference' for grouping the communication protocol and make them look better. For example, consider a situation that you've made a protocol called xyz and have to explain it to someone. You would start the sentence by pointing (referencing) to a layer such as 'This is a Layer 3 protocol and…' which would help the listener understand you better. In other words, the OSI network model is conveniently used as a vocabulary that's understood between the people who know a little bit about the network.
However, this is nothing more than a reference let alone a policy, so it doesn't mean that every network protocol and networking device have to abide the rule. As said before, a sentence like 'If abc should be in the OSI, it would be placed in Layer 4' would suffice. 

So, it's just that the OpenFlow happens to use a couple of layers' information. 

====[/column]

=== Action

An action is something like a @<em>{verb} that specifies how to cook the packet that arrived at the switch. 
The phrase such as 'Rewrite the packet by OpenFlow and forward' is often used and this rewriting process can be realized by the actions. So let's take a look at what kind of actions are defined in OpenFlow 1.0.

There are 4 main types of actions.

 * Forward: Forward the packet from the designated port 
 * Modify-Field: Rewrite the content of the packet
 * Drop: Drop packets
 * Enqueue: Attach packet to the designated switch for each port (for QoS)

Like a verb, the action is performed sequentially as commanded - 'Make onigiri, eat, and clean up'. For example, if packets need to be rewritten and outputted from the designated port, the list of actions should be something like this.

//emlist{
[Modify-Field, Forward]
//}

Here, note that the actions are performed sequentially as commanded. If these orders are changed, you would get a different result. For example, if it's 'Eat onigiri and then make onigiri', you would have an onigiri left in the end. So if you reverse the above example, packets would be forwarded first. Even if the 'Modify-Field' happens afterwards, the packet would end up being destroyed after it's rewritten.

//emlist{
# Forwarding happens first before the packet is rewritten.
[Forward, Modify-Field]
//}

Identical verbs can be placed multiple times.

//emlist{
[Modify-Field A, Modify-Field B, Forward A, Forward B]
//}

The above example would be read as 'Rewrite field A and B, and forward it to port A and B'. Like this, a list of multiple actions@<fn>{num_actions} can be created when rewriting multiple fields or outputting packets to multiple ports.

//footnote[num_actions][The maximum number of actions is dependent on the implementation of OpenFlow switch and controller. Normally the numbers wouldn't cause any problems.]

Drop is a special kind of action and it's not really specifically defined. In case there aren't any Forward actions in the action list, the packets aren't forwarded anywhere but discarded. Therefore, it's conveniently called a Drop action.

Now let's take a look at the most widely used Forward and Modify-Field action, and what they're capable of doing in detail.

==== Forward action

Forward action output packets from the designated port. Port numbers can be designated but logical ports, defined for the special purposes, can be used as well.

 * Port Number: Output packets from the designated port number
 * IN_PORT: Output packets to ingress port
 * ALL: Output packets to all ports except the ingress port
 * FLOOD: Output packets according to the spanning tree that switch made
 * CONTROLLER: Explicitly send packets to the controller and trigger Packet In
 * NORMAL: Forward packets using switch's function
 * LOCAL: Sends packets to the switch's local stack. Used when there's a need to hand the packets over to the application that runs on the local stack. Not used very often.

Among these actions, FLOOD and NORMAL are logical ports when the function of OpenFlow and conventional switches is used together. 

==== Modify-Field action

Modify-Field action rewrites various parts of the packet. 

 * Rewrite source MAC address
 * Rewrite destination MAC address
 * Rewrite source IP address
 * Rewrite destination IP address
 * Rewrite ToS field
 * Rewrite TCP/UDP source port
 * Rewrite TCP/UDP destination port
 * Delete VLAN header
 * Rewrite VLAN ID (If no VLAN header exists, grant a new one)
 * Rewrite VLAN priority (If no VLAN header exists, grant a new one)

Let's look at what these actions can do and their typical usage. 

===== Rewrite MAC address

A typical example of rewriting MAC address is the router. OpenFlow supports the action of rewriting source and destination MAC address that is necessary for implementing the router function. 

//image[rewrite_mac][Rewriting source and destination MAC address in router][width=12cm]

A router works between two different networks and controls the traffic of the packets that come and go. When Host A sends a packet to the Host B residing in another network, the router receives the packet and determines where to forward it based on the destination IP address. Then the router rewrites the destination MAC address of the packet with the host MAC address of where the router is supposed to send the packet to. In addition, the router rewrites the source with the its own MAC address before forwarding the data. 

===== Rewrite IP address

A well-known example of rewriting the IP address is the NAT (Network Address Translation). OpenFlow supports the action of rewriting source and destination IP address that is necessary for implementing the NAT function.

//image[rewrite_ip_address][Rewriting source and destination IP address in NAT][width=12cm]

Routers connected to the Internet rewrites the IP address to enable the communication between a private and a global network. 
When a client in the private network wants to communicate with a server in the Internet, the gateway sends the packet after rewriting the source IP address of the packet that arrived from the private network 
with the gateway's global IP address. The reply from the server to the client works the same way, only by rewriting the packet in vice-versa.

===== Rewrite ToS field

ToS field is used when a router needs to designate the processing priority of the received packets to control the communication QoS. OpenFlow supports this action of rewriting ToS field.

===== Rewrite TCP/UDP port number

A typical example of rewriting TCP/UDP port is the IP masquerade. OpenFlow supports the action of rewriting source and destination TCP/UDP port that is necessary for implementing the IP masquerading function. 

//image[rewrite_port][Rewriting source and destination TCP/UDP port in IP masquerade][width=12cm]

In an environment such as broadband routers where multiple hosts simultaneously communicate with a single global address, TCP/UDP port numbers may be redundant with just the NAT. IP masquerading solves this issue by assigning the private network's port number to each host and translating the port numbers for each communication. 

===== Rewrite VLAN header

Rewriting VLAN header is used for the special purpose of connecting the traditional network constructed with tagged VLANs and the network constructed with OpenFlow. VLAN divides the network constructed with the conventional switches (network within a broadcast's reach) into multiple networks and sometimes the divided network itself is called VLAN. It's the VLAN's tag (VLAN ID) that distinguishes each VLAN and the VLAN header that's assigned to the packet contains this tag information. There are three kinds of Modify-Field actions that is required for handling the VLAN header. 

//image[strip_vlan][Usage of rewriting the VLAN header action][width=12cm]

: Delete VLAN header
 An action that puts the packet with the VLAN header back to a regular packet by eliminating the VLAN header from the packet that flows in the VLAN. 

: Rewrite VLAN ID
  Rewrite the VLAN ID of the VLAN packet. For example one can set an action that changes VLAN ID to 3. In addition, one can assign a VLAN header with a VLAN ID to a packet without a VLAN header. 

: Rewrite VLAN priority
 Change the packet forwarding priority on VLAN. This priority is used when the traffic types (data, voice, video, and etc.) are specified and the value ranges from 0 (lowest) to 7 (highest).

=== Statistical information

In OpenFlow 1.0, below statistical information can be acquired per flow entry.

 * Number of received packets
 * Number of received bytes
 * Elapsed time since the flow entry was created (second)
 * Elapsed time since the flow entry was created (nano second)

== Wrap-up

We took a look at the nuts and bolts in the OpenFlow specification. You can probably call yourself a OpenFlow specialist just by having learned all the things in the section. Let us introduce some popular programming frameworks for developing OpenFlow controller in the next section.


