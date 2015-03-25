$LOAD_PATH.unshift File.join(__dir__, '..')

require 'grape'
require 'trema'
require 'sliceable_switch'

class SliceableSwitch < PathManager
  # REST API of RoutingSwitch
  # rubocop:disable ClassLength
  class RestApi < Grape::API
    format :json

    helpers do
      def sliceable_switch
        Trema.controller_process(ENV['TREMA_SOCKET_DIR']).sliceable_switch
      end

      def rest_api
        yield
      rescue SliceableSwitch::SliceNotFoundError,
             SliceableSwitch::PortNotFoundError,
             MacAddressNotFoundError => exception
        error! exception.message, 404
      rescue SliceableSwitch::SliceAlreadyExistsError,
             SliceableSwitch::PortAlreadyExistsError,
             SliceableSwitch::MacAddressAlreadyExistsError => exception
        error! exception.message, 409
      end
    end

    desc 'Creates a slice'
    post :slices do
      rest_api { sliceable_switch.add_slice params.fetch(:name) }
    end

    desc 'Deletes a slice'
    delete :slices do
      rest_api { sliceable_switch.delete_slice params.fetch(:name) }
    end

    desc 'Lists slices'
    get :slices do
      rest_api do
        sliceable_switch.slice_list.map { |each| { name: each } }
      end
    end

    desc 'Shows a slice'
    get 'slices/:slice_id' do
      rest_api do
        sliceable_switch.find_slice params.fetch(:slice_id)
        { name: params.fetch(:slice_id) }
      end
    end

    desc 'Adds a port to a slice'
    post 'slices/:slice_id/ports' do
      rest_api do
        sliceable_switch.
          find_slice(params[:slice_id]).
          add_port(dpid: params[:dpid].to_i, port_no: params[:port_no].to_i)
      end
    end

    desc 'Deletes a port from a slice'
    delete 'slices/:slice_id/ports' do
      rest_api do
        sliceable_switch.
          find_slice(params[:slice_id]).
          delete_port(dpid: params[:dpid].to_i, port_no: params[:port_no].to_i)
      end
    end

    desc 'Lists ports'
    get 'slices/:slice_id/ports' do
      rest_api do
        sliceable_switch.find_slice(params[:slice_id]).ports.map do |each|
          each.merge(name: "#{format('%#x', each[:dpid])}:#{each[:port_no]}")
        end
      end
    end

    desc 'Shows a port'
    get 'slices/:slice_id/ports/:port_id' do
      rest_api do
        dpid_str, port_no_str = params[:port_id].split(':')
        sliceable_switch.
          find_slice(params[:slice_id]).
          find_port(dpid: dpid_str.hex, port_no: port_no_str.to_i).
          merge(name: params[:port_id])
      end
    end

    desc 'Adds a host to a slice'
    post '/slices/:slice_id/ports/:port_id/mac_addresses' do
      rest_api do
        unless /\A(\S+):(\d+)\Z/ =~ params[:port_id]
          fail "Invalid switch port #{params[:port]}"
        end
        dpid_str = Regexp.last_match(1)
        port_no = Regexp.last_match(2).to_i
        dpid = (/\A0x/ =~ dpid_str) ? dpid_str.hex : dpid_str.to_i
        sliceable_switch.
          find_slice(params[:slice_id]).
          add_mac_address(params[:name], dpid: dpid, port_no: port_no)
      end
    end

    desc 'Deletes a host from a slice'
    delete '/slices/:slice_id/ports/:port_id/mac_addresses' do
      rest_api do
        unless /\A(\S+):(\d+)\Z/ =~ params[:port_id]
          fail "Invalid switch port #{params[:port]}"
        end
        dpid_str = Regexp.last_match(1)
        port_no = Regexp.last_match(2).to_i
        dpid = (/\A0x/ =~ dpid_str) ? dpid_str.hex : dpid_str.to_i
        sliceable_switch.
          find_slice(params[:slice_id]).
          delete_mac_address(params[:name], dpid: dpid, port_no: port_no)
      end
    end

    desc 'List MAC addresses'
    get 'slices/:slice_id/ports/:port_id/mac_addresses' do
      rest_api do
        dpid_str, port_no_str = params[:port_id].split(':')
        sliceable_switch.
          find_slice(params[:slice_id]).
          mac_addresses(dpid: dpid_str.hex,
                        port_no: port_no_str.to_i).map do |each|
          { name: each }
        end
      end
    end

    desc 'Shows a MAC address'
    get 'slices/:slice_id/ports/:port_id/mac_addresses/:mac_address_id' do
      rest_api do
        dpid_str, port_no_str = params[:port_id].split(':')
        sliceable_switch.
          find_slice(params[:slice_id]).
          mac_addresses(dpid: dpid_str.hex,
                        port_no: port_no_str.to_i).each do |each|
          if each == params[:mac_address_id]
            return { name: params[:mac_address_id] }
          end
        end
        error! "MAC address #{params[:mac_address_id]} not found", 404
      end
    end
  end
  # rubocop:enable ClassLength
end
