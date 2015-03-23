require 'trema'
require 'sliceable_switch'

describe SliceableSwitch do
  Given { allow(Trema::Logger).to receive(:new).and_return(spy('logger')) }
  Given(:sliceable_switch) { SliceableSwitch.new.tap(&:start) }

  describe '#slice_list' do
    When(:initial_slices) { sliceable_switch.slice_list }
    Then { initial_slices == [] }
  end

  describe '#find_slice' do
    context "with 'foo'" do
      When(:result) { sliceable_switch.find_slice('foo') }
      Then { result == Failure(SliceNotFoundError) }
    end
  end

  describe '#add_slice' do
    context "with 'foo'" do
      When { sliceable_switch.add_slice 'foo' }
      Then { sliceable_switch.slice_list == ['foo'] }

      describe '#ports' do
        context "with 'foo'" do
          When(:initial_ports) { sliceable_switch.ports 'foo' }
          Then { initial_ports == [] }
        end
      end

      describe '#find_slice' do
        context "with 'foo'" do
          When(:initial_slice) { sliceable_switch.find_slice 'foo' }
          Then { initial_slice == {} }
        end
      end

      describe '#mac_addresses' do
        context "with 'foo', 0x1, 1" do
          When(:result) { sliceable_switch.mac_addresses('foo', 0x1, 1) }
          Then { result == Failure(PortNotFoundError) }
        end
      end

      describe '#add_slice' do
        context "with 'foo'" do
          When(:result) { sliceable_switch.add_slice 'foo' }
          Then { result == Failure(RuntimeError) }
        end
      end

      describe '#add_port_to_slice' do
        context "with 'foo', 0x1, 1" do
          When { sliceable_switch.add_port_to_slice('foo', 0x1, 1) }
          Then do
            sliceable_switch.find_port('foo', 0x1, 1) ==
              { dpid: 0x1, port_no: 1 }
          end
          Then { sliceable_switch.ports('foo') == [{ dpid: 0x1, port_no: 1 }] }

          describe '#mac_addresses' do
            context "with 'foo', 0x1, 1" do
              When(:initial_mac_addresses) do
                sliceable_switch.mac_addresses('foo', 0x1, 1)
              end
              Then { initial_mac_addresses == [] }
            end
          end

          describe '#delete_port_from_slice' do
            context "with 'foo', 0x1, 1" do
              When { sliceable_switch.delete_port_from_slice('foo', 0x1, 1) }
              Then { sliceable_switch.ports('foo') == [] }
            end
          end
        end
      end

      describe '#add_mac_address_to_slice' do
        context "with '11:11:11:11:11:11', 'foo', 0x1, 1" do
          When do
            sliceable_switch.add_mac_address_to_slice('11:11:11:11:11:11',
                                                      'foo', 0x1, 1)
          end
          Then { sliceable_switch.ports('foo') == [{ dpid: 0x1, port_no: 1 }] }
          Then do
            sliceable_switch.
              mac_addresses('foo', 0x1, 1) == ['11:11:11:11:11:11']
          end

          describe '#delete_mac_address_from_slice' do
            context "with '11:11:11:11:11:11', 'foo', 0x1, 1" do
              When do
                sliceable_switch.
                  delete_mac_address_from_slice('11:11:11:11:11:11',
                                                'foo', 0x1, 1)
              end
              Then do
                sliceable_switch.ports('foo') == [{ dpid: 0x1, port_no: 1 }]
              end
              Then { sliceable_switch.mac_addresses('foo', 0x1, 1) == [] }
            end
          end
        end
      end
    end
  end

  describe '#delete_slice' do
    context "with 'foo'" do
      When(:result) { sliceable_switch.delete_slice 'foo' }
      Then { result == Failure(SliceNotFoundError) }
    end
  end

  describe '#add_port_to_slice' do
    context "with 'foo', 0x1, 1" do
      When(:result) { sliceable_switch.add_port_to_slice('foo', 0x1, 1) }
      Then { result == Failure(SliceNotFoundError) }
    end
  end

  describe '#delete_port_from_slice' do
    context "with 'foo', 0x1, 1" do
      When(:result) { sliceable_switch.delete_port_from_slice('foo', 0x1, 1) }
      Then { result == Failure(SliceNotFoundError) }
    end
  end

  describe '#mac_addresses' do
    context "with 'foo', 0x1, 1" do
      When(:result) { sliceable_switch.mac_addresses('foo', 0x1, 1) }
      Then { result == Failure(SliceNotFoundError) }
    end
  end

  describe '#add_mac_address_to_slice' do
    context "with '11:11:11:11:11:11', 'foo', 0x1, 1" do
      When(:result) do
        sliceable_switch.add_mac_address_to_slice('11:11:11:11:11:11',
                                                  'foo', 0x1, 1)
      end
      Then { result == Failure(SliceNotFoundError) }
    end
  end

  describe '#delete_mac_address_from_slice' do
    context "with '11:11:11:11:11:11', 'foo', 0x1, 1" do
      When(:result) do
        sliceable_switch.delete_mac_address_from_slice('11:11:11:11:11:11',
                                                       'foo', 0x1, 1)
      end
      Then { result == Failure(SliceNotFoundError) }
    end
  end
end
