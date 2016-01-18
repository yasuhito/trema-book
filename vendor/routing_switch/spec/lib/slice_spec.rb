require 'slice'

describe Slice, '.create' do
  Given { Slice.destroy_all }
  Given(:slice) { Slice.create('slice') }

  describe '#add_port' do
    context 'with dpid: 0x1, port: 1' do
      When { slice.add_port dpid: 0x1, port_no: 1 }
      When(:found_port) { slice.find_port(dpid: 0x1, port_no: 1) }
      Then { found_port.dpid == 0x1 }
      Then { found_port.port_no == 1 }
      Then { slice.ports.size == 1 }
      Then { slice.ports.first.dpid == 0x1 }
      Then { slice.ports.first.port_no == 1 }

      describe '#add_port' do
        context 'with dpid: 0x1, port: 1' do
          When(:result) { slice.add_port dpid: 0x1, port_no: 1 }
          Then { result == Failure(Slice::PortAlreadyExistsError) }
        end
      end

      describe '#delete_port' do
        context 'with dpid: 0x1, port: 1' do
          When { slice.delete_port dpid: 0x1, port_no: 1 }
          Then { slice.ports == [] }
        end
      end

      describe '#delete_mac_address' do
        context "with '11:11:11:11:11:11', dpid: 0x1, port_no: 1" do
          When(:result) do
            slice.delete_mac_address('11:11:11:11:11:11', dpid: 0x1, port_no: 1)
          end
          Then { result == Failure(Slice::MacAddressNotFoundError) }
        end
      end
    end
  end

  describe '#delete_port' do
    context 'with dpid: 0x1, port: 1' do
      When(:result) { slice.delete_port dpid: 0x1, port_no: 1 }
      Then { result == Failure(Slice::PortNotFoundError) }
    end
  end

  describe '#ports' do
    When(:initial_ports) { slice.ports }
    Then { initial_ports == [] }
  end

  describe '#find_port' do
    context 'with dpid: 0x1, port: 1' do
      When(:result) { slice.find_port dpid: 0x1, port_no: 1 }
      Then { result == Failure(Slice::PortNotFoundError) }
    end
  end

  describe '#add_mac_address' do
    context "with '11:11:11:11:11:11', dpid: 0x1, port_no: 1" do
      When { slice.add_mac_address('11:11:11:11:11:11', dpid: 0x1, port_no: 1) }
      Then do
        slice.mac_addresses(dpid: 0x1, port_no: 1) == ['11:11:11:11:11:11']
      end

      describe '#add_mac_address' do
        context "with '11:11:11:11:11:11', dpid: 0x1, port_no: 1" do
          When(:result) do
            slice.add_mac_address('11:11:11:11:11:11', dpid: 0x1, port_no: 1)
          end
          Then do
            result == Failure(Slice::MacAddressAlreadyExistsError)
          end
        end
      end
    end
  end

  describe '#delete_mac_address' do
    context "with '11:11:11:11:11:11', dpid: 0x1, port_no: 1" do
      When(:result) do
        slice.delete_mac_address('11:11:11:11:11:11', dpid: 0x1, port_no: 1)
      end
      Then { result == Failure(Slice::PortNotFoundError) }
    end
  end

  describe '#mac_addresses' do
    context 'with dpid: 0x1, port_no: 1' do
      When(:result) { slice.mac_addresses(dpid: 0x1, port_no: 1) }
      Then { result == Failure(Slice::PortNotFoundError) }
    end
  end
end
