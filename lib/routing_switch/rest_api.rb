$LOAD_PATH.unshift File.join(__dir__, '..')

require 'grape'
require 'trema'
require 'sliceable_switch'

class RoutingSwitch < Trema::Controller
  # REST API of RoutingSwitch
  # rubocop:disable ClassLength
  class RestApi < Grape::API
    format :json

    helpers do
      def sliceable_switch
        Trema.controller_process(ENV['TREMA_SOCKET_DIR']).sliceable_switch
      end
    end

    desc 'Creates a slice'
    post :slices do
      sliceable_switch.add_slice(params.fetch(:name))
    end

    desc 'Deletes a slice'
    delete :slices do
      sliceable_switch.delete_slice(params.fetch(:name))
    end

    desc 'Lists slices'
    get :slices do
      sliceable_switch.slice_list.map { |each| { name: each } }
    end

    desc 'Shows a slice'
    get 'slices/:slice_id' do
      begin
        sliceable_switch.find_slice(params[:slice_id])
        { name: params[:slice_id] }
      rescue SliceableSwitch::SliceNotFoundError
        error! "Slice named '#{params[:slice_id]}' not found", 404
      end
    end

    desc 'Adds a port to a slice'
    post 'slices/:slice_id/ports' do
      sliceable_switch.add_port_to_slice(params[:slice_id],
                                         dpid: params[:dpid].to_i,
                                         port_no: params[:port_no].to_i)
    end

    desc 'Deletes a port from a slice'
    delete 'slices/:slice_id/ports' do
      sliceable_switch.delete_port_from_slice(params[:slice_id],
                                              dpid: params[:dpid].to_i,
                                              port_no: params[:port_no].to_i)
    end

    desc 'Lists ports'
    get 'slices/:slice_id/ports' do
      begin
        sliceable_switch.ports(params[:slice_id]).map do |each|
          each.merge(name: "#{format('%#x', each[:dpid])}:#{each[:port_no]}")
        end
      rescue SliceableSwitch::SliceNotFoundError
        error! "Slice named '#{params[:slice_id]}' not found", 404
      end
    end

    desc 'Shows a port'
    get 'slices/:slice_id/ports/:port_id' do
      begin
        dpid_str, port_no_str = params[:port_id].split(':')
        sliceable_switch.
          find_port(params[:slice_id],
                    dpid: dpid_str.hex, port_no: port_no_str.to_i).
          merge(name: params[:port_id])
      rescue SliceableSwitch::SliceNotFoundError
        error! "Slice named '#{params[:slice_id]}' not found", 404
      rescue SliceableSwitch::PortNotFoundError
        error! "Port named '#{params[:port_id]}' not found", 404
      end
    end

    desc 'Adds a host to a slice'
    post '/slices/:slice_id/ports/:port_id/mac_addresses' do
      unless /\A(\S+):(\d+)\Z/ =~ params[:port_id]
        fail "Invalid switch port #{params[:port]}"
      end
      dpid_str = Regexp.last_match(1)
      port_no = Regexp.last_match(2).to_i
      dpid = (/\A0x/ =~ dpid_str) ? dpid_str.hex : dpid_str.to_i
      sliceable_switch.add_mac_address_to_slice(params[:name],
                                                params[:slice_id],
                                                dpid: dpid, port_no: port_no)
    end

    desc 'Deletes a host from a slice'
    delete '/slices/:slice_id/ports/:port_id/mac_addresses' do
      unless /\A(\S+):(\d+)\Z/ =~ params[:port_id]
        fail "Invalid switch port #{params[:port]}"
      end
      dpid_str = Regexp.last_match(1)
      port_no = Regexp.last_match(2).to_i
      dpid = (/\A0x/ =~ dpid_str) ? dpid_str.hex : dpid_str.to_i
      sliceable_switch.
        delete_mac_address_from_slice(params[:name],
                                      params[:slice_id],
                                      dpid: dpid,
                                      port_no: port_no)
    end

    desc 'List MAC addresses'
    get 'slices/:slice_id/ports/:port_id/mac_addresses' do
      begin
        dpid_str, port_no_str = params[:port_id].split(':')
        sliceable_switch.mac_addresses(params[:slice_id],
                                       dpid: dpid_str.hex,
                                       port_no: port_no_str.to_i).map do |each|
          { name: each }
        end
      rescue SliceableSwitch::SliceNotFoundError
        error! "Slice named '#{params[:slice_id]}' not found", 404
      rescue SliceableSwitch::PortNotFoundError
        error! "Port named '#{params[:port_id]}' not found", 404
      end
    end

    desc 'Shows a MAC address'
    get 'slices/:slice_id/ports/:port_id/mac_addresses/:mac_address_id' do
      begin
        dpid_str, port_no_str = params[:port_id].split(':')
        sliceable_switch.mac_addresses(params[:slice_id],
                                       dpid: dpid_str.hex,
                                       port_no: port_no_str.to_i).each do |each|
          if each == params[:mac_address_id]
            return { name: params[:mac_address_id] }
          end
        end
        error! "MAC address '#{params[:mac_address_id]}' not found", 404
      rescue SliceableSwitch::SliceNotFoundError
        error! "Slice named '#{params[:slice_id]}' not found", 404
      rescue SliceableSwitch::PortNotFoundError
        error! "Port named '#{params[:port_id]}' not found", 404
      end
    end
  end
  # rubocop:enable ClassLength
end
