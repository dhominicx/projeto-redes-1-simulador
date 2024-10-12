/*
 * Universidade Federal do Pará
 * Instituto de Tecnologia - ITEC
 * Faculdade de Computação e Telecomunicações
 *
 * Contrucao de Topologia de Redes Simples Utilizando ns-3
 * Redes de Computadores - 2024.2
 *
 * Dhomini Bezerra Picanco - 202306840009
 * Antonio Carlos Bessa Martins Neto - 202306840039
 * Rafael Felipe Pantoja Sales - 202306840003
 * 
 * https://github.com/dhominicx/projeto-redes-1-simulador
 * 
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/netanim-module.h"

// Network topology
//
//      n0    n1
//      |     |
// n7 - n8 - n9 - n2
//      |  /  |   
// n6 - n11 - n10 - n3
//      |     |
//      n5    n4
//
// 10.1.1.0
// 255.255.255.0
// point-to-point
//

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("FirstScriptExample");

int
main(int argc, char *argv[])
{
	CommandLine cmd(__FILE__);
	cmd.Parse(argc, argv);

	Time::SetResolution(Time::NS);
	LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
	LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

	// Create 12 ns-3 node objects
	NodeContainer nodes;
	nodes.Create(12);

	// Create pointToPoint object and set data rate and delay
	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));

	// Create connexions following our topology
	NetDeviceContainer devices;
	devices.Add (pointToPoint.Install (nodes.Get(0), nodes.Get(8)));   // 1-9
    devices.Add (pointToPoint.Install (nodes.Get(1), nodes.Get(9)));   // 2-10
    devices.Add (pointToPoint.Install (nodes.Get(2), nodes.Get(9)));   // 3-10
    devices.Add (pointToPoint.Install (nodes.Get(3), nodes.Get(10)));  // 4-11
    devices.Add (pointToPoint.Install (nodes.Get(4), nodes.Get(10)));  // 5-11
    devices.Add (pointToPoint.Install (nodes.Get(5), nodes.Get(11)));  // 6-12
    devices.Add (pointToPoint.Install (nodes.Get(6), nodes.Get(11)));  // 7-12
    devices.Add (pointToPoint.Install (nodes.Get(7), nodes.Get(8)));   // 8-9
	
	devices.Add (pointToPoint.Install (nodes.Get(8), nodes.Get(9)));   // 9-10
    devices.Add (pointToPoint.Install (nodes.Get(8), nodes.Get(11)));  // 9-12
	devices.Add (pointToPoint.Install (nodes.Get(8), nodes.Get(10)));   // 8-9
    devices.Add (pointToPoint.Install (nodes.Get(9), nodes.Get(10)));  // 10-11
    devices.Add (pointToPoint.Install (nodes.Get(10), nodes.Get(11))); // 11-12
    devices.Add (pointToPoint.Install (nodes.Get(11), nodes.Get(9)));  // 12-10

	// Set mobility
	MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

	// Install Internet Stack (TCP, UDP, IP, etc.) on each node 
	InternetStackHelper stack;
	stack.Install(nodes);

	// Define network base address and mask
	Ipv4AddressHelper address;
	address.SetBase("10.1.1.0", "255.255.255.0");

	Ipv4InterfaceContainer interfaces = address.Assign(devices);

	// Set UDP echo server on node 8, 9, 10 and 11
	UdpEchoServerHelper echoServer(9); // Use port number 9

	ApplicationContainer serverApps = echoServer.Install(nodes.Get(8));
	serverApps.Start(Seconds(1.0));
	serverApps.Stop(Seconds(10.0));

	UdpEchoClientHelper echoClient (interfaces.GetAddress (8), 9); // Conectar ao IP do nó 8, porta 9
        echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
        echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
        echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

	// Install application on each node and schedule events
	for (uint32_t i = 0; i < 8; ++i)
	{
		ApplicationContainer clientApps = echoClient.Install (nodes.Get (i));
		clientApps.Start (Seconds (2.0));
		clientApps.Stop (Seconds (10.0));
	}

	// Enable pcap tracing
    pointToPoint.EnablePcapAll("redes-pcap");

    double scale = 15.0;
	// Configure NetAnim
	AnimationInterface anim ("anim1.xml");
	anim.SetConstantPosition(nodes.Get(0), 2.0*scale, 4.0*scale);
	anim.SetConstantPosition(nodes.Get(1), 3.0*scale, 4.0*scale);
	anim.SetConstantPosition(nodes.Get(2), 4.0*scale, 3.0*scale);
	anim.SetConstantPosition(nodes.Get(3), 4.0*scale, 2.0*scale);
	anim.SetConstantPosition(nodes.Get(4), 3.0*scale, 1.0*scale);
	anim.SetConstantPosition(nodes.Get(5), 2.0*scale, 1.0*scale);
	anim.SetConstantPosition(nodes.Get(6), 1.0*scale, 2.0*scale);
	anim.SetConstantPosition(nodes.Get(7), 1.0*scale, 3.0*scale);
	anim.SetConstantPosition(nodes.Get(8), 2.0*scale, 3.0*scale);
	anim.SetConstantPosition(nodes.Get(9), 3.0*scale, 3.0*scale);
	anim.SetConstantPosition(nodes.Get(10), 3.0*scale, 2.0*scale);
	anim.SetConstantPosition(nodes.Get(11), 2.0*scale, 2.0*scale);
       
	// Start scheduled events and finish simulation
	Simulator::Run();
	Simulator::Destroy();
	
	return 0;
}
