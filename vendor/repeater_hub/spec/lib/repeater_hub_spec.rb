require 'trema'
require 'repeater_hub'

describe RepeaterHub do
  Given(:logger) { spy('logger') }
  Given(:repeater_hub) do
    RepeaterHub.new.tap do |controller|
      allow(controller).to receive(:logger).and_return(logger)
    end
  end

  describe '#start' do
    When { repeater_hub.start(args) }

    context 'with []' do
      Given(:args) { [] }
      Then do
        expect(logger).to(have_received(:info).
                          with('RepeaterHub started.'))
      end
    end
  end

  describe '#packet_in' do
    When { repeater_hub.packet_in(dpid, message) }

    context "with 0xabc, 'PacketIn'" do
      Given(:dpid) { 0xabc }
      Given(:message) { 'PacketIn' }
      Given do
        allow(Pio::ExactMatch).to(receive(:new).with(message).
                                  and_return('EXACT MATCH'))
      end
      Given { allow(repeater_hub).to receive(:send_flow_mod_add) }
      Given { allow(repeater_hub).to receive(:send_packet_out) }
      Then do
        expect(repeater_hub).to(have_received(:send_flow_mod_add).
                                with(0xabc,
                                     match: 'EXACT MATCH',
                                     actions: Pio::SendOutPort.new(:flood)))
        expect(repeater_hub).to(have_received(:send_flow_mod_add).
                                with(0xabc,
                                     match: 'EXACT MATCH',
                                     actions: Pio::SendOutPort.new(:flood)))
      end
    end
  end
end
