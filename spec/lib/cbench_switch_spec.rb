require 'trema'
require 'cbench_switch'

describe CbenchSwitch do
  Given(:logger) { spy('logger') }
  Given(:cbench) do
    CbenchSwitch.new.tap do |controller|
      allow(controller).to receive(:logger).and_return(logger)
    end
  end

  describe '#start' do
    context 'with []' do
      When { cbench.start([]) }
      Then do
        expect(logger).to(have_received(:info).with('CbenchSwitch started.'))
      end
    end
  end

  describe '#packet_in' do
    When { cbench.packet_in(dpid, message) }

    context 'with 0xabc, PacketIn double' do
      Given(:dpid) { 0xabc }
      Given(:message) do
        double('packet_in').tap do |packet_in|
          allow(packet_in).to receive(:buffer_id).and_return('BUFFER ID')
          allow(packet_in).to receive(:in_port).and_return(99)
        end
      end
      Given do
        allow(Pio::ExactMatch).to(receive(:new).with(message).
                                  and_return('EXACT MATCH'))
      end
      Given { allow(cbench).to receive(:send_flow_mod_add) }
      Then do
        expect(cbench).to(have_received(:send_flow_mod_add).
                          with(0xabc,
                               match: 'EXACT MATCH',
                               buffer_id: 'BUFFER ID',
                               actions: Pio::SendOutPort.new(100)))
      end
    end
  end
end
