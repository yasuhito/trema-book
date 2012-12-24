= OpenFlow use case

//lead{
OpenFlow seems all-purpose at a glance, but is that true? Let's think about the situations where it would fit perfectly well.
//}

//indepimage[pipeline][][width=10cm]

== Academia-born OpenFlow

OpenFlow switches can turn into any kind of networking devices as long as there's a programmed controller. Roughly speaking, as long as the controller is implemented, the OpenFlow switches can be anything from simple devices such as regular switches, to a bit more complicated equipments such as routers, load balancers, firewalls, and NATs. Of course, not all feature of the dedicated machines can be implemented and the performance might degrade with a poor implementation since parts of the features are implemented as a software. However, it still doesn't change the fact that anything is possible depending on the software.

This 'anything is possible' feature was originally born by the needs of academic background such as universities and laboratories. 'I want to do a research on a whole new network, free from the limitations of the conventional switches and routers. But remodeling the firmware of switches and routers sounds too hard. I don't want to make a new hardware from the scratch… A large-scaled virtual network is okay for a testbed but it's too different from the real Internet'. OpenFlow was devised to solve these kinds of dilemmas.

== Why is OpenFlow attracting attention?

So, why did OpenFlow -- originally used for researching Internet -- start attracting attention from the industries represented by the enormous data centers?

Two things that matter the most to data centers are throughput (processing ability per a unit time) and cost. Gigantic data centers such as Google need to equip a lot of switches and servers in order to process the endless requests from all over the world in a short time, i.e., to achieve a higher throughput. If these equipments are expensive, the total cost would be humongous just by the hardware expense. Therefore, they use an inexpensive hardware assembled with the commodity components that even you can get from the Akihabara (famous electronics district in Tokyo). 

The reliability of data centers is guaranteed by the software rather than the hardware. Since massive amounts of servers and switches are used in large-scaled data centers, the reliability of individual hardwares is ignored. In an environment where there are hundreds of thousands of servers, no one expects all those machines to keep working without any problems in the first place. Instead, the reliability is guaranteed from the upper software layer. Everything that's related to making the system trustworthy -- monitoring, assuring redundancy, managing faults, and recovering automatically -- is implemented as a middleware. 

OpenFlow fits well to this data center model. It decouples the software and the hardware; controller part that does the controlling and the OpenFlow switch part that only follows the instructions. This is similar to the data center model; a middleware guaranteeing the reliability by controlling the whole system, and a lot of commodity hardwares that are controlled. The collaboration with data center's middleware -- which guarantees the reliability by monitoring, assuring redundancy, recovering automatically -- is convenient since the controller is implemented as a software. It's more cost efficient and highly reliable than network and application individually guaranteeing the reliability. 

Above all, data center is a platform where new technologies such as OpenFlow can be easily adopted. Data centers are expanded in large units by the floors or buildings rather than by the servers. Hence, a unique new technology can be introduced without having to worry about the mutual connectivity with the conventional system.

As it happens, this OpenFlow's architecture of 'the software controller and the hardware is nicely decoupled and the collaboration with the conventional middleware is convenient' matches that of the data center. This is why OpenFlow is getting attention. 

== All that use cases

Here we'll be covering some of the ways to implement the fundamental networking devices such as switches and routers. But before we go on, let's look at what OpenFlow can do in more detail.

Rephrasing the question 'What can I do with the OpenFlow?' would be 'What can I do with the flow table?'. OpenFlow switch forwarding packets one after another according to the flow table remind the authors of the Waterworks game that we used to play in the grade school. It's a game where you lay out cards of water pipes in different shapes to deliver the water from the valve to the tap. What you can do with the flow table well resembles this game. The four basic functions follow.

 1. Forward packets
 2. Check flow rate 
 3. Rewrite
 4. Branch

This is everything. Forward the packets by outputting the packet from the designated switch port. Check the flow rate of the forwarded packets. Rewrite the packets. Replicate the packets and output them from the switch ports. By combining these functions freely, you can create various types of networks. 

Let us finally move on to the actual use cases. Notice what kind of combination with the above 4 cards is used to realize each case.

=== Switch

Using 'forward packets' card lets you realize the simplest switch with the OpenFlow (@<img>{switch}). The switch checks the destination MAC address of the received packet and forwards the packet to the port where the host with the MAC address is connected.

//image[switch][Realize switch with OpenFlow][width=8cm]

If 'check flow rate' card is combined, traffic aggregating function is appended to the switch (@<img>{traffic_switch}). The controller can summarize the whole network's traffic by adding up how many packets were forwarded for each flow table.

//image[traffic_switch][Realize the switch with traffic aggregating function by OpenFlow][width=8cm]

The details on how to implement the switch with the OpenFlow and the traffic aggregating function will be explained in @<chap>{learning_switch} and @<chap>{traffic_monitor}, respectively.

=== Router

Routers can be realized with OpenFlow when 'forward packets' and 'rewrite' cards are combined (@<img>{router}). Routers operate between two different networks, forwarding and rewriting packets so that the two networks can communicate. When the packet goes through the router, the router rewrites the packet's destination and source MAC address before forwarding the packet.

