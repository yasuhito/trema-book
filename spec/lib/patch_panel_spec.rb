require 'trema'
require 'patch_panel'

describe PatchPanel do
  Given(:patch_panel) { PatchPanel.new }

  context 'when patch_panel.conf = "1 2\n3 4"' do
    Given(:config) { "1 2\n3 4" }
    Given { allow(File).to receive(:open).and_return(StringIO.new(config)) }

    describe '#start' do
      When { patch_panel.start(argv) }

      context "with ['patch_panel.rb']" do
        Given(:argv) { ['patch_panel.rb'] }
        Then { patch_panel.instance_variable_get(:@patch) == [[1, 2], [3, 4]] }

        describe '#switch_ready' do
          When { patch_panel.switch_ready(dpid) }

          context 'with 0xabc' do
            Given(:dpid) { 0xabc }
            Given { allow(patch_panel).to receive(:send_flow_mod_add) }
            Then do
              expect(patch_panel).to(have_received(:send_flow_mod_add).
                                     with(0xabc,
                                          match: Pio::Match.new(in_port: 1),
                                          actions: Pio::SendOutPort.new(2)))
              expect(patch_panel).to(have_received(:send_flow_mod_add).
                                     with(0xabc,
                                          match: Pio::Match.new(in_port: 2),
                                          actions: Pio::SendOutPort.new(1)))
              expect(patch_panel).to(have_received(:send_flow_mod_add).
                                     with(0xabc,
                                          match: Pio::Match.new(in_port: 3),
                                          actions: Pio::SendOutPort.new(4)))
              expect(patch_panel).to(have_received(:send_flow_mod_add).
                                     with(0xabc,
                                          match: Pio::Match.new(in_port: 4),
                                          actions: Pio::SendOutPort.new(3)))
            end
          end
        end
      end
    end
  end
end
