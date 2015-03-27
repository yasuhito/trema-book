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
      rest_api { sliceable_switch.slice_list }
    end

    desc 'Shows a slice'
    get 'slices/:slice_id' do
      rest_api { sliceable_switch.find_slice params.fetch(:slice_id) }
    end

    desc 'Adds a port to a slice'
    post 'slices/:slice_id/ports' do
      rest_api do
        sliceable_switch.add_port_to_slice(params[:slice_id],
                                           dpid: params[:dpid].to_i,
                                           port_no: params[:port_no].to_i)
      end
    end

    desc 'Deletes a port from a slice'
    delete 'slices/:slice_id/ports' do
      rest_api do
        sliceable_switch.delete_port_from_slice(params[:slice_id],
                                                dpid: params[:dpid].to_i,
                                                port_no: params[:port_no].to_i)
      end
    end

    desc 'Lists ports'
    get 'slices/:slice_id/ports' do
      rest_api { sliceable_switch.ports(params[:slice_id]) }
    end

    desc 'Shows a port'
    get 'slices/:slice_id/ports/:port_id' do
      rest_api do
        sliceable_switch.find_port(params[:slice_id],
                                   Slice::Port.parse(params[:port_id]))
      end
    end

    desc 'Adds a host to a slice'
    post '/slices/:slice_id/ports/:port_id/mac_addresses' do
      rest_api do
        sliceable_switch.add_mac_address(params[:slice_id],
                                         params[:name],
                                         Slice::Port.parse(params[:port_id]))
      end
    end

    desc 'Deletes a host from a slice'
    delete '/slices/:slice_id/ports/:port_id/mac_addresses' do
      rest_api do
        sliceable_switch.delete_mac_address(params[:slice_id],
                                            params[:name],
                                            Slice::Port.parse(params[:port_id]))
      end
    end

    desc 'List MAC addresses'
    get 'slices/:slice_id/ports/:port_id/mac_addresses' do
      rest_api do
        sliceable_switch.
          mac_addresses(params[:slice_id],
                        Slice::Port.parse(params[:port_id]))
      end
    end

    desc 'Shows a MAC address'
    get 'slices/:slice_id/ports/:port_id/mac_addresses/:mac_address_id' do
      rest_api do
        sliceable_switch.find_mac_address(params[:slice_id],
                                          Slice::Port.parse(params[:port_id]),
                                          params[:mac_address_id])
      end
    end
  end
  # rubocop:enable ClassLength
end
