require 'trema'
require 'patch_panel'

describe PatchPanel do
  Given(:logger) { spy('logger') }
  Given(:patch_panel) do
    PatchPanel.new.tap do |controller|
      allow(controller).to receive(:logger).and_return(logger)
    end
  end

  describe '#start([])' do
    Given { patch_panel.start([]) }

    describe '#switch_ready(0xabc)' do
      Given { patch_panel.switch_ready(0xabc) }

      describe '#create_patch' do
        Given { allow(patch_panel).to receive(:send_flow_mod_add) }

        context 'with 0xabc, 1, 2' do
          When { patch_panel.create_patch 0xabc, 1, 2 }

          Then do
            expect(patch_panel).to(have_received(:send_flow_mod_add).
                                   with(0xabc,
                                        match: Pio::Match.new(in_port: 1),
                                        actions: Pio::SendOutPort.new(2)))
            expect(patch_panel).to(have_received(:send_flow_mod_add).
                                   with(0xabc,
                                        match: Pio::Match.new(in_port: 2),
                                        actions: Pio::SendOutPort.new(1)))
          end
        end
      end

      describe '#delete_patch' do
        Given { allow(patch_panel).to receive(:send_flow_mod_delete) }

        context 'with 0xabc, 1, 2' do
          When { patch_panel.delete_patch 0xabc, 1, 2 }

          Then do
            expect(patch_panel).to(have_received(:send_flow_mod_delete).
                                   with(0xabc,
                                        match: Pio::Match.new(in_port: 1)))
            expect(patch_panel).to(have_received(:send_flow_mod_delete).
                                   with(0xabc,
                                        match: Pio::Match.new(in_port: 2)))
          end
        end
      end
    end
  end
end