//image[router][Realize router with OpenFlow][width=12cm]

The details on how to implement the router with OpenFlow will be explained in @<chap>{router_part1} and @<chap>{router_part2}

=== Load balancer

The so called load balancer can be realized by the OpenFlow if 'check flow rate' is appended to the router (@<img>{load_balancer}). A load balancer is a device or a software that distributes the accesses from clients to multiple backend servers, to lower the load of servers with high access rates such as web servers.

//image[load_balancer][Realize load balancer with OpenFlow][width=10cm]

The operations of load balancer follow.

 1. When a request from a client arrives, determine the backend server in charge
 2. Rewrite the request packet so that it can reach the backend server (same as the router)
 3. Output the rewritten packet from the switch port that the backend server is connected to 

//noindent
The load between the backend servers may not be balanced depending on the access situation. Using 'check flow rate' card, the backend with a lower load is chosen with priority.

The number of necessary backend servers depends on the period of time. For example, the number of backend servers can be reduced in the middle of the night when access rate decrease. On the contrary, it must be increased around after lunch time when many people tend to surf the net.

If there's an API that adjusts the number of the backend servers, the load balancing function and adjusting the number of the backend servers can be collaborated (@<img>{advanced_load_balancer}). This is because the traffic can be examined by 'check flow rate' and accordingly, the number of the backend servers can be automatically adjusted through the API.

//image[advanced_load_balancer][Example of high-spec load balancer that adjusts the number of backend servers according to the traffic amount][width=10cm]

As shown above, the controller can append functions to the network by collaborating with conventional middleware such as backend server. The controller collaborates easily with various kinds of conventional middleware through the APIs since the controller can be implemented by major programming languages which will be introduced in @<chap>{openflow_frameworks}.

We've introduced several examples on how to implement some basic networking devices with the OpenFlow so far. From now on, we'll be digging deeper into the patterns of how to control the networking route by the OpenFlow.

=== Fully utilize the bandwidth 

If you use 'forward packet', large data such as VM images can be effectively forwarded by using multiple routes. Multiple connections from the source to the destination are made, and the connections are used simultaneously to forward the data (@<img>{maximize_bandwidth}).

//image[maximize_bandwidth][Earn bandwidth by using multiple routes][width=12cm]

A detailed example of this 'fully utilize the bandwidth' will be introduced in @<chap>{google}.

=== Packet replication

When 'branch' is used, the packets sent from the server are copied at the switch and delivered to multiple clients (@<img>{multicast}). Even if the number of clients increase, the switches will replicate the packets so there's no need to multiply the number of packets from the server. As a result, the network bandwidth can be saved.

//image[multicast][Replicate packets and deliver them to multiple clients][width=10cm]

In addition, the 'branch' will allow you to create redundant routes. If replicated packets are sent to each route, the network can be recovered without loosing a single packet in case of a failure (@<img>{fail_over}). Think of a situation where a client and a server communicate. The left-most switch sends the replicated packets to both of the routes. The receiver accepts packets from only one of the routes. That way, even when there's a failure in one route, none of the packet will get lost since the other route is still working.

//image[fail_over][Create redundant route with OpenFlow][width=12cm]

What's important here is that the controller sees the whole picture and can make decision on routes at will. In the conventional network, each switch could only select a specific route, such as the shortest one, based on its individually calculated optimal route. However in the OpenFlow, the controller can change the route in any way and in any time it wants based on the overall knowledge on the route and the traffic situation.

=== Create and change the network structure as you want 

By using the OpenFlow, you can change the network structure freely without being limited by the physical structure. Let's say two hosts and two networks are connected to a switch as shown in @<img>{patch_panel}. 
The controller can select the connection of hosts and networks freely by inserting a flow entry that says 'forward packet'. For example, Host A belongs to the Network B by mutually forwarding the packet between Host A's port and Network B's port.

//image[patch_panel][Select the network where each host belongs][width=12cm]

The hardware with this kind of function is called a patch panel and we'll be covering how to implement it with the OpenFlow in more details in @<chap>{patch_panel}. A more advanced example, a network virtualization so to speak, will be introduced in @<chap>{sliceable_switch} and @<chap>{datacenter_wakame}.

== Wrap-up

We looked at some specific use cases where the OpenFlow fits perfectly. You can add any kind of functions you want depending on the programming in the OpenFlow but it's not really realistic to implement every functions of the dedicated networking devices. Instead, it matches well with the world where the software controls the hardware, represented by the data centers. Implement the necessary functions for automation/optimization of network operation and achieve higher throughputs with the OpenFlow, while leaving the parts related to the reliability to other middleware. That's how OpenFlow should be used to display its great abilities. 

In the next section, we'll be introducing the specification of the OpenFlow in detail.

== Reference

: The Datacenter as a Computer: An Introduction to the Design of Warehouse-Scale Machines (Luiz Andre Barroso, Urs Holzle/Morgan and Claypool Publishers)
 Google's enormous data center is called 'warehouse scale computer (WSC)' and the software as a computer controls tens of thousands servers squeezed into the gigantic warehouse. The book discusses the overall architecture of the WSC and the designs of middleware, based on an affluent data that only the actual data center operators could know.
