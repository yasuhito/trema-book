require 'trema'
require 'routing_switch'

describe RoutingSwitch do
  Given { allow(Trema::Logger).to receive(:new).and_return(spy('logger')) }
  Given(:topology) { spy('topology') }
  Given(:routing_switch) do
    RoutingSwitch.new.tap do |routing_switch|
      allow(TopologyController).to receive(:new).and_return(topology)
      routing_switch.start []
    end
  end

  describe '#switch_ready' do
    context 'with 0xabc' do
      When { routing_switch.switch_ready(0xabc) }
      Then { expect(topology).to have_received(:switch_ready).with(0xabc) }
    end
  end

  describe '#features_reply' do
    context 'with 0xabc, :features_reply' do
      When { routing_switch.features_reply(0xabc, :features_reply) }
      Then do
        expect(topology).to(have_received(:features_reply).
                            with(0xabc, :features_reply))
      end
    end
  end

  describe '#switch_disconnected' do
    context 'with 0xabc' do
      When { routing_switch.switch_disconnected(0xabc) }
      Then do
        expect(topology).to have_received(:switch_disconnected).with(0xabc)
      end
    end
  end

  describe '#port_modify' do
    context 'with 0xabc, :port_modify' do
      When { routing_switch.port_modify(0xabc, :port_modify) }
      Then do
        expect(topology).to(have_received(:port_modify).
                            with(0xabc, :port_modify))
      end
    end
  end

  describe '#packet_in' do
    context 'with 0xabc, :packet_in (LLDP)' do
      Given(:packet_in) { double('packet_in', lldp?: true) }
      When { routing_switch.packet_in(0xabc, packet_in) }
      Then do
        expect(topology).to have_received(:packet_in).with(0xabc, packet_in)
      end
    end
  end
end
