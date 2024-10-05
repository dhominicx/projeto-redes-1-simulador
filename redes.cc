/*
 * Universidade Federal do Pará
 * Instituto de Tecnologia - ITEC
 * Faculdade de Compuutação e Telecomunicações
 *
 * Contrucao de Topologia de Redes Simples Utilizando ns-3
 * Redes de Computadores - 2024.2
 *
 * Dhomini Bezerra Picanco - 202306840009
 * Antonio Carlos Bessa Martins Neto - 202306840039
 * Rafael Felipe Pantoja Sales - 202306840003
 */

#include "ns3/applications-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/network-module.h"
#include "ns3/point-to-point-module.h"

using namespace ns3;

int
main(int argc, char *argv[])
{
	Time::SetResolution(Time::NS);

	// Create 12 ns-3 node objects
	NodeContainer nodes;
	nodes.Create(12);

	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay", StringValue("5ms"));

	NetDeviceContainer devices;
	// devices = pointToPoint.Install(nodes);
	
	// Create connexions following our topology
	devices.Add (pointToPoint.Install (nodes.Get(0), nodes.Get(8)));   // 1-9
        devices.Add (pointToPoint.Install (nodes.Get(1), nodes.Get(9)));   // 2-10
        devices.Add (pointToPoint.Install (nodes.Get(2), nodes.Get(9)));   // 3-10
        devices.Add (pointToPoint.Install (nodes.Get(3), nodes.Get(10)));  // 4-11
        devices.Add (pointToPoint.Install (nodes.Get(4), nodes.Get(10)));  // 5-11
        devices.Add (pointToPoint.Install (nodes.Get(5), nodes.Get(11)));  // 6-12
        devices.Add (pointToPoint.Install (nodes.Get(6), nodes.Get(11)));  // 7-12
        devices.Add (pointToPoint.Install (nodes.Get(7), nodes.Get(8)));   // 8-9
									   //
        devices.Add (pointToPoint.Install (nodes.Get(8), nodes.Get(9)));   // 9-10
        devices.Add (pointToPoint.Install (nodes.Get(8), nodes.Get(11)));  // 9-12
        devices.Add (pointToPoint.Install (nodes.Get(9), nodes.Get(10)));  // 10-11
        devices.Add (pointToPoint.Install (nodes.Get(10), nodes.Get(11))); // 11-12
        devices.Add (pointToPoint.Install (nodes.Get(11), nodes.Get(9)));  // 12-10

	InternetStackHelper stack;
	stack.Install(nodes);

	Ipv4AddressHelper address;
	address.SetBase("10.1.1.0", "255.255.255.0");

	Ipv4InterfaceContainer interfaces = address.Assign(devices);

	UdpEchoServerHelper echoServer(9); // Use port number 9

	ApplicationContainer serverApps = echoServer.Install(nodes.Get(8));
	serverApps.Start(Seconds(1.0));
	serverApps.Stop(Seconds(10.0));

	UdpEchoClientHelper echoClient (interfaces.GetAddress (0), 9); // Conectar ao IP do nó 1, porta 9
        echoClient.SetAttribute ("MaxPackets", UintegerValue (1));
        echoClient.SetAttribute ("Interval", TimeValue (Seconds (1.0)));
        echoClient.SetAttribute ("PacketSize", UintegerValue (1024));

	for (uint32_t i = 1; i < 12; ++i)
	{
		ApplicationContainer clientApps = echoClient.Install (nodes.Get (i));
		clientApps.Start (Seconds (2.0));
		clientApps.Stop (Seconds (10.0));
	}

       
	// Star simulation 
	Simulator::Run();
	Simulator::Destroy();
	
	return 0;
}
