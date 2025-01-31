from mininet.topo import SingleSwitchTopo
from mininet.net import Mininet
from mininet.cli import CLI

# Create a simple topology
topo = SingleSwitchTopo(2)
net = Mininet(topo)
net.start()

# Start iPerf server on h1
h1 = net.get('h1')
h2 = net.get('h2')

# Start iperf server in the background
h1.cmd('iperf -s &')

# Run iperf client on h2 and capture the output
iperf_client_output = h2.cmd('iperf -c h1')

# Print the results of the iperf client
print("iPerf Client Output:")
print(iperf_client_output)

# Open Mininet CLI (optional, for further interaction)
CLI(net)

# Stop the network
net.stop()