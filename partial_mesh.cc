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
#include "string.h"
#include "stdio.h"

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
	uint32_t nclient = 4;
	uint32_t nserver = 8;

	uint32_t maxpackets = 1;
	double interval = 1.0;
	uint32_t packetsize = 1024;
	
	bool tracing = false;

	CommandLine cmd(__FILE__);
	cmd.AddValue("nclient", "Set client node", nclient);
    cmd.AddValue("nserver", "Set server node", nserver);

	cmd.AddValue("maxpackets", "Set max number of packets", maxpackets);
	cmd.AddValue("interval", "Set interval", interval);
	cmd.AddValue("packetsize", "Set packet size", packetsize);

	cmd.AddValue("tracing", "Configure tracing option to generate pcap files", tracing);

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
	// NetDeviceContainer devices;
	// devices.Add(pointToPoint.Install(nodes.Get(0), nodes.Get(8)));   // 0-8
    // devices.Add(pointToPoint.Install(nodes.Get(1), nodes.Get(9)));   // 1-9
    // devices.Add(pointToPoint.Install(nodes.Get(2), nodes.Get(9)));   // 2-9
    // devices.Add(pointToPoint.Install(nodes.Get(3), nodes.Get(10)));  // 3-10
    // devices.Add(pointToPoint.Install(nodes.Get(4), nodes.Get(10)));  // 4-10
    // devices.Add(pointToPoint.Install(nodes.Get(5), nodes.Get(11)));  // 5-11
    // devices.Add(pointToPoint.Install(nodes.Get(6), nodes.Get(11)));  // 6-11
    // devices.Add(pointToPoint.Install(nodes.Get(7), nodes.Get(8)));   // 7-8
	
	// devices.Add(pointToPoint.Install(nodes.Get(8), nodes.Get(9)));   // 8-9
    // devices.Add(pointToPoint.Install(nodes.Get(8), nodes.Get(11)));  // 8-11
	// devices.Add(pointToPoint.Install(nodes.Get(8), nodes.Get(10)));  // 8-10
    // devices.Add(pointToPoint.Install(nodes.Get(9), nodes.Get(10)));  // 9-10
    // devices.Add(pointToPoint.Install(nodes.Get(10), nodes.Get(11))); // 10-11
    // devices.Add(pointToPoint.Install(nodes.Get(11), nodes.Get(9)));  // 11-9

	// Set mobility
	MobilityHelper mobility;
    mobility.SetMobilityModel("ns3::ConstantPositionMobilityModel");
    mobility.Install(nodes);

	// Install Internet Stack(TCP, UDP, IP, etc.) on each node 
	InternetStackHelper stack;
	stack.Install(nodes);

	// Define network base address and mask
	// Ipv4AddressHelper address;
	// address.SetBase("10.1.1.0", "255.255.255.0");

	// Ipv4InterfaceContainer interfaces = address.Assign(devices);

	Ipv4AddressHelper address;
	int subnet = 1;

	// Connect some of the nodes to form a partial mesh
	auto connectNodes = [&](uint32_t n1, uint32_t n2) {
		NetDeviceContainer devices;
		devices = pointToPoint.Install(NodeContainer(nodes.Get(n1), nodes.Get(n2)));
		std::ostringstream subnetStream;
		subnetStream << "10.1." << subnet++ << ".0";
		address.SetBase(subnetStream.str().c_str(), "255.255.255.0");
		address.Assign(devices);
	};

	// Example connections for a partial mesh topology
	connectNodes(0, 8); // 8 -> 10.1.1.2
	connectNodes(1, 9); // 9 -> 10.1.2.2
	connectNodes(2, 9); 
	connectNodes(3, 10); // 10 -> 10.1.4.2
	connectNodes(4, 10);
	connectNodes(5, 11); // 11 -> 11.1.6.2
	connectNodes(6, 11);
    connectNodes(7, 8);
	connectNodes(8, 9);
	connectNodes(8, 11);
	connectNodes(8, 10);
	connectNodes(9, 10);
	connectNodes(10, 11);
	connectNodes(11, 9);

	// Enable routing
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();

	// Set UDP echo server on node 8
	UdpEchoServerHelper echoServer(9); // Use port number 9

	ApplicationContainer serverApps = echoServer.Install(nodes.Get(nserver));
	serverApps.Start(Seconds(1.0));	// Set star time for traffic generation
	serverApps.Stop(Seconds(10.0)); // Set stop time for traffic generation

	char server_ip[9];

	if(nserver == 8) {
		strncpy(server_ip, "10.1.1.2", 9);
	}
	if(nserver == 9) {
		strncpy(server_ip, "10.1.2.2", 9);
	}
	if(nserver == 10) {
		strncpy(server_ip, "10.1.4.2", 9);
	}
	if(nserver == 11) {
		strncpy(server_ip, "11.1.6.2", 9);
	}

	UdpEchoClientHelper echoClient(Ipv4Address(server_ip), 9); // Conectar ao IP do nó 8, porta 9
	echoClient.SetAttribute("MaxPackets", UintegerValue(maxpackets));
	echoClient.SetAttribute("Interval", TimeValue(Seconds(interval)));
	echoClient.SetAttribute("PacketSize", UintegerValue(packetsize));

	// Install application on each node and schedule events
	/* for(uint32_t i = 0; i < 8; ++i)
	{
		ApplicationContainer clientApps = echoClient.Install(nodes.Get(i));
		clientApps.Start(Seconds(2.0));
		clientApps.Stop(Seconds(4.0));
	} */

	ApplicationContainer clientApps = echoClient.Install(nodes.Get(nclient));
		clientApps.Start(Seconds(2.0));
		clientApps.Stop(Seconds(10.0));

	// Enable pcap tracing
	if(tracing){
		pointToPoint.EnablePcapAll("redes-pcap");
	}

	// Configure NetAnim
    double scale = 15.0;
	AnimationInterface anim("redes-anim.xml");
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
