$LOAD_PATH.unshift File.join(__dir__, '..')

require 'grape'
require 'sliceable_switch'
require 'trema'

class SliceableSwitch < PathManager
  # Rest API helper methods
  module RestApiHelpers
    def sliceable_switch
      @sliceable_switch ||=
        Trema.controller_process(ENV['TREMA_SOCKET_DIR']).sliceable_switch
    end

    def rest_api
      yield
    rescue SliceNotFoundError,
           PortNotFoundError,
           MacAddressNotFoundError => not_found_error
      error! not_found_error.message, 404
    rescue SliceAlreadyExistsError,
           PortAlreadyExistsError,
           MacAddressAlreadyExistsError => already_exists_error
      error! already_exists_error.message, 409
    end
  end

  # REST API of RoutingSwitch
  # rubocop:disable ClassLength
  class RestApi < Grape::API
    format :json

    helpers RestApiHelpers

    desc 'Creates a slice.'
    params do
      requires :name, type: String, desc: 'Slice ID.'
    end
    post :slices do
      rest_api { sliceable_switch.add_slice params[:name] }
    end

    desc 'Deletes a slice.'
    params do
      requires :name, type: String, desc: 'Slice ID.'
    end
    delete :slices do
      rest_api { sliceable_switch.delete_slice params[:name] }
    end

    desc 'Lists slices.'
    get :slices do
      rest_api { sliceable_switch.slice_list }
    end

    desc 'Shows a slice.'
    params do
      requires :slice_id, type: String, desc: 'Slice ID.'
    end
    get 'slices/:slice_id' do
      rest_api { sliceable_switch.find_slice params[:slice_id] }
    end

    desc 'Adds a port to a slice.'
    params do
      requires :slice_id, type: String, desc: 'Slice ID.'
      requires :dpid, type: Integer, desc: 'Datapath ID.'
      requires :port_no, type: Integer, desc: 'Port number.'
    end
    post 'slices/:slice_id/ports' do
      rest_api do
        sliceable_switch.add_port_to_slice(params[:slice_id],
                                           dpid: params[:dpid],
                                           port_no: params[:port_no])
      end
    end

    desc 'Deletes a port from a slice.'
    params do
      requires :slice_id, type: String, desc: 'Slice ID.'
      requires :dpid, type: Integer, desc: 'Datapath ID.'
      requires :port_no, type: Integer, desc: 'Port number.'
    end
    delete 'slices/:slice_id/ports' do
      rest_api do
        sliceable_switch.delete_port_from_slice(params[:slice_id],
                                                dpid: params[:dpid],
                                                port_no: params[:port_no])
      end
    end

    desc 'Lists ports.'
    params do
      requires :slice_id, type: String, desc: 'Slice ID.'
    end
    get 'slices/:slice_id/ports' do
      rest_api { sliceable_switch.ports(params[:slice_id]) }
    end

    desc 'Shows a port.'
    params do
      requires :slice_id, type: String, desc: 'Slice ID.'
      requires :port_id, type: String, desc: 'Port ID.'
    end
    get 'slices/:slice_id/ports/:port_id' do
      rest_api do
        sliceable_switch.find_port(params[:slice_id],
                                   Slice::Port.parse(params[:port_id]))
      end
    end

    desc 'Adds a host to a slice.'
    params do
      requires :slice_id, type: String, desc: 'Slice ID.'
      requires :port_id, type: String, desc: 'Port ID.'
      requires :name, type: String, desc: 'MAC address.'
    end
    post '/slices/:slice_id/ports/:port_id/mac_addresses' do
      rest_api do
        sliceable_switch.add_mac_address(params[:slice_id],
                                         params[:name],
                                         Slice::Port.parse(params[:port_id]))
      end
    end

    desc 'Deletes a host from a slice.'
    params do
      requires :slice_id, type: String, desc: 'Slice ID.'
      requires :port_id, type: String, desc: 'Port ID.'
      requires :name, type: String, desc: 'MAC address.'
    end
    delete '/slices/:slice_id/ports/:port_id/mac_addresses' do
      rest_api do
        sliceable_switch.delete_mac_address(params[:slice_id],
                                            params[:name],
                                            Slice::Port.parse(params[:port_id]))
      end
    end

    desc 'List MAC addresses.'
    params do
      requires :slice_id, type: String, desc: 'Slice ID.'
      requires :port_id, type: String, desc: 'Port ID.'
    end
    get 'slices/:slice_id/ports/:port_id/mac_addresses' do
      rest_api do
        sliceable_switch.
          mac_addresses(params[:slice_id],
                        Slice::Port.parse(params[:port_id]))
      end
    end

    desc 'Shows a MAC address.'
    params do
      requires :slice_id, type: String, desc: 'Slice ID.'
      requires :port_id, type: String, desc: 'Port ID.'
      requires :mac_address_id, type: String, desc: 'MAC address.'
    end
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
