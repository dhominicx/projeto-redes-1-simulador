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
#include "ns3/csma-module.h"
#include "ns3/internet-module.h"
#include "ns3/mobility-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"
#include "ns3/ssid.h"
#include "ns3/yans-wifi-helper.h"
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
	uint32_t nWifi = 3;
	bool tracing = false;

	CommandLine cmd(__FILE__);
	cmd.Parse(argc, argv);

	Time::SetResolution(Time::NS);
	LogComponentEnable("UdpEchoClientApplication", LOG_LEVEL_INFO);
	LogComponentEnable("UdpEchoServerApplication", LOG_LEVEL_INFO);

	// Create 12 ns-3 node objects
	NodeContainer p2pNodes;
	p2pNodes.Create(12);

	// Create pointToPoint object and set data rate and delay
	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("20Mbps"));
	pointToPoint.SetChannelAttribute("Delay", StringValue("1ms"));

	// Create p2p devices and connexions following our topology
	NetDeviceContainer p2pDevices;
	p2pDevices.Add (pointToPoint.Install (p2pNodes.Get(0), p2pNodes.Get(8)));   // 1-9
    p2pDevices.Add (pointToPoint.Install (p2pNodes.Get(1), p2pNodes.Get(9)));   // 2-10
    p2pDevices.Add (pointToPoint.Install (p2pNodes.Get(2), p2pNodes.Get(9)));   // 3-10
    p2pDevices.Add (pointToPoint.Install (p2pNodes.Get(3), p2pNodes.Get(10)));  // 4-11
    p2pDevices.Add (pointToPoint.Install (p2pNodes.Get(4), p2pNodes.Get(10)));  // 5-11
    p2pDevices.Add (pointToPoint.Install (p2pNodes.Get(5), p2pNodes.Get(11)));  // 6-12
    p2pDevices.Add (pointToPoint.Install (p2pNodes.Get(6), p2pNodes.Get(11)));  // 7-12
    p2pDevices.Add (pointToPoint.Install (p2pNodes.Get(7), p2pNodes.Get(8)));   // 8-9
	
    p2pDevices.Add (pointToPoint.Install (p2pNodes.Get(8), p2pNodes.Get(9)));   // 9-10
    p2pDevices.Add (pointToPoint.Install (p2pNodes.Get(8), p2pNodes.Get(11)));  // 9-12
    p2pDevices.Add (pointToPoint.Install (p2pNodes.Get(9), p2pNodes.Get(10)));  // 10-11
    p2pDevices.Add (pointToPoint.Install (p2pNodes.Get(10), p2pNodes.Get(11))); // 11-12
    p2pDevices.Add (pointToPoint.Install (p2pNodes.Get(11), p2pNodes.Get(9)));  // 12-10


	// Create wifi devices and connexions following our topology
	NodeContainer wifiStaNodes;
    wifiStaNodes.Create(nWifi);
    NodeContainer wifiApNode = p2pNodes.Get(0);

	YansWifiChannelHelper channel = YansWifiChannelHelper::Default();
    YansWifiPhyHelper phy;
    phy.SetChannel(channel.Create());

	WifiMacHelper mac;
    Ssid ssid = Ssid("ns-3-ssid");

    WifiHelper wifi;

	NetDeviceContainer staDevices;
    mac.SetType("ns3::StaWifiMac", "Ssid", SsidValue(ssid), "ActiveProbing", BooleanValue(false));
    staDevices = wifi.Install(phy, mac, wifiStaNodes);

    NetDeviceContainer apDevices;
    mac.SetType("ns3::ApWifiMac", "Ssid", SsidValue(ssid));
    apDevices = wifi.Install(phy, mac, wifiApNode);

	// Install Internet Stack (TCP, UDP, IP, etc.) on each node 
	InternetStackHelper stack;
	stack.Install(p2pNodes);
	stack.Install(wifiApNode);
	stack.Install(wifiStaNodes);
	
	// Define network base address and mask
	Ipv4AddressHelper address;
	address.SetBase("10.1.1.0", "255.255.255.0");

	Ipv4InterfaceContainer interfaces = address.Assign(p2pDevices);

	// Set UDP echo server on node 8
	UdpEchoServerHelper echoServer(9); // Use port number 9

	ApplicationContainer serverApps = echoServer.Install(p2pNodes.Get(8));
	serverApps.Start(Seconds(1.0));
	serverApps.Stop(Seconds(10.0));
	
	UdpEchoClientHelper echoClient (interfaces.GetAddress (8), 9); // Conectar ao IP do nó 8, porta 9
        echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
        echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
        echoClient.SetAttribute ("PacketSize", UintegerValue (1024));


	// Install application on each node and schedule events
	for (uint32_t i = 0; i < 8; ++i)
	{
		ApplicationContainer clientApps = echoClient.Install (p2pNodes.Get (i));
		clientApps.Start (Seconds (2.0));
		clientApps.Stop (Seconds (10.0));
	}

	// Enable pcap tracing
	if (tracing)
	{
		pointToPoint.EnablePcapAll("redes-pcap");
	}
    

	// Configure NetAnim and set node positions
    double escale = 20.0;
	AnimationInterface anim ("anim1.xml");
	anim.SetConstantPosition(p2pNodes.Get(0), 2.0*escale, 4.0*escale);
	anim.SetConstantPosition(p2pNodes.Get(1), 3.0*escale, 4.0*escale);
	anim.SetConstantPosition(p2pNodes.Get(2), 4.0*escale, 3.0*escale);
	anim.SetConstantPosition(p2pNodes.Get(3), 4.0*escale, 2.0*escale);
	anim.SetConstantPosition(p2pNodes.Get(4), 3.0*escale, 1.0*escale);
	anim.SetConstantPosition(p2pNodes.Get(5), 2.0*escale, 1.0*escale);
	anim.SetConstantPosition(p2pNodes.Get(6), 1.0*escale, 2.0*escale);
	anim.SetConstantPosition(p2pNodes.Get(7), 1.0*escale, 3.0*escale);
	anim.SetConstantPosition(p2pNodes.Get(8), 2.0*escale, 3.0*escale);
	anim.SetConstantPosition(p2pNodes.Get(9), 3.0*escale, 3.0*escale);
	anim.SetConstantPosition(p2pNodes.Get(10), 3.0*escale, 2.0*escale);
	anim.SetConstantPosition(p2pNodes.Get(11), 2.0*escale, 2.0*escale);
       
	// Start scheduled events and finish simulation
	Simulator::Run();
	Simulator::Destroy();
	
	return 0;
}
